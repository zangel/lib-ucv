#ifndef COM_BALDZARIKA_AR_POINT2_H
#define COM_BALDZARIKA_AR_POINT2_H

namespace com { namespace baldzarika { namespace ar {

	class Point2
		: public j2cpp::object<Point2>
	{
	public:
		J2CPP_DECLARE_CLASS

		J2CPP_DECLARE_METHOD(0)
		J2CPP_DECLARE_METHOD(1)

		J2CPP_DECLARE_FIELD(0)
		J2CPP_DECLARE_FIELD(1)

		explicit Point2(jobject jobj);

		Point2();
		Point2(jfloat x, jfloat y);

		operator j2cpp::local_ref<j2cpp::java::lang::Object>() const;

		j2cpp::field<
			J2CPP_CLASS_NAME,
			J2CPP_FIELD_NAME(0),
			J2CPP_FIELD_SIGNATURE(0),
			jfloat
		> m_X;

		j2cpp::field<
			J2CPP_CLASS_NAME,
			J2CPP_FIELD_NAME(1),
			J2CPP_FIELD_SIGNATURE(1),
			jfloat
		> m_Y;
	};

} //namespace ar
} //namespace baldzarika
} //namespace com

#endif //COM_BALDZARIKA_AR_POINT2_H
