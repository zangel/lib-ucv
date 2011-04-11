#ifndef BALDZARIKA_AR_MARKER_H
#define BALDZARIKA_AR_MARKER_H

#include <baldzarika/ucv/surf.h>

namespace baldzarika { namespace ar {

	class marker
	{
	public:
		typedef ucv::fixed_point<10, 21> gray_t;
		typedef ucv::gil::pixel<gray_t, ucv::gil::gray_layout_t> gray_pixel_t;
		typedef ucv::gil::image< gray_pixel_t, false, std::allocator<unsigned char> > gray_image_t;
		typedef gray_image_t::view_t gray_view_t;
		
		marker();
		marker(marker const &that);
		marker(ucv::size2ui const &sz);
		marker(gray_image_t const &img);
		marker(ucv::gil::gray8c_view_t gv);
		marker(ucv::gil::rgb8c_view_t rgbv);
		~marker();

		static boost::shared_ptr<marker>	load(std::string const &fn);
		static bool							can_load(std::string const &fn);



		ucv::size2ui						get_size() const;

		gray_image_t const&					get_image() const;
		gray_image_t&						get_image();
		gray_t const&						get_median() const;

		
	private:
		gray_image_t		m_img;
		gray_t				m_median;
	};

} //namespace ar
} //namespace baldzarika

#endif //BALDZARIKA_AR_MARKER_H