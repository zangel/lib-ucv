#include <config.h>
#include <com/baldzarika/ar/fiducial/BCHMarkerModel.h>
#include <com/baldzarika/ar/fiducial/MarkerModel.h>
#include <com_baldzarika_ar_fiducial_BCHMarkerModel.h>

#pragma GCC visibility push(default)

jlong Java_com_baldzarika_ar_fiducial_BCHMarkerModel_create(JNIEnv */*e*/, jclass /*BCHMarkerModel.class*/ )
{
	using namespace com::baldzarika::ar::fiducial;
	using namespace j2cpp;
	return BCHMarkerModel::create();
}

#pragma GCC visibility pop

namespace com { namespace baldzarika { namespace ar { namespace fiducial {

	J2CPP_DEFINE_CLASS(BCHMarkerModel,"com/baldzarika/ar/fiducial/BCHMarkerModel")

	BCHMarkerModel::BCHMarkerModel(jobject jobj)
		: j2cpp::object<BCHMarkerModel>(jobj)
	{
	}

	BCHMarkerModel::operator j2cpp::local_ref<MarkerModel>() const
	{
		return j2cpp::local_ref<MarkerModel>(get_jobject());
	}

	jlong BCHMarkerModel::create()
	{
		return reinterpret_cast<jlong>( new MarkerModel::px_t(
			new ::baldzarika::ar::fiducial::bch_marker_model()
			)
		);
	}

} //namespace fiducial
} //namespace ar
} //namespace baldzarika
} //namespace com
