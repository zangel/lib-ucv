#include <baldzarika/ar/config.h>
#include <baldzarika/ar/tracker.h>
#include <baldzarika/ar/marker.h>
#include <baldzarika/ucv/integral.h>
#include <baldzarika/ucv/match_feature_points.h>
#include <baldzarika/ucv/homography.h>

namespace baldzarika { namespace ar {

	boost::uint32_t const tracker::DEFAULT_SURF_OCTAVES=3;
	boost::uint32_t const tracker::DEFAULT_SURF_INTERVALS=4;
	boost::uint32_t const tracker::DEFAULT_SURF_SAMPLE_STEPS=2;
	float const tracker::DEFAULT_SURF_TRESHOLD=4.0e-4f;

	boost::uint32_t const tracker::DEFAULT_KLT_WIN_SIZE=7;
	boost::uint32_t const tracker::DEFAULT_KLT_LEVELS=4;
	boost::uint32_t const tracker::DEFAULT_KLT_MAX_ITERATIONS=10;
	float const	tracker::DEFAULT_KLT_EPSILON=1.0e-3f;

	boost::uint32_t const tracker::DEFAULT_TRACKER_MIN_MARKER_FEATURES=8;
	boost::uint32_t const tracker::DEFAULT_TRACKER_MAX_MARKER_FEATURES=16;
		
	tracker::marker_state::marker_state(boost::shared_ptr<tracker> const &t, boost::shared_ptr<marker> const &m)
		: m_tracker(t)
		, m_marker(m)
		, m_detected(false)
	{
	}

	tracker::marker_state::~marker_state()
	{
	}

	bool tracker::marker_state::is_detected() const
	{
		return m_detected;
	}

	boost::shared_ptr<marker> const& tracker::marker_state::get_marker() const
	{
		return m_marker;
	}

	tracker::marker_state::points2_t const& tracker::marker_state::get_frame_features() const
	{
		return m_frame_features;
	}

	void tracker::marker_state::collect_features()
	{
		if(boost::shared_ptr<tracker> t=m_tracker.lock())
		{
			if(m_marker && !m_marker->get_size().empty())
			{
				ucv::surf marker_surf(
					m_marker->get_size(),
					t->m_surf.octaves(),
					t->m_surf.intervals(),
					t->m_surf.sample_step(),
					t->m_surf.treshold()
					);

				ucv::surf::integral_image_t marker_integral_img(
					m_marker->get_size().width(),
					m_marker->get_size().height(),
					4
				);
				
				ucv::integral(
					ucv::gil::const_view(m_marker->get_image()),
					ucv::gil::view(marker_integral_img),
					m_marker->get_median()
				);

				if(marker_surf.set_integral_view(ucv::gil::const_view(marker_integral_img)))
				{
					marker_surf.build_response_layers();
					marker_surf.detect(m_features);
					marker_surf.describe(m_features);
				}
			}
		}
	}

	void tracker::marker_state::set_detected(bool d)
	{
		m_detected=d;
		if(boost::shared_ptr<tracker> t=m_tracker.lock())
			t->m_marker_state_changed(shared_from_this(), SC_DETECTION);
	}

	void tracker::marker_state::update_pose()
	{
		if(boost::shared_ptr<tracker> t=m_tracker.lock())
			t->m_marker_state_changed(shared_from_this(), SC_POSE);
	}

	tracker::tracker(ucv::size2ui const &fs)
		: m_min_marker_features(DEFAULT_TRACKER_MIN_MARKER_FEATURES)
		, m_max_marker_features(DEFAULT_TRACKER_MAX_MARKER_FEATURES)
		, m_ios()
		, m_ios_work(m_ios)
		, m_worker()
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
		stop();
	}

	ucv::size2ui tracker::get_frame_size() const
	{
		return m_surf.size();
	}

	bool tracker::set_frame_size(ucv::size2ui const &fs)
	{
		if(is_started())
			return false;
		
		m_surf.resize(fs);
		m_klt_tracker.set_frame_size(fs);
		m_integral_frame_stg[0].recreate(fs.width(), fs.height(), 4);
		m_integral_frame_stg[1].recreate(fs.width(), fs.height(), 4);
		return true;
	}

