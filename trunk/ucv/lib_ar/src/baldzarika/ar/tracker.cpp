#include <baldzarika/ar/config.h>
#include <baldzarika/ar/tracker.h>
#include <baldzarika/ar/marker.h>
#include <baldzarika/ucv/integral.h>
#include <baldzarika/ucv/match_feature_points.h>
#include <baldzarika/ucv/homography.h>

namespace baldzarika { namespace ar {

	boost::uint32_t const tracker::DEFAULT_SURF_OCTAVES=2;
	boost::uint32_t const tracker::DEFAULT_SURF_INTERVALS=4;
	boost::uint32_t const tracker::DEFAULT_SURF_SAMPLE_STEPS=2;
	float const tracker::DEFAULT_SURF_TRESHOLD=1.0e-3f;

	boost::uint32_t const tracker::DEFAULT_KLT_WIN_SIZE=5;
	boost::uint32_t const tracker::DEFAULT_KLT_LEVELS=4;
	boost::uint32_t const tracker::DEFAULT_KLT_MAX_ITERATIONS=10;
	float const	tracker::DEFAULT_KLT_EPSILON=1.0e-3f;

	boost::uint32_t const tracker::DEFAULT_TRACKER_MIN_MARKER_FEATURES=8;
	boost::uint32_t const tracker::DEFAULT_TRACKER_MAX_MARKER_FEATURES=16;
	float const	tracker::DEFAULT_TRACKER_SELECT_FP_SCALE=2.0f;
	float const	tracker::DEFAULT_TRACKER_SELECT_FP_MIN_AREA=2.0e-2f;

	namespace {

		struct compare_relative_fp_scales
		{
			bool operator()(
				std::pair< tracker::feature_point_t::desc_value_type, std::size_t > const &a,
				std::pair< tracker::feature_point_t::desc_value_type, std::size_t > const &b
			) const
			{
				return a.first<b.first;
			}
		};

	} //namespace anonymous

		
	tracker::marker_state::marker_state(boost::shared_ptr<tracker> const &t, boost::shared_ptr<marker> const &m)
		: m_tracker(t)
		, m_marker(m)
		, m_detected(false)
	{
	}

	tracker::marker_state::~marker_state()
	{
	}

	boost::shared_ptr<tracker> tracker::marker_state::get_tracker() const
	{
		return m_tracker.lock();
	}

	bool tracker::marker_state::is_detected() const
	{
		return m_detected;
	}

	boost::shared_ptr<marker> const& tracker::marker_state::get_marker() const
	{
		return m_marker;
	}

	tracker::marker_state::points2_t const& tracker::marker_state::get_frame_points() const
	{
		return m_frame_points;
	}

	ucv::matrix33f const& tracker::marker_state::get_homography_matrix() const
	{
		return m_hmatrix;
	}

	void tracker::marker_state::set_detected(bool d)
	{
		m_detected=d;
	}

	tracker::tracker(ucv::size2ui const &fs)
		: m_min_marker_features(DEFAULT_TRACKER_MIN_MARKER_FEATURES)
		, m_max_marker_features(DEFAULT_TRACKER_MAX_MARKER_FEATURES)
		, m_select_fp_scale(DEFAULT_TRACKER_SELECT_FP_SCALE)
		, m_select_fp_min_area(DEFAULT_TRACKER_SELECT_FP_MIN_AREA)
		, m_ios()
		, m_ios_work(m_ios)
		, m_worker()
		, m_is_started(false)
		, m_integral_frame_seq(0)
		, m_integral_views(2)
		, m_surf(fs,
			DEFAULT_SURF_OCTAVES,
			DEFAULT_SURF_INTERVALS,
			DEFAULT_SURF_SAMPLE_STEPS,
			DEFAULT_SURF_TRESHOLD
		)
		, m_klt_tracker(fs,
			ucv::size2ui(DEFAULT_KLT_WIN_SIZE,DEFAULT_KLT_WIN_SIZE),
			DEFAULT_KLT_LEVELS,
			DEFAULT_KLT_MAX_ITERATIONS,
			DEFAULT_KLT_EPSILON
		)
	{
		m_integral_frame_stg[0].recreate(fs.width(), fs.height(), 4);
		m_integral_frame_stg[1].recreate(fs.width(), fs.height(), 4);
	}

	tracker::~tracker()
	{
		if(is_active())
		{
			stop();
			m_worker.join();
		}
	}

	ucv::size2ui tracker::get_frame_size() const
	{
		return m_surf.size();
	}

