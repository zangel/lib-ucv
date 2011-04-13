#ifndef COM_BALDZARIKA_AR_FRAME_H
#define COM_BALDZARIKA_AR_FRAME_H

namespace com { namespace baldzarika { namespace ar {

	class Size2;

	class Frame
		: public j2cpp::object<Frame>
	{
	public:
		typedef std::pair<
			::baldzarika::ar::tracker::gray_t,
			::baldzarika::ar::tracker::gray_image_t
		> px_t;

		J2CPP_DECLARE_CLASS
		J2CPP_DECLARE_FIELD(0)

		explicit Frame(jobject jobj);

		void					create();
		void					create(j2cpp::local_ref<Size2> const &fs);
		void					destroy();

		j2cpp::local_ref<Size2>	getSize();
		jdouble					getMedianPixelValue();
		jboolean				setPixels(j2cpp::local_ref< j2cpp::array<jbyte,1> > const &data, jint pfmt);

		j2cpp::field<
			J2CPP_CLASS_NAME,
			J2CPP_FIELD_NAME(0),
			J2CPP_FIELD_SIGNATURE(0),
			jlong
		> m_px;
	};

} //namespace ar
} //namespace baldzarika
} //namespace com

#endif //COM_BALDZARIKA_AR_FRAME_H

