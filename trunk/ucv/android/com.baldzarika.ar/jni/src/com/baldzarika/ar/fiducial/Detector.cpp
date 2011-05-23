#include <config.h>
#include <com/baldzarika/ar/fiducial/Detector.h>
#include <com/baldzarika/ar/Size2.h>
#include <com/baldzarika/ar/fiducial/MarkerModel.h>
#include <com_baldzarika_ar_fiducial_Detector.h>
#include <com_baldzarika_ar_fiducial_Detector_MarkerState.h>

void Java_com_baldzarika_ar_fiducial_Detector_00024MarkerState_create(JNIEnv */*e*/, jobject ms, jlong px)
{
	using namespace com::baldzarika::ar;
	using namespace com::baldzarika::ar::fiducial;
	using namespace j2cpp;
	Detector::MarkerState(ms).create(px);
}

void Java_com_baldzarika_ar_fiducial_Detector_00024MarkerState_destroy(JNIEnv */*e*/, jobject ms)
{
	using namespace com::baldzarika::ar;
	using namespace com::baldzarika::ar::fiducial;
	using namespace j2cpp;
	Detector::MarkerState(ms).destroy();
}

jboolean Java_com_baldzarika_ar_fiducial_Detector_00024MarkerState_isDetected(JNIEnv */*e*/, jobject ms)
{
	using namespace com::baldzarika::ar;
	using namespace com::baldzarika::ar::fiducial;
	using namespace j2cpp;
	return Detector::MarkerState(ms).isDetected();
}

jint Java_com_baldzarika_ar_fiducial_Detector_00024MarkerState_getMarkerId(JNIEnv */*e*/, jobject ms)
{
	using namespace com::baldzarika::ar;
	using namespace com::baldzarika::ar::fiducial;
	using namespace j2cpp;
	return Detector::MarkerState(ms).getMarkerId();
}

jobject Java_com_baldzarika_ar_fiducial_Detector_00024MarkerState_getMarkerModel(JNIEnv */*e*/, jobject ms)
{
	using namespace com::baldzarika::ar;
	using namespace com::baldzarika::ar::fiducial;
	using namespace j2cpp;
	return Detector::MarkerState(ms).getMarkerModel().get_jobject();
}

jobject Java_com_baldzarika_ar_fiducial_Detector_00024MarkerState_getDetector(JNIEnv */*e*/, jobject ms)
{
	using namespace com::baldzarika::ar;
	using namespace com::baldzarika::ar::fiducial;
	using namespace j2cpp;
	return Detector::MarkerState(ms).getDetector().get_jobject();
}

jobject Java_com_baldzarika_ar_fiducial_Detector_00024MarkerState_getHomography(JNIEnv */*e*/, jobject ms)
{
	using namespace com::baldzarika::ar;
	using namespace com::baldzarika::ar::fiducial;
	using namespace j2cpp;
	return Detector::MarkerState(ms).getHomography().get_jobject();
}

jboolean JNICALL Java_com_baldzarika_ar_fiducial_Detector_00024MarkerState_getCameraPose(JNIEnv */*e*/, jobject ms, jfloatArray cameraPose)
{
	using namespace com::baldzarika::ar::fiducial;
	using namespace j2cpp;
	return Detector::MarkerState(ms).getCameraPose(local_ref< array< jfloat,1 > >(cameraPose));
}

void Java_com_baldzarika_ar_fiducial_Detector_create(JNIEnv */*e*/, jobject d, jobject frameSize)
{
	using namespace com::baldzarika::ar;
	using namespace com::baldzarika::ar::fiducial;
	using namespace j2cpp;
	Detector(d).create(local_ref<Size2>(frameSize));
}

void Java_com_baldzarika_ar_fiducial_Detector_destroy(JNIEnv */*e*/, jobject d)
{
	using namespace com::baldzarika::ar::fiducial;
	using namespace j2cpp;
	Detector(d).destroy();
}

jboolean Java_com_baldzarika_ar_fiducial_Detector_addMarkerModel(JNIEnv */*e*/, jobject d, jobject markerModel)
{
	using namespace com::baldzarika::ar::fiducial;
	using namespace j2cpp;
	return Detector(d).addMarkerModel(local_ref<MarkerModel>(markerModel));
}

jboolean Java_com_baldzarika_ar_fiducial_Detector_removeMarkerModel(JNIEnv */*e*/, jobject d, jobject markerModel)
{
	using namespace com::baldzarika::ar::fiducial;
	using namespace j2cpp;
	return Detector(d).removeMarkerModel(local_ref<MarkerModel>(markerModel));
}

jobject Java_com_baldzarika_ar_fiducial_Detector_getFrameSize(JNIEnv */*e*/, jobject d)
{
	using namespace com::baldzarika::ar;
	using namespace com::baldzarika::ar::fiducial;
	using namespace j2cpp;
	return Detector(d).getFrameSize().get_jobject();
}