	boost::shared_ptr<tracker::marker_state> tracker::add_marker(boost::shared_ptr<marker> const &m)
	{
		if(is_started())
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
	
	bool tracker::start()
	{
		if(is_started())
			return true;

		m_integral_frame_seq=0;
		m_integral_views.clear();

		m_worker=boost::thread(boost::bind(&boost::asio::io_service::run, &m_ios)).move();
		if(is_started())
		{
			m_ios.post( boost::bind(&tracker::on_start, this) );
			return true;
		}
		return false;
	}

	bool tracker::is_started() const
	{
		return m_worker.get_id()!=boost::thread::id();
	}

	bool tracker::stop()
	{
		if(!is_started())
			return false;
		m_ios.post( boost::bind(&tracker::on_stop, this) );
		m_ios.stop();
		m_worker.join();
		if(!is_started() && m_ios.stopped())
		{
			m_ios.reset();
			return true;
		}
		return false;
	}

	bool tracker::update(tracker::gray_const_view_t gv, tracker::gray_t const &sm)
	{
		if(!is_started())
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
			ms->collect_features();
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
			}
		}
	}

	void tracker::on_stop()
	{
	}


	void tracker::detect_markers()
	{
		if(m_surf.set_integral_view(m_integral_views.front()))
		{
			feature_points_t frame_features;
			m_surf.build_response_layers();
			m_surf.detect(frame_features);
			m_surf.describe(frame_features);
			
			if(frame_features.size()>=m_min_marker_features)
			{
				marker_states_t::index<marker_state::detected_tag>::type &marker_states_by_detected=
					m_marker_states.get<marker_state::detected_tag>();
			
				for(marker_states_t::iterator itms=m_marker_states.begin();itms!=m_marker_states.end();++itms)
				{
					boost::shared_ptr<marker_state> pms=*itms;

					std::vector<
						std::pair<
							std::vector< ucv::surf::feature_point_t >::const_iterator,
							std::vector< ucv::surf::feature_point_t >::const_iterator
						>
					> marker_matches;

					ucv::match_feature_points<
						ucv::surf::feature_point_t,
						std::vector<ucv::surf::feature_point_t>,
						std::vector< ucv::surf::feature_point_t>
					>(pms->m_features, frame_features, marker_matches);

					if(marker_matches.size()>m_min_marker_features)
					{
						if(	ucv::find_homography_ransac(
								marker_matches,
								pms->m_hmatrix
							)
						)
						{
							std::size_t n_feature_to_track=std::min(marker_matches.size(), m_max_marker_features);

							pms->m_tracking_features.resize(n_feature_to_track);
							pms->m_frame_features.resize(n_feature_to_track);

							
							for(std::size_t m=0;m<n_feature_to_track;++m)
							{
								pms->m_tracking_features[m]=marker_matches[m].first;
								pms->m_frame_features[m]=
									feature_point_t::point2_t(
										marker_matches[m].second->x,
										marker_matches[m].second->y
									);
							}
							
							//mark as detected
							marker_states_by_detected.modify(
								m_marker_states.project<marker_state::detected_tag>(itms),
								boost::bind(
									&marker_state::set_detected,
									_1,
									true
								)
							);
						}
					}
				}
			}
		}
	}

	void tracker::track_markers(std::vector<marker_states_t::iterator> const &dms)
	{
		marker_states_t::index<marker_state::detected_tag>::type &marker_states_by_detected=
			m_marker_states.get<marker_state::detected_tag>();

		for(std::size_t m=0;m<dms.size();++m)
		{
			boost::shared_ptr<marker_state> const &pdms=*dms[m];
			
			std::vector<feature_point_t::point2_t> curr_pts;
			std::vector<bool> status;
			m_klt_tracker(pdms->m_frame_features, curr_pts, status);
			BOOST_ASSERT(pdms->m_frame_features.size()==curr_pts.size() && pdms->m_frame_features.size()==status.size());

			marker_state::feature_to_point2_matches_t f2p2_matches;
			for(std::size_t p=0;p<pdms->m_frame_features.size();++p)
			{
				if(!status[p])
					continue;
				f2p2_matches.push_back(
					marker_state::feature_to_point2_match_t(
						pdms->m_tracking_features[p],
						curr_pts.begin()+p
					)
				);
			}
			
			if(f2p2_matches.size()<m_min_marker_features)
			{
				marker_states_by_detected.modify(
					m_marker_states.project<marker_state::detected_tag>(dms[m]),
					boost::bind(
						&marker_state::set_detected,
						_1,
						false
					)
				);
			}
			else
			{
				if(	ucv::find_homography_ransac(
						f2p2_matches,
						pdms->m_hmatrix
					)
				)
				{
					pdms->m_frame_features.clear();
					pdms->m_tracking_features.clear();

					for(std::size_t m=0;m<f2p2_matches.size();++m)
					{
						pdms->m_tracking_features.push_back(f2p2_matches[m].first);
						pdms->m_frame_features.push_back(*f2p2_matches[m].second);
					}
					pdms->update_pose();
				}
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
				}
			}
		}
	}

} //namespace ar
} //namespace baldzarika
