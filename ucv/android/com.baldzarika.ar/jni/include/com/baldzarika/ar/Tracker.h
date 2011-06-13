#ifndef COM_BALDZARIKA_AR_TRACKER_H
#define COM_BALDZARIKA_AR_TRACKER_H

namespace com { namespace baldzarika { namespace ar {

	class Size2;
	class Tracker;

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

			void						initialize(jlong px);
			void						destroy();

			j2cpp::local_ref<Tracker> 	getTracker();
			jboolean					isDetected();
			j2cpp::local_ref<Size2>		getMarkerSize();


			j2cpp::local_ref<j2cpp::android::graphics::Matrix>
										getHomography();

			jboolean					getCameraPose(j2cpp::local_ref< j2cpp::array<jfloat,1> > const &cameraPose);

			j2cpp::field<
				J2CPP_CLASS_NAME,
				J2CPP_FIELD_NAME(0),
				J2CPP_FIELD_SIGNATURE(0),
				jlong
			> m_px;
		};

		class Callback
			: public j2cpp::object<Callback>
		{
		public:

			J2CPP_DECLARE_CLASS

			J2CPP_DECLARE_METHOD(0)
			J2CPP_DECLARE_METHOD(1)

			explicit Callback(jobject jobj);

			void	onRunningStateChanged(j2cpp::local_ref<Tracker> const &t, jint rs);
			void	onMarkerStateChanged(j2cpp::local_ref<MarkerState> const &ms, jint sc);
		};

	}// namespace _Tracker

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
		typedef _Tracker::Callback Callback;


		J2CPP_DECLARE_CLASS

		J2CPP_DECLARE_FIELD(0)
		J2CPP_DECLARE_FIELD(1)

		static jx_t						get(px_t const &px);
		static px_t						get(jx_t const &jx);

		explicit Tracker(jobject jobj);
		Tracker(jlong px);

		void							initialize(jlong px);
		void 							destroy();

		j2cpp::local_ref<Size2>			getFrameSize();
		jboolean						setFrameSize(j2cpp::local_ref<Size2> const &fs);

		jfloat							getCameraFovy();
		jboolean						setCameraFovy(jfloat fovy);

		jfloat							getCameraZNear();
		jboolean						setCameraZNear(jfloat zNear);

		jfloat							getCameraZFar();
		jboolean						setCameraZFar(jfloat zFar);

		jfloat							getCameraFocalLength();
		jboolean 						getCameraProjection(j2cpp::local_ref< j2cpp::array<jfloat,1> > const &cameraProjection);

		jboolean						start();
		jboolean						isActive();
		jboolean						isStarting();
		jboolean						isStarted();
		jboolean						stop();
		jboolean						waitToStop();

		jboolean						processFrame(j2cpp::local_ref< j2cpp::array<jbyte,1> > const &data, jint pfmt, jint width, jint height);

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
			j2cpp::local_ref< Callback >
		> m_cb;
	};


} //namespace ar
} //namespace baldzarika
} //namespace com

#endif //COM_BALDZARIKA_AR_TRACKER_H
