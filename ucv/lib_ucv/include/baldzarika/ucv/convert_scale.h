#ifndef BALDZARIKA_UCV_CONVERT_SCALE_H
#define BALDZARIKA_UCV_CONVERT_SCALE_H


#include <baldzarika/ucv/point2.h>
#include <baldzarika/ucv/size2.h>
#include <baldzarika/ucv/gil_channel_traits.h>


namespace baldzarika { namespace ucv {

	template <typename SVT, typename DVT, typename PT>
	bool convert_scale(SVT src, DVT dst, PT scale=1.0f)
	{
		typedef typename SVT::value_type	src_pixel_t;
		typedef typename DVT::value_type	dst_pixel_t;

		typedef typename gil::channel_type<src_pixel_t>::type src_channel_t;
		typedef typename gil::channel_type<dst_pixel_t>::type dst_channel_t;

		if(src.width()!=dst.width() || src.height()!=dst.height() || !src.width()*dst.width())
			return false;

		for(std::size_t y=0;y<static_cast<std::size_t>(src.height());++y)
		{
			src_channel_t *src_row=reinterpret_cast<src_channel_t *>(src.row_begin(y));
			dst_channel_t *dst_row=reinterpret_cast<dst_channel_t *>(dst.row_begin(y));
			for(std::size_t x=0;x<static_cast<std::size_t>(src.width());++x, ++src_row, ++dst_row)
				*dst_row=PT(*src_row)*scale;
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

		float const inv_262143=scale/262143.0f;

		if(y.width()/2!=uv.width() || y.height()/2!=uv.height() || !uv.width()*uv.height() || y.width()!=dst.width() || y.height()!=dst.height())
			return false;
		
		float median_sum=0.0f;
		for(std::size_t r=0;r<static_cast<std::size_t>(y.height());++r)
		{
			y_channel_t *y_row=reinterpret_cast<y_channel_t *>(y.row_begin(r));
			uv_channel_t *uv_row=reinterpret_cast<uv_channel_t *>(uv.row_begin(r>>1));
			dst_channel_t *dst_row=reinterpret_cast<dst_channel_t *>(dst.row_begin(r));
			for(std::size_t c=0;c<y.width();++c)
			{
				boost::int32_t Y=std::max<boost::int32_t>(0,static_cast<boost::int32_t>(*y_row)-16);
				boost::int32_t V=static_cast<boost::int32_t>((*uv_row) & 0x00ff)-128;
				boost::int32_t U=static_cast<boost::int32_t>(((*uv_row) & 0xff00) >> 8)-128;
								
				boost::int32_t B=std::min<boost::int32_t>(262143, std::max<boost::int32_t>(1192*Y+2066*U, 0));
				boost::int32_t G=std::min<boost::int32_t>(262143, std::max<boost::int32_t>(1192*Y-833*V-400*U, 0));
				boost::int32_t R=std::min<boost::int32_t>(262143, std::max<boost::int32_t>(1192*Y+1634*V, 0));

				float v=inv_262143*(0.11f*B+0.59f*G+0.3f*R);
				*dst_row=v;
				median_sum+=v;
				y_row++;
				uv_row+=(c%2?1:0);
				dst_row++;
			}
		}
		mv=dst_channel_t(median_sum/float(dst.width()*dst.height()));
		return true;
	}

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_CONVERT_SCALE_H
