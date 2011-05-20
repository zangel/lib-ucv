#include <baldzarika/ar/config.h>
#include <baldzarika/ar/fiducial/detector.h>

namespace baldzarika { namespace ar { namespace fiducial {

	float const detector::DEFAULT_EDGE_DETECTION_THRESHOLD=0.4f;
	float const detector::DEFAULT_POLYGON_APPROXIMATION_EPS=3.0f;
	float const detector::DEFAULT_SAME_MARKER_MAX_AREA=1.0e-3f;
	boost::uint32_t const detector::DEFAULT_KEEP_DETECTED_FRAME_COUNT=10;

	detector::locked_frame::locked_frame()
		: m_is_dirty(false)
	{
	}

	detector::locked_frame::locked_frame(boost::shared_ptr<detector> const &d)
		: m_detector(d)
		, m_lock(d->m_update_sync)
		, m_is_dirty(false)
	{
	}

	detector::locked_frame::locked_frame(locked_frame const &that)
		: m_detector(that.m_detector)
		, m_lock(that.m_lock.move())
		, m_is_dirty(that.m_is_dirty)
	{
	}
	
	detector::locked_frame::~locked_frame()
	{
		if(m_is_dirty)
		{
			if(boost::shared_ptr<detector> d=m_detector.lock())
			{
				if(d->m_running_state!=RS_STOPPED && d->m_running_state<RS_STOPPING)
				{
					BOOST_ASSERT(!d->m_frame_is_dirty);
					d->m_gaussian_blur(ucv::gil::const_view(d->m_frame), ucv::gil::view(d->m_blurred_frame));
					d->m_frame_is_dirty=true;
					d->m_ios.post(boost::bind(&detector::on_update, d.get()));
				}
			}
		}
	}

	detector::locked_frame::operator bool() const
	{
		if(boost::shared_ptr<detector> d=m_detector.lock())
			return m_lock;
		return false;
	}

	gray_view_t detector::locked_frame::get_view()
	{
		if(boost::shared_ptr<detector> d=m_detector.lock())
		{
			if(m_lock)
			{
				m_is_dirty=true;
				return ucv::gil::view(d->m_frame);
			}
		}
		return gray_view_t();
	}

	detector::marker_state::marker_state(boost::shared_ptr<marker_model_holder> const &mmh, marker_id_t mid, bool det)
		: m_marker_model_holder(mmh)
		, m_marker_id(mid)
		, m_is_detected(det)
		, m_undetected_frame_count(0)
	{
	}

	detector::marker_state::~marker_state()
	{
	}

	boost::shared_ptr<detector> detector::marker_state::get_detector() const
	{
		if(boost::shared_ptr<marker_model_holder> mmh=m_marker_model_holder.lock())
			return mmh->get_detector();
		return boost::shared_ptr<detector>();
	}

	boost::shared_ptr<marker_model> detector::marker_state::get_marker_model() const
	{
		if(boost::shared_ptr<marker_model_holder> mmh=m_marker_model_holder.lock())
			return mmh->get_marker_model();
		return boost::shared_ptr<marker_model>();
	}

	marker_id_t detector::marker_state::get_marker_id() const
	{
		return m_marker_id;
	}

	bool detector::marker_state::is_detected() const
	{
		return m_is_detected;
	}

	ucv::matrix33f const& detector::marker_state::get_homography() const
	{
		return m_homography;
	}

	void detector::marker_state::set_detected(bool d)
	{
		m_is_detected=d;
	}

	detector::marker_model_holder::marker_model_holder(boost::shared_ptr<detector> const &d, boost::shared_ptr<marker_model> const &mm)
		: m_detector(d)
		, m_marker_model(mm)
	{
	}

	detector::marker_model_holder::~marker_model_holder()
	{
	}

	boost::shared_ptr<detector> detector::marker_model_holder::get_detector() const
	{
		return m_detector.lock();
	}

	boost::shared_ptr<marker_model> const& detector::marker_model_holder::get_marker_model() const
	{
		return m_marker_model;
	}

