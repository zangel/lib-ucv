#ifndef BALDZARIKA_AR_MARKERLESS_MARKER_H
#define BALDZARIKA_AR_MARKERLESS_MARKER_H

#include <baldzarika/ar/markerless/typedefs.h>

namespace baldzarika { namespace ar { namespace markerless {

	class marker
	{
	public:
		marker();
		marker(marker const &that); 
		marker(math::size2ui const &sz);
		marker(gray_image_t const &img);
		marker(ucv::gil::gray8c_view_t gv);
		marker(ucv::gil::rgb8c_view_t rgbv);
		~marker();

		static bool							can_load(std::string const &fn);
		static boost::shared_ptr<marker>	create_from_file(std::string const &fn);

		bool								load(std::string const &fn);
		bool								save(std::string const &fn);
		bool								set(ucv::gil::gray8c_view_t gv);
		

		math::size2ui						get_size() const;

		gray_image_t const&					get_image() const;
		gray_image_t&						get_image();
		gray_t const&						get_median() const;

		
	private:
		gray_image_t		m_img;
		gray_t				m_median;

	public:
		mutable boost::any	m_any_data;
	};

} //namespace markerless
} //namespace ar
} //namespace baldzarika

#endif //BALDZARIKA_AR_MARKERLESS_MARKER_H