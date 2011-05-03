#include <config.h>
#include <com_baldzarika_ar_Tracker.h>
#include <com_baldzarika_ar_Tracker_MarkerState.h>
#include <com_baldzarika_ar_Tracker_Callback.h>

#include <com/baldzarika/ar/Size2.h>
#include <com/baldzarika/ar/Point2.h>
#include <com/baldzarika/ar/Marker.h>
#include <com/baldzarika/ar/Frame.h>

#include <com/baldzarika/ar/Tracker.h>

void Java_com_baldzarika_ar_Tracker_00024MarkerState_create(JNIEnv */*e*/, jobject ms, jlong px)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	Tracker::MarkerState(ms).create(px);
}

void Java_com_baldzarika_ar_Tracker_00024MarkerState_destroy(JNIEnv */*e*/, jobject ms)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	Tracker::MarkerState(ms).destroy();
}

jboolean Java_com_baldzarika_ar_Tracker_00024MarkerState_isDetected(JNIEnv */*e*/, jobject ms)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Tracker::MarkerState(ms).isDetected();
}

jobject Java_com_baldzarika_ar_Tracker_00024MarkerState_getMarker(JNIEnv */*e*/, jobject ms)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Tracker::MarkerState(ms).getMarker().get_jobject();
}

jobject Java_com_baldzarika_ar_Tracker_00024MarkerState_getHomography(JNIEnv */*e*/, jobject ms)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Tracker::MarkerState(ms).getHomography().get_jobject();
}

jobjectArray Java_com_baldzarika_ar_Tracker_00024MarkerState_getMarkerCorners(JNIEnv */*e*/, jobject ms)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return reinterpret_cast<jobjectArray>(
			Tracker::MarkerState(ms).getMarkerCorners().get_jobject()
	);
}

jint Java_com_baldzarika_ar_Tracker_00024MarkerState_getFeatureMatchesSize(JNIEnv */*e*/, jobject ms)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Tracker::MarkerState(ms).getFeatureMatchesSize();
}

void Java_com_baldzarika_ar_Tracker_create(JNIEnv */*e*/, jobject t, jobject fs)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	Tracker(t).create(local_ref<Size2>(fs));
}

void Java_com_baldzarika_ar_Tracker_destroy(JNIEnv */*e*/, jobject t)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	Tracker(t).destroy();
}

jobject Java_com_baldzarika_ar_Tracker_getFrameSize(JNIEnv */*e*/, jobject t)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Tracker(t).getFrameSize().get_jobject();
}

jboolean Java_com_baldzarika_ar_Tracker_setFrameSize(JNIEnv */*e*/, jobject t, jobject fs)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Tracker(t).setFrameSize(local_ref<Size2>(fs));
}

jfloat Java_com_baldzarika_ar_Tracker_getDetectionTreshold(JNIEnv */*e*/, jobject t)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Tracker(t).getDetectionTreshold();
}

jboolean Java_com_baldzarika_ar_Tracker_setDetectionTreshold(JNIEnv */*e*/, jobject t, jfloat treshold)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Tracker(t).setDetectionTreshold(treshold);
}

jint Java_com_baldzarika_ar_Tracker_getDetectionMinFeatures(JNIEnv */*e*/, jobject t)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Tracker(t).getDetectionMinFeatures();
}

jboolean Java_com_baldzarika_ar_Tracker_setDetectionMinFeatures(JNIEnv */*e*/, jobject t, jint minFeatures)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Tracker(t).setDetectionMinFeatures(minFeatures);
}

jint Java_com_baldzarika_ar_Tracker_getTrackingMaxFeatures(JNIEnv */*e*/, jobject t)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Tracker(t).getTrackingMaxFeatures();
}

jboolean Java_com_baldzarika_ar_Tracker_setTrackingMaxFeatures(JNIEnv */*e*/, jobject t, jint maxFeatures)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Tracker(t).setTrackingMaxFeatures(maxFeatures);
}

jint Java_com_baldzarika_ar_Tracker_getTrackingHalfWinSize(JNIEnv */*e*/, jobject t)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Tracker(t).getTrackingHalfWinSize();
}

jboolean Java_com_baldzarika_ar_Tracker_setTrackingHalfWinSize(JNIEnv */*e*/, jobject t, jint halfWinSize)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Tracker(t).setTrackingHalfWinSize(halfWinSize);
}