	detector::detector(ucv::size2ui const &fs)
		: m_polygon_approximation_eps(DEFAULT_POLYGON_APPROXIMATION_EPS)
		, m_same_marker_max_area(DEFAULT_SAME_MARKER_MAX_AREA)
		, m_keep_detected_frame_count(DEFAULT_KEEP_DETECTED_FRAME_COUNT)
		, m_ios()
		, m_ios_work(m_ios)
		, m_worker()
		, m_running_state(RS_STOPPED)
		, m_frame_is_dirty(false)
		, m_frame(fs.width(), fs.height())
		, m_blurred_frame(fs.width(), fs.height())
		, m_gaussian_blur(fs)
		, m_canny(fs,DEFAULT_EDGE_DETECTION_THRESHOLD, DEFAULT_EDGE_DETECTION_THRESHOLD*3.0f)
	{
	}

	detector::~detector()
	{
		stop();
		wait_to_stop();
	}

	ucv::size2ui detector::get_frame_size() const
	{
		return ucv::size2ui(m_frame.width(),m_frame.height());
	}

	bool detector::set_frame_size(ucv::size2ui const &fs)
	{
		if(m_running_state)
			return false;
		m_frame.recreate(fs.width(),fs.height());
		m_blurred_frame.recreate(fs.width(),fs.height());
		m_gaussian_blur.set_frame_size(fs);
		m_canny.set_frame_size(fs);
		return true;
	}

	bool detector::add_marker_model(boost::shared_ptr<marker_model> const &mm)
	{
		if(!mm) return false;
		if(is_active()) return false;
		boost::shared_ptr<marker_model_holder> new_holder(
			new marker_model_holder(
				shared_from_this(),
				mm
			)
		);

		if(m_marker_model_holders.push_back(new_holder).second)
			return true;
		return false;
	}

	bool detector::remove_marker_model(boost::shared_ptr<marker_model> const &mm)
	{
		if(!mm) return false;
		if(is_active()) return false;

		marker_model_holders_t::index<marker_model_holder::marker_model_tag>::type &holders_by_model=
			m_marker_model_holders.get<marker_model_holder::marker_model_tag>();

		marker_model_holders_t::index<marker_model_holder::marker_model_tag>::type::iterator mmhi=
			holders_by_model.find(mm);
				
		if(mmhi==holders_by_model.end()) return false;

		holders_by_model.erase(mmhi);
		return true;
	}

	detector::running_state_signal_t& detector::running_state_changed() const
	{
		return m_running_state_singal;
	}

	detector::marker_state::changed_signal_t& detector::marker_state_changed() const
	{
		return m_marker_state_changed_signal;
	}

	bool detector::start()
	{
		if(m_running_state==RS_STOPPED || m_running_state==RS_STOPPING)
		{
			m_worker.join();
			m_ios.reset();
		}
		
		if(m_running_state!=RS_STOPPED)
			return false;
		
		m_frame_is_dirty=false;
		m_running_state=RS_STARTING;
		m_worker=boost::thread(boost::bind(&detector::run, this)).move();
		return true;
	}

	bool detector::is_active() const
	{
		return m_running_state>RS_STOPPED;
	}

	bool detector::is_started() const
	{
		return m_running_state==RS_STARTED;
	}

	bool detector::is_starting() const
	{
		return m_running_state==RS_STARTING;
	}

	bool detector::stop()
	{
		if(m_running_state==RS_STOPPED || m_running_state==RS_STOPPING)
			return false;
		while(m_running_state!=RS_STARTED)
			boost::this_thread::yield();
		m_running_state=RS_STOPPING;
		m_ios.stop();
		return true;
	}

	bool detector::wait_to_stop()
	{
		if(m_running_state>RS_STOPPED && m_running_state<RS_STOPPING)
			return false;
		m_worker.join();
		return true;
	}

