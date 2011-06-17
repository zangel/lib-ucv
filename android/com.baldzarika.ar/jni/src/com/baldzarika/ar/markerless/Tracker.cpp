#include <config.h>
#include <com/baldzarika/ar/Tracker.h>
#include <com/baldzarika/ar/markerless/Tracker.h>
#include <com/baldzarika/ar/Size2.h>
#include <com/baldzarika/ar/markerless/Marker.h>
#include <com_baldzarika_ar_markerless_Tracker.h>
#include <com_baldzarika_ar_markerless_Tracker_MarkerState.h>

#pragma GCC visibility push(default)

jobject Java_com_baldzarika_ar_markerless_Tracker_00024MarkerState_getMarker(JNIEnv */*e*/, jobject ms)
{
	using namespace com::baldzarika::ar::markerless;
	using namespace j2cpp;
	return Tracker::MarkerState(ms).getMarker().get_jobject();
}

jlong Java_com_baldzarika_ar_markerless_Tracker_create(JNIEnv */*e*/, jclass /*markerClazz*/, jobject frameSize)
{
	using namespace com::baldzarika::ar::markerless;
	using namespace j2cpp;
	return Tracker::create(local_ref<com::baldzarika::ar::Size2>(frameSize));
}

jfloat Java_com_baldzarika_ar_markerless_Tracker_getDetectionTreshold(JNIEnv */*e*/, jobject t)
{
	using namespace com::baldzarika::ar::markerless;
	using namespace j2cpp;
	return Tracker(t).getDetectionTreshold();
}

jboolean Java_com_baldzarika_ar_markerless_Tracker_setDetectionTreshold(JNIEnv */*e*/, jobject t, jfloat treshold)
{
	using namespace com::baldzarika::ar::markerless;
	using namespace j2cpp;
	return Tracker(t).setDetectionTreshold(treshold);
}

jint Java_com_baldzarika_ar_markerless_Tracker_getDetectionMinFeatures(JNIEnv */*e*/, jobject t)
{
	using namespace com::baldzarika::ar::markerless;
	using namespace j2cpp;
	return Tracker(t).getDetectionMinFeatures();
}


jboolean Java_com_baldzarika_ar_markerless_Tracker_setDetectionMinFeatures(JNIEnv */*e*/, jobject t, jint minFeatures)
{
	using namespace com::baldzarika::ar::markerless;
	using namespace j2cpp;
	return Tracker(t).setDetectionMinFeatures(minFeatures);
}

jint Java_com_baldzarika_ar_markerless_Tracker_getTrackingMaxFeatures(JNIEnv */*e*/, jobject t)
{
	using namespace com::baldzarika::ar::markerless;
	using namespace j2cpp;
	return Tracker(t).getTrackingMaxFeatures();
}

jboolean Java_com_baldzarika_ar_markerless_Tracker_setTrackingMaxFeatures(JNIEnv */*e*/, jobject t, jint maxFeatures)
{
	using namespace com::baldzarika::ar::markerless;
	using namespace j2cpp;
	return Tracker(t).setTrackingMaxFeatures(maxFeatures);
}

jint Java_com_baldzarika_ar_markerless_Tracker_getTrackingHalfWinSize(JNIEnv */*e*/, jobject t)
{
	using namespace com::baldzarika::ar::markerless;
	using namespace j2cpp;
	return Tracker(t).getTrackingHalfWinSize();
}

jboolean Java_com_baldzarika_ar_markerless_Tracker_setTrackingHalfWinSize(JNIEnv */*e*/, jobject t, jint halfWinSize)
{
	using namespace com::baldzarika::ar::markerless;
	using namespace j2cpp;
	return Tracker(t).setTrackingHalfWinSize(halfWinSize);
}

jint Java_com_baldzarika_ar_markerless_Tracker_getTrackingNumLevels(JNIEnv */*e*/, jobject t)
{
	using namespace com::baldzarika::ar::markerless;
	using namespace j2cpp;
	return Tracker(t).getTrackingNumLevels();
}