jint Java_com_baldzarika_ar_Tracker_getTrackingNumLevels(JNIEnv */*e*/, jobject t)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Tracker(t).getTrackingNumLevels();
}

jboolean Java_com_baldzarika_ar_Tracker_setTrackingNumLevels(JNIEnv */*e*/, jobject t, jint numLevels)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Tracker(t).setTrackingNumLevels(numLevels);
}

jint Java_com_baldzarika_ar_Tracker_getTrackingMaxIterations(JNIEnv */*e*/, jobject t)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Tracker(t).getTrackingMaxIterations();
}

jboolean Java_com_baldzarika_ar_Tracker_setTrackingMaxIterations(JNIEnv */*e*/, jobject t, jint maxIters)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Tracker(t).setTrackingMaxIterations(maxIters);
}

jfloat Java_com_baldzarika_ar_Tracker_getDetectionMaxDiffNorm(JNIEnv */*e*/, jobject t)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Tracker(t).getDetectionMaxDiffNorm();
}

jboolean Java_com_baldzarika_ar_Tracker_setDetectionMaxDiffNorm(JNIEnv */*e*/, jobject t, jfloat maxDiffNorm)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Tracker(t).setDetectionMaxDiffNorm(maxDiffNorm);
}

jfloat Java_com_baldzarika_ar_Tracker_getTrackingMaxDiffNorm(JNIEnv */*e*/, jobject t)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Tracker(t).getTrackingMaxDiffNorm();
}

jboolean Java_com_baldzarika_ar_Tracker_setTrackingMaxDiffNorm(JNIEnv */*e*/, jobject t, jfloat maxDiffNorm)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Tracker(t).setTrackingMaxDiffNorm(maxDiffNorm);
}


jobject Java_com_baldzarika_ar_Tracker_addMarker(JNIEnv */*e*/, jobject t, jobject m)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Tracker(t).addMarker(local_ref<Marker>(m)).get_jobject();
}

jboolean Java_com_baldzarika_ar_Tracker_start(JNIEnv */*e*/, jobject t)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Tracker(t).start();
}

jboolean Java_com_baldzarika_ar_Tracker_isStarted(JNIEnv */*e*/, jobject t)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Tracker(t).isStarted();
}

jboolean Java_com_baldzarika_ar_Tracker_isActive(JNIEnv */*e*/, jobject t)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Tracker(t).isActive();
}

jboolean Java_com_baldzarika_ar_Tracker_stop(JNIEnv */*e*/, jobject t)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Tracker(t).stop();
}

jboolean Java_com_baldzarika_ar_Tracker_update(JNIEnv */*e*/, jobject t, jobject frame)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Tracker(t).update(local_ref<Frame>(frame));
}

namespace com { namespace baldzarika { namespace ar {

	namespace _Tracker {

		J2CPP_DEFINE_CLASS(MarkerState,"com/baldzarika/ar/Tracker$MarkerState")
		J2CPP_DEFINE_METHOD(MarkerState,0,"<init>","(J)V")
		J2CPP_DEFINE_FIELD(MarkerState,0,"m_px","J")

		MarkerState::jx_t MarkerState::get(MarkerState::px_t const &px)
		{
			if(px->m_any_data.type()==typeid(jref_t))
				return jx_t(boost::any_cast<jref_t>(px->m_any_data));
			return jx_t();
		}

		MarkerState::px_t MarkerState::get(MarkerState::jx_t const &jx)
		{
			if(	px_t *ppx=reinterpret_cast<px_t*>(
					static_cast<jlong>(jx->m_px)
				)
			)
			{
				return *ppx;
			}
			return px_t();
		}

		MarkerState::MarkerState(jobject jobj)
			: j2cpp::object<MarkerState>(jobj)
			, m_px(get_jobject())
		{
		}

		MarkerState::MarkerState(jlong px)
			: j2cpp::object<MarkerState>(
				j2cpp::call_new_object<
					J2CPP_CLASS_NAME,
					J2CPP_METHOD_NAME(0),
					J2CPP_METHOD_SIGNATURE(0)
				>(px))
			, m_px(get_jobject())
		{
		}

		jboolean MarkerState::isDetected()
		{
			if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
				return (*ppx)->is_detected()?JNI_TRUE:JNI_FALSE;
		}

