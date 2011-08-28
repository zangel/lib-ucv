#include <baldzarika/ar/config.h>
#include <baldzarika/ar/markerless/tracker.h>
#include <baldzarika/ar/markerless/marker.h>

namespace baldzarika { namespace ar { namespace markerless {

	boost::uint32_t const tracker::DEFAULT_SURF_OCTAVES=2;
	boost::uint32_t const tracker::DEFAULT_SURF_INTERVALS=2;
	boost::uint32_t const tracker::DEFAULT_SURF_SAMPLE_STEPS=2;
	float const tracker::DEFAULT_SURF_TRESHOLD=1.0e-2f;
	float const tracker::DEFAULT_SURF_MATCH_DIST=0.6f;

	boost::uint32_t const tracker::DEFAULT_KLT_HALF_WIN_SIZE=5;
	boost::uint32_t const tracker::DEFAULT_KLT_LEVELS=4;
	boost::uint32_t const tracker::DEFAULT_KLT_MAX_ITERATIONS=2;
	float const	tracker::DEFAULT_KLT_EPSILON=1.0e-4f;

	boost::uint32_t const tracker::DEFAULT_TRACKER_MIN_MARKER_FEATURES=8;
	boost::uint32_t const tracker::DEFAULT_TRACKER_MAX_MARKER_FEATURES=24;
	float const	tracker::DEFAULT_TRACKER_SELECT_FP_SCALE=1.0f;
	float const	tracker::DEFAULT_TRACKER_SELECT_FP_MIN_AREA=1.3e-3f;
	float const	tracker::DEFAULT_DETECTION_MAX_DIFF_NORM=6.0e-3f;
	float const	tracker::DEFAULT_TRACKING_MAX_DIFF_NORM=1.0e-4f;

	namespace {

		struct compare_relative_fp_scales
		{
			typedef std::pair< tracker::feature_point_t::value_type, std::size_t > compare_t;
			
			bool operator()(compare_t const &a, compare_t const &b) const
			{
				return a.first<b.first;
			}
		};

		void add_feature_point(std::vector<tracker::feature_point_t> &fps, tracker::feature_point_t::base_type const &p, boost::int32_t s, bool lap)
		{
			fps.push_back(
				tracker::feature_point_t(
					p,
					s,
					lap
				)
			);
		}

	} //namespace anonymous

		
	tracker::marker_state::marker_state(boost::shared_ptr<tracker> const &t, boost::shared_ptr<marker> const &m)
		: m_tracker(t)
		, m_marker(m)
	{
	}

	tracker::marker_state::~marker_state()
	{
	}

	boost::shared_ptr<ar::tracker> tracker::marker_state::get_tracker() const
	{
		return m_tracker.lock();
	}

	math::size2ui tracker::marker_state::get_marker_size() const
	{
		return m_marker->get_size();
	}

	boost::shared_ptr<marker> const& tracker::marker_state::get_marker() const
	{
		return m_marker;
	}

	void tracker::marker_state::set_detected(bool d)
	{
		ar::tracker::marker_state::set_detected(d);
	}

	void tracker::marker_state::set_homography(math::matrix33f const &hm)
	{
		ar::tracker::marker_state::set_homography(hm);
	}

	tracker::tracker(math::size2ui const &fs)
		: ar::tracker(fs)
		, m_detection_threshold(DEFAULT_SURF_TRESHOLD)
		, m_min_marker_features(DEFAULT_TRACKER_MIN_MARKER_FEATURES)
		, m_max_marker_features(DEFAULT_TRACKER_MAX_MARKER_FEATURES)
		, m_select_fp_scale(DEFAULT_TRACKER_SELECT_FP_SCALE)
		, m_select_fp_min_area(DEFAULT_TRACKER_SELECT_FP_MIN_AREA)
		, m_detection_max_diff_norm(DEFAULT_DETECTION_MAX_DIFF_NORM)
		, m_tracking_max_diff_norm(DEFAULT_TRACKING_MAX_DIFF_NORM)
		, m_frame_seq(0)
		, m_gray_integral_buffer(2)
		, m_detector(fs, DEFAULT_SURF_OCTAVES, DEFAULT_SURF_INTERVALS, DEFAULT_SURF_SAMPLE_STEPS)
		, m_orientation_estimator()
		, m_describer()
		, m_klt_tracker(fs,
			math::size2ui(DEFAULT_KLT_HALF_WIN_SIZE,DEFAULT_KLT_HALF_WIN_SIZE),
			DEFAULT_KLT_LEVELS,
			DEFAULT_KLT_MAX_ITERATIONS,
			DEFAULT_KLT_EPSILON
		)
	{
		m_gray_frame_stg[0].recreate(fs.width(), fs.height());
		m_gray_frame_stg[1].recreate(fs.width(), fs.height());
		m_integral_frame_stg[0].recreate(fs.width()+1, fs.height()+1);
		m_integral_frame_stg[1].recreate(fs.width()+1, fs.height()+1);
	}

