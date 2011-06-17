#include <config.h>
#include <com/baldzarika/ar/Point2.h>

namespace com { namespace baldzarika { namespace ar {

	J2CPP_DEFINE_CLASS(Point2,"com/baldzarika/ar/Point2")
	J2CPP_DEFINE_METHOD(Point2,0,"<init>","()V")
	J2CPP_DEFINE_METHOD(Point2,1,"<init>","(FF)V")
	J2CPP_DEFINE_FIELD(Point2,0,"m_X","F")
	J2CPP_DEFINE_FIELD(Point2,1,"m_Y","F")


	Point2::Point2(jobject jobj)
		: j2cpp::object<Point2>(jobj)
		, m_X(get_jobject())
		, m_Y(get_jobject())
	{
	}

	Point2::Point2()
		: j2cpp::object<Point2>(
			j2cpp::call_new_object<
				J2CPP_CLASS_NAME,
				J2CPP_METHOD_NAME(0),
				J2CPP_METHOD_SIGNATURE(0)
			>())
		, m_X(get_jobject())
		, m_Y(get_jobject())
	{
	}


	Point2::Point2(jfloat x, jfloat y)
		: j2cpp::object<Point2>(
			j2cpp::call_new_object<
				J2CPP_CLASS_NAME,
				J2CPP_METHOD_NAME(1),
				J2CPP_METHOD_SIGNATURE(1)
			>(x,y))
		, m_X(get_jobject())
		, m_Y(get_jobject())
	{
	}

	Point2::operator j2cpp::local_ref<j2cpp::java::lang::Object>() const
	{
		return j2cpp::local_ref<j2cpp::java::lang::Object>(get_jobject());
	}

} //namespace ar
} //namespace baldzarika
} //namespace com
