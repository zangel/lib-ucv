#include <baldzarika/ar/config.h>
#include <baldzarika/ar/tracker.h>

namespace baldzarika { namespace ar {


	float const tracker::DEFAULT_CAMERA_FOVY=65.0f;
	float const tracker::DEFAULT_Z_NEAR=0.1f;
	float const tracker::DEFAULT_Z_FAR=500.0f;

	tracker::marker_state::marker_state()
		: m_is_detected(false)
		, m_homography(math::matrix33f::identity())
		, m_camera_pose(math::matrix44f::identity())
	{
	}

	tracker::marker_state::marker_state(bool det)
		: m_is_detected(det)
		, m_homography(math::matrix33f::identity())
		, m_camera_pose(math::matrix44f::identity())
	{
	}

	tracker::marker_state::~marker_state()
	{
	}

	bool tracker::marker_state::is_detected() const
	{
		return m_is_detected;
	}

	math::matrix33f const& tracker::marker_state::get_homography() const
	{
		return m_homography;
	}

	math::matrix44f const& tracker::marker_state::get_camera_pose() const
	{
		return m_camera_pose;
	}

	void tracker::marker_state::set_detected(bool d)
	{
		m_is_detected=d;
	}

	void tracker::marker_state::set_homography(math::matrix33f const &hm)
	{
		m_homography=hm;

		boost::shared_ptr<tracker> t=get_tracker();
		BOOST_ASSERT(t);

		math::size2ui fs=t->get_frame_size();
		math::size2ui ms=get_marker_size();
		float focal_length=t->get_camera_focal_length();

		ucv::camera_pose(focal_length, focal_length, math::point2f(float(fs.width())*0.5f,float(fs.height())*0.5f), m_homography, m_camera_pose);

		math::matrix44f cam_adjust=math::matrix44f::identity();

		cam_adjust(0,3)=float(ms.width())*0.5f;

		cam_adjust(1,1)=-1.0f;
		cam_adjust(1,3)=float(ms.height())*0.5f;
		
		m_camera_pose*=cam_adjust;
	}

	tracker::tracker(math::size2ui const &fs)
		: m_frame_size(fs)
		, m_camera_fovy(DEFAULT_CAMERA_FOVY)
		, m_z_near(DEFAULT_Z_NEAR)
		, m_z_far(DEFAULT_Z_FAR)
		, m_ios()
		, m_ios_work(m_ios)
		, m_worker()
		, m_running_state(RS_STOPPED)
	{
		update_camera_projection();
	}


	tracker::~tracker()
	{
		stop();
		wait_to_stop();
	}

	math::size2ui const& tracker::get_frame_size() const
	{
		return m_frame_size;
	}

	bool tracker::set_frame_size(math::size2ui const &fs)
	{
		if(m_running_state)
			return false;
		if(m_frame_size==fs)
			return true;
		if(!on_set_frame_size(fs))
			return false;
		m_frame_size=fs;
		return true;
	}

	float tracker::get_camera_fovy() const
	{
		return m_camera_fovy;
	}

	bool tracker::set_camera_fovy(float fovy)
	{
		if(m_running_state)
			return false;
		m_camera_fovy=fovy;
		update_camera_projection();
		return true;
	}

	float tracker::get_z_near() const
	{
		return m_z_near;
	}

	bool tracker::set_z_near(float zn)
	{
		if(m_running_state)
			return false;
		m_z_near=zn;
		update_camera_projection();
		return true;
	}

	float tracker::get_z_far() const
	{
		return m_z_far;
	}

	bool tracker::set_z_far(float zf)
	{
		if(m_running_state)
			return false;
		m_z_far=zf;
		update_camera_projection();
		return true;
	}
	
	float tracker::get_camera_focal_length() const
	{
		return m_camera_focal_length;
	}

	math::matrix44f const& tracker::get_camera_projection() const
	{
		return m_camera_projection;
	}

	tracker::running_state_signal_t& tracker::running_state_changed() const
	{
		return m_running_state_singal;
	}

	tracker::marker_state::changed_signal_t& tracker::marker_state_changed() const
	{
		return m_marker_state_changed_signal;
	}

	bool tracker::start()
	{
		if(m_running_state==RS_STOPPED || m_running_state==RS_STOPPING)
		{
			m_worker.join();
			m_ios.reset();
		}
		
		if(m_running_state!=RS_STOPPED)
			return false;
		
		m_running_state=RS_STARTING;
		m_worker=boost::thread(boost::bind(&tracker::run, this)).move();
		return true;
	}

	bool tracker::is_active() const
	{
		return m_running_state>RS_STOPPED;
	}

	bool tracker::is_started() const
	{
		return m_running_state==RS_STARTED;
	}

	bool tracker::is_starting() const
	{
		return m_running_state==RS_STARTING;
	}

	bool tracker::stop()
	{
		if(m_running_state==RS_STOPPED || m_running_state==RS_STOPPING)
			return false;
		while(m_running_state!=RS_STARTED)
			boost::this_thread::yield();
		m_running_state=RS_STOPPING;
		m_ios.stop();
		return true;
	}

	bool tracker::wait_to_stop()
	{
		if(m_running_state>RS_STOPPED && m_running_state<RS_STOPPING)
			return false;
		m_worker.join();
		return true;
	}

	bool tracker::process_frame(ucv::gil::gray8c_view_t gv)
	{
		if(m_running_state==RS_STOPPED || m_running_state>RS_STARTED)
			return false;
		{
			boost::mutex::scoped_try_lock try_lock_update(m_update_sync);
			if(try_lock_update)
			{
				if(!on_process_frame(gv))
					return false;
				m_ios.post(boost::bind(&tracker::update, this));
			}
		}
		return true;
	}

	void tracker::update_camera_projection()
	{
		float ty=std::tan(m_camera_fovy*0.5f/360.0f*math::constant::pi<float>());
		m_camera_focal_length=float(m_frame_size.height())/(2.0f*ty);

		m_camera_projection=math::matrix44f::identity();

		m_camera_projection(0,0)=2.0f*m_camera_focal_length/float(m_frame_size.width());
		m_camera_projection(0,2)=2.0f*(float(m_frame_size.width())*0.5f)/float(m_frame_size.width())-1.0f;

		m_camera_projection(1,1)=-(2.0f*m_camera_focal_length/float(m_frame_size.height()));
		m_camera_projection(1,2)=-(2.0f*(float(m_frame_size.height())*0.5f)/float(m_frame_size.height())-1.0f);

		m_camera_projection(2,2)=(m_z_far+m_z_near)/(m_z_far-m_z_near);
		m_camera_projection(2,3)=-2.0f*m_z_far*m_z_near/(m_z_far-m_z_near);
		m_camera_projection(3,2)=1.0f;
		m_camera_projection(3,3)=0.0f;
	}


	void tracker::run()
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

	void tracker::update()
	{
		boost::mutex::scoped_lock lock_update(m_update_sync);
		on_update();
	}

} //namespace ar
} //namespace baldzarika
