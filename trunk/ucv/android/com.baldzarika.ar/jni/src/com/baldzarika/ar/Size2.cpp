#include <config.h>
#include <com/baldzarika/ar/Size2.h>

namespace com { namespace baldzarika { namespace ar {

	J2CPP_DEFINE_CLASS(Size2,"com/baldzarika/ar/Size2")
	J2CPP_DEFINE_METHOD(Size2,0,"<init>","()V")
	J2CPP_DEFINE_METHOD(Size2,1,"<init>","(II)V")
	J2CPP_DEFINE_FIELD(Size2,0,"m_Width","I")
	J2CPP_DEFINE_FIELD(Size2,1,"m_Height","I")


	Size2::Size2(jobject jobj)
		: j2cpp::object<Size2>(jobj)
		, m_Width(get_jobject())
		, m_Height(get_jobject())
	{
	}

	Size2::Size2()
		: j2cpp::object<Size2>(
			j2cpp::call_new_object<
				J2CPP_CLASS_NAME,
				J2CPP_METHOD_NAME(0),
				J2CPP_METHOD_SIGNATURE(0)
			>())
		, m_Width(get_jobject())
		, m_Height(get_jobject())
	{
	}


	Size2::Size2(jint width, jint height)
		: j2cpp::object<Size2>(
			j2cpp::call_new_object<
				J2CPP_CLASS_NAME,
				J2CPP_METHOD_NAME(1),
				J2CPP_METHOD_SIGNATURE(1)
			>(width,height))
		, m_Width(get_jobject())
		, m_Height(get_jobject())
	{
	}

} //namespace ar
} //namespace baldzarika
} //namespace com
