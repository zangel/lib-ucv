#include <baldzarika/ar/config.h>
#include <baldzarika/ar/fiducial/tracker.h>

namespace baldzarika { namespace ar { namespace fiducial {

	float const tracker::DEFAULT_EDGE_DETECTION_THRESHOLD=0.4f;
	float const tracker::DEFAULT_POLYGON_APPROXIMATION_EPS=3.0f;
	float const tracker::DEFAULT_SAME_MARKER_MAX_AREA=1.0e-2f;
	boost::uint32_t const tracker::DEFAULT_KEEP_DETECTED_FRAME_COUNT=10;

	tracker::marker_state::marker_state(boost::shared_ptr<marker_model_holder> const &mmh, marker_id_t mid, bool det)
		: ar::tracker::marker_state(det)
		, m_marker_model_holder(mmh)
		, m_marker_id(mid)
		, m_undetected_frame_count(0)
	{

	}

	tracker::marker_state::~marker_state()
	{
	}

	boost::shared_ptr<ar::tracker> tracker::marker_state::get_tracker() const
	{
		if(boost::shared_ptr<marker_model_holder> mmh=m_marker_model_holder.lock())
			return mmh->get_tracker();
		return boost::shared_ptr<tracker>();
	}

	math::size2ui tracker::marker_state::get_marker_size() const
	{
		boost::shared_ptr<marker_model_holder> mmh=m_marker_model_holder.lock();
		BOOST_ASSERT(mmh);
		BOOST_ASSERT(mmh->m_marker_model);
		return mmh->m_marker_model->get_marker_size(m_marker_id);
	}

	boost::shared_ptr<marker_model> tracker::marker_state::get_marker_model() const
	{
		if(boost::shared_ptr<marker_model_holder> mmh=m_marker_model_holder.lock())
			return mmh->get_marker_model();
		return boost::shared_ptr<marker_model>();
	}

	marker_id_t tracker::marker_state::get_marker_id() const
	{
		return m_marker_id;
	}

	void tracker::marker_state::set_detected(bool d)
	{
		ar::tracker::marker_state::set_detected(d);		
	}

	void tracker::marker_state::set_homography(math::matrix33f const &hm)
	{
		ar::tracker::marker_state::set_homography(hm);
	}

	tracker::marker_model_holder::marker_model_holder(boost::shared_ptr<tracker> const &t, boost::shared_ptr<marker_model> const &mm)
		: m_tracker(t)
		, m_marker_model(mm)
	{
	}

	tracker::marker_model_holder::~marker_model_holder()
	{
	}

	boost::shared_ptr<tracker> tracker::marker_model_holder::get_tracker() const
	{
		return m_tracker.lock();
	}

	boost::shared_ptr<marker_model> const& tracker::marker_model_holder::get_marker_model() const
	{
		return m_marker_model;
	}

	tracker::tracker(math::size2ui const &fs)
		: ar::tracker(fs)
		, m_polygon_approximation_eps(DEFAULT_POLYGON_APPROXIMATION_EPS)
		, m_same_marker_max_area(DEFAULT_SAME_MARKER_MAX_AREA)
		, m_keep_detected_frame_count(DEFAULT_KEEP_DETECTED_FRAME_COUNT)
		, m_frame_is_dirty(false)
		, m_frame(fs.width(), fs.height())
		//, m_blurred_frame(fs.width(), fs.height())
		//, m_gaussian_blur(fs)
		, m_canny(fs,DEFAULT_EDGE_DETECTION_THRESHOLD, DEFAULT_EDGE_DETECTION_THRESHOLD*3.0f)
	{
	}

	tracker::~tracker()
	{
	}

	bool tracker::add_marker_model(boost::shared_ptr<marker_model> const &mm)
	{
		if(!mm) return false;
		if(m_running_state)
			return false;
		boost::shared_ptr<marker_model_holder> new_holder(
			new marker_model_holder(
				boost::shared_ptr<tracker>(shared_from_this(), boost::detail::static_cast_tag()),
				mm
			)
		);

		if(m_marker_model_holders.push_back(new_holder).second)
			return true;
		return false;
	}

