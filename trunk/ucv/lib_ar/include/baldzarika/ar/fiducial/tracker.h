#ifndef BALDZARIKA_AR_FIDUCIAL_TRACKER_H
#define BALDZARIKA_AR_FIDUCIAL_TRACKER_H

#include <baldzarika/ar/fiducial/typedefs.h>

namespace baldzarika { namespace ar { namespace fiducial {

	class model;

	class tracker
		: public boost::enable_shared_from_this<tracker>
	{
	public:
		tracker();
		~tracker();

	};

} //namespace fiducial
} //namespace ar
} //namespace baldzarika

#endif //BALDZARIKA_AR_FIDUCIAL_TRACKER_H