	bool tracker::set_frame_size(ucv::size2ui const &fs)
	{
		if(is_active())
			return false;
		
		m_surf.resize(fs);
		m_klt_tracker.set_frame_size(fs);
		m_integral_frame_stg[0].recreate(fs.width(), fs.height(), 4);
		m_integral_frame_stg[1].recreate(fs.width(), fs.height(), 4);
		return true;
	}

	boost::shared_ptr<tracker::marker_state> tracker::add_marker(boost::shared_ptr<marker> const &m)
	{
		if(is_active())
			return boost::shared_ptr<marker_state>();
		boost::shared_ptr<marker_state> new_marker_state( new marker_state(shared_from_this(), m) );
		if(m_marker_states.push_back(new_marker_state).second)
			return new_marker_state;
		return boost::shared_ptr<marker_state>();
	}

	bool tracker::remove_marker(boost::shared_ptr<marker> const &m)
	{
		return false;
	}

	bool tracker::remove_marker_state(boost::shared_ptr<marker_state> const &ms)
	{
		return false;
	}

	tracker::marker_state::changed_signal_t& tracker::marker_state_changed() const
	{
		return m_marker_state_changed;
	}

	tracker::start_stop_signal_t& tracker::start_stop() const
	{
		return m_start_stop;
	}
	
	tracker::stats_signal_t& tracker::stats() const
	{
		return m_stats;
	}

	bool tracker::start()
	{
		if(is_active())
			return false;

		m_worker.join();
		m_ios.reset();

		m_integral_frame_seq=0;
		m_integral_views.clear();

		m_worker=boost::thread(boost::bind(&tracker::run, this)).move();
		return is_active();
	}

	bool tracker::is_active() const
	{
		return m_worker.get_id()!=boost::thread::id();
	}

	bool tracker::is_started() const
	{
		return m_is_started;
	}

	bool tracker::stop()
	{
		if(!is_active())
			return false;
		m_ios.stop();
		return true;
	}

	bool tracker::update(tracker::gray_const_view_t gv, tracker::gray_t const &sm)
	{
		if(!is_active())
			return false;
		{
			boost::mutex::scoped_try_lock try_lock_update(m_update_sync);
			if(try_lock_update)
			{
				if(!m_integral_views.full())
				{
					ucv::integral(
						gv,
						ucv::gil::view(m_integral_frame_stg[(m_integral_frame_seq & 0x0001)]),
						sm
					);
					
					m_integral_views.push_front(
						ucv::gil::const_view(m_integral_frame_stg[(m_integral_frame_seq & 0x0001)])
					);
					
					m_integral_frame_seq++;
					m_ios.post( boost::bind(&tracker::on_update, this) );
				}
			}
		}
		return true;
	}

	void tracker::on_start()
	{
		m_start_stop(shared_from_this(), true);

		marker_states_t::index<marker_state::detected_tag>::type &markers_by_detected=m_marker_states.get<marker_state::detected_tag>();
		for(marker_states_t::iterator it_marker=m_marker_states.begin();it_marker!=m_marker_states.end();++it_marker)
		{
			boost::shared_ptr<marker_state> ms=*it_marker;
			BOOST_ASSERT(ms);
			markers_by_detected.modify(
				m_marker_states.project<marker_state::detected_tag>(it_marker),
				boost::bind(
					&marker_state::set_detected,
					_1,
					false
				)
			);
			m_marker_state_changed(ms, marker_state::SC_DETECTION);
			describe_marker(*ms);
		}
		
	}

	void tracker::on_update()
	{
		boost::mutex::scoped_lock lock_update(m_update_sync);

		if(!m_integral_views.empty())
		{
			marker_states_t::index<marker_state::detected_tag>::type &marker_states_by_detected=
				m_marker_states.get<marker_state::detected_tag>();
		
			std::pair<
				marker_states_t::index<marker_state::detected_tag>::type::iterator,
				marker_states_t::index<marker_state::detected_tag>::type::iterator
			> any_detected_markers(
				marker_states_by_detected.lower_bound(true),
				marker_states_by_detected.upper_bound(true)
			);

			if(any_detected_markers.first!=any_detected_markers.second)
			{
				if(m_integral_views.full())
				{
					integral_view_buffer_t::iterator curr_frame_it=m_integral_views.begin();
					integral_view_buffer_t::iterator prev_frame_it=curr_frame_it;
					prev_frame_it++;
					
					m_klt_tracker.set_integral_views(*prev_frame_it, *curr_frame_it);

					std::vector<marker_states_t::iterator> detected_markers;

					for(marker_states_t::index<marker_state::detected_tag>::type::iterator itdm=any_detected_markers.first;
						itdm!=any_detected_markers.second; itdm++)
						detected_markers.push_back(m_marker_states.project<marker_state::order_tag>(itdm));
					
					track_markers(detected_markers);

					m_integral_views.pop_back();
				}
			}
			else
			{
				detect_markers();
				if(m_integral_views.full())
					m_integral_views.pop_back();
			}
		}
	}