	bool detector::update(gray_const_view_t gv)
	{
		if(m_running_state==RS_STOPPED || m_running_state>RS_STARTED)
			return false;
		{
			boost::mutex::scoped_try_lock try_lock_update(m_update_sync);
			if(try_lock_update)
			{
				if(!m_frame_is_dirty)
				{
					ucv::copy_pixels(gv,ucv::gil::view(m_frame));
					m_gaussian_blur(ucv::gil::const_view(m_frame), ucv::gil::view(m_blurred_frame));
					m_frame_is_dirty=true;
					m_ios.post(boost::bind(&detector::on_update, this));
				}
			}
		}
		return true;
	}

	detector::locked_frame detector::lock_frame()
	{
		if(m_running_state==RS_STOPPED || m_running_state>RS_STARTED)
			return locked_frame();
		return locked_frame(shared_from_this());
	}
	
	void detector::on_start()
	{
	}

	void detector::on_update()
	{
		boost::mutex::scoped_lock lock_update(m_update_sync);
		if(m_frame_is_dirty)
		{
			std::list< ar::fiducial::contour_t > contours;
			m_canny(ucv::gil::const_view(m_blurred_frame), contours);
			
			for(std::list<contour_t>::iterator conti=contours.begin();conti!=contours.end();++conti)
				if(conti->m_is_closed) conti->aproximate(m_polygon_approximation_eps);
			
			for(marker_model_holders_t::iterator mmhi=m_marker_model_holders.begin();mmhi!=m_marker_model_holders.end();++mmhi)
			{
				detect_markers(*mmhi,contours);
			}
			
			m_frame_is_dirty=false;
		}
	}

	void detector::on_stop()
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

	void detector::detect_markers(boost::shared_ptr<marker_model_holder> const &mmh, std::list<contour_t> const &contours)
	{
		BOOST_ASSERT(mmh);
		boost::shared_ptr<marker_model> const &mm=mmh->m_marker_model;
		BOOST_ASSERT(mm);
		
		for(marker_model_holder::marker_states_t::iterator msi=mmh->m_marker_states.begin();msi!=mmh->m_marker_states.end();++msi)
		{
			boost::shared_ptr<marker_state> const &ms=*msi;
			if(ms->m_is_detected)
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

				new_marker_state->m_homography=dm.m_homography;

				if(mmh->m_marker_states.push_back(new_marker_state).second)
					m_marker_state_changed_signal(new_marker_state, marker_state::SC_DETECTION);
			}
			else
			{
				float const max_dist=std::sqrt(float(m_frame.width()*m_frame.height())*m_same_marker_max_area);

				marker_model_holder::marker_states_t::index<marker_state::marker_id_tag>::type::iterator smi=
					ms_by_id.end();

				float min_dist=std::numeric_limits<float>::max();
				
				ucv::point2f dm_pos(dm.m_homography(0,2), dm.m_homography(1,2));
				for(marker_model_holder::marker_states_t::index<marker_state::marker_id_tag>::type::iterator dmci=existing_markers.first;
					dmci!=existing_markers.second;
					++dmci )
				{
					boost::shared_ptr<marker_state> const &cand_ms=*dmci;
					if(!cand_ms->m_is_detected) continue;
					
					float dist=ucv::vector2f(ucv::point2f(cand_ms->m_homography(0,2), cand_ms->m_homography(1,2))).length();
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
					same_marker->m_homography=dm.m_homography;
					if(same_marker->m_is_detected)
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
						if(!cand_ms->m_is_detected)
						{
							new_marker_state=cand_ms;
							new_marker_state->m_homography=dm.m_homography;
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
							new_marker_state->m_homography=dm.m_homography;
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
			if(ms->m_is_detected && ms->m_undetected_frame_count>=m_keep_detected_frame_count)
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

	void detector::run()
	{
		m_running_state_singal(shared_from_this(), m_running_state);
		on_start();
		m_running_state=RS_STARTED;
		m_running_state_singal(shared_from_this(), m_running_state);
		m_ios.run();
		m_running_state_singal(shared_from_this(), m_running_state);
		on_stop();
		m_running_state=RS_STOPPED;
		m_running_state_singal(shared_from_this(), m_running_state);
	}

} //namespace fiducial
} //namespace ar
} //namespace baldzarika
