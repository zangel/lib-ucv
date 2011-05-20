#ifndef COM_BALDZARIKA_AR_FIDUCIAL_MARKER_MODEL_H
#define COM_BALDZARIKA_AR_FIDUCIAL_MARKER_MODEL_H

namespace com { namespace baldzarika { namespace ar { namespace fiducial {

	class MarkerModel
		: public j2cpp::object<MarkerModel>
	{
	public:
		typedef boost::shared_ptr<
			::baldzarika::ar::fiducial::marker_model
		> px_t;

		typedef j2cpp::local_ref<MarkerModel> jx_t;
		typedef j2cpp::global_ref<MarkerModel> jref_t;


		J2CPP_DECLARE_CLASS
		J2CPP_DECLARE_FIELD(0)

		static jx_t				get(px_t const &px);
		static px_t				get(jx_t const &jx);

		template < typename DNT, typename DWT >
		static void tie(boost::shared_ptr<DNT> const &px, j2cpp::local_ref<DWT> const &jx)
		{
			px_t *ppx=new px_t(px);
			jref_t jref(jx);
			(*ppx)->m_any_data=jref;
			jref->m_px=reinterpret_cast<jlong>(ppx);
		}

		explicit MarkerModel(jobject jobj);


		void 		destroy();

		j2cpp::field<
			J2CPP_CLASS_NAME,
			J2CPP_FIELD_NAME(0),
			J2CPP_FIELD_SIGNATURE(0),
			jlong
		> m_px;
	};

} //namespace fiducial
} //namespace ar
} //namespace baldzarika
} //namespace com

#endif //COM_BALDZARIKA_AR_FIDUCIAL_MARKER_MODEL_H