	tracker::~tracker()
	{
	}

	float tracker::get_detection_treshold() const
	{
		return m_detection_threshold;
	}

	bool tracker::set_detection_treshold(float dt)
	{
		if(m_running_state)
			return false;
		m_detection_threshold=dt;
		return true;
	}

	boost::uint32_t tracker::get_detection_min_features() const
	{
		return m_min_marker_features;
	}

	bool tracker::set_detection_min_features(boost::uint32_t mf)
	{
		if(m_running_state)
			return false;
		m_min_marker_features=mf;
		return true;
	}

	boost::uint32_t tracker::get_tracking_max_features() const
	{
		return m_max_marker_features;
	}

	bool tracker::set_tracking_max_features(boost::uint32_t mf)
	{
		if(m_running_state)
			return false;
		m_max_marker_features=mf;
		return true;
	}

	boost::uint32_t tracker::get_tracking_half_win_size() const
	{
		return m_klt_tracker.get_half_win_size().width();
	}

	bool tracker::set_tracking_half_win_size(boost::uint32_t thws)
	{
		if(m_running_state)
			return false;
		m_klt_tracker.set_half_win_size(math::size2ui(thws,thws));
		return true;
	}

	boost::uint32_t tracker::get_tracking_num_levels() const
	{
		return m_klt_tracker.get_num_levels();
	}

	bool tracker::set_tracking_num_levels(boost::uint32_t nl)
	{
		if(m_running_state)
			return false;
		m_klt_tracker.set_num_levels(nl);
		return true;
	}

	boost::uint32_t tracker::get_tracking_max_iterations() const
	{
		return m_klt_tracker.get_max_iterations();
	}

	bool tracker::set_tracking_max_iterations(boost::uint32_t mi)
	{
		if(m_running_state)
			return false;
		m_klt_tracker.set_max_iterations(mi);
		return true;
	}

	float tracker::get_detection_max_diff_norm() const
	{
		return m_detection_max_diff_norm;
	}

	bool tracker::set_detection_max_diff_norm(float mdn)
	{
		if(m_running_state)
			return false;
		m_detection_max_diff_norm=mdn;
		return true;
	}

	float tracker::get_tracking_max_diff_norm() const
	{
		return m_tracking_max_diff_norm;
	}

	bool tracker::set_tracking_max_diff_norm(float mdn)
	{
		if(m_running_state)
			return false;
		m_tracking_max_diff_norm=mdn;
		return true;
	}

	bool tracker::add_marker(boost::shared_ptr<marker> const &m)
	{
		if(m_running_state)
			return false;
		if(!m_marker_states.push_back(
			boost::shared_ptr<marker_state>(
				new marker_state(
					boost::shared_ptr<tracker>(shared_from_this(), boost::detail::static_cast_tag()),
					m)
				)
			).second ) return false;
		return true;
	}

	bool tracker::remove_marker(boost::shared_ptr<marker> const &m)
	{
		if(!m) return false;
		if(m_running_state)
			return false;

		marker_states_t::index<marker_state::marker_identity_tag>::type &marker_states_by_marker=
			m_marker_states.get<marker_state::marker_identity_tag>();

		marker_states_t::index<marker_state::marker_identity_tag>::type::iterator msi=
			marker_states_by_marker.find(m);
				
		if(msi==marker_states_by_marker.end()) return false;

		marker_states_by_marker.erase(msi);
		return true;
	}