		j2cpp::local_ref<Marker> MarkerState::getMarker()
		{
			if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
				return Marker::get((*ppx)->get_marker());
			return j2cpp::local_ref<Marker>();
		}

		j2cpp::local_ref<j2cpp::android::graphics::Matrix> MarkerState::getHomography()
		{
			if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
			{
				::baldzarika::ucv::matrix33f const &hm=(*ppx)->get_homography_matrix();
				j2cpp::android::graphics::Matrix ret_val;
				ret_val.setValues(j2cpp::array<jfloat, 1>(hm.data(), 9));
				return ret_val;
			}
			return j2cpp::local_ref<j2cpp::android::graphics::Matrix>();
		}

		j2cpp::local_ref< j2cpp::array< j2cpp::local_ref< Point2 >, 1 > >
		MarkerState::getMarkerCorners()
		{
			if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
			{
				::baldzarika::ar::tracker::marker_state::points2_t mcs;
				(*ppx)->get_marker_corners(mcs);

				j2cpp::array< j2cpp::local_ref< Point2 >, 1 > ret_val(mcs.size());

				for(std::size_t mc=0;mc<mcs.size();++mc)
					ret_val[mc]=Point2(
							static_cast<jfloat>(mcs[mc].x),
							static_cast<jfloat>(mcs[mc].y)
					);
				return ret_val;
			}
			return j2cpp::local_ref< j2cpp::array< j2cpp::local_ref< Point2 >, 1 > >();
		}

		jint MarkerState::getFeatureMatchesSize()
		{
			if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
			{
				return (*ppx)->get_frame_points().size();
			}
			return 0;
		}

		void MarkerState::create(jlong px)
		{
			px_t *ppx=reinterpret_cast<px_t *>(px);
			(*ppx)->m_any_data=jref_t(get_jobject());
			m_px=reinterpret_cast<jlong>(ppx);
		}

		void MarkerState::destroy()
		{
			if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
			{
				(*ppx)->m_any_data=boost::any();
				delete ppx;
				m_px=0;
			}
		}

		J2CPP_DEFINE_CLASS(Callback,"com/baldzarika/ar/Tracker$Callback")
		J2CPP_DEFINE_METHOD(Callback,0,"onMarkerStateChanged","(Lcom/baldzarika/ar/Tracker$MarkerState;I)V")
		J2CPP_DEFINE_METHOD(Callback,1,"onTrackerStart","(Lcom/baldzarika/ar/Tracker;)V")
		J2CPP_DEFINE_METHOD(Callback,2,"onTrackerStop","(Lcom/baldzarika/ar/Tracker;)V")
		J2CPP_DEFINE_METHOD(Callback,3,"onTrackerStats","(Lcom/baldzarika/ar/Tracker;I)V")

		Callback::Callback(jobject jobj)
			: j2cpp::object<Callback>(jobj)
		{
		}

		void Callback::onMarkerStateChanged(j2cpp::local_ref<MarkerState> const &ms, jint sc)
		{
			j2cpp::call_method<
				J2CPP_CLASS_NAME,
				J2CPP_METHOD_NAME(0),
				J2CPP_METHOD_SIGNATURE(0),
				void
			>(get_jobject(), ms, sc);
		}

		void Callback::onTrackerStart(j2cpp::local_ref<Tracker> const &t)
		{
			j2cpp::call_method<
				J2CPP_CLASS_NAME,
				J2CPP_METHOD_NAME(1),
				J2CPP_METHOD_SIGNATURE(1),
				void
			>(get_jobject(), t);
		}

		void Callback::onTrackerStop(j2cpp::local_ref<Tracker> const &t)
		{
			j2cpp::call_method<
				J2CPP_CLASS_NAME,
				J2CPP_METHOD_NAME(2),
				J2CPP_METHOD_SIGNATURE(2),
				void
			>(get_jobject(), t);
		}

		void Callback::onTrackerStats(j2cpp::local_ref<Tracker> const &t, jint nff)
		{
			j2cpp::call_method<
				J2CPP_CLASS_NAME,
				J2CPP_METHOD_NAME(3),
				J2CPP_METHOD_SIGNATURE(3),
				void
			>(get_jobject(), t, nff);
		}

	} //namespace _Tracker

	J2CPP_DEFINE_CLASS(Tracker,"com/baldzarika/ar/Tracker")
	J2CPP_DEFINE_FIELD(Tracker,0,"m_px","J")
	J2CPP_DEFINE_FIELD(Tracker,1,"m_cb","Lcom/baldzarika/ar/Tracker$Callback;")