jboolean Java_com_baldzarika_ar_markerless_Tracker_setTrackingNumLevels(JNIEnv */*e*/, jobject t, jint numLevels)
{
	using namespace com::baldzarika::ar::markerless;
	using namespace j2cpp;
	return Tracker(t).setTrackingNumLevels(numLevels);
}

jint Java_com_baldzarika_ar_markerless_Tracker_getTrackingMaxIterations(JNIEnv */*e*/, jobject t)
{
	using namespace com::baldzarika::ar::markerless;
	using namespace j2cpp;
	return Tracker(t).getTrackingMaxIterations();
}

jboolean Java_com_baldzarika_ar_markerless_Tracker_setTrackingMaxIterations(JNIEnv */*e*/, jobject t, jint maxIterations)
{
	using namespace com::baldzarika::ar::markerless;
	using namespace j2cpp;
	return Tracker(t).setTrackingMaxIterations(maxIterations);
}

jfloat Java_com_baldzarika_ar_markerless_Tracker_getDetectionMaxDiffNorm(JNIEnv */*e*/, jobject t)
{
	using namespace com::baldzarika::ar::markerless;
	using namespace j2cpp;
	return Tracker(t).getDetectionMaxDiffNorm();
}

jboolean Java_com_baldzarika_ar_markerless_Tracker_setDetectionMaxDiffNorm(JNIEnv */*e*/, jobject t, jfloat maxDiffNorm)
{
	using namespace com::baldzarika::ar::markerless;
	using namespace j2cpp;
	return Tracker(t).setDetectionMaxDiffNorm(maxDiffNorm);
}

jfloat Java_com_baldzarika_ar_markerless_Tracker_getTrackingMaxDiffNorm(JNIEnv */*e*/, jobject t)
{
	using namespace com::baldzarika::ar::markerless;
	using namespace j2cpp;
	return Tracker(t).getTrackingMaxDiffNorm();
}

jboolean Java_com_baldzarika_ar_markerless_Tracker_setTrackingMaxDiffNorm(JNIEnv */*e*/, jobject t, jfloat maxDiffNorm)
{
	using namespace com::baldzarika::ar::markerless;
	using namespace j2cpp;
	return Tracker(t).setTrackingMaxDiffNorm(maxDiffNorm);
}

jboolean Java_com_baldzarika_ar_markerless_Tracker_addMarker(JNIEnv */*e*/, jobject t, jobject marker)
{
	using namespace com::baldzarika::ar::markerless;
	using namespace j2cpp;
	return Tracker(t).addMarker(local_ref<Marker>(marker));
}

jboolean Java_com_baldzarika_ar_markerless_Tracker_removeMarker(JNIEnv */*e*/, jobject t, jobject marker)
{
	using namespace com::baldzarika::ar::markerless;
	using namespace j2cpp;
	return Tracker(t).removeMarker(local_ref<Marker>(marker));
}

#pragma GCC visibility pop

namespace com { namespace baldzarika { namespace ar { namespace markerless {

namespace _Tracker {

		J2CPP_DEFINE_CLASS(MarkerState,"com/baldzarika/ar/markerless/Tracker$MarkerState")
		J2CPP_DEFINE_METHOD(MarkerState,0,"<init>","(J)V")

		MarkerState::px_t MarkerState::get_px() const
		{
			return px_t(::com::baldzarika::ar::Tracker::MarkerState::get(*this), boost::detail::dynamic_cast_tag());
		}

		MarkerState::MarkerState(jobject jobj)
			: j2cpp::object<MarkerState>(jobj)
		{
		}

		MarkerState::MarkerState(jlong px)
		: j2cpp::object<MarkerState>(
			j2cpp::call_new_object<
				J2CPP_CLASS_NAME,
				J2CPP_METHOD_NAME(0),
				J2CPP_METHOD_SIGNATURE(0)
			>(px))
		{
		}

