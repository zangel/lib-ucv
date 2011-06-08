#ifndef BALDZARIKA_AR_TRACKER_H
#define BALDZARIKA_AR_TRACKER_H

#include <baldzarika/ar/common_typedefs.h>

namespace baldzarika { namespace ar {


	class tracker
		: public boost::enable_shared_from_this<tracker>
		, private boost::noncopyable
	{
	public:

		static float const DEFAULT_CAMERA_FOVY;
		static float const DEFAULT_Z_NEAR;
		static float const DEFAULT_Z_FAR;

		typedef enum
		{
			RS_STOPPED=0,
			RS_STARTING,
			RS_STARTED,
			RS_STOPPING
		} eRunningState;

		typedef boost::signals2::signal<void (boost::shared_ptr<tracker> const &, eRunningState)> running_state_signal_t;

		class marker_state
			: public boost::enable_shared_from_this<marker_state>
			, private boost::noncopyable
		{
		public:
			friend class tracker;
			
			typedef enum
			{
				SC_DETECTION=0,
				SC_POSE,
				SC_DETECT_NOTIFY
			} eSC;

			typedef boost::signals2::signal<void (boost::shared_ptr<marker_state> const &, eSC)> changed_signal_t;

		protected:
			marker_state();
			marker_state(bool det);

		public:
			virtual ~marker_state();
						
			virtual boost::shared_ptr<tracker>	get_tracker() const=0;
			virtual math::size2ui				get_marker_size() const=0;
			
			bool								is_detected() const;
			math::matrix33f const&				get_homography() const;
			math::matrix44f const&				get_camera_pose() const;
			
		protected:
			virtual void						set_detected(bool d);
			virtual void						set_homography(math::matrix33f const &hm);
					
		private:
			bool								m_is_detected;
			math::matrix33f						m_homography;
			math::matrix44f						m_camera_pose;
		
		public:
			mutable boost::any					m_any_data;
		};

		tracker(math::size2ui const &fs);
		virtual ~tracker();


		math::size2ui const&					get_frame_size() const;
		bool									set_frame_size(math::size2ui const &fs);

		float									get_camera_fovy() const;
		bool									set_camera_fovy(float fovy);

		float									get_camera_focal_length() const;
		math::matrix44f const&					get_camera_projection() const;

		running_state_signal_t&					running_state_changed() const;
		marker_state::changed_signal_t&			marker_state_changed() const;
		
		bool									start();
		bool									is_active() const;
		bool									is_starting() const;
		bool									is_started() const;
		bool									stop();
		bool									wait_to_stop();

		bool									process_frame(ucv::gil::gray8c_view_t gv);


	protected:
		

		virtual void							on_start()=0;
		virtual void							on_update()=0;
		virtual void							on_stop()=0;

		virtual bool							on_set_frame_size(math::size2ui const &fs)=0;
		virtual bool							on_process_frame(ucv::gil::gray8c_view_t gv)=0;

	private:
		void									update_camera_projection();
		void									run();
		void									update();
		

	protected:
		math::size2ui							m_frame_size;
		float									m_camera_fovy;
		float									m_z_near;
		float									m_z_far;
		float									m_camera_focal_length;
		math::matrix44f							m_camera_projection;
		
		mutable boost::asio::io_service			m_ios;
		boost::asio::io_service::work			m_ios_work;
		boost::thread							m_worker;
		eRunningState							m_running_state;
		
		boost::mutex							m_update_sync;

		mutable running_state_signal_t			m_running_state_singal;
		mutable marker_state::changed_signal_t	m_marker_state_changed_signal;

	public:
		mutable boost::any						m_any_data;
	};

} //namespace ar
} //namesapce baldzarika

#endif //BALDZARIKA_AR_TRACKER_H