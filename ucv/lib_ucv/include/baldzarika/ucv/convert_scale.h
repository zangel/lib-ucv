#ifndef BALDZARIKA_UCV_CONVERT_SCALE_H
#define BALDZARIKA_UCV_CONVERT_SCALE_H


#include <baldzarika/ucv/point2.h>
#include <baldzarika/ucv/size2.h>
#include <baldzarika/ucv/gil_channel_traits.h>

namespace baldzarika { namespace ucv {

	template <typename SVT, typename DVT, typename PT>
	bool convert_scale(SVT src, DVT dst, PT scale=1.0f, typename gil::channel_type<typename DVT::value_type>::type &mv=typename gil::channel_type<typename DVT::value_type>::type())
	{
		typedef typename SVT::value_type	src_pixel_t;
		typedef typename DVT::value_type	dst_pixel_t;

		typedef typename gil::channel_type<src_pixel_t>::type src_channel_t;
		typedef typename gil::channel_type<dst_pixel_t>::type dst_channel_t;

		if(src.width()!=dst.width() || src.height()!=dst.height() || !src.width()*dst.width())
			return false;

		dst_channel_t const inv_width=detail::constant::one<dst_channel_t>()/dst_channel_t(dst.width());
		dst_channel_t const inv_height=detail::constant::one<dst_channel_t>()/dst_channel_t(dst.height());

		mv=detail::constant::zero<dst_channel_t>();

		for(std::size_t y=0;y<static_cast<std::size_t>(src.height());++y)
		{
			dst_channel_t row_median=detail::constant::zero<dst_channel_t>();
			src_channel_t const *src_row=reinterpret_cast<src_channel_t const *>(src.row_begin(y));
			dst_channel_t *dst_row=reinterpret_cast<dst_channel_t *>(dst.row_begin(y));
			for(std::size_t x=0;x<static_cast<std::size_t>(src.width());++x, ++src_row, ++dst_row)
			{
				dst_channel_t v=static_cast<dst_channel_t>(PT(*src_row)*scale);
				*dst_row=v;
				row_median+=v*inv_width;
			}
			mv+=row_median*inv_height;
		}
		return true;
	}

	template < typename DVT >
	bool convert_nv16_to_gray(gil::gray8_view_t y, gil::gray16_view_t uv, DVT dst, typename gil::channel_type<typename DVT::value_type>::type &mv, float scale=1.0f)
	{
		typedef gil::channel_type<gil::gray8_pixel_t>::type y_channel_t;
		typedef gil::channel_type<gil::gray16_pixel_t>::type uv_channel_t;


		typedef typename DVT::value_type	dst_pixel_t;
		typedef typename gil::channel_type<dst_pixel_t>::type dst_channel_t;

		static dst_channel_t const r_s=0.30f;
		static dst_channel_t const g_s=0.59f;
		static dst_channel_t const b_s=0.11f;

		dst_channel_t const c_scale=scale;

		if(y.width()/2!=uv.width() || y.height()/2!=uv.height() || !uv.width()*uv.height() || y.width()!=dst.width() || y.height()!=dst.height())
			return false;
		
		dst_channel_t const inv_width=detail::constant::one<dst_channel_t>()/dst_channel_t(dst.width());
		dst_channel_t const inv_height=detail::constant::one<dst_channel_t>()/dst_channel_t(dst.height());

		dst_channel_t median_sum=detail::constant::zero<dst_channel_t>();

		for(std::size_t r=0;r<static_cast<std::size_t>(y.height());++r)
		{
			y_channel_t *y_row=reinterpret_cast<y_channel_t *>(y.row_begin(r));
			uv_channel_t *uv_row=reinterpret_cast<uv_channel_t *>(uv.row_begin(r>>1));
			dst_channel_t *dst_row=reinterpret_cast<dst_channel_t *>(dst.row_begin(r));
			dst_channel_t median_row_sum=detail::constant::zero<dst_channel_t>();
			for(std::size_t c=0;c<y.width();++c)
			{
				boost::int32_t Y=std::max<boost::int32_t>(0,static_cast<boost::int32_t>(*y_row)-16);
				boost::int32_t V=static_cast<boost::int32_t>((*uv_row) & 0x00ff)-128;
				boost::int32_t U=static_cast<boost::int32_t>(((*uv_row) & 0xff00) >> 8)-128;
								
				dst_channel_t B=dst_channel_t(
						std::min<boost::int32_t>(262143, std::max<boost::int32_t>(1192*Y+2066*U, 0))>>10
					)*detail::constant::i_255<dst_channel_t>();

				dst_channel_t G=dst_channel_t(
						std::min<boost::int32_t>(262143, std::max<boost::int32_t>(1192*Y-833*V-400*U, 0))>>10
					)*detail::constant::i_255<dst_channel_t>();

				dst_channel_t R=dst_channel_t(
						std::min<boost::int32_t>(262143, std::max<boost::int32_t>(1192*Y+1634*V, 0))>>10
					)*detail::constant::i_255<dst_channel_t>();

				dst_channel_t v=c_scale*(B*b_s+G*g_s+R*r_s);
				*dst_row=v;
				median_row_sum+=v;
				y_row++;
				uv_row+=(c%2?1:0);
				dst_row++;
			}
			median_sum+=median_row_sum*inv_width;
		}
		mv=median_sum*inv_height;
		return true;
	}

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_CONVERT_SCALE_H
