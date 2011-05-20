#include <config.h>
#include <com/baldzarika/ar/fiducial/BCHMarkerModel.h>
#include <com/baldzarika/ar/fiducial/MarkerModel.h>
#include <com_baldzarika_ar_fiducial_BCHMarkerModel.h>

void JNICALL Java_com_baldzarika_ar_fiducial_BCHMarkerModel_create(JNIEnv */*e*/, jobject bch_mm)
{
	using namespace com::baldzarika::ar::fiducial;
	using namespace j2cpp;
	BCHMarkerModel(bch_mm).create();
}

namespace com { namespace baldzarika { namespace ar { namespace fiducial {

	J2CPP_DEFINE_CLASS(BCHMarkerModel,"com/baldzarika/ar/fiducial/BCHMarkerModel")

	BCHMarkerModel::jx_t BCHMarkerModel::get(BCHMarkerModel::px_t const &px)
	{
		if(j2cpp::local_ref<MarkerModel> jx=MarkerModel::get(px))
			return jx_t(jx.get_jobject());
		return jx_t();
	}

	BCHMarkerModel::px_t BCHMarkerModel::get(BCHMarkerModel::jx_t const &jx)
	{
		if(MarkerModel::px_t px=MarkerModel::get(jx))
			return px_t(px, boost::detail::dynamic_cast_tag());
		return px_t();
	}

	BCHMarkerModel::BCHMarkerModel(jobject jobj)
		: j2cpp::object<BCHMarkerModel>(jobj)
	{
	}

	BCHMarkerModel::operator j2cpp::local_ref<MarkerModel>() const
	{
		return j2cpp::local_ref<MarkerModel>(get_jobject());
	}

	void BCHMarkerModel::create()
	{
		px_t px(new ::baldzarika::ar::fiducial::bch_marker_model());
		MarkerModel::tie(px, j2cpp::local_ref<BCHMarkerModel>(get_jobject()));
	}

} //namespace fiducial
} //namespace ar
} //namespace baldzarika
} //namespace com
