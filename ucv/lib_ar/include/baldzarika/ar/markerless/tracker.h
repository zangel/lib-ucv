#ifndef BALDZARIKA_AR_MARKERLESS_TRACKER_H
#define BALDZARIKA_AR_MARKERLESS_TRACKER_H

#include <baldzarika/ar/tracker.h>
#include <baldzarika/ar/markerless/typedefs.h>

namespace baldzarika { namespace ar { namespace markerless {
	
	class marker;

	class tracker
		: public ar::tracker
	{
	public:
		
		typedef ucv::surf::feature_point_t feature_point_t;
		typedef std::vector<feature_point_t> feature_points_t;
		
		typedef ucv::klt_tracker<ucv::surf::integral_t::IS, ucv::surf::integral_t::FS> klt_tracker_t;
		typedef boost::circular_buffer<ucv::surf::const_integral_view_t> integral_view_buffer_t;

		static boost::uint32_t const	DEFAULT_SURF_OCTAVES;
		static boost::uint32_t const	DEFAULT_SURF_INTERVALS;
		static boost::uint32_t const	DEFAULT_SURF_SAMPLE_STEPS;
		static float const				DEFAULT_SURF_TRESHOLD;
		static float const				DEFAULT_SURF_MATCH_DIST;

		static boost::uint32_t const	DEFAULT_KLT_HALF_WIN_SIZE;
		static boost::uint32_t const	DEFAULT_KLT_LEVELS;
		static boost::uint32_t const	DEFAULT_KLT_MAX_ITERATIONS;
		static float const				DEFAULT_KLT_EPSILON;

		static boost::uint32_t const	DEFAULT_TRACKER_MIN_MARKER_FEATURES;
		static boost::uint32_t const	DEFAULT_TRACKER_MAX_MARKER_FEATURES;
		static float const				DEFAULT_TRACKER_SELECT_FP_SCALE;
		static float const				DEFAULT_TRACKER_SELECT_FP_MIN_AREA;

		static float const				DEFAULT_DETECTION_MAX_DIFF_NORM;
		static float const				DEFAULT_TRACKING_MAX_DIFF_NORM;


		class marker_state
			: public ar::tracker::marker_state
		{
		public:
			typedef std::vector<feature_point_t::point2_t> points2_t;
			
			friend class tracker;

			struct order_tag {};
			struct identity_tag {};
			struct marker_identity_tag {};
			struct detected_tag {};
			
			marker_state(boost::shared_ptr<tracker> const &t, boost::shared_ptr<marker> const &m);
			~marker_state();

			boost::shared_ptr<ar::tracker>			get_tracker() const;
			math::size2ui							get_marker_size() const;

			boost::shared_ptr<marker> const&		get_marker() const;

		protected:
			void									set_detected(bool d);
			void									set_homography(math::matrix33f const &hm);

		private:
			boost::weak_ptr<tracker>				m_tracker;
			boost::shared_ptr<marker>				m_marker;
			feature_points_t						m_features;
			ucv::surf::fps_by_pos_tree_t			m_features_kdtree;
			points2_t								m_marker_points;
			points2_t								m_frame_points;
			
		};

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
				boost::multi_index::hashed_unique
				<
					boost::multi_index::tag<marker_state::marker_identity_tag>,
					BOOST_MULTI_INDEX_CONST_MEM_FUN(marker_state, boost::shared_ptr<marker> const&, get_marker)
				>,
				boost::multi_index::ordered_non_unique
				<
					boost::multi_index::tag<marker_state::detected_tag>,
					BOOST_MULTI_INDEX_CONST_MEM_FUN(ar::tracker::marker_state, bool, is_detected)
				>
			>
		> marker_states_t;
	
	public:
		tracker(math::size2ui const &fs);
		~tracker();

		float									get_detection_treshold() const;
		bool									set_detection_treshold(float dt);

		boost::uint32_t							get_detection_min_features() const;
		bool									set_detection_min_features(boost::uint32_t mf);

		boost::uint32_t							get_tracking_max_features() const;
		bool									set_tracking_max_features(boost::uint32_t mf);

		boost::uint32_t							get_tracking_half_win_size() const;
		bool									set_tracking_half_win_size(boost::uint32_t thws);

		boost::uint32_t							get_tracking_num_levels() const;
		bool									set_tracking_num_levels(boost::uint32_t nl);

		boost::uint32_t							get_tracking_max_iterations() const;
		bool									set_tracking_max_iterations(boost::uint32_t mi);

		float									get_detection_max_diff_norm() const;
		bool									set_detection_max_diff_norm(float mdn);

		float									get_tracking_max_diff_norm() const;
		bool									set_tracking_max_diff_norm(float mdn);

		bool									add_marker(boost::shared_ptr<marker> const &m);
		bool									remove_marker(boost::shared_ptr<marker> const &m);
	
	protected:
		void									on_start();
		void									on_update();
		void									on_stop();
		bool									on_set_frame_size(math::size2ui const &fs);
		bool									on_process_frame(ucv::gil::gray8c_view_t gv);

	private:
		void									describe_marker(marker_state &ms);
		bool									detect_marker(marker_state &ms, feature_points_t const &ffs);
		void									detect_markers();
		bool									track_marker(marker_state &dms);
		void									track_markers(std::vector<marker_states_t::iterator> const &dms);

		template < typename I1, typename I2 >
		void									find_marker_homography(std::vector< std::pair<I1, I2> > const &oim, math::size2ui const &ms, math::matrix33f &hm);
	
	private:
		boost::uint32_t							m_min_marker_features;
		boost::uint32_t							m_max_marker_features;
		float									m_select_fp_scale;
		float									m_select_fp_min_area;
		float									m_detection_max_diff_norm;
		float									m_tracking_max_diff_norm;
		
		gray_image_t							m_integral_tmp;
		ucv::surf::integral_image_t				m_integral_frame_stg[2];
		boost::uint32_t							m_integral_frame_seq;
		integral_view_buffer_t					m_integral_views;
								
		ucv::surf								m_surf;
		klt_tracker_t							m_klt_tracker;
		
		marker_states_t							m_marker_states;
	};

} //namespace markerless
} //namespace ar
} //namespace baldzarika

#endif //BALDZARIKA_AR_MARKERLESS_TRACKER_H
