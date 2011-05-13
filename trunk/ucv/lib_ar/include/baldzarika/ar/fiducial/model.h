#ifndef BALDZARIKA_AR_FIDUCIAL_DESCRIPTOR_H
#define BALDZARIKA_AR_FIDUCIAL_DESCRIPTOR_H

#include <baldzarika/ar/fiducial/typedefs.h>

namespace baldzarika { namespace ar { namespace fiducial {

	class descriptor;

	class model
		: public boost::enable_shared_from_this<model>
	{
	public:
		model();
		virtual ~model();

		virtual ucv::size2ui const&		marker_size() const=0;
		virtual bool					detect(gray_const_view_t img, contour_t const &cont, ucv::matrix33f &hm, boost::int32_t &id) const=0;
	};

} //namespace fiducial
} //namespace ar
} //namespace baldzarika

#endif //BALDZARIKA_AR_FIDUCIAL_DESCRIPTOR_H
