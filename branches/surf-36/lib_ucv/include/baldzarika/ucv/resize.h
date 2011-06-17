#ifndef BALDZARIKA_UCV_RESIZE_H
#define BALDZARIKA_UCV_RESIZE_H

namespace baldzarika { namespace ucv {

	template < typename SVT, typename DVT, typename CV >
	bool half_sample_and_convert(SVT src, DVT dst, CV converter)
	{
		typedef typename SVT::value_type	src_pixel_t;
		typedef typename DVT::value_type	dst_pixel_t;

		typedef typename gil::channel_type<src_pixel_t>::type src_channel_t;
		typedef typename gil::channel_type<dst_pixel_t>::type dst_channel_t;

		if(src.width()/2!=dst.width() && src.height()/2!=dst.height())
			return false;

		dst_channel_t tmp[4];
		for(boost::int32_t y=0;y<dst.height();++y)
		{
			src_channel_t const *src_row=reinterpret_cast<src_channel_t const *>(src.row_begin(y<<1));
			src_channel_t const *nxt_src_row=reinterpret_cast<src_channel_t const *>(src.row_begin((y<<1)+1));
			dst_channel_t *dst_row=reinterpret_cast<dst_channel_t *>(dst.row_begin(y));

			converter.begin_row();

			for(boost::int32_t x=0;x<dst.width();++x)
			{
				converter(src_row[x<<1],			tmp[0]);
				converter(src_row[(x<<1)+1],		tmp[1]);
				converter(nxt_src_row[x<<1],		tmp[2]);
				converter(nxt_src_row[(x<<1)+1],	tmp[3]);
				dst_row[x]=(tmp[0]+tmp[1]+tmp[2]+tmp[3]).operator>>(2);
			}
			converter.end_row();
		}
		return true;
	}

	template < typename SVT, typename DVT, typename CV >
	bool resize_and_convert(SVT src, DVT dst, CV converter)
	{
		if(src.width()/2==dst.width() && src.height()/2==dst.height())
			return half_sample_and_convert(src, dst, converter);
		return false;
	}

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_RESIZE_H