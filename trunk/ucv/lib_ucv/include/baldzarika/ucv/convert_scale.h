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
			for(std::size_t x=0;x<static_cast<std::size_t>(src.width());++x)
				dst_row[x]=PT(src_row[x])*scale;
		}
		return true;
	}

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_CONVERT_SCALE_H