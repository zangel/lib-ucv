#ifndef BALDZARIKA_AR_MARKER_H
#define BALDZARIKA_AR_MARKER_H

#include <baldzarika/ucv/surf.h>

namespace baldzarika { namespace ar {

	class marker
	{
	public:
		typedef ucv::surf::gray_pixel_t pixel_t;
		typedef ucv::surf::gray_image_t image_t;
		typedef ucv::surf::gray_view_t view_t;

		marker();
		marker(marker const &that);
		marker(ucv::size2ui const &sz);
		marker(image_t const &img);
		marker(ucv::gil::gray8_view_t gv);
		marker(ucv::gil::rgb8_view_t rgbv);
		~marker();

		ucv::size2ui		get_size() const;
		image_t const&		get_image() const;
		view_t				get_view();

	private:
		image_t				m_img;
	};

} //namespace ar
} //namespace baldzarika

#endif //BALDZARIKA_AR_MARKER_H