jboolean Java_com_baldzarika_ar_fiducial_Detector_setFrameSize(JNIEnv */*e*/, jobject d, jobject frameSize)
{
	using namespace com::baldzarika::ar;
	using namespace com::baldzarika::ar::fiducial;
	using namespace j2cpp;
	return Detector(d).setFrameSize(local_ref<Size2>(frameSize));
}

jfloat Java_com_baldzarika_ar_fiducial_Detector_getCameraFovy(JNIEnv */*e*/, jobject d)
{
	using namespace com::baldzarika::ar::fiducial;
	using namespace j2cpp;
	return Detector(d).getCameraFovy();
}

jboolean Java_com_baldzarika_ar_fiducial_Detector_setCameraFovy(JNIEnv */*e*/, jobject d, jfloat fovy)
{
	using namespace com::baldzarika::ar::fiducial;
	using namespace j2cpp;
	return Detector(d).setCameraFovy(fovy);
}

jfloat Java_com_baldzarika_ar_fiducial_Detector_getCameraFocalLength(JNIEnv */*e*/, jobject d)
{
	using namespace com::baldzarika::ar::fiducial;
	using namespace j2cpp;
	return Detector(d).getCameraFocalLength();
}

jboolean Java_com_baldzarika_ar_fiducial_Detector_getCameraProjection(JNIEnv */*e*/, jobject d, jfloatArray cameraProjection)
{
	using namespace com::baldzarika::ar::fiducial;
	using namespace j2cpp;
	return Detector(d).getCameraProjection(local_ref< array< jfloat,1 > >(cameraProjection));
}

jboolean Java_com_baldzarika_ar_fiducial_Detector_start(JNIEnv */*e*/, jobject d)
{
	using namespace com::baldzarika::ar::fiducial;
	using namespace j2cpp;
	return Detector(d).start();
}

jboolean Java_com_baldzarika_ar_fiducial_Detector_isActive(JNIEnv */*e*/, jobject d)
{
	using namespace com::baldzarika::ar::fiducial;
	using namespace j2cpp;
	return Detector(d).isActive();
}

jboolean Java_com_baldzarika_ar_fiducial_Detector_isStarting(JNIEnv */*e*/, jobject d)
{
	using namespace com::baldzarika::ar::fiducial;
	using namespace j2cpp;
	return Detector(d).isStarting();
}

jboolean Java_com_baldzarika_ar_fiducial_Detector_isStarted(JNIEnv */*e*/, jobject d)
{
	using namespace com::baldzarika::ar::fiducial;
	using namespace j2cpp;
	return Detector(d).isStarted();
}

jboolean Java_com_baldzarika_ar_fiducial_Detector_stop(JNIEnv */*e*/, jobject d)
{
	using namespace com::baldzarika::ar::fiducial;
	using namespace j2cpp;
	return Detector(d).stop();
}

jboolean Java_com_baldzarika_ar_fiducial_Detector_waitToStop(JNIEnv */*e*/, jobject d)
{
	using namespace com::baldzarika::ar::fiducial;
	using namespace j2cpp;
	return Detector(d).waitToStop();
}

jboolean Java_com_baldzarika_ar_fiducial_Detector_update(JNIEnv */*e*/, jobject d, jbyteArray data, jint pfmt)
{
	using namespace com::baldzarika::ar::fiducial;
	using namespace j2cpp;
	return Detector(d).update(local_ref< array< jbyte,1> >(data), pfmt);
}

namespace com { namespace baldzarika { namespace ar { namespace fiducial {

	namespace _Detector {

		J2CPP_DEFINE_CLASS(MarkerState,"com/baldzarika/ar/fiducial/Detector$MarkerState")
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

		jint MarkerState::getMarkerId()
		{
			if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
				return (*ppx)->get_marker_id();
			return ::baldzarika::ar::fiducial::NULL_MARKER_ID;
		}

		j2cpp::local_ref<MarkerModel> MarkerState::getMarkerModel()
		{
			if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
				return MarkerModel::get((*ppx)->get_marker_model());
			return j2cpp::local_ref<MarkerModel>();
		}

		j2cpp::local_ref<Detector> MarkerState::getDetector()
		{
			if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
				return Detector::get((*ppx)->get_detector());
			return j2cpp::local_ref<Detector>();
		}

		j2cpp::local_ref<j2cpp::android::graphics::Matrix> MarkerState::getHomography()
		{
			if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
			{
				::baldzarika::ucv::matrix33f const &hm=(*ppx)->get_homography();
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
					::baldzarika::ucv::matrix44f const &cp=(*ppx)->get_camera_pose();
					for(boost::uint32_t r=0;r<4;++r)
						for(boost::uint32_t c=0;c<4;++c)
							*dst++=cp(r,c);
					return JNI_TRUE;
				}

			}
			return JNI_FALSE;
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

