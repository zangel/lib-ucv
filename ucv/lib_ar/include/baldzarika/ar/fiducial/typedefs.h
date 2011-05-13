#ifndef BALDZARIKA_AR_FIDUCIAL_TYPEDEFS_H
#define BALDZARIKA_AR_FIDUCIAL_TYPEDEFS_H

#include <baldzarika/ucv/fixed_point.h>
#include <baldzarika/ucv/gil_channel_traits.h>
#include <baldzarika/ucv/contour.h>
#include <baldzarika/ucv/matrix.h>

namespace baldzarika { namespace ar { namespace fiducial {
	
	typedef ucv::fixed_point<10, 21> gray_t;
	typedef ucv::gil::pixel<gray_t, ucv::gil::gray_layout_t> gray_pixel_t;
	typedef ucv::gil::image< gray_pixel_t, false, std::allocator<unsigned char> > gray_image_t;
	typedef gray_image_t::view_t gray_view_t;
	typedef gray_image_t::const_view_t gray_const_view_t;

	typedef ucv::fixed_point<10, 21> real_t;
	typedef ucv::contour<real_t> contour_t;


} //namespace fiducial
} //namespace ar
} //namespace baldzarika

#endif //BALDZARIKA_AR_FIDUCIAL_TYPEDEFS_H
