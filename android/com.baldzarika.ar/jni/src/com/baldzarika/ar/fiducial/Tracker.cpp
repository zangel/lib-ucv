#include <config.h>
#include <com/baldzarika/ar/Tracker.h>
#include <com/baldzarika/ar/fiducial/Tracker.h>
#include <com/baldzarika/ar/Size2.h>
#include <com/baldzarika/ar/fiducial/MarkerModel.h>
#include <com_baldzarika_ar_fiducial_Tracker.h>
#include <com_baldzarika_ar_fiducial_Tracker_MarkerState.h>

#pragma GCC visibility push(default)

jint Java_com_baldzarika_ar_fiducial_Tracker_00024MarkerState_getMarkerId(JNIEnv */*e*/, jobject ms)
{
	using namespace com::baldzarika::ar::fiducial;
	using namespace j2cpp;
	return Tracker::MarkerState(ms).getMarkerId();
}

jobject Java_com_baldzarika_ar_fiducial_Tracker_00024MarkerState_getMarkerModel(JNIEnv */*e*/, jobject ms)
{
	using namespace com::baldzarika::ar::fiducial;
	using namespace j2cpp;
	return Tracker::MarkerState(ms).getMarkerModel().get_jobject();
}

jlong Java_com_baldzarika_ar_fiducial_Tracker_create(JNIEnv */*e*/, jclass /*trackerClazz*/, jobject frameSize)
{
	using namespace com::baldzarika::ar::fiducial;
	using namespace j2cpp;
	return Tracker::create(local_ref<com::baldzarika::ar::Size2>(frameSize));
}

jboolean Java_com_baldzarika_ar_fiducial_Tracker_addMarkerModel(JNIEnv */*e*/, jobject t, jobject markerModel)
{
	using namespace com::baldzarika::ar::fiducial;
	using namespace j2cpp;
	return Tracker(t).addMarkerModel(local_ref<MarkerModel>(markerModel));
}

jboolean Java_com_baldzarika_ar_fiducial_Tracker_removeMarkerModel(JNIEnv */*e*/, jobject t, jobject markerModel)
{
	using namespace com::baldzarika::ar::fiducial;
	using namespace j2cpp;
	return Tracker(t).removeMarkerModel(local_ref<MarkerModel>(markerModel));
}

#pragma GCC visibility pop

namespace com { namespace baldzarika { namespace ar { namespace fiducial {

	namespace _Tracker {

		J2CPP_DEFINE_CLASS(MarkerState,"com/baldzarika/ar/fiducial/Tracker$MarkerState")
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

		jint MarkerState::getMarkerId()
		{
			if(px_t px=get_px())
				return px->get_marker_id();
			return ::baldzarika::ar::fiducial::NULL_MARKER_ID;
		}

		j2cpp::local_ref<MarkerModel> MarkerState::getMarkerModel()
		{
			if(px_t px=get_px())
				return MarkerModel::get(px->get_marker_model());
			return j2cpp::local_ref<MarkerModel>();
		}

	} //namespace _Tracker

	J2CPP_DEFINE_CLASS(Tracker,"com/baldzarika/ar/fiducial/Tracker")

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
			new ::baldzarika::ar::fiducial::tracker(
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

	jboolean Tracker::addMarkerModel(j2cpp::local_ref<MarkerModel> const &markerModel)
	{
		if(px_t px=get_px())
		{
			if(MarkerModel::px_t mm=MarkerModel::get(markerModel))
				return px->add_marker_model(mm)?JNI_TRUE:JNI_FALSE;
		}
		return JNI_FALSE;
	}

	jboolean Tracker::removeMarkerModel(j2cpp::local_ref<MarkerModel> const &markerModel)
	{
		if(px_t px=get_px())
		{
			if(MarkerModel::px_t mm=MarkerModel::get(markerModel))
				return px->remove_marker_model(mm)?JNI_TRUE:JNI_FALSE;
		}
		return JNI_FALSE;
	}

} //namespace fiducial
} //namespace ar
} //namespace baldzarika
} //namespace com