		MarkerState::operator j2cpp::local_ref< ::com::baldzarika::ar::Tracker::MarkerState >() const
		{
			return j2cpp::local_ref< ::com::baldzarika::ar::Tracker::MarkerState >(get_jobject());
		}

		j2cpp::local_ref<Marker> MarkerState::getMarker()
		{
			if(px_t px=get_px())
				return Marker::get(px->get_marker());
			return j2cpp::local_ref<Marker>();
		}

	} //namespace _Tracker

	J2CPP_DEFINE_CLASS(Tracker,"com/baldzarika/ar/markerless/Tracker")

	Tracker::px_t Tracker::get_px() const
	{
		return px_t(::com::baldzarika::ar::Tracker::get(*this), boost::detail::dynamic_cast_tag());
	}

	Tracker::Tracker(jobject jobj)
	: j2cpp::object<Tracker>(jobj)
	{
	}

	jlong Tracker::create(j2cpp::local_ref<Size2> const &fs)
	{
		::com::baldzarika::ar::Tracker::px_t px(
			new ::baldzarika::ar::markerless::tracker(
				::baldzarika::math::size2ui(
					jint(fs->m_Width), jint(fs->m_Height)
				)
			)
		);

		px->running_state_changed().connect(boost::bind(&Tracker::onRunningStateChanged, _1, _2));
		px->marker_state_changed().connect(boost::bind(&Tracker::onMarkerStateChanged, _1, _2));

		return reinterpret_cast<jlong>(new ::com::baldzarika::ar::Tracker::px_t(px));
	}

	Tracker::operator j2cpp::local_ref< ::com::baldzarika::ar::Tracker >() const
	{
		return j2cpp::local_ref< ::com::baldzarika::ar::Tracker >(get_jobject());
	}

	void Tracker::onRunningStateChanged(boost::shared_ptr< ::baldzarika::ar::tracker > const &t, ::baldzarika::ar::tracker::eRunningState rs)
	{
		if(rs==::baldzarika::ar::tracker::RS_STARTING)
			j2cpp::environment::get()->attach_current_thread();

		if(::com::baldzarika::ar::Tracker::jx_t jx=::com::baldzarika::ar::Tracker::get(t))
			if(j2cpp::local_ref< ::com::baldzarika::ar::Tracker::Callback > cb=jx->m_cb)
				cb->onRunningStateChanged(jx, rs);

		if(rs==::baldzarika::ar::tracker::RS_STOPPED)
			j2cpp::environment::get()->detach_current_thread();
	}

	void Tracker::onMarkerStateChanged(boost::shared_ptr< ::baldzarika::ar::tracker::marker_state > const &ms, ::baldzarika::ar::tracker::marker_state::eSC sc)
	{
		j2cpp::local_ref< ::com::baldzarika::ar::Tracker::MarkerState > jms=::com::baldzarika::ar::Tracker::MarkerState::get(ms);

		if(sc==::baldzarika::ar::tracker::marker_state::SC_DETECTION && ms->is_detected() && !jms)
		{
			jms=j2cpp::local_ref< ::com::baldzarika::ar::Tracker::MarkerState >(
				MarkerState(
					reinterpret_cast<jlong>( new ::com::baldzarika::ar::Tracker::MarkerState::px_t(ms) )
				)
			);
		}

		if(::com::baldzarika::ar::Tracker::jx_t jx=::com::baldzarika::ar::Tracker::get(ms->get_tracker()))
			if(j2cpp::local_ref< ::com::baldzarika::ar::Tracker::Callback > cb=jx->m_cb)
				cb->onMarkerStateChanged(jms, sc);

		if(sc==::baldzarika::ar::tracker::marker_state::SC_DETECTION && !ms->is_detected() && jms)
		{
			jms->destroy();
		}
	}

	jfloat Tracker::getDetectionTreshold()
	{
		if(px_t px=get_px())
			return px->get_detection_treshold();
		return 0.0f;
	}