		J2CPP_DEFINE_CLASS(Callback,"com/baldzarika/ar/fiducial/Detector$Callback")
		J2CPP_DEFINE_METHOD(Callback,0,"onRunningStateChanged","(Lcom/baldzarika/ar/fiducial/Detector;I)V")
		J2CPP_DEFINE_METHOD(Callback,1,"onMarkerStateChanged","(Lcom/baldzarika/ar/fiducial/Detector$MarkerState;I)V")

		Callback::Callback(jobject jobj)
			: j2cpp::object<Callback>(jobj)
		{
		}

		void Callback::onRunningStateChanged(j2cpp::local_ref<Detector> const &detector, jint rs)
		{
			j2cpp::call_method<
				J2CPP_CLASS_NAME,
				J2CPP_METHOD_NAME(0),
				J2CPP_METHOD_SIGNATURE(0),
				void
			>(get_jobject(), detector, rs);
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

	} //namespace _Detector

	J2CPP_DEFINE_CLASS(Detector,"com/baldzarika/ar/fiducial/Detector")
	J2CPP_DEFINE_FIELD(Detector,0,"m_px","J")
	J2CPP_DEFINE_FIELD(Detector,1,"m_cb","Lcom/baldzarika/ar/fiducial/Detector$Callback;")

	Detector::jx_t Detector::get(Detector::px_t const &px)
	{
		if(px->m_any_data.type()==typeid(jref_t))
			return jx_t(boost::any_cast<jref_t>(px->m_any_data));
		return jx_t();
	}

	Detector::px_t Detector::get(Detector::jx_t const &jx)
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

	void Detector::onDetectorRunningStateChanged(px_t const &d, ::baldzarika::ar::fiducial::detector::eRunningState rs)
	{
		if(rs==::baldzarika::ar::fiducial::detector::RS_STARTING)
			j2cpp::environment::get()->attach_current_thread();

		if(Detector::jx_t jx=Detector::get(d))
			if(j2cpp::local_ref< Detector::Callback > cb=jx->m_cb)
				cb->onRunningStateChanged(jx, rs);

		if(rs==::baldzarika::ar::fiducial::detector::RS_STOPPED)
			j2cpp::environment::get()->detach_current_thread();
	}

	void Detector::onMarkerStateChanged(MarkerState::px_t const &ms, ::baldzarika::ar::fiducial::detector::marker_state::eSC sc)
	{
		j2cpp::local_ref<MarkerState> jms=MarkerState::get(ms);

		if(sc==::baldzarika::ar::fiducial::detector::marker_state::SC_DETECTION && ms->is_detected())
		{
			jms=j2cpp::local_ref<MarkerState>(
				MarkerState(
					reinterpret_cast<jlong>( new MarkerState::px_t(ms))
				)
			);
		}

		if(Detector::jx_t jx=Detector::get(ms->get_detector()))
			if(j2cpp::local_ref< Detector::Callback > cb=jx->m_cb)
				cb->onMarkerStateChanged(jms, sc);

		if(sc==::baldzarika::ar::fiducial::detector::marker_state::SC_DETECTION && !ms->is_detected())
		{
			jms->destroy();
		}
	}

	Detector::Detector(jobject jobj)
		: j2cpp::object<Detector>(jobj)
		, m_px(get_jobject())
		, m_cb(get_jobject())
	{
	}

	void Detector::create(j2cpp::local_ref<Size2> const &fs)
	{
		px_t *ppx=new px_t(
			new ::baldzarika::ar::fiducial::detector(
				::baldzarika::ucv::size2ui(
					jint(fs->m_Width), jint(fs->m_Height)
				)
			)
		);
		(*ppx)->m_any_data=jref_t(get_jobject());
		m_px=reinterpret_cast<jlong>(ppx);

		(*ppx)->running_state_changed().connect(
			boost::bind(
				&Detector::onDetectorRunningStateChanged, _1, _2
			)
		);

		(*ppx)->marker_state_changed().connect(
			boost::bind(
				&Detector::onMarkerStateChanged, _1, _2
			)
		);
	}

	void Detector::destroy()
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
		{
			(*ppx)->m_any_data=boost::any();
			delete ppx;
			m_px=0;
		}
	}

	jboolean Detector::addMarkerModel(j2cpp::local_ref<MarkerModel> const &markerModel)
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
		{
			if(MarkerModel::px_t mm=MarkerModel::get(markerModel))
			{
				return (*ppx)->add_marker_model(mm)?
					JNI_TRUE:
					JNI_FALSE;
			}
		}
		return JNI_FALSE;
	}

