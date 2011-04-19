#include <baldzarika/ar/config.h>
#include <baldzarika/ar/marker.h>
#include <baldzarika/ucv/convert_scale.h>
#include <boost/algorithm/string/predicate.hpp>

#define png_infopp_NULL (png_infopp)0
#define int_p_NULL (int*)0

#include <boost/gil/extension/io/png_io.hpp>

namespace baldzarika { namespace ar {


	boost::shared_ptr<marker> marker::create_from_file(std::string const &fn)
	{
		boost::shared_ptr<marker> new_marker(new marker());
		if(new_marker->load(fn))
			return new_marker;
		return boost::shared_ptr<marker>();
	}


	marker::marker()
		: m_median(-1)
	{
	}

	marker::marker(marker const &that)
		: m_img(that.m_img)
		, m_median(that.m_median)
	{
	}

	marker::marker(ucv::size2ui const &sz)
		: m_img(sz.width(), sz.height())
		, m_median(-1)
	{
	}

	marker::marker(gray_image_t const &img)
		: m_img(img)
		, m_median(-1)
	{
	}

	marker::marker(ucv::gil::gray8c_view_t gv)
		: m_img(gv.width(), gv.height())
		, m_median(-1)
	{
		ucv::convert_scale(
			gv,
			ucv::gil::view(m_img),
			1.0f/255.0f,
			m_median
		);
	}

	marker::marker(ucv::gil::rgb8c_view_t rgbv)
		: m_img(rgbv.width(), rgbv.height())
		, m_median(-1)
	{
	}

	marker::~marker()
	{
	}

	bool marker::load(std::string const &fn)
	{
		if(boost::algorithm::iends_with(fn, ".png"))
		{
			ucv::gil::gray8_image_t marker_img;
			ucv::gil::png_read_and_convert_image(fn, marker_img);
			if(marker_img.width()*marker_img.height())
			{
				m_img.recreate(marker_img.width(),marker_img.height());
				ucv::convert_scale(
					ucv::gil::const_view(marker_img),
					ucv::gil::view(m_img),
					1.0f/255.0f,
					m_median
				);
				return true;
			}
		}
		return false;
	}

	bool marker::save(std::string const &fn)
	{
		if(boost::algorithm::iends_with(fn, ".png"))
		{
			if(m_img.width()*m_img.height())
			{
				ucv::gil::gray8_image_t save_img(m_img.width(),m_img.height());
				unsigned char mv(0);
				ucv::convert_scale(
					ucv::gil::const_view(m_img),
					ucv::gil::view(save_img),
					255.0f,
					mv
				);
				
				ucv::gil::png_write_view(fn, ucv::gil::const_view(save_img));
				return true;
			}
		}
		return false;
	}

	bool marker::set(ucv::gil::gray8c_view_t gv)
	{
		if(gv.width()*gv.height())
		{
			m_img.recreate(gv.width(),gv.height());
			ucv::convert_scale(
				gv,
				ucv::gil::view(m_img),
				1.0f/255.0f,
				m_median
			);
			return true;
		}
		return false;
	}

	ucv::size2ui marker::get_size() const 
	{
		return ucv::size2ui(m_img.width(), m_img.height());
	}

	marker::gray_image_t const& marker::get_image() const
	{
		return m_img;
	}

	marker::gray_image_t& marker::get_image()
	{
		return m_img;
	}

	marker::gray_t const& marker::get_median() const
	{
		return m_median;
	}

} //namespace ar
} //namespace baldzarika
