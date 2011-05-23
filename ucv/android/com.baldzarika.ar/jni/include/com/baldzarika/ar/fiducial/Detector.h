#ifndef COM_BALDZARIKA_AR_FIDUCIAL_DETECTOR_H
#define COM_BALDZARIKA_AR_FIDUCIAL_DETECTOR_H

namespace com { namespace baldzarika { namespace ar {

	class Size2;

} //namespace ar
} //namespace baldzarika
} //namespace com

namespace com { namespace baldzarika { namespace ar { namespace fiducial {

	class MarkerModel;
	class Detector;

	namespace _Detector {

		class MarkerState
			: public j2cpp::object<MarkerState>
		{
		public:
			typedef boost::shared_ptr<
				::baldzarika::ar::fiducial::detector::marker_state
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

			jboolean						isDetected();
			jint							getMarkerId();
			j2cpp::local_ref<MarkerModel>	getMarkerModel();
			j2cpp::local_ref<Detector>		getDetector();

			j2cpp::local_ref<j2cpp::android::graphics::Matrix>
											getHomography();

			jboolean						getCameraPose(j2cpp::local_ref< j2cpp::array<jfloat,1> > const &cameraPose);

			void							create(jlong px);
			void							destroy();

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

			void	onRunningStateChanged(j2cpp::local_ref<Detector> const &detector, jint rs);
			void	onMarkerStateChanged(j2cpp::local_ref<MarkerState> const &markerState, jint sc);
		};

	} //namespace _Detector

	class Detector
		: public j2cpp::object<Detector>
	{
	public:
		typedef boost::shared_ptr<
			::baldzarika::ar::fiducial::detector
		> px_t;

		typedef j2cpp::local_ref<Detector> jx_t;
		typedef j2cpp::global_ref<Detector> jref_t;

		typedef _Detector::Callback Callback;
		typedef _Detector::MarkerState MarkerState;

		J2CPP_DECLARE_CLASS

		J2CPP_DECLARE_FIELD(0)
		J2CPP_DECLARE_FIELD(1)

		static jx_t				get(px_t const &px);
		static px_t				get(jx_t const &jx);

		static void				onDetectorRunningStateChanged(px_t const&, ::baldzarika::ar::fiducial::detector::eRunningState);
		static void				onMarkerStateChanged(MarkerState::px_t const &, ::baldzarika::ar::fiducial::detector::marker_state::eSC);


		explicit 	Detector(jobject jobj);

		void		create(j2cpp::local_ref<Size2> const &fs);
		void		create();
		void 		destroy();


		jboolean	addMarkerModel(j2cpp::local_ref<MarkerModel> const &markerModel);
		jboolean	removeMarkerModel(j2cpp::local_ref<MarkerModel> const &markerModel);

		j2cpp::local_ref<Size2>	getFrameSize();
		jboolean				setFrameSize(j2cpp::local_ref<Size2> const &frameSize);

		jfloat		getCameraFovy();
		jboolean	setCameraFovy(jfloat fovy);

		jfloat		getCameraFocalLength();
		jboolean 	getCameraProjection(j2cpp::local_ref< j2cpp::array<jfloat,1> > const &cameraProjection);


		jboolean	start();
		jboolean	isActive();
		jboolean	isStarting();
		jboolean	isStarted();
		jboolean	stop();
		jboolean	waitToStop();

		jboolean	update(j2cpp::local_ref< j2cpp::array<jbyte,1> > const &data, jint pfmt);

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

} //namespace fiducial
} //namespace ar
} //namespace baldzarika
} //namespace com

#endif //COM_BALDZARIKA_AR_FIDUCIAL_MARKER_MODEL_H
