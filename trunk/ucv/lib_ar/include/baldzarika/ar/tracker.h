#ifndef BALDZARIKA_AR_TRACKER_H
#define BALDZARIKA_AR_TRACKER_H

#include <baldzarika/ucv/surf.h>
#include <baldzarika/ucv/klt_tracker.h>
#include <baldzarika/ucv/matrix.h>

namespace baldzarika { namespace ar {
	
	class marker;

	class tracker
		: public boost::enable_shared_from_this<tracker>
		, public boost::noncopyable
	{
	public:

		typedef ucv::surf::feature_point_t feature_point_t;
		typedef std::vector<feature_point_t> feature_points_t;
		
		typedef ucv::fixed_point<10, 21> gray_t;
		typedef ucv::gil::pixel<gray_t, ucv::gil::gray_layout_t> gray_pixel_t;
		typedef ucv::gil::image< gray_pixel_t, false, std::allocator<unsigned char> > gray_image_t;
		typedef gray_image_t::view_t gray_view_t;
		typedef gray_image_t::const_view_t gray_const_view_t;

		typedef ucv::klt_tracker<ucv::surf::integral_t::IS, ucv::surf::integral_t::FS> klt_tracker_t;
		typedef boost::circular_buffer<ucv::surf::const_integral_view_t> integral_view_buffer_t;

		static boost::uint32_t const	DEFAULT_SURF_OCTAVES;
		static boost::uint32_t const	DEFAULT_SURF_INTERVALS;
		static boost::uint32_t const	DEFAULT_SURF_SAMPLE_STEPS;
		static float const				DEFAULT_SURF_TRESHOLD;

		static boost::uint32_t const	DEFAULT_KLT_WIN_SIZE;
		static boost::uint32_t const	DEFAULT_KLT_LEVELS;
		static boost::uint32_t const	DEFAULT_KLT_MAX_ITERATIONS;
		static float const				DEFAULT_KLT_EPSILON;

		static boost::uint32_t const	DEFAULT_TRACKER_MIN_MARKER_FEATURES;
		static boost::uint32_t const	DEFAULT_TRACKER_MAX_MARKER_FEATURES;


		class marker_state
			: public boost::enable_shared_from_this<marker_state>
			, public boost::noncopyable
		{
		public:
			typedef std::vector<feature_points_t::const_iterator> feature_point_handles_t;
			typedef std::vector<feature_point_t::point2_t> points2_t;
			typedef std::pair<feature_points_t::const_iterator, points2_t::const_iterator> feature_to_point2_match_t;
			typedef std::vector<feature_to_point2_match_t> feature_to_point2_matches_t;



			friend class tracker;

			typedef enum
			{
				SC_DETECTION=0,
				SC_POSE
			} eSC;

			typedef boost::signals2::signal<void (boost::shared_ptr<marker_state> const &, eSC)> changed_signal_t;

			struct order_tag {};
			struct identity_tag {};
			struct marker_identity_tag {};
			struct detected_tag {};
			
			marker_state(boost::shared_ptr<tracker> const &t, boost::shared_ptr<marker> const &m);
			~marker_state();

			bool									is_detected() const;
			boost::shared_ptr<marker> const&		get_marker() const;
			points2_t const&						get_frame_features() const;

		protected:
			void									collect_features();
			void									set_detected(bool d);
			void									update_pose();
		
		private:
			boost::weak_ptr<tracker>				m_tracker;
			boost::shared_ptr<marker>				m_marker;
			feature_points_t						m_features;
			feature_point_handles_t					m_tracking_features;
			points2_t								m_frame_features;
			bool									m_detected;
			ucv::matrix33f							m_hmatrix;
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
					BOOST_MULTI_INDEX_CONST_MEM_FUN(marker_state, bool, is_detected)
				>
			>
		> marker_states_t;
	
	public:
		tracker(ucv::size2ui const &fs);
		~tracker();

		ucv::size2ui							get_frame_size() const;
		bool									set_frame_size(ucv::size2ui const &fs);

		boost::shared_ptr<marker_state>			add_marker(boost::shared_ptr<marker> const &m);
		bool									remove_marker(boost::shared_ptr<marker> const &m);
		bool									remove_marker_state(boost::shared_ptr<marker_state> const &ms);

		marker_state::changed_signal_t&			marker_state_changed() const;


		bool									start();
		bool									is_started() const;
		bool									stop();

		bool									update(gray_const_view_t gv, gray_t const &sm=gray_t(-1));

	protected:
		void									on_start();
		void									on_update();
		void									on_stop();

	private:
		void									detect_markers();
		void									track_markers(std::vector<marker_states_t::iterator> const &dms);


	private:
		boost::uint32_t							m_min_marker_features;
		boost::uint32_t							m_max_marker_features;
		
		mutable boost::asio::io_service			m_ios;
		boost::asio::io_service::work			m_ios_work;
		boost::thread							m_worker;

		boost::mutex							m_update_sync;
		ucv::surf::integral_image_t				m_integral_frame_stg[2];
		boost::uint32_t							m_integral_frame_seq;
		integral_view_buffer_t					m_integral_views;
								
		ucv::surf								m_surf;
		klt_tracker_t							m_klt_tracker;

		marker_states_t							m_marker_states;
		mutable marker_state::changed_signal_t	m_marker_state_changed;
	};

} //namespace ar
} //namespace baldzarika

#endif //BALDZARIKA_AR_TRACKER_H
