#ifndef BALDZARIKA_AR_FIDUCIAL_TYPEDEFS_H
#define BALDZARIKA_AR_FIDUCIAL_TYPEDEFS_H

namespace baldzarika { namespace ar { namespace fiducial {
	
	typedef math::fixed_point<10, 21> gray_t;
	typedef ucv::gil::pixel<gray_t, ucv::gil::gray_layout_t> gray_pixel_t;
	typedef ucv::gil::image< gray_pixel_t, false, std::allocator<unsigned char> > gray_image_t;
	typedef gray_image_t::view_t gray_view_t;
	typedef gray_image_t::const_view_t gray_const_view_t;

	typedef ucv::contour<math::real_t> contour_t;

	typedef boost::uint32_t marker_id_t;

	static marker_id_t const NULL_MARKER_ID=marker_id_t(-1);


} //namespace fiducial
} //namespace ar
} //namespace baldzarika

#endif //BALDZARIKA_AR_FIDUCIAL_TYPEDEFS_H
