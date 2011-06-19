#ifndef BALDZARIKA_UCV_INTEGRAL_H
#define BALDZARIKA_UCV_INTEGRAL_H

#include <baldzarika/ucv/gil_channel_traits.h>

namespace baldzarika { namespace ucv {
		
	template <typename IVT, typename RT>
	static inline RT box_integral(IVT const &iv, boost::int32_t x, boost::int32_t y, boost::uint32_t w, boost::uint32_t h)
	{
		typedef typename IVT::value_type iv_pixel_t;
		typedef typename gil::channel_type<iv_pixel_t>::type iv_channel_t;

		boost::int32_t x1=std::min<boost::int32_t>(x,iv.width())-1;
		boost::int32_t y1=std::min<boost::int32_t>(y,iv.height())-1;
		boost::int32_t x2=std::min<boost::int32_t>(x+w,iv.width())-1;
		boost::int32_t y2=std::min<boost::int32_t>(y+h,iv.height())-1;

		iv_channel_t A=((y1>=0 && x1>=0)?iv(x1,y1).operator iv_channel_t():math::constant::zero<iv_channel_t>());
		iv_channel_t B=((y1>=0 && x2>=0)?iv(x2,y1).operator iv_channel_t():math::constant::zero<iv_channel_t>());
		iv_channel_t C=((y2>=0 && x1>=0)?iv(x1,y2).operator iv_channel_t():math::constant::zero<iv_channel_t>());
		iv_channel_t D=((y2>=0 && x2>=0)?iv(x2,y2).operator iv_channel_t():math::constant::zero<iv_channel_t>());

		return RT((A-B)-(C-D));
	}

	template <typename IVT, typename RT>
	RT box_integral(IVT const &iv, math::point2i const &p, math::size2ui const &s)
	{
		return box_integral<IVT,RT>(iv,p.x(), p.y(), s.width(), s.height());
	}


	template <typename SVT, typename DVT>
	bool integral(SVT const &src, DVT const &dst, typename gil::channel_type<typename SVT::value_type>::type const &sm=typename gil::channel_type<typename SVT::value_type>::type(-1))
	{
		typedef typename SVT::value_type	src_pixel_t;
		typedef typename DVT::value_type	dst_pixel_t;

		typedef typename gil::channel_type<src_pixel_t>::type src_channel_t;
		typedef typename gil::channel_type<dst_pixel_t>::type dst_channel_t;

		if(src.width()!=dst.width() || src.height()!=dst.height() || !src.width()*dst.width())
			return false;

		src_channel_t src_median=sm;
		if(src_median<math::constant::zero<src_channel_t>())
		{
			typedef math::fixed_point<10,21> median_t;

			median_t median_sum=math::constant::zero<median_t>();

			for(std::size_t y=0;y<static_cast<std::size_t>(src.height());++y)
			{
				median_t row_sum=math::constant::zero<median_t>();
				src_channel_t const *src_row=reinterpret_cast<src_channel_t const *>(src.row_begin(y));
				for(std::size_t x=0;x<static_cast<std::size_t>(src.width());++x)
					row_sum+=median_t(*src_row++);
				median_sum+=row_sum/median_t(src.width());
			}
			src_median=src_channel_t(median_sum/median_t(src.height()));
		}
		
		
		src_channel_t const *src_row=reinterpret_cast<src_channel_t const *>(src.row_begin(0));
		dst_channel_t *dst_row=reinterpret_cast<dst_channel_t *>(dst.row_begin(0));
		dst_channel_t sum=0;
		for(std::size_t x=0;x<static_cast<std::size_t>(src.width());++x)
		{
			sum+=dst_channel_t(src_row[x]-src_median);
			dst_row[x]=sum;
		}

		for(std::size_t y=1;y<static_cast<std::size_t>(src.height());++y)
		{
			src_row=reinterpret_cast<src_channel_t const *>(src.row_begin(y));
			dst_row=reinterpret_cast<dst_channel_t *>(dst.row_begin(y));
			dst_channel_t *prev_dst_row=reinterpret_cast<dst_channel_t *>(dst.row_begin(y-1));

			sum=0;
			for(std::size_t x=0;x<static_cast<std::size_t>(src.width());++x) 
			{
				sum+=dst_channel_t(src_row[x]-src_median);
				dst_row[x]=sum+prev_dst_row[x];
			}
		}
		return true;
	}

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_INTEGRAL_H