	bool tracker::remove_marker_model(boost::shared_ptr<marker_model> const &mm)
	{
		if(!mm) return false;
		if(m_running_state)
			return false;

		marker_model_holders_t::index<marker_model_holder::marker_model_tag>::type &holders_by_model=
			m_marker_model_holders.get<marker_model_holder::marker_model_tag>();

		marker_model_holders_t::index<marker_model_holder::marker_model_tag>::type::iterator mmhi=
			holders_by_model.find(mm);
				
		if(mmhi==holders_by_model.end()) return false;

		holders_by_model.erase(mmhi);
		return true;
	}

	void tracker::on_start()
	{
	}

	void tracker::on_update()
	{
		if(m_frame_is_dirty)
		{
			std::list< ar::fiducial::contour_t > contours;
			m_canny(ucv::gil::const_view(m_frame), contours);
			
			for(std::list<contour_t>::iterator conti=contours.begin();conti!=contours.end();++conti)
				if(conti->m_is_closed) conti->aproximate(m_polygon_approximation_eps);
			
			for(marker_model_holders_t::iterator mmhi=m_marker_model_holders.begin();mmhi!=m_marker_model_holders.end();++mmhi)
				detect_markers(*mmhi,contours);
			
			m_frame_is_dirty=false;
		}
	}
	
	bool tracker::on_set_frame_size(math::size2ui const &fs)
	{
		m_frame.recreate(fs.width(),fs.height());
		//m_blurred_frame.recreate(fs.width(),fs.height());
		//m_gaussian_blur.set_frame_size(fs);
		m_canny.set_frame_size(fs);
		return true;
	}

	bool tracker::on_process_frame(ucv::gil::gray8c_view_t gv)
	{
		if(!m_frame_is_dirty)
		{
			ucv::convert(
				gv,
				ucv::gil::view(m_frame),
				ucv::detail::grayscale_convert()
			);
			//m_gaussian_blur(ucv::gil::const_view(m_frame), ucv::gil::view(m_blurred_frame));
			m_frame_is_dirty=true;
			return true;
		}
		return false;
	}

	void tracker::on_stop()
	{
		for(marker_model_holders_t::iterator mmhi=m_marker_model_holders.begin();mmhi!=m_marker_model_holders.end();++mmhi)
		{
			boost::shared_ptr<marker_model_holder> const &mmh=*mmhi;
			BOOST_ASSERT(mmh);
			marker_model_holder::marker_states_t::index<marker_state::detected_tag>::type &ms_by_detected=mmh->m_marker_states.get<marker_state::detected_tag>();
			for(marker_model_holder::marker_states_t::iterator msi=mmh->m_marker_states.begin();msi!=mmh->m_marker_states.end();++msi)
			{
				boost::shared_ptr<marker_state> ms=*msi;
				BOOST_ASSERT(ms);
				
				if(!ms->is_detected()) continue;
				
				ms_by_detected.modify(
					mmh->m_marker_states.project<marker_state::detected_tag>(msi),
					boost::bind(
						&marker_state::set_detected,
						_1,
						false
					)
				);
				m_marker_state_changed_signal(ms, marker_state::SC_DETECTION);
			}
			mmh->m_marker_states.clear();
		}
	}

