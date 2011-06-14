#ifndef COM_BALDZARIKA_AR_MARKERLESS_TRACKER_H
#define COM_BALDZARIKA_AR_MARKERLESS_TRACKER_H

namespace com { namespace baldzarika { namespace ar {

	class Size2;

	namespace _Tracker {
		class MarkerState;
	} //namespace _Tracker

	class Tracker;

} //namespace ar
} //namespace baldzarika
} //namespace com

namespace com { namespace baldzarika { namespace ar { namespace markerless {

	class Marker;
	class Tracker;

	namespace _Tracker {

		class MarkerState
			: public j2cpp::object<MarkerState>
		{
		public:
			typedef boost::shared_ptr<
				::baldzarika::ar::markerless::tracker::marker_state
			> px_t;

			typedef j2cpp::local_ref<MarkerState> jx_t;

			J2CPP_DECLARE_CLASS

			J2CPP_DECLARE_METHOD(0)

			px_t						get_px() const;

			explicit MarkerState(jobject jobj);
			MarkerState(jlong px);

			operator j2cpp::local_ref< ::com::baldzarika::ar::_Tracker::MarkerState >() const;
			j2cpp::local_ref<Marker>	getMarker();
		};
	} //namespace _Tracker

	class Tracker
		: public j2cpp::object<Tracker>
	{
	public:
		typedef boost::shared_ptr<
			::baldzarika::ar::markerless::tracker
		> px_t;

		typedef _Tracker::MarkerState MarkerState;

		J2CPP_DECLARE_CLASS

		px_t							get_px() const;

		explicit 						Tracker(jobject jobj);

		static jlong					create(j2cpp::local_ref<Size2> const &fs);

		operator j2cpp::local_ref< ::com::baldzarika::ar::Tracker >() const;

		static void						onRunningStateChanged(boost::shared_ptr< ::baldzarika::ar::tracker > const&, ::baldzarika::ar::tracker::eRunningState);
		static void						onMarkerStateChanged(boost::shared_ptr< ::baldzarika::ar::tracker::marker_state > const&, ::baldzarika::ar::tracker::marker_state::eSC);



		jfloat							getDetectionTreshold();
		jboolean						setDetectionTreshold(jfloat dt);

		jint							getDetectionMinFeatures();
		jboolean						setDetectionMinFeatures(jint minFeatures);

		jint							getTrackingMaxFeatures();
		jboolean						setTrackingMaxFeatures(jint maxFeatures);

		jint							getTrackingHalfWinSize();
		jboolean						setTrackingHalfWinSize(jint halfWinSize);

		jint							getTrackingNumLevels();
		jboolean						setTrackingNumLevels(jint numLevels);

		jint							getTrackingMaxIterations();
		jboolean						setTrackingMaxIterations(jint maxIters);

		jfloat							getDetectionMaxDiffNorm();
		jboolean						setDetectionMaxDiffNorm(jfloat maxDiffNorm);

		jfloat							getTrackingMaxDiffNorm();
		jboolean						setTrackingMaxDiffNorm(jfloat maxDiffNorm);

		jboolean						addMarker(j2cpp::local_ref<Marker> const &marker);


	};

} //namespace markerless
} //namespace ar
} //namespace baldzarika
} //namespace com

#endif //COM_BALDZARIKA_AR_MARKERLESS_TRACKER_H