	void tracker::on_stop()
	{
		m_start_stop(shared_from_this(), false);
	}

	void tracker::run()
	{
		m_is_started=true;
		on_start();
		m_ios.run();
		m_is_started=false;
		on_stop();
	}


	void tracker::describe_marker(marker_state &ms)
	{
		if(ms.m_marker && !ms.m_marker->get_size().empty())
		{
			ucv::surf marker_surf(
				ms.m_marker->get_size(),
				3, //m_surf.octaves(),
				4, //m_surf.intervals(),
				2, //m_surf.sample_step(),
				1.0e-3f//m_surf.treshold()
			);

			ucv::surf::integral_image_t marker_integral_img(
				ms.m_marker->get_size().width(),
				ms.m_marker->get_size().height(),
				4
			);

			ucv::integral(
				ucv::gil::const_view(ms.m_marker->get_image()),
				ucv::gil::view(marker_integral_img),
				ms.m_marker->get_median()
			);

			if(marker_surf.set_integral_view(ucv::gil::const_view(marker_integral_img)))
			{
				marker_surf.build_response_layers();
				marker_surf.detect(ms.m_features);
				marker_surf.describe(ms.m_features);
			}
		}
	}


	bool tracker::detect_marker(marker_state &ms, feature_points_t const &ffs)
	{
		std::vector<
			std::pair<
				std::vector< ucv::surf::feature_point_t >::const_iterator,
				std::vector< ucv::surf::feature_point_t >::const_iterator
			>
		> marker_matches;

		ucv::match_feature_points<
			ucv::surf::feature_point_t,
			std::vector<ucv::surf::feature_point_t>,
			std::vector<ucv::surf::feature_point_t>
		>(ms.m_features, ffs, marker_matches);

		m_stats(shared_from_this(), marker_matches.size());

		if(marker_matches.size()>=m_min_marker_features)
		{
			if(	ucv::find_homography_ransac(
					marker_matches,
					ms.m_hmatrix
				)
			)
			{
				feature_point_t::desc_value_type const select_scale=m_select_fp_scale;
				feature_point_t::value_type const inv_select_fp_min_distance=1.0f/sqrt(m_surf.size().area()*m_select_fp_min_area);
	
				
				feature_point_t::value_type const marker_left=ucv::detail::constant::zero<feature_point_t::value_type>();
				feature_point_t::value_type const marker_right=ms.get_marker()->get_size().width();
				feature_point_t::value_type const marker_top=ucv::detail::constant::zero<feature_point_t::value_type>();
				feature_point_t::value_type const marker_bottom=ms.get_marker()->get_size().height();
				
				std::vector<
					std::pair<
						feature_point_t::desc_value_type,
						std::size_t
					>
				> marker_inliers;

				marker_state::points2_t marker_points(ffs.size());

				ucv::matrix33f inv_hm(ms.m_hmatrix.inverse());
				
				for(std::size_t f=0;f<ffs.size();++f)
				{
					marker_points[f]=inv_hm*static_cast<feature_point_t::point2_t const &>(ffs[f]);
					if(	marker_points[f].x>marker_left && marker_points[f].x<marker_right &&
						marker_points[f].y>marker_top && marker_points[f].y<marker_bottom
					)
					{
						boost::uint32_t fp_g_x=static_cast<boost::uint32_t>(ffs[f].x*inv_select_fp_min_distance);
						boost::uint32_t fp_g_y=static_cast<boost::uint32_t>(ffs[f].y*inv_select_fp_min_distance);
						feature_point_t::desc_value_type fp_sel_scale=fabs(ffs[f].m_scale-select_scale);

						std::size_t si;
						for(si=0;si<marker_inliers.size();++si)
						{
							boost::uint32_t si_g_x=static_cast<boost::uint32_t>(ffs[marker_inliers[si].second].x*inv_select_fp_min_distance);
							boost::uint32_t si_g_y=static_cast<boost::uint32_t>(ffs[marker_inliers[si].second].y*inv_select_fp_min_distance);
							
							if(fp_g_x==si_g_x && fp_g_y==si_g_y)
							{
								if(fp_sel_scale<marker_inliers[si].first)
								{
									//replace with better match
									marker_inliers[si].first=fp_sel_scale;
									marker_inliers[si].second=f;
								}
								break;
							}
						}

						if(si<marker_inliers.size())
							continue;
						
						marker_inliers.push_back(
							std::make_pair(
								fabs(ffs[f].m_scale-select_scale),
								f
							)
						);
					}
				}

				std::sort(marker_inliers.begin(), marker_inliers.end(), compare_relative_fp_scales());
				BOOST_ASSERT(marker_inliers.size()>=m_min_marker_features);
				m_stats(shared_from_this(), marker_inliers.size());
				if(marker_inliers.size()>=m_min_marker_features)
				{
					std::size_t select_n_inliers=std::min(marker_inliers.size(), m_max_marker_features);
				
					ms.m_marker_points.resize(select_n_inliers);
					ms.m_frame_points.resize(select_n_inliers);
					
					for(std::size_t il=0;il<select_n_inliers;++il)
					{
						ms.m_marker_points[il]=marker_points[marker_inliers[il].second];
						ms.m_frame_points[il]=static_cast<feature_point_t::point2_t const &>(ffs[marker_inliers[il].second]);
					}
					return true;
				}
			}
		}
		return false;
	}

