#ifndef COM_BALDZARIKA_AR_FIDUCIAL_BCH_MARKER_MODEL_H
#define COM_BALDZARIKA_AR_FIDUCIAL_BCH_MARKER_MODEL_H

namespace com { namespace baldzarika { namespace ar { namespace fiducial {

	class MarkerModel;

	class BCHMarkerModel
		: public j2cpp::object<BCHMarkerModel>
	{
	public:

		J2CPP_DECLARE_CLASS

		explicit BCHMarkerModel(jobject jobj);

		operator j2cpp::local_ref<MarkerModel>() const;

		static jlong			create();
	};

} //namespace fiducial
} //namespace ar
} //namespace baldzarika
} //namespace com

#endif //COM_BALDZARIKA_AR_FIDUCIAL_BCH_MARKER_MODEL_H
