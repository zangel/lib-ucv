#ifndef BALDZARIKA_AR_FIDUCIAL_TRACKER_H
#define BALDZARIKA_AR_FIDUCIAL_TRACKER_H

#include <baldzarika/ar/tracker.h>
#include <baldzarika/ar/fiducial/typedefs.h>
#include <baldzarika/ar/fiducial/marker_model.h>

namespace baldzarika { namespace ar { namespace fiducial {

	class model;

	class tracker
		: public ar::tracker
	{
	
	protected:
		class marker_model_holder;

	public:

		static float const DEFAULT_EDGE_DETECTION_THRESHOLD;
		static float const DEFAULT_POLYGON_APPROXIMATION_EPS;
		static float const DEFAULT_SAME_MARKER_MAX_AREA;
		static boost::uint32_t const DEFAULT_KEEP_DETECTED_FRAME_COUNT;

		class marker_state
			: public ar::tracker::marker_state
		{
		public:
			friend class tracker;
			friend class marker_model_holder;

			struct order_tag {};
			struct identity_tag {};
			struct marker_id_tag {};
			struct detected_tag {};

			marker_state(boost::shared_ptr<marker_model_holder> const &mmh, marker_id_t mid, bool det=false);
			virtual ~marker_state();
						
			boost::shared_ptr<ar::tracker>			get_tracker() const;
			math::size2ui							get_marker_size() const;

			boost::shared_ptr<marker_model>			get_marker_model() const;
			marker_id_t								get_marker_id() const;
			
		protected:
			void									set_detected(bool d);
			void									set_homography(math::matrix33f const &hm);
					
		private:
			boost::weak_ptr<marker_model_holder>	m_marker_model_holder;
			marker_id_t								m_marker_id;
			boost::uint32_t							m_undetected_frame_count;
		};
	
	protected:
		
		class marker_model_holder
		{
		public:
			friend class tracker;
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
						BOOST_MULTI_INDEX_CONST_MEM_FUN(ar::tracker::marker_state, bool, is_detected)
					>
				>
			> marker_states_t;

			struct order_tag {};
			struct marker_model_tag {};

			marker_model_holder(boost::shared_ptr<tracker> const &d, boost::shared_ptr<marker_model> const &mm);
			virtual ~marker_model_holder();

			boost::shared_ptr<tracker>				get_tracker() const;
			boost::shared_ptr<marker_model> const&	get_marker_model() const;
		
		private:
			boost::weak_ptr<tracker>		m_tracker;
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


		//typedef ucv::gaussian_blur<ar::fiducial::gray_t, 3> gaussian_blur_t;
		typedef ucv::canny<gray_t, 3> canny_t;
		
	
	public:
		tracker(math::size2ui const &fs);
		~tracker();

		bool									add_marker_model(boost::shared_ptr<marker_model> const &mm);
		bool									remove_marker_model(boost::shared_ptr<marker_model> const &mm);

	protected:
		void									on_start();
		void									on_update();
		void									on_stop();

		bool									on_set_frame_size(math::size2ui const &fs);
		bool									on_process_frame(ucv::gil::gray8c_view_t gv);

		void									detect_markers(boost::shared_ptr<marker_model_holder> const &mmh, std::list<contour_t> const &contours);
						
	private:
		float									m_polygon_approximation_eps;
		float									m_same_marker_max_area;
		boost::uint32_t							m_keep_detected_frame_count;

		bool									m_frame_is_dirty;
		
		gray_image_t							m_frame;
		//gray_image_t							m_blurred_frame;
		//gaussian_blur_t						m_gaussian_blur;
		canny_t									m_canny;

		marker_model_holders_t					m_marker_model_holders;

	};

} //namespace fiducial
} //namespace ar
} //namespace baldzarika

#endif //BALDZARIKA_AR_FIDUCIAL_TRACKER_H