	jboolean Tracker::setDetectionTreshold(jfloat dt)
	{
		if(px_t px=get_px())
			return px->set_detection_treshold(dt)?JNI_TRUE:JNI_FALSE;
		return JNI_FALSE;
	}

	jint Tracker::getDetectionMinFeatures()
	{
		if(px_t px=get_px())
			return px->get_detection_min_features();
		return 0;
	}

	jboolean Tracker::setDetectionMinFeatures(jint minFeatures)
	{
		if(px_t px=get_px())
			return px->set_detection_min_features(minFeatures)?JNI_TRUE:JNI_FALSE;
		return JNI_FALSE;
	}

	jint Tracker::getTrackingMaxFeatures()
	{
		if(px_t px=get_px())
			return px->get_tracking_max_features();
		return 0;
	}

	jboolean Tracker::setTrackingMaxFeatures(jint maxFeatures)
	{
		if(px_t px=get_px())
			return px->set_tracking_max_features(maxFeatures)?JNI_TRUE:JNI_FALSE;
		return JNI_FALSE;
	}

	jint Tracker::getTrackingHalfWinSize()
	{
		if(px_t px=get_px())
			return px->get_tracking_half_win_size();
		return 0;
	}

	jboolean Tracker::setTrackingHalfWinSize(jint halfWinSize)
	{
		if(px_t px=get_px())
			return px->set_tracking_half_win_size(halfWinSize)?JNI_TRUE:JNI_FALSE;
		return JNI_FALSE;
	}

	jint Tracker::getTrackingNumLevels()
	{
		if(px_t px=get_px())
			return px->get_tracking_num_levels();
		return 0;
	}

	jboolean Tracker::setTrackingNumLevels(jint numLevels)
	{
		if(px_t px=get_px())
			return px->set_tracking_num_levels(numLevels)?JNI_TRUE:JNI_FALSE;
		return JNI_FALSE;
	}

	jint Tracker::getTrackingMaxIterations()
	{
		if(px_t px=get_px())
			return px->get_tracking_max_iterations();
		return 0;
	}

	jboolean Tracker::setTrackingMaxIterations(jint maxIters)
	{
		if(px_t px=get_px())
			return px->set_tracking_max_iterations(maxIters)?JNI_TRUE:JNI_FALSE;
		return JNI_FALSE;
	}

	jfloat Tracker::getDetectionMaxDiffNorm()
	{
		if(px_t px=get_px())
			return px->get_detection_max_diff_norm();
		return 0.0f;
	}

	jboolean Tracker::setDetectionMaxDiffNorm(jfloat maxDiffNorm)
	{
		if(px_t px=get_px())
			return px->set_detection_max_diff_norm(maxDiffNorm)?JNI_TRUE:JNI_FALSE;
		return JNI_FALSE;
	}

	jfloat Tracker::getTrackingMaxDiffNorm()
	{
		if(px_t px=get_px())
			return px->get_tracking_max_diff_norm();
		return 0.0f;
	}

	jboolean Tracker::setTrackingMaxDiffNorm(jfloat maxDiffNorm)
	{
		if(px_t px=get_px())
			return px->set_tracking_max_diff_norm(maxDiffNorm)?JNI_TRUE:JNI_FALSE;
		return JNI_FALSE;
	}

	jboolean Tracker::addMarker(j2cpp::local_ref<Marker> const &marker)
	{
		if(px_t px=get_px())
			return px->add_marker(Marker::get(marker))?JNI_TRUE:JNI_FALSE;
		return JNI_FALSE;
	}

	jboolean Tracker::removeMarker(j2cpp::local_ref<Marker> const &marker)
	{
		if(px_t px=get_px())
			return px->remove_marker(Marker::get(marker))?JNI_TRUE:JNI_FALSE;
		return JNI_FALSE;
	}

} //namespace markerless
} //namespace ar
} //namespace baldzarika
} //namespace com