	void tracker::detect_markers()
	{
		if(m_surf.set_integral_view(m_integral_views.front()))
		{
			feature_points_t frame_features;
			m_surf.build_response_layers();
			m_surf.detect(frame_features);
			m_surf.describe(frame_features);
			
			m_stats(shared_from_this(), frame_features.size());

			if(frame_features.size()>=m_min_marker_features)
			{
				marker_states_t::index<marker_state::detected_tag>::type &marker_states_by_detected=
					m_marker_states.get<marker_state::detected_tag>();
			
				for(marker_states_t::iterator itms=m_marker_states.begin();itms!=m_marker_states.end();++itms)
				{
					boost::shared_ptr<marker_state> pms=*itms;
					BOOST_ASSERT(pms);
					
					if(detect_marker(*pms, frame_features))
					{
						marker_states_by_detected.modify(
							m_marker_states.project<marker_state::detected_tag>(itms),
							boost::bind(
								&marker_state::set_detected,
								_1,
								true
							)
						);
						m_marker_state_changed(pms, marker_state::SC_DETECTION);
					}
					else
						m_marker_state_changed(pms, marker_state::SC_DETECT_NOTIFY);
				}
			}
		}
	}


	bool tracker::track_marker(marker_state &dms)
	{
		marker_state::points2_t curr_pts, marker_pts(dms.m_marker_points);
		std::vector<bool> status;
		m_klt_tracker(dms.m_frame_points, curr_pts, status);
		BOOST_ASSERT(dms.m_frame_points.size()==curr_pts.size() && dms.m_frame_points.size()==status.size());

		std::vector<
			std::pair<
				marker_state::points2_t::const_iterator,
				marker_state::points2_t::const_iterator
			>
		> matches;

		for(std::size_t p=0;p<dms.m_frame_points.size();++p)
		{
			if(!status[p])
				continue;

			matches.push_back(
				std::make_pair(
					marker_pts.begin()+p,
					curr_pts.begin()+p
				)
			);
		}

		if(matches.size()>=m_min_marker_features)
		{
			if(	ucv::find_homography_ransac(
					matches,
					dms.m_hmatrix
				)
			)
			{
				dms.m_marker_points.resize(matches.size());
				dms.m_frame_points.resize(matches.size());
				for(std::size_t m=0;m<matches.size();++m)
				{
					dms.m_marker_points[m]=*matches[m].first;
					dms.m_frame_points[m]=*matches[m].second;
				}
				return true;
			}
		}
		return false;
	}

	void tracker::track_markers(std::vector<marker_states_t::iterator> const &dms)
	{
		marker_states_t::index<marker_state::detected_tag>::type &marker_states_by_detected=
			m_marker_states.get<marker_state::detected_tag>();

		for(std::size_t m=0;m<dms.size();++m)
		{
			boost::shared_ptr<marker_state> const &pdms=*dms[m];

			if(track_marker(*pdms))
				m_marker_state_changed(pdms, marker_state::SC_POSE);
			else
			{
				marker_states_by_detected.modify(
					m_marker_states.project<marker_state::detected_tag>(dms[m]),
						boost::bind(
						&marker_state::set_detected,
						_1,
						false
					)
				);
				m_marker_state_changed(pdms, marker_state::SC_DETECTION);
			}
		}
	}

} //namespace ar
} //namespace baldzarika