	void tracker::detect_markers(boost::shared_ptr<marker_model_holder> const &mmh, std::list<contour_t> const &contours)
	{
		BOOST_ASSERT(mmh);
		boost::shared_ptr<marker_model> const &mm=mmh->m_marker_model;
		BOOST_ASSERT(mm);
		
		for(marker_model_holder::marker_states_t::iterator msi=mmh->m_marker_states.begin();msi!=mmh->m_marker_states.end();++msi)
		{
			boost::shared_ptr<marker_state> const &ms=*msi;
			if(ms->is_detected())
				ms->m_undetected_frame_count++;
		}

		std::list<marker_model::detect_info> detected_markers;
		mm->detect_markers(ucv::gil::const_view(m_frame),contours,detected_markers);

		marker_model_holder::marker_states_t::index<marker_state::detected_tag>::type &ms_by_detected=mmh->m_marker_states.get<marker_state::detected_tag>();
		marker_model_holder::marker_states_t::index<marker_state::marker_id_tag>::type &ms_by_id=mmh->m_marker_states.get<marker_state::marker_id_tag>();

		for(std::list<marker_model::detect_info>::const_iterator dmi=detected_markers.begin();dmi!=detected_markers.end();++dmi)
		{
			marker_model::detect_info const &dm=*dmi;

			std::pair<
				marker_model_holder::marker_states_t::index<marker_state::marker_id_tag>::type::iterator,
				marker_model_holder::marker_states_t::index<marker_state::marker_id_tag>::type::iterator
			> existing_markers (
				ms_by_id.lower_bound(dm.m_marker_id),
				ms_by_id.upper_bound(dm.m_marker_id)
			);

			if(existing_markers.first==existing_markers.second)
			{
				boost::shared_ptr<marker_state> new_marker_state(
					new marker_state(mmh, dm.m_marker_id, true)
				);

				new_marker_state->set_homography(dm.m_homography);
				
				if(mmh->m_marker_states.push_back(new_marker_state).second)
					m_marker_state_changed_signal(new_marker_state, marker_state::SC_DETECTION);
			}
			else
			{
				float const max_dist=std::sqrt(float(m_frame.width()*m_frame.height())*m_same_marker_max_area);

				marker_model_holder::marker_states_t::index<marker_state::marker_id_tag>::type::iterator smi=
					ms_by_id.end();

				float min_dist=std::numeric_limits<float>::max();
				
				math::point2f dm_pos(dm.m_homography(0,2), dm.m_homography(1,2));
				for(marker_model_holder::marker_states_t::index<marker_state::marker_id_tag>::type::iterator dmci=existing_markers.first;
					dmci!=existing_markers.second;
					++dmci )
				{
					boost::shared_ptr<marker_state> const &cand_ms=*dmci;
					if(!cand_ms->is_detected()) continue;
					
					float dist=math::vector2f(
						math::point2f(cand_ms->get_homography()(0,2), cand_ms->get_homography()(1,2))-
						math::point2f(dm.m_homography(0,2), dm.m_homography(1,2))
					).length();
					if(dist<max_dist && dist<min_dist)
					{
						min_dist=dist;
						smi=dmci;
					}
				}

				if(smi!=ms_by_id.end())
				{
					boost::shared_ptr<marker_state> const &same_marker=*smi;
					same_marker->m_undetected_frame_count=0;
					same_marker->set_homography(dm.m_homography);
					if(same_marker->is_detected())
					{
						m_marker_state_changed_signal(same_marker, marker_state::SC_POSE);
					}
					else
					{
						ms_by_detected.modify(
							mmh->m_marker_states.project<marker_state::detected_tag>(smi),
							boost::bind(
								&marker_state::set_detected,
								_1,
								true
							)
						);
						m_marker_state_changed_signal(same_marker, marker_state::SC_DETECTION);
					}
				}
				else
				{
					boost::shared_ptr<marker_state> new_marker_state;
					for(marker_model_holder::marker_states_t::index<marker_state::marker_id_tag>::type::iterator umsi=existing_markers.first;
						umsi!=existing_markers.second;
						++umsi )
					{
						boost::shared_ptr<marker_state> const &cand_ms=*umsi;
						if(!cand_ms->is_detected())
						{
							new_marker_state=cand_ms;
							new_marker_state->set_homography(dm.m_homography);
							new_marker_state->m_undetected_frame_count=0;
							ms_by_detected.modify(
								mmh->m_marker_states.project<marker_state::detected_tag>(umsi),
								boost::bind(
									&marker_state::set_detected,
									_1,
									true
								)
							);
							break;
						}
					}

					if(!new_marker_state)
					{
						new_marker_state.reset(new marker_state(mmh, dm.m_marker_id, true));
						if(mmh->m_marker_states.push_back(new_marker_state).second)
							new_marker_state->set_homography(dm.m_homography);
						else
							new_marker_state.reset();
					}

					if(new_marker_state)
						m_marker_state_changed_signal(new_marker_state, marker_state::SC_DETECTION);
				}
			}
		}


		for(marker_model_holder::marker_states_t::iterator msi=mmh->m_marker_states.begin();msi!=mmh->m_marker_states.end();++msi)
		{
			boost::shared_ptr<marker_state> const &ms=*msi;
			if(ms->is_detected() && ms->m_undetected_frame_count>=m_keep_detected_frame_count)
			{
				ms_by_detected.modify(
					mmh->m_marker_states.project<marker_state::detected_tag>(msi),
					boost::bind(
						&marker_state::set_detected,
						_1,
						false
					)
				);
				m_marker_state_changed_signal(ms, marker_state::SC_DETECTION);
			}
		}
	}

} //namespace fiducial
} //namespace ar
} //namespace baldzarika
