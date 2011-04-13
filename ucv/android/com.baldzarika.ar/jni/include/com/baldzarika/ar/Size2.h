#ifndef COM_BALDZARIKA_AR_SIZE2_H
#define COM_BALDZARIKA_AR_SIZE2_H

namespace com { namespace baldzarika { namespace ar {

	class Size2
		: public j2cpp::object<Size2>
	{
	public:
		J2CPP_DECLARE_CLASS

		J2CPP_DECLARE_METHOD(0)
		J2CPP_DECLARE_METHOD(1)

		J2CPP_DECLARE_FIELD(0)
		J2CPP_DECLARE_FIELD(1)

		explicit Size2(jobject jobj);

		Size2();
		Size2(jint width, jint height);

		j2cpp::field<
			J2CPP_CLASS_NAME,
			J2CPP_FIELD_NAME(0),
			J2CPP_FIELD_SIGNATURE(0),
			jint
		> m_Width;

		j2cpp::field<
			J2CPP_CLASS_NAME,
			J2CPP_FIELD_NAME(1),
			J2CPP_FIELD_SIGNATURE(1),
			jint
		> m_Height;
	};

} //namespace ar
} //namespace baldzarika
} //namespace com

#endif //COM_BALDZARIKA_AR_SIZE2_H
