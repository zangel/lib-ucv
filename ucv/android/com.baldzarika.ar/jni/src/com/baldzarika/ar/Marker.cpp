#include <config.h>
#include <com_baldzarika_ar_Marker.h>
#include <com/baldzarika/ar/Marker.h>
#include <com/baldzarika/ar/Size2.h>

void Java_com_baldzarika_ar_Marker_create__(JNIEnv */*e*/, jobject m)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	Marker(m).create();
}

void Java_com_baldzarika_ar_Marker_create__Lcom_baldzarika_ar_Size2_2(JNIEnv */*e*/, jobject m, jobject ms)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	Marker(m).create(local_ref<Size2>(ms));
}

void Java_com_baldzarika_ar_Marker_destroy(JNIEnv */*e*/, jobject m)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	Marker(m).destroy();
}

jobject Java_com_baldzarika_ar_Marker_getSize(JNIEnv */*e*/, jobject m)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Marker(m).getSize().get_jobject();
}

jboolean Java_com_baldzarika_ar_Marker_load(JNIEnv */*e*/, jobject m, jstring fileName)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Marker(m).load(local_ref<java::lang::String>(fileName));
}

namespace com { namespace baldzarika { namespace ar {

	J2CPP_DEFINE_CLASS(Marker,"com/baldzarika/ar/Marker")
	J2CPP_DEFINE_FIELD(Marker,0,"m_px","J")

	Marker::jx_t Marker::get(Marker::px_t const &px)
	{
		if(px->m_any_data.type()==typeid(jx_t))
			return boost::any_cast<jx_t>(px->m_any_data);
		return jx_t();
	}

	Marker::px_t Marker::get(Marker::jx_t const &jx)
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(jx->m_px)))
			return *ppx;
		return px_t();
	}

	Marker::Marker(jobject jobj)
		: j2cpp::object<Marker>(jobj)
		, m_px(get_jobject())
	{
	}

	void Marker::create()
	{
		px_t *ppx=new px_t(new ::baldzarika::ar::marker());
		(*ppx)->m_any_data=jx_t(get_jobject());
		m_px=reinterpret_cast<jlong>(ppx);
	}

	void Marker::create(j2cpp::local_ref<Size2> const &ms)
	{
		px_t *ppx=new px_t(
			new ::baldzarika::ar::marker(
				::baldzarika::ucv::size2ui(
					jint(ms->m_Width), jint(ms->m_Height)
				)
			)
		);
		(*ppx)->m_any_data=jx_t(get_jobject());
		m_px=reinterpret_cast<jlong>(ppx);
	}

	void Marker::destroy()
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
		{
			(*ppx)->m_any_data=boost::any();
			delete ppx;
			m_px=0;
		}
	}

	j2cpp::local_ref<Size2>	Marker::getSize()
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
			return j2cpp::local_ref<Size2>(
				Size2((*ppx)->get_size().width(),(*ppx)->get_size().height())
			);
		return j2cpp::local_ref<Size2>(Size2(0,0));
	}

	jboolean Marker::load(j2cpp::local_ref<j2cpp::java::lang::String> const &fileName)
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
		{
			j2cpp::local_ref< j2cpp::array<jbyte,1> > str_bytes=fileName->getBytes();
			return (*ppx)->load(std::string(
					str_bytes->data(),
					str_bytes->data()+str_bytes->size()
				)
			)?JNI_TRUE:JNI_FALSE;
		}
		return JNI_FALSE;
	}

} //namespace ar
} //namespace baldzarika
} //namespace com
