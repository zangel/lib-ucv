#include <config.h>
#include <com/baldzarika/ar/fiducial/MarkerModel.h>
#include <com_baldzarika_ar_fiducial_MarkerModel.h>

#pragma GCC visibility push(default)

void Java_com_baldzarika_ar_fiducial_MarkerModel_initialize(JNIEnv */*e*/, jobject mm, jlong px)
{
	using namespace com::baldzarika::ar::fiducial;
	using namespace j2cpp;
	MarkerModel(mm).initialize(px);
}

void Java_com_baldzarika_ar_fiducial_MarkerModel_destroy(JNIEnv */*E*/, jobject mm)
{
	using namespace com::baldzarika::ar::fiducial;
	using namespace j2cpp;
	MarkerModel(mm).destroy();
}

#pragma GCC visibility pop

namespace com { namespace baldzarika { namespace ar { namespace fiducial {

	J2CPP_DEFINE_CLASS(MarkerModel,"com/baldzarika/ar/fiducial/MarkerModel")
	J2CPP_DEFINE_FIELD(MarkerModel,0,"m_px","J")

	MarkerModel::jx_t MarkerModel::get(MarkerModel::px_t const &px)
	{
		if(px->m_any_data.type()==typeid(jref_t))
			return jx_t(boost::any_cast<jref_t>(px->m_any_data));
		return jx_t();
	}

	MarkerModel::px_t MarkerModel::get(MarkerModel::jx_t const &jx)
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(jx->m_px)))
			return *ppx;
		return px_t();
	}

	MarkerModel::MarkerModel(jobject jobj)
		: j2cpp::object<MarkerModel>(jobj)
		, m_px(get_jobject())
	{
	}

	void MarkerModel::initialize(jlong px)
	{
		px_t *ppx=reinterpret_cast<px_t *>(px);
		(*ppx)->m_any_data=jref_t(get_jobject());
		m_px=reinterpret_cast<jlong>(ppx);
	}

	void MarkerModel::destroy()
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
		{
			(*ppx)->m_any_data=boost::any();
			m_px=0;
			delete ppx;
		}
	}
} //namespace fiducial
} //namespace ar
} //namespace baldzarika
} //namespace com
