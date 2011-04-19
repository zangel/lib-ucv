#ifndef COM_BALDZARIKA_AR_TRACKER_H
#define COM_BALDZARIKA_AR_TRACKER_H

namespace com { namespace baldzarika { namespace ar {

	class Size2;
	class Frame;
	class Marker;

	namespace _Tracker {

		class MarkerState
			: public j2cpp::object<MarkerState>
		{
		public:
			typedef boost::shared_ptr<
				::baldzarika::ar::tracker::marker_state
			> px_t;

			typedef j2cpp::local_ref<MarkerState> jx_t;
			typedef j2cpp::global_ref<MarkerState> jref_t;

			J2CPP_DECLARE_CLASS

			J2CPP_DECLARE_METHOD(0)

			J2CPP_DECLARE_FIELD(0)

			static jx_t					get(px_t const &px);
			static px_t					get(jx_t const &jx);


			explicit MarkerState(jobject jobj);
			MarkerState(jlong px);

			jboolean					isDetected();
			j2cpp::local_ref<Marker>	getMarker();

			void						create(jlong px);
			void						destroy();

			j2cpp::field<
				J2CPP_CLASS_NAME,
				J2CPP_FIELD_NAME(0),
				J2CPP_FIELD_SIGNATURE(0),
				jlong
			> m_px;
		};

		class MarkerStateCallback
			: public j2cpp::object<MarkerStateCallback>
		{
		public:

			J2CPP_DECLARE_CLASS

			J2CPP_DECLARE_METHOD(0)

			explicit MarkerStateCallback(jobject jobj);

			void	onStateChanged(j2cpp::local_ref<MarkerState> const &ms, jint sc);
		};

	} //namespace _Tracker

	class Tracker
		: public j2cpp::object<Tracker>
	{
	public:

		typedef boost::shared_ptr<
			::baldzarika::ar::tracker
		> px_t;

		typedef j2cpp::local_ref<Tracker>	jx_t;
		typedef j2cpp::global_ref<Tracker>	jref_t;

		typedef _Tracker::MarkerState MarkerState;
		typedef _Tracker::MarkerStateCallback MarkerStateCallback;


		J2CPP_DECLARE_CLASS

        J2CPP_DECLARE_FIELD(0)
		J2CPP_DECLARE_FIELD(1)

		static jx_t						get(px_t const &px);
		static px_t						get(jx_t const &jx);

		static void 					onTrackerStartStop(px_t const &t, bool ss);
		static void 					onMarkerStateChanged(MarkerState::px_t const &ms, ::baldzarika::ar::tracker::marker_state::eSC sc);

		explicit Tracker(jobject jobj);

		j2cpp::local_ref<Size2>			getFrameSize();
		jboolean						setFrameSize(j2cpp::local_ref<Size2> const &fs);

		j2cpp::local_ref<MarkerState>	addMarker(j2cpp::local_ref<Marker> const &marker);


		jboolean						start();
		jboolean						isStarted();
		jboolean						stop();
		jboolean						update(j2cpp::local_ref<Frame> const &frame);

		void							create(j2cpp::local_ref<Size2> const &fs);
		void							destroy();


		j2cpp::field<
			J2CPP_CLASS_NAME,
			J2CPP_FIELD_NAME(0),
			J2CPP_FIELD_SIGNATURE(0),
			jlong
		> m_px;

		j2cpp::field<
			J2CPP_CLASS_NAME,
			J2CPP_FIELD_NAME(1),
			J2CPP_FIELD_SIGNATURE(1),
			j2cpp::local_ref< MarkerStateCallback >
		> m_MSCB;
	};

} //namespace ar
} //namespace baldzarika
} //namespace com

#endif //COM_BALDZARIKA_AR_FRAME_H

