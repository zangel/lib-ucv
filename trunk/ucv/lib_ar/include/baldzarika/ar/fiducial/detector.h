#ifndef BALDZARIKA_AR_FIDUCIAL_DETECTOR_H
#define BALDZARIKA_AR_FIDUCIAL_DETECTOR_H

#include <baldzarika/ar/fiducial/typedefs.h>

namespace baldzarika { namespace ar { namespace fiducial {

	class model;

	class detector
		: public boost::enable_shared_from_this<detector>
	{
	public:
		detector();
		~detector();

	};

} //namespace fiducial
} //namespace ar
} //namespace baldzarika

#endif //BALDZARIKA_AR_FIDUCIAL_DETECTOR_H