	void tracker::on_start()
	{
		m_frame_seq=0;
		m_gray_integral_buffer.clear();

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
			m_marker_state_changed_signal(ms, marker_state::SC_DETECTION);
			describe_marker(*ms);
		}
	}

	void tracker::on_update()
	{
		if(!m_gray_integral_buffer.empty())
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

			std::vector<marker_states_t::iterator> detected_markers;

			for(marker_states_t::index<marker_state::detected_tag>::type::iterator itdm=any_detected_markers.first;
				itdm!=any_detected_markers.second; itdm++)
				detected_markers.push_back(m_marker_states.project<marker_state::order_tag>(itdm));

			if(detected_markers.empty())
				detect_markers();
			else
				track_markers(detected_markers);
		}
	}

	void tracker::on_stop()
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
			m_marker_state_changed_signal(ms, marker_state::SC_DETECTION);
		}
	}
	
	bool tracker::on_set_frame_size(math::size2ui const &fs)
	{
		m_detector.resize(fs);
		m_klt_tracker.set_frame_size(fs);
		m_gray_frame_stg[0].recreate(fs.width(), fs.height());
		m_gray_frame_stg[1].recreate(fs.width(), fs.height());
		m_integral_frame_stg[0].recreate(fs.width()+1, fs.height()+1);
		m_integral_frame_stg[1].recreate(fs.width()+1, fs.height()+1);
		return true;
	}

	bool tracker::on_process_frame(ucv::gil::gray8c_view_t gv)
	{
		if(!m_gray_integral_buffer.full())
		{
			gray_t median_value;
			ucv::convert(
				gv,
				ucv::gil::view(m_gray_frame_stg[(m_frame_seq & 0x0001)]),
				ucv::detail::grayscale_convert_and_median<gray_t>(
					median_value,
					gv.width(),
					gv.height()
				)
			);

			ucv::integral(
				ucv::gil::const_view(m_gray_frame_stg[(m_frame_seq & 0x0001)]),
				ucv::gil::view(m_integral_frame_stg[(m_frame_seq & 0x0001)]),
				median_value
			);
					
			m_gray_integral_buffer.push_front(
				std::make_pair(
					ucv::gil::const_view(m_gray_frame_stg[(m_frame_seq & 0x0001)]),
					ucv::gil::const_view(m_integral_frame_stg[(m_frame_seq & 0x0001)])
				)
			);
					
			m_frame_seq++;
			return true;
		}
		return false;
	}

	void tracker::describe_marker(marker_state &ms)
	{
		if(ms.m_marker && !ms.m_marker->get_size().empty())
		{
			hessian_detector_t det(ms.m_marker->get_size(), DEFAULT_SURF_OCTAVES, DEFAULT_SURF_INTERVALS, DEFAULT_SURF_SAMPLE_STEPS);
			
			gray_image_t marker_integral_img(
				ms.m_marker->get_size().width()+1,
				ms.m_marker->get_size().height()+1
			);

			ucv::integral(
				ucv::gil::const_view(ms.m_marker->get_image()),
				ucv::gil::view(marker_integral_img),
				ms.m_marker->get_median()
			);

			det.update(ucv::gil::const_view(marker_integral_img));
			det.detect<gray_t>(
				m_detection_threshold,
				boost::bind(&add_feature_point,
					boost::ref(ms.m_features),
					_1,
					_2,
					_3
				)
			);

			orientation_estimator_t().estimate(
				ucv::gil::const_view(marker_integral_img),
				ms.m_features.begin(),
				ms.m_features.end()
			);

			describer_t().describe(
				ucv::gil::const_view(ms.m_marker->get_image()),
				ms.m_features.begin(),
				ms.m_features.end()
			);
		}
	}


	bool tracker::detect_marker(marker_state &ms, feature_points_t const &ffs)
	{
		std::vector<
			std::pair<
				std::vector< feature_point_t >::const_iterator,
				std::vector< feature_point_t >::const_iterator
			>
		> marker_matches;

		ucv::surf::match_feature_points<
			feature_point_t,
			std::vector<feature_point_t>,
			std::vector<feature_point_t>
		>(ms.m_features, ffs, marker_matches, DEFAULT_SURF_MATCH_DIST);

		if(marker_matches.size()>=m_min_marker_features)
		{
			math::matrix33f hm;
#if 1
			ucv::find_homography(marker_matches, ms.get_marker_size(), hm);

#else
			if(	ucv::find_homography_ransac(
					marker_matches,
					hm
				)
			)
#endif
			{

				feature_point_t::value_type const select_scale=m_select_fp_scale;
				feature_point_t::value_type const inv_select_fp_min_distance=1.0f/sqrt(m_detector.size().area()*m_select_fp_min_area);

				feature_point_t::value_type const marker_left=math::constant::zero<feature_point_t::value_type>();
				feature_point_t::value_type const marker_right=feature_point_t::value_type(ms.get_marker()->get_size().width());
				feature_point_t::value_type const marker_top=math::constant::zero<feature_point_t::value_type>();
				feature_point_t::value_type const marker_bottom=feature_point_t::value_type(ms.get_marker()->get_size().height());

				feature_point_t::value_type const inv_marker_width=
					math::constant::one<feature_point_t::value_type>()/
					feature_point_t::value_type(ms.get_marker()->get_size().width());

				feature_point_t::value_type const inv_marker_height=
					math::constant::one<feature_point_t::value_type>()/
					feature_point_t::value_type(ms.get_marker()->get_size().height());
								
				std::vector<
					std::pair<
						feature_point_t::value_type,
						std::size_t
					>
				> marker_inliers;

				marker_state::points2_t marker_points(ffs.size());

				math::matrix33f inv_hm(hm.inverted());

				for(std::size_t f=0;f<ffs.size();++f)
				{
					marker_points[f]=ffs[f].transformed(inv_hm);

					if(	marker_points[f].x()>marker_left && marker_points[f].x()<marker_right &&
						marker_points[f].y()>marker_top && marker_points[f].y()<marker_bottom
					)
					{
						boost::uint32_t fp_g_x=static_cast<boost::uint32_t>(ffs[f].x()*inv_select_fp_min_distance);
						boost::uint32_t fp_g_y=static_cast<boost::uint32_t>(ffs[f].y()*inv_select_fp_min_distance);

						feature_point_t::value_type mp_dist=math::point2<feature_point_t::value_type>(
							(
								feature_point_t::value_type(marker_points[f].x()*inv_marker_width)-
								math::constant::half<feature_point_t::value_type>()
							)*math::constant::two<feature_point_t::value_type>(),
							(
								feature_point_t::value_type(marker_points[f].y()*inv_marker_height)-
								math::constant::half<feature_point_t::value_type>()
							)*math::constant::two<feature_point_t::value_type>()
						).length();
						
						feature_point_t::value_type fp_sel_scale=std::abs(ffs[f].m_scale-select_scale)*(math::constant::one<feature_point_t::value_type>()-mp_dist);

						std::size_t si;
						for(si=0;si<marker_inliers.size();++si)
						{
							boost::uint32_t si_g_x=static_cast<boost::uint32_t>(ffs[marker_inliers[si].second].x()*inv_select_fp_min_distance);
							boost::uint32_t si_g_y=static_cast<boost::uint32_t>(ffs[marker_inliers[si].second].y()*inv_select_fp_min_distance);
							
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
								fp_sel_scale,
								f
							)
						);
					}
				}

				std::sort(marker_inliers.begin(), marker_inliers.end(), compare_relative_fp_scales());
				BOOST_ASSERT(marker_inliers.size()>=m_min_marker_features);
				
				if(marker_inliers.size()>=m_min_marker_features)
				{
					std::size_t select_n_inliers=std::min(marker_inliers.size(), m_max_marker_features);
				
					ms.m_marker_points.resize(select_n_inliers);
					ms.m_frame_points.resize(select_n_inliers);
					
					for(std::size_t il=0;il<select_n_inliers;++il)
					{
						ms.m_marker_points[il]=marker_points[marker_inliers[il].second];
						ms.m_frame_points[il]=static_cast<feature_point_t::base_type const &>(ffs[marker_inliers[il].second]);
					}
					ms.set_homography(hm);
					return true;
				}
			}
		}
		ms.m_marker_points.resize(marker_matches.size());
		ms.m_frame_points.resize(marker_matches.size());
		for(std::size_t m=0;m<marker_matches.size();++m)
		{
			ms.m_marker_points[m]=*marker_matches[m].first;
			ms.m_frame_points[m]=*marker_matches[m].second;
		}
		return false;
	}

	void tracker::detect_markers()
	{
		//discard all but last frame
		if(m_gray_integral_buffer.size()<2)
			return;

		gray_integral_buffer_t::const_iterator curr_frame_it=m_gray_integral_buffer.begin();
		gray_integral_buffer_t::const_iterator prev_frame_it=curr_frame_it;
		prev_frame_it++;
		
		gray_t const detection_max_diff_norm=m_detection_max_diff_norm;
		gray_t frame_diff=ucv::norm_l1<gray_t>(prev_frame_it->second, curr_frame_it->second, 2);

		while(m_gray_integral_buffer.size()>1)
			m_gray_integral_buffer.pop_back();

		if(frame_diff>detection_max_diff_norm)
			return;

		m_detector.update(curr_frame_it->second);
		feature_points_t frame_features;
		m_detector.detect<float>(
			m_detection_threshold,
			boost::bind(&add_feature_point,
				boost::ref(frame_features),
				_1,
				_2,
				_3
			)
		);

		m_orientation_estimator.estimate(
			curr_frame_it->second,
			frame_features.begin(),
			frame_features.end()
		);
		m_describer.describe(
			curr_frame_it->first,
			frame_features.begin(),
			frame_features.end()
		);
			
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
					m_marker_state_changed_signal(pms, marker_state::SC_DETECTION);
				}
				else
					m_marker_state_changed_signal(pms, marker_state::SC_DETECT_NOTIFY);
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
			math::matrix33f hm;
