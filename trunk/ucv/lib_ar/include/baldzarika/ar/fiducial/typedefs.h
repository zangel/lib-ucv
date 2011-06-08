#ifndef BALDZARIKA_AR_FIDUCIAL_TYPEDEFS_H
#define BALDZARIKA_AR_FIDUCIAL_TYPEDEFS_H

#include <baldzarika/ar/common_typedefs.h>

namespace baldzarika { namespace ar { namespace fiducial {
	
	typedef ucv::contour<math::real_t> contour_t;
	typedef boost::uint32_t marker_id_t;
	static marker_id_t const NULL_MARKER_ID=marker_id_t(-1);

} //namespace fiducial
} //namespace ar
} //namespace baldzarika

#endif //BALDZARIKA_AR_FIDUCIAL_TYPEDEFS_H