	Tracker::jx_t Tracker::get(Tracker::px_t const &px)
	{
		if(px->m_any_data.type()==typeid(jref_t))
			return jx_t(boost::any_cast<jref_t>(px->m_any_data));
		return jx_t();
	}

	Tracker::px_t Tracker::get(Tracker::jx_t const &jx)
	{
		if(	px_t *ppx=reinterpret_cast<px_t*>(
				static_cast<jlong>(jx->m_px)
			)
		)
		{
			return *ppx;
		}
		return px_t();
	}

	void Tracker::onTrackerStartStop(Tracker::px_t const &t, bool ss)
	{
		if(ss)
		{
			j2cpp::environment::get()->attach_current_thread();
			if(Tracker::jx_t jx=Tracker::get(t))
				if(j2cpp::local_ref< Tracker::Callback > cb=jx->m_cb)
					cb->onTrackerStart(jx);
		}
		else
		{
			if(Tracker::jx_t jx=Tracker::get(t))
				if(j2cpp::local_ref< Tracker::Callback > cb=jx->m_cb)
					cb->onTrackerStop(jx);
			j2cpp::environment::get()->detach_current_thread();
		}
	}

	void Tracker::onTrackerStats(px_t const &t, boost::uint32_t nff)
	{
		if(Tracker::jx_t jx=Tracker::get(t))
			if(j2cpp::local_ref< Tracker::Callback > cb=jx->m_cb)
				cb->onTrackerStats(jx, nff);
	}

	void Tracker::onMarkerStateChanged(Tracker::MarkerState::px_t const &ms, ::baldzarika::ar::tracker::marker_state::eSC sc)
	{
		if(Tracker::jx_t jx=Tracker::get(ms->get_tracker()))
			if(j2cpp::local_ref< Tracker::Callback > mscb=jx->m_cb)
				mscb->onMarkerStateChanged(Tracker::MarkerState::get(ms), static_cast<jint>(sc));
	}

	Tracker::Tracker(jobject jobj)
		: j2cpp::object<Tracker>(jobj)
		, m_px(get_jobject())
		, m_cb(get_jobject())
	{
	}

	j2cpp::local_ref<Size2> Tracker::getFrameSize()
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
			return j2cpp::local_ref<Size2>(
				Size2((*ppx)->get_frame_size().width(),(*ppx)->get_frame_size().height())
			);
		return j2cpp::local_ref<Size2>(Size2(0,0));
	}

	jboolean Tracker::setFrameSize(j2cpp::local_ref<Size2> const &fs)
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
			return (*ppx)->set_frame_size(
				::baldzarika::ucv::size2ui(
					jint(fs->m_Width),
					jint(fs->m_Height)
				)
			)?JNI_TRUE:JNI_FALSE;
		return JNI_FALSE;
	}

