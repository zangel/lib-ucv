#ifndef BALDZARIKA_UCV_COPY_PIXELS_H
#define BALDZARIKA_UCV_COPY_PIXELS_H

namespace baldzarika { namespace ucv {

	template < typename SVT, typename DVT >
	bool copy_pixels(SVT src, DVT dst)
	{
		typedef typename SVT::value_type	src_pixel_t;
		typedef typename DVT::value_type	dst_pixel_t;

		typedef typename gil::channel_type<src_pixel_t>::type src_channel_t;
		typedef typename gil::channel_type<dst_pixel_t>::type dst_channel_t;

		if(src.width()!=dst.width() || src.height()!=dst.height() || !src.width()*dst.width())
			return false;

		for(boost::int32_t y=0;y<src.height();++y)
		{
			src_channel_t const *src_row=reinterpret_cast<src_channel_t const *>(src.row_begin(y));
			dst_channel_t *dst_row=reinterpret_cast<dst_channel_t *>(dst.row_begin(y));
			for(boost::int32_t x=0;x<src.width();++x)
				*dst_row++=static_cast<dst_channel_t>(*src_row++);
		}
		return true;
	}

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_COPY_PIXELS_H
