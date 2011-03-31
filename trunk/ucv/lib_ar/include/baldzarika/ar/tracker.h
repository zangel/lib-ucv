#ifndef BALDZARIKA_AR_TRACKER_H
#define BALDZARIKA_AR_TRACKER_H

#include <baldzarika/ucv/surf.h>
#include <baldzarika/ucv/homography.h>

namespace baldzarika { namespace ar {
	
	class marker;

	class tracker
		: public boost::enable_shared_from_this<tracker>
		, public boost::noncopyable
	{
	public:
		typedef ucv::surf::feature_point_t feature_point_t;
		typedef ucv::homography::matrix_t hmatrix_t;

		static boost::uint32_t const	DEFAULT_SURF_OCTAVES;
		static boost::uint32_t const	DEFAULT_SURF_INTERVALS;
		static boost::uint32_t const	DEFAULT_SURF_SAMPLE_STEPS;
		static float const				DEFAULT_SURF_TRESHOLD;
		
				
		class marker_state
			: public boost::enable_shared_from_this<marker_state>
			, protected boost::noncopyable
		{
		public:
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

			bool								is_detected() const;
			boost::shared_ptr<marker> const&	get_marker() const;

		protected:
			void								set_detected(bool d);
			void								collect_features();
		
		private:
			boost::weak_ptr<tracker>		m_tracker;
			boost::shared_ptr<marker>		m_marker;
			std::vector<feature_point_t>	m_features;
			bool							m_detected;
			hmatrix_t						m_hmatrix;
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

		ucv::size2ui						get_frame_size() const;
		bool								set_frame_size(ucv::size2ui const &fs);

		boost::shared_ptr<marker_state>		add_marker(boost::shared_ptr<marker> const &m);
		bool								remove_marker(boost::shared_ptr<marker> const &m);
		bool								remove_marker_state(boost::shared_ptr<marker_state> const &ms);

		marker_state::changed_signal_t		marker_state_changed() const;


		bool								start();
		bool								is_started() const;
		bool								stop();

		bool								update(ucv::surf::gray_view_t gv);

	protected:
		void								on_start();
		void								on_update();
		void								on_stop();


	private:
		mutable boost::asio::io_service		m_ios;
		boost::asio::io_service::work		m_ios_work;
		boost::thread						m_worker;
		ucv::surf							m_surf;
		boost::mutex						m_surf_sync;
		marker_states_t						m_marker_states;

		marker_state::changed_signal_t		m_marker_state_changed;
	};

} //namespace ar
} //namespace baldzarika

#endif //BALDZARIKA_AR_TRACKER_H
