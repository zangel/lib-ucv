#include <config.h>
#include <com/baldzarika/ar/Tracker.h>
#include <com/baldzarika/ar/Size2.h>
#include <com_baldzarika_ar_Tracker.h>
#include <com_baldzarika_ar_Tracker_MarkerState.h>


void Java_com_baldzarika_ar_Tracker_00024MarkerState_initialize(JNIEnv */*e*/, jobject ms, jlong px)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	Tracker::MarkerState(ms).initialize(px);

}
void Java_com_baldzarika_ar_Tracker_00024MarkerState_destroy(JNIEnv */*e*/, jobject ms)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	Tracker::MarkerState(ms).destroy();
}

jobject Java_com_baldzarika_ar_Tracker_00024MarkerState_getTracker(JNIEnv */*e*/, jobject ms)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Tracker::MarkerState(ms).getTracker().get_jobject();
}

jboolean Java_com_baldzarika_ar_Tracker_00024MarkerState_isDetected(JNIEnv */*e*/, jobject ms)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Tracker::MarkerState(ms).isDetected();

}

jobject Java_com_baldzarika_ar_Tracker_00024MarkerState_getMarkerSize(JNIEnv */*e*/, jobject ms)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Tracker::MarkerState(ms).getMarkerSize().get_jobject();
}

jobject Java_com_baldzarika_ar_Tracker_00024MarkerState_getHomography(JNIEnv */*e*/, jobject ms)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Tracker::MarkerState(ms).getHomography().get_jobject();
}

jboolean Java_com_baldzarika_ar_Tracker_00024MarkerState_getCameraPose(JNIEnv */*e*/, jobject ms, jfloatArray cameraPose)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Tracker::MarkerState(ms).getCameraPose(local_ref< array< jfloat,1 > >(cameraPose));
}

void Java_com_baldzarika_ar_Tracker_initialize(JNIEnv */*e*/, jobject t, jlong px)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	Tracker(t).initialize(px);
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

jboolean Java_com_baldzarika_ar_Tracker_setFrameSize(JNIEnv */*e*/, jobject t, jobject frameSize)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Tracker(t).setFrameSize(local_ref<Size2>(frameSize));
}

jfloat Java_com_baldzarika_ar_Tracker_getCameraFovy(JNIEnv */*e*/, jobject t)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Tracker(t).getCameraFovy();
}

jboolean Java_com_baldzarika_ar_Tracker_setCameraFovy(JNIEnv */*e*/, jobject t, jfloat fovy)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Tracker(t).setCameraFovy(fovy);
}

jfloat Java_com_baldzarika_ar_Tracker_getCameraZNear(JNIEnv */*e*/, jobject t)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Tracker(t).getCameraZNear();
}

jboolean Java_com_baldzarika_ar_Tracker_setCameraZNear(JNIEnv */*e*/, jobject t, jfloat zNear)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Tracker(t).setCameraZNear(zNear);
}

jfloat Java_com_baldzarika_ar_Tracker_getCameraZFar(JNIEnv */*e*/, jobject t)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Tracker(t).getCameraZFar();
}

jboolean Java_com_baldzarika_ar_Tracker_setCameraZFar(JNIEnv */*e*/, jobject t, jfloat zFar)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Tracker(t).setCameraZFar(zFar);
}

jfloat Java_com_baldzarika_ar_Tracker_getCameraFocalLength(JNIEnv */*e*/, jobject t)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Tracker(t).getCameraFocalLength();
}

jboolean Java_com_baldzarika_ar_Tracker_getCameraProjection(JNIEnv */*e*/, jobject t, jfloatArray cameraProjection)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Tracker(t).getCameraProjection(local_ref< array< jfloat,1 > >(cameraProjection));
}

jboolean Java_com_baldzarika_ar_Tracker_start(JNIEnv */*e*/, jobject t)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Tracker(t).start();
}

jboolean Java_com_baldzarika_ar_Tracker_isActive(JNIEnv */*e*/, jobject t)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Tracker(t).isActive();
}