#if 1
			ucv::find_homography(matches,dms.get_marker_size(), hm);
#else
			if(	ucv::find_homography_ransac(
					matches,
					hm
				)
			)
#endif

			{
				dms.m_marker_points.resize(matches.size());
				dms.m_frame_points.resize(matches.size());
				for(std::size_t m=0;m<matches.size();++m)
				{
					dms.m_marker_points[m]=*matches[m].first;
					dms.m_frame_points[m]=*matches[m].second;
				}
				dms.set_homography(hm);
				return true;
			}
		}
		dms.m_marker_points.resize(0);
		dms.m_frame_points.resize(0);
		return false;
	}

	void tracker::track_markers(std::vector<marker_states_t::iterator> const &dms)
	{
		if(m_gray_integral_buffer.size()<2)
			return;

		gray_integral_buffer_t::iterator curr_frame_it=m_gray_integral_buffer.begin();
		gray_integral_buffer_t::iterator prev_frame_it=curr_frame_it;
		prev_frame_it++;

#if 0
		gray_t const tracking_max_diff_norm=m_tracking_max_diff_norm;
		gray_t frame_diff=ucv::norm_l1<gray_t>(*prev_frame_it, *curr_frame_it, 4);

		

		if(frame_diff<tracking_max_diff_norm)
#else		
		if(true)
#endif
		{
			m_klt_tracker.set_integral_views(prev_frame_it->second, curr_frame_it->second);

			marker_states_t::index<marker_state::detected_tag>::type &marker_states_by_detected=
				m_marker_states.get<marker_state::detected_tag>();

			for(std::size_t m=0;m<dms.size();++m)
			{
				boost::shared_ptr<marker_state> const &pdms=*dms[m];

				if(track_marker(*pdms))
					m_marker_state_changed_signal(pdms, marker_state::SC_POSE);
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
					m_marker_state_changed_signal(pdms, marker_state::SC_DETECTION);
				}
			}
		}
		else
		{
			//discard last frame
			m_gray_integral_buffer.pop_front();
		}

		//make room for next frames
		while(m_gray_integral_buffer.size()>1)
			m_gray_integral_buffer.pop_back();
	}

} //namespace markerless
} //namespace ar
} //namespace baldzarika
