#ifndef BALDZARIKA_UCV_RESIZE_H
#define BALDZARIKA_UCV_RESIZE_H

namespace baldzarika { namespace ucv {

	template < typename SVT, typename DVT >
	bool half_sample_and_scale(SVT src, DVT dst, typename gil::channel_type<typename DVT::value_type>::type s)
	{
		typedef typename SVT::value_type	src_pixel_t;
		typedef typename DVT::value_type	dst_pixel_t;

		typedef typename gil::channel_type<src_pixel_t>::type src_channel_t;
		typedef typename gil::channel_type<dst_pixel_t>::type dst_channel_t;

		if(src.width()/2!=dst.width() && src.height()/2!=dst.height())
			return false;

		for(boost::int32_t y=0;y<static_cast<std::size_t>(dst.height());++y)
		{
			src_channel_t const *src_row=reinterpret_cast<src_channel_t *>(src.row_begin(y<<1));
			src_channel_t const *nxt_src_row=reinterpret_cast<src_channel_t *>(src.row_begin((y<<1)+1));
			dst_channel_t *dst_row=reinterpret_cast<dst_channel_t *>(dst.row_begin(y));

			for(boost::int32_t x=0;x<dst.width();++x)
				dst_row[x]=
				s*
				(
					(
						dst_channel_t(src_row[x<<1])+
						dst_channel_t(src_row[(x<<1)+1])+
						dst_channel_t(nxt_src_row[x<<1])+
						dst_channel_t(nxt_src_row[(x<<1)+1])
					).operator>>(2)
				);
		}
		return true;
	}

	template < typename SVT, typename DVT >
	bool resize_and_scale(SVT src, DVT dst, typename gil::channel_type<typename DVT::value_type>::type s)
	{
		if(src.width()/2==dst.width() && src.height()/2==dst.height())
			return half_sample_and_scale(src, dst, s);
		return false;
	}

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_RESIZE_H