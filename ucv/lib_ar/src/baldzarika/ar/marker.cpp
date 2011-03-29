#include <baldzarika/ar/config.h>
#include <baldzarika/ar/marker.h>

#define png_infopp_NULL (png_infopp)0
#define int_p_NULL (int*)0

#include <boost/gil/extension/io/png_io.hpp>

namespace baldzarika { namespace ar {

	marker::marker()
	{
	}

	marker::marker(marker const &that)
		: m_img(that.m_img)
	{
	}

	marker::marker(ucv::size2ui const &sz)
		: m_img(sz.width(), sz.height())
	{
	}

	marker::marker(image_t const &img)
		: m_img(img)
	{
	}

	marker::marker(ucv::gil::gray8_view_t gv)
		: m_img(gv.width(), gv.height())
	{
	}

	marker::marker(ucv::gil::rgb8_view_t rgbv)
		: m_img(rgbv.width(), rgbv.height())
	{
	}

	marker::~marker()
	{
	}

	ucv::size2ui marker::get_size() const 
	{
		return ucv::size2ui(m_img.width(), m_img.height());
	}

	marker::image_t const& marker::get_image() const
	{
		return m_img;
	}

	marker::view_t marker::get_view()
	{
		return ucv::gil::view(m_img);
	}
} //namespace ar
} //namespace baldzarika
