#ifndef BALDZARIKA_UCV_BINARIZE_H
#define BALDZARIKA_UCV_BINARIZE_H

namespace baldzarika { namespace ucv {

	template < typename SVT, typename DVT, typename P>
	static inline bool binarize(SVT src, DVT dst, boost::int32_t bw, boost::int32_t bh, P const &pred)
	{
		typedef typename SVT::value_type	src_pixel_t;
		typedef typename gil::channel_type<src_pixel_t>::type src_channel_t;

		typedef typename DVT::value_type	dst_pixel_t;
		typedef typename gil::channel_type<dst_pixel_t>::type dst_channel_t;

		boost::uint32_t const threshold=(bw*bh/2)+1;
		if(dst.width()*bw!=src.width() || dst.height()*bh!=src.height())
			return false;

		for(boost::int32_t y=0;y<dst.height();++y)
		{
			dst_channel_t *dst_row=reinterpret_cast<dst_channel_t *>(dst.row_begin(y));
			for(boost::int32_t x=0;x<dst.width();++x)
			{
				boost::uint32_t pcount=0;
				for(boost::int32_t by=0;by<bh;++by)
				{
					src_channel_t const *src_row=reinterpret_cast<src_channel_t const *>(src.row_begin(y*bh+by))+x*bw;
					for(boost::int32_t bx=0;bx<bw;++bx)
					{
						pcount+=pred(src_row[bx])?1:0;
					}
				}
				dst_row[x]=pcount<threshold?
					math::constant::zero<dst_channel_t>():
					math::constant::one<dst_channel_t>();
			}
		}

		return true;
	}

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_BINARIZE_H