	jfloat Tracker::getDetectionTreshold()
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
			return (*ppx)->get_detection_treshold();
		return 0.0f;
	}

	jboolean Tracker::setDetectionTreshold(jfloat dt)
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
			return (*ppx)->set_detection_treshold(dt)?JNI_TRUE:JNI_FALSE;
		return JNI_FALSE;
	}

	jint Tracker::getDetectionMinFeatures()
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
			return (*ppx)->get_detection_min_features();
		return -1;
	}

	jboolean Tracker::setDetectionMinFeatures(jint minFeatures)
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
			return (*ppx)->set_detection_min_features(minFeatures)?JNI_TRUE:JNI_FALSE;
		return JNI_FALSE;
	}

	jint Tracker::getTrackingMaxFeatures()
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
			return (*ppx)->get_tracking_max_features();
		return -1;
	}

	jboolean Tracker::setTrackingMaxFeatures(jint maxFeatures)
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
			return (*ppx)->set_tracking_max_features(maxFeatures)?JNI_TRUE:JNI_FALSE;
		return JNI_FALSE;
	}

	jint Tracker::getTrackingHalfWinSize()
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
			return (*ppx)->get_tracking_half_win_size();
		return -1;
	}

	jboolean Tracker::setTrackingHalfWinSize(jint halfWinSize)
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
			return (*ppx)->set_tracking_half_win_size(halfWinSize)?JNI_TRUE:JNI_FALSE;
		return JNI_FALSE;
	}

	jint Tracker::getTrackingNumLevels()
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
			return (*ppx)->get_tracking_num_levels();
		return -1;
	}

	jboolean Tracker::setTrackingNumLevels(jint numLevels)
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
			return (*ppx)->set_tracking_num_levels(numLevels)?JNI_TRUE:JNI_FALSE;
		return JNI_FALSE;
	}

	jint Tracker::getTrackingMaxIterations()
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
			return (*ppx)->get_tracking_max_iterations();
		return -1;
	}

	jboolean Tracker::setTrackingMaxIterations(jint maxIters)
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
			return (*ppx)->set_tracking_max_iterations(maxIters)?JNI_TRUE:JNI_FALSE;
		return JNI_FALSE;
	}

	jfloat Tracker::getDetectionMaxDiffNorm()
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
			return (*ppx)->get_detection_max_diff_norm();
		return std::numeric_limits<float>::max();
	}

	jboolean Tracker::setDetectionMaxDiffNorm(jfloat maxDiffNorm)
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
			return (*ppx)->set_detection_max_diff_norm(maxDiffNorm)?JNI_TRUE:JNI_FALSE;
		return JNI_FALSE;
	}

	jfloat Tracker::getTrackingMaxDiffNorm()
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
			return (*ppx)->get_tracking_max_diff_norm();
		return std::numeric_limits<float>::max();
	}

	jboolean Tracker::setTrackingMaxDiffNorm(jfloat maxDiffNorm)
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
			return (*ppx)->set_tracking_max_diff_norm(maxDiffNorm)?JNI_TRUE:JNI_FALSE;
		return JNI_FALSE;
	}

	j2cpp::local_ref<Tracker::MarkerState> Tracker::addMarker(j2cpp::local_ref<Marker> const &marker)
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
		{
			if(MarkerState::px_t ms_ppx=(*ppx)->add_marker(Marker::get(marker)))
			{
				return j2cpp::local_ref<Tracker::MarkerState>(
					Tracker::MarkerState(
						reinterpret_cast<jlong>( new MarkerState::px_t(ms_ppx) )
					)
				);
			}
		}
		return j2cpp::local_ref<Tracker::MarkerState>();
	}

	jboolean Tracker::start()
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
			return (*ppx)->start()?JNI_TRUE:JNI_FALSE;
		return JNI_FALSE;
	}

	jboolean Tracker::isStarted()
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
			return (*ppx)->is_started()?JNI_TRUE:JNI_FALSE;
		return JNI_FALSE;
	}

	jboolean Tracker::isActive()
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
			return (*ppx)->is_active()?JNI_TRUE:JNI_FALSE;
		return JNI_FALSE;
	}

	jboolean Tracker::stop()
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
			return (*ppx)->stop()?JNI_TRUE:JNI_FALSE;
		return JNI_FALSE;
	}

	jboolean Tracker::update(j2cpp::local_ref<Frame> const &frame)
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
		{
			if(Frame::px_t *frame_ppx=Frame::get(Frame::jx_t(frame)))
			{
				return (*ppx)->update(
					::baldzarika::ucv::gil::const_view(frame_ppx->second),
					frame_ppx->first
				)?JNI_TRUE:JNI_FALSE;
			}
		}
		return JNI_FALSE;
	}

	void Tracker::create(j2cpp::local_ref<Size2> const &fs)
	{
		px_t *ppx=new px_t(
			new ::baldzarika::ar::tracker(
				::baldzarika::ucv::size2ui(
					jint(fs->m_Width), jint(fs->m_Height)
				)
			)
		);

		(*ppx)->m_any_data=jref_t(get_jobject());

		(*ppx)->marker_state_changed().connect(
			boost::bind(
				&Tracker::onMarkerStateChanged, _1, _2
			)
		);

		(*ppx)->start_stop().connect(
			boost::bind(
				&Tracker::onTrackerStartStop, _1, _2
			)
		);

		(*ppx)->stats().connect(
			boost::bind(
				&Tracker::onTrackerStats, _1, _2
			)
		);

		m_px=reinterpret_cast<jlong>(ppx);
	}

	void Tracker::destroy()
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
		{
			(*ppx)->m_any_data=boost::any();
			delete ppx;
			m_px=0;
		}
	}


} //namespace ar
} //namespace baldzarika
} //namespace com