	jboolean Detector::removeMarkerModel(j2cpp::local_ref<MarkerModel> const &markerModel)
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
		{
			if(MarkerModel::px_t mm=MarkerModel::get(markerModel))
			{
				return (*ppx)->remove_marker_model(mm)?
					JNI_TRUE:
					JNI_FALSE;
			}
		}
		return JNI_FALSE;
	}

	j2cpp::local_ref<Size2>	Detector::getFrameSize()
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
			return j2cpp::local_ref<Size2>(
				Size2((*ppx)->get_frame_size().width(),(*ppx)->get_frame_size().height())
			);
		return j2cpp::local_ref<Size2>(Size2(0,0));
	}

	jboolean Detector::setFrameSize(j2cpp::local_ref<Size2> const &fs)
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

	jfloat Detector::getCameraFovy()
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
			return (*ppx)->get_camera_fovy();
		return 0.0f;
	}

	jboolean Detector::setCameraFovy(jfloat fovy)
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
			return (*ppx)->set_camera_fovy(fovy)?JNI_TRUE:JNI_FALSE;
		return JNI_FALSE;
	}

	jfloat Detector::getCameraFocalLength()
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
			return (*ppx)->get_camera_focal_length();
		return 0.0f;
	}

	jboolean Detector::getCameraProjection(j2cpp::local_ref< j2cpp::array<jfloat,1> > const &cameraProjection)
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
		{
			if(cameraProjection->length()==16)
			{
				jfloat *dst=cameraProjection->data();
				::baldzarika::ucv::matrix44f const &cp=(*ppx)->get_camera_projection();
				for(boost::uint32_t r=0;r<4;++r)
					for(boost::uint32_t c=0;c<4;++c)
						*dst++=cp(r,c);
				return JNI_TRUE;
			}
		}
		return JNI_FALSE;
	}

	jboolean Detector::start()
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
		{
			return (*ppx)->start()?
				JNI_TRUE:
				JNI_FALSE;
		}
		return JNI_FALSE;
	}

	jboolean Detector::isActive()
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
		{
			return (*ppx)->is_active()?
				JNI_TRUE:
				JNI_FALSE;
		}
		return JNI_FALSE;
	}

	jboolean Detector::isStarting()
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
		{
			return (*ppx)->is_starting()?
				JNI_TRUE:
				JNI_FALSE;
		}
		return JNI_FALSE;
	}

	jboolean Detector::isStarted()
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
		{
			return (*ppx)->is_started()?
				JNI_TRUE:
				JNI_FALSE;
		}
		return JNI_FALSE;
	}

	jboolean Detector::stop()
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
		{
			return (*ppx)->stop()?
				JNI_TRUE:
				JNI_FALSE;
		}
		return JNI_FALSE;
	}

	jboolean Detector::waitToStop()
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
		{
			return (*ppx)->wait_to_stop()?
				JNI_TRUE:
				JNI_FALSE;
		}
		return JNI_FALSE;
	}

	jboolean Detector::update(j2cpp::local_ref< j2cpp::array<jbyte,1> > const &data, jint pfmt)
	{
		if(px_t *p_px=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
		{
			::baldzarika::ucv::size2ui frameSize=(*p_px)->get_frame_size();
			switch(pfmt)
			{
			case 17:
				{
					if(data->size()>=(frameSize.area()+frameSize.width()*frameSize.height()/2))
					{
						if(::baldzarika::ar::fiducial::detector::locked_frame frame_lock=(*p_px)->lock_frame())
						{
							::baldzarika::ar::fiducial::gray_t median;
#if 0
							return ::baldzarika::ucv::convert_nv16_to_gray(
								//y channel
								::baldzarika::ucv::gil::interleaved_view(
									frameSize.width(), frameSize.height(),
									reinterpret_cast< ::baldzarika::ucv::gil::gray8_pixel_t * >(data->data()),
									frameSize.width()
								),
								//uv channels
								::baldzarika::ucv::gil::interleaved_view(
									frameSize.width()/2, frameSize.height()/2,
									reinterpret_cast< ::baldzarika::ucv::gil::gray16_pixel_t * >(data->data()+frameSize.width()*frameSize.height()),
									(frameSize.width()/2)*2
								),
								frame_lock.get_view(),
								median
							)?JNI_TRUE:JNI_FALSE;
#else
							return ::baldzarika::ucv::convert_scale(
								//y channel
								::baldzarika::ucv::gil::interleaved_view(
									frameSize.width(), frameSize.height(),
									reinterpret_cast< ::baldzarika::ucv::gil::gray8_pixel_t * >(data->data()),
									frameSize.width()
								),
								frame_lock.get_view(),
								::baldzarika::ar::fiducial::gray_t(1.0f/255.0f),
								median
							)?JNI_TRUE:JNI_FALSE;
#endif
						}
					}
				}
				break;
			}
		}
		return JNI_FALSE;
	}

} //namespace fiducial
} //namespace ar
} //namespace baldzarika
} //namespace com