jboolean Java_com_baldzarika_ar_Tracker_isStarting(JNIEnv */*e*/, jobject t)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Tracker(t).isStarting();
}

jboolean Java_com_baldzarika_ar_Tracker_isStarted(JNIEnv */*e*/, jobject t)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Tracker(t).isStarted();
}

jboolean Java_com_baldzarika_ar_Tracker_stop(JNIEnv */*e*/, jobject t)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Tracker(t).stop();
}

jboolean Java_com_baldzarika_ar_Tracker_waitToStop(JNIEnv */*e*/, jobject t)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Tracker(t).waitToStop();
}

jboolean Java_com_baldzarika_ar_Tracker_processFrame(JNIEnv */*e*/, jobject t, jbyteArray data, jint pfmt, jint width, jint height)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Tracker(t).processFrame(local_ref< array< jbyte,1> >(data), pfmt, width, height);
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
			if(	px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(jx->m_px)))
				return *ppx;
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

		void MarkerState::initialize(jlong px)
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
				m_px=0;
				delete ppx;
			}
		}

		jboolean MarkerState::isDetected()
		{
			if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
				return (*ppx)->is_detected()?JNI_TRUE:JNI_FALSE;
		}

		j2cpp::local_ref<Tracker> MarkerState::getTracker()
		{
			if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
				return Tracker::get((*ppx)->get_tracker());
			return j2cpp::local_ref<Tracker>();
		}

		j2cpp::local_ref<Size2> MarkerState::getMarkerSize()
		{
			if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
				return j2cpp::local_ref<Size2>(
					Size2((*ppx)->get_marker_size().width(),(*ppx)->get_marker_size().height())
				);
			return j2cpp::local_ref<Size2>(Size2(0,0));
		}

		j2cpp::local_ref<j2cpp::android::graphics::Matrix> MarkerState::getHomography()
		{
			if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
			{
				::baldzarika::math::matrix33f const &hm=(*ppx)->get_homography();
				j2cpp::android::graphics::Matrix ret_val;
				ret_val.setValues(j2cpp::array<jfloat, 1>(hm.data(), 9));
				return ret_val;
			}
			return j2cpp::local_ref<j2cpp::android::graphics::Matrix>();
		}

		jboolean MarkerState::getCameraPose(j2cpp::local_ref< j2cpp::array<jfloat,1> > const &cameraPose)
		{
			if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
			{
				if(cameraPose->length()==16)
				{
					jfloat *dst=cameraPose->data();
					::baldzarika::math::matrix44f const &cp=(*ppx)->get_camera_pose();
					for(boost::uint32_t c=0;c<4;++c)
						for(boost::uint32_t r=0;r<4;++r)
							*dst++=cp(r,c);
					return JNI_TRUE;
				}

			}
			return JNI_FALSE;
		}



		J2CPP_DEFINE_CLASS(Callback,"com/baldzarika/ar/Tracker$Callback")
		J2CPP_DEFINE_METHOD(Callback,0,"onRunningStateChanged","(Lcom/baldzarika/Tracker;I)V")
		J2CPP_DEFINE_METHOD(Callback,1,"onMarkerStateChanged","(Lcom/baldzarika/ar/Tracker$MarkerState;I)V")

		Callback::Callback(jobject jobj)
			: j2cpp::object<Callback>(jobj)
		{
		}

		void Callback::onRunningStateChanged(j2cpp::local_ref<Tracker> const &tracker, jint rs)
		{
			j2cpp::call_method<
				J2CPP_CLASS_NAME,
				J2CPP_METHOD_NAME(0),
				J2CPP_METHOD_SIGNATURE(0),
				void
			>(get_jobject(), tracker, rs);
		}

		void Callback::onMarkerStateChanged(j2cpp::local_ref<MarkerState> const &markerState, jint sc)
		{
			j2cpp::call_method<
				J2CPP_CLASS_NAME,
				J2CPP_METHOD_NAME(1),
				J2CPP_METHOD_SIGNATURE(1),
				void
			>(get_jobject(), markerState, sc);
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
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(jx->m_px)))
			return *ppx;
		return px_t();
	}

	Tracker::Tracker(jobject jobj)
		: j2cpp::object<Tracker>(jobj)
		, m_px(get_jobject())
		, m_cb(get_jobject())
	{
	}

	void Tracker::initialize(jlong px)
	{
		px_t *ppx=reinterpret_cast<px_t *>(px);
		(*ppx)->m_any_data=jref_t(get_jobject());
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

	j2cpp::local_ref<Size2>	Tracker::getFrameSize()
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
				::baldzarika::math::size2ui(
					jint(fs->m_Width),
					jint(fs->m_Height)
				)
			)?JNI_TRUE:JNI_FALSE;
		return JNI_FALSE;
	}

	jfloat Tracker::getCameraFovy()
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
			return (*ppx)->get_camera_fovy();
		return 0.0f;
	}

	jboolean Tracker::setCameraFovy(jfloat fovy)
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
			return (*ppx)->set_camera_fovy(fovy)?JNI_TRUE:JNI_FALSE;
		return JNI_FALSE;
	}

	jfloat Tracker::getCameraZNear()
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
			return (*ppx)->get_camera_z_near();
		return 0.0f;
	}

	jboolean Tracker::setCameraZNear(jfloat zNear)
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
			return (*ppx)->set_camera_z_near(zNear)?JNI_TRUE:JNI_FALSE;
		return JNI_FALSE;
	}

	jfloat Tracker::getCameraZFar()
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
			return (*ppx)->get_camera_z_far();
		return 0.0f;
	}

	jboolean Tracker::setCameraZFar(jfloat zFar)
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
			return (*ppx)->set_camera_z_far(zFar)?JNI_TRUE:JNI_FALSE;
		return JNI_FALSE;
	}

	jfloat Tracker::getCameraFocalLength()
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
			return (*ppx)->get_camera_focal_length();
		return 0.0f;
	}

	jboolean Tracker::getCameraProjection(j2cpp::local_ref< j2cpp::array<jfloat,1> > const &cameraProjection)
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
		{
			if(cameraProjection->length()==16)
			{
				jfloat *dst=cameraProjection->data();
				::baldzarika::math::matrix44f const &cp=(*ppx)->get_camera_projection();

				for(boost::uint32_t c=0;c<4;++c)
					for(boost::uint32_t r=0;r<4;++r)
						*dst++=cp(r,c);
				return JNI_TRUE;
			}
		}
		return JNI_FALSE;
	}

	jboolean Tracker::start()
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
			return (*ppx)->start()?JNI_TRUE:JNI_FALSE;
		return JNI_FALSE;
	}

	jboolean Tracker::isActive()
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
			return (*ppx)->is_active()?JNI_TRUE:JNI_FALSE;
		return JNI_FALSE;
	}

	jboolean Tracker::isStarting()
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
			return (*ppx)->is_starting()?JNI_TRUE:JNI_FALSE;
		return JNI_FALSE;
	}

	jboolean Tracker::isStarted()
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
			return (*ppx)->is_started()?JNI_TRUE:JNI_FALSE;
		return JNI_FALSE;
	}

	jboolean Tracker::stop()
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
			return (*ppx)->stop()?JNI_TRUE:JNI_FALSE;
		return JNI_FALSE;
	}

	jboolean Tracker::waitToStop()
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
			return (*ppx)->wait_to_stop()?JNI_TRUE:JNI_FALSE;
		return JNI_FALSE;
	}

	jboolean Tracker::processFrame(j2cpp::local_ref< j2cpp::array<jbyte,1> > const &data, jint pfmt, jint width, jint height)
	{
		if(px_t *p_px=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
		{
			switch(pfmt)
			{
			case 17:
				{
					return (*p_px)->process_frame(
							::baldzarika::ucv::gil::interleaved_view(
							width, height,
							reinterpret_cast< ::baldzarika::ucv::gil::gray8_pixel_t const * >(data->data()),
							width
						)
					)?JNI_TRUE:JNI_FALSE;
				} break;
			}
		}
		return JNI_FALSE;
	}



} //namespace ar
} //namespace baldzarika
} //namespace com
