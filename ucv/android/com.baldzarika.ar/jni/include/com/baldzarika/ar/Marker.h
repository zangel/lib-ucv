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

		J2CPP_DECLARE_CLASS
		J2CPP_DECLARE_FIELD(0)

		explicit Marker(jobject jobj);

		void					create();
		void					create(j2cpp::local_ref<Size2> const &ms);
		void					destroy();

		j2cpp::local_ref<Size2>	getSize();
		jboolean 				load(j2cpp::local_ref<j2cpp::java::lang::String> const &fileName);

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
