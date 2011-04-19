#ifndef COM_BALDZARIKA_AR_MARKER_H
#define COM_BALDZARIKA_AR_MARKER_H

namespace com { namespace baldzarika { namespace ar {

	class Size2;

	class Marker
		: public j2cpp::object<Marker>
	{
	public:
		typedef boost::shared_ptr<
			::baldzarika::ar::marker
		> px_t;

		typedef j2cpp::local_ref<Marker> jx_t;
		typedef j2cpp::global_ref<Marker> jref_t;


		J2CPP_DECLARE_CLASS
		J2CPP_DECLARE_FIELD(0)

		static jx_t				get(px_t const &px);
		static px_t				get(jx_t const &jx);


		explicit Marker(jobject jobj);

		j2cpp::local_ref<Size2>	getSize();
		jboolean 				loadImage(j2cpp::local_ref<j2cpp::java::lang::String> const &fileName);
		jboolean 				setImage(j2cpp::local_ref<j2cpp::android::graphics::Bitmap> const &bitmap);
		jboolean 				saveImage(j2cpp::local_ref<j2cpp::java::lang::String> const &fileName);

		void					create();
		void					create(j2cpp::local_ref<Size2> const &ms);
		void					destroy();

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

#endif //COM_BALDZARIKA_AR_MARKER_H
