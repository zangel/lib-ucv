#ifndef BALDZARIKA_AR_FIDUCIAL_DETECTOR_H
#define BALDZARIKA_AR_FIDUCIAL_DETECTOR_H

#include <baldzarika/ar/fiducial/typedefs.h>
#include <baldzarika/ar/fiducial/marker_model.h>

namespace baldzarika { namespace ar { namespace fiducial {

	class model;

	class detector
		: public boost::enable_shared_from_this<detector>
		, private boost::noncopyable
	{
	
	protected:
		class marker_model_holder;

	public:

		static float const DEFAULT_EDGE_DETECTION_THRESHOLD;
		static float const DEFAULT_POLYGON_APPROXIMATION_EPS;
		static float const DEFAULT_SAME_MARKER_MAX_AREA;
		static float const DEFAULT_CAMERA_FOVY;
		static boost::uint32_t const DEFAULT_KEEP_DETECTED_FRAME_COUNT;

		typedef enum
		{
			RS_STOPPED=0,
			RS_STARTING,
			RS_STARTED,
			RS_STOPPING
		} eRunningState;

		typedef boost::signals2::signal<void (boost::shared_ptr<detector> const &, eRunningState)> running_state_signal_t;
		
		class marker_state
			: public boost::enable_shared_from_this<marker_state>
			, private boost::noncopyable
		{
		public:
			friend class detector;
			friend class marker_model_holder;

			typedef enum
			{
				SC_DETECTION=0,
				SC_POSE,
				SC_DETECT_NOTIFY
			} eSC;

			typedef boost::signals2::signal<void (boost::shared_ptr<marker_state> const &, eSC)> changed_signal_t;

			struct order_tag {};
			struct identity_tag {};
			struct marker_id_tag {};
			struct detected_tag {};


			marker_state(boost::shared_ptr<marker_model_holder> const &mmh, marker_id_t mid, bool det=false);
			virtual ~marker_state();
						
			boost::shared_ptr<detector>				get_detector() const;
			boost::shared_ptr<marker_model>			get_marker_model() const;
			marker_id_t								get_marker_id() const;
			ucv::size2ui							get_marker_size() const;
			bool									is_detected() const;
			ucv::matrix33f const&					get_homography() const;
			ucv::matrix44f const&					get_camera_pose() const;
			
		protected:
			void									set_detected(bool d);
			void									set_homography(ucv::matrix33f const &hm);
					
		private:
			boost::weak_ptr<marker_model_holder>	m_marker_model_holder;
			marker_id_t								m_marker_id;
			
			bool									m_is_detected;
			boost::uint32_t							m_undetected_frame_count;
			ucv::matrix33f							m_homography;
			ucv::matrix44f							m_camera_pose;

		public:
			mutable boost::any						m_any_data;
		};


		class locked_frame
		{
			friend class detector;
		private:
			locked_frame();
			locked_frame(boost::shared_ptr<detector> const &d);
		
		public:
			locked_frame(locked_frame const &that);
			~locked_frame();
			
											operator bool() const;
			gray_view_t						get_view();
		
		private:
			boost::weak_ptr<detector>				m_detector;
			mutable boost::mutex::scoped_try_lock	m_lock;
			bool									m_is_dirty;
		};

	protected:
		
		class marker_model_holder
		{
		public:
			friend class detector;
			friend class marker_state;
			
			typedef boost::multi_index_container
			<
				boost::shared_ptr<marker_state>,
				boost::multi_index::indexed_by
				<
					boost::multi_index::random_access
					<
						boost::multi_index::tag<marker_state::order_tag>
					>,
					boost::multi_index::hashed_unique
					<
						boost::multi_index::tag<marker_state::identity_tag>,
						boost::multi_index::identity< boost::shared_ptr<marker_state> >
					>,
					boost::multi_index::ordered_non_unique
					<
						boost::multi_index::tag<marker_state::marker_id_tag>,
						BOOST_MULTI_INDEX_CONST_MEM_FUN(marker_state, marker_id_t, get_marker_id)
					>,
					boost::multi_index::ordered_non_unique
					<
						boost::multi_index::tag<marker_state::detected_tag>,
						BOOST_MULTI_INDEX_CONST_MEM_FUN(marker_state, bool, is_detected)
					>
				>
			> marker_states_t;

			struct order_tag {};
			struct marker_model_tag {};

			marker_model_holder(boost::shared_ptr<detector> const &d, boost::shared_ptr<marker_model> const &mm);
			virtual ~marker_model_holder();

			boost::shared_ptr<detector>				get_detector() const;
			boost::shared_ptr<marker_model> const&	get_marker_model() const;
		
		private:
			boost::weak_ptr<detector>		m_detector;
			boost::shared_ptr<marker_model>	m_marker_model;
			marker_states_t					m_marker_states;
		};

		typedef boost::multi_index_container
		<
			boost::shared_ptr<marker_model_holder>,
			boost::multi_index::indexed_by
			<
				boost::multi_index::random_access
				<
					boost::multi_index::tag<marker_model_holder::order_tag>
				>,
				boost::multi_index::hashed_unique
				<
					boost::multi_index::tag<marker_model_holder::marker_model_tag>,
					BOOST_MULTI_INDEX_CONST_MEM_FUN(marker_model_holder, boost::shared_ptr<marker_model> const&, get_marker_model)
				>
			>
		> marker_model_holders_t;


		typedef ucv::gaussian_blur<ar::fiducial::gray_t, 3> gaussian_blur_t;
		typedef ucv::canny<ar::fiducial::gray_t, 3> canny_t;
		
	
	public:
		detector(ucv::size2ui const &fs);
		~detector();

		ucv::size2ui							get_frame_size() const;
		bool									set_frame_size(ucv::size2ui const &fs);

		float									get_camera_fovy() const;
		bool									set_camera_fovy(float fovy);

		float									get_camera_focal_length() const;
		ucv::matrix44f const&					get_camera_projection() const;
		
		bool									add_marker_model(boost::shared_ptr<marker_model> const &mm);
		bool									remove_marker_model(boost::shared_ptr<marker_model> const &mm);

		running_state_signal_t&					running_state_changed() const;
		marker_state::changed_signal_t&			marker_state_changed() const;
		
		bool									start();
		bool									is_active() const;
		bool									is_starting() const;
		bool									is_started() const;
		bool									stop();
		bool									wait_to_stop();

		bool									update(gray_const_view_t gv);
		locked_frame							lock_frame();


	protected:
		void									on_start();
		void									on_update();
		void									on_stop();
		void									detect_markers(boost::shared_ptr<marker_model_holder> const &mmh, std::list<contour_t> const &contours);

		void									update_camera_projection();
						
	private:
		void									run();

	private:
		float									m_polygon_approximation_eps;
		float									m_same_marker_max_area;
		boost::uint32_t							m_keep_detected_frame_count;

		float									m_camera_fovy;
		float									m_camera_focal_length;
		ucv::matrix44f							m_camera_projection;
		
		mutable boost::asio::io_service			m_ios;
		boost::asio::io_service::work			m_ios_work;
		boost::thread							m_worker;
		eRunningState							m_running_state;
		
		boost::mutex							m_update_sync;
		bool									m_frame_is_dirty;
		
		gray_image_t							m_frame;
		gray_image_t							m_blurred_frame;
		gaussian_blur_t							m_gaussian_blur;
		canny_t									m_canny;
												
		marker_model_holders_t					m_marker_model_holders;

		mutable running_state_signal_t			m_running_state_singal;
		mutable marker_state::changed_signal_t	m_marker_state_changed_signal;

	public:
		mutable boost::any						m_any_data;
	};

} //namespace fiducial
} //namespace ar
} //namespace baldzarika

#endif //BALDZARIKA_AR_FIDUCIAL_DETECTOR_H
