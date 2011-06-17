#ifndef BALDZARIKA_AR_COMMON_TYPEDEFS_H
#define BALDZARIKA_AR_COMMON_TYPEDEFS_H

namespace baldzarika { namespace ar {

	typedef math::fixed_point<10, 21> gray_t;
	typedef ucv::gil::pixel<gray_t, ucv::gil::gray_layout_t> gray_pixel_t;
	typedef ucv::gil::image< gray_pixel_t, false, std::allocator<unsigned char> > gray_image_t;
	typedef gray_image_t::view_t gray_view_t;
	typedef gray_image_t::const_view_t gray_const_view_t;

} //namespace ar
} //namespace baldzarika

#endif //BALDZARIKA_AR_COMMON_TYPEDEFS_H