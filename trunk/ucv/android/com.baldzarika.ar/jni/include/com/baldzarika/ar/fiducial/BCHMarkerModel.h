#ifndef COM_BALDZARIKA_AR_FIDUCIAL_BCH_MARKER_MODEL_H
#define COM_BALDZARIKA_AR_FIDUCIAL_BCH_MARKER_MODEL_H

namespace com { namespace baldzarika { namespace ar { namespace fiducial {

	class MarkerModel;

	class BCHMarkerModel
		: public j2cpp::object<BCHMarkerModel>
	{
	public:

		typedef boost::shared_ptr<
			::baldzarika::ar::fiducial::bch_marker_model
		> px_t;

		typedef j2cpp::local_ref<BCHMarkerModel> jx_t;

		J2CPP_DECLARE_CLASS
		J2CPP_DECLARE_FIELD(0)



		static jx_t				get(px_t const &px);
		static px_t				get(jx_t const &jx);

		explicit BCHMarkerModel(jobject jobj);

		operator j2cpp::local_ref<MarkerModel>() const;

		void 					create();
	};

} //namespace fiducial
} //namespace ar
} //namespace baldzarika
} //namespace com

#endif //COM_BALDZARIKA_AR_FIDUCIAL_BCH_MARKER_MODEL_H
