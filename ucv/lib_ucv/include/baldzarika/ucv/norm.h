#ifndef BALDZARIKA_UCV_NORM_H
#define BALDZARIKA_UCV_NORM_H

#include <baldzarika/ucv/integral.h>

namespace baldzarika { namespace ucv {

	template < typename RT, typename IVT >
	RT norm_l1(IVT i1, IVT i2, boost::uint32_t s)
	{
		typedef typename IVT::value_type	iv_pixel_t;
		typedef typename gil::channel_type<iv_pixel_t>::type iv_channel_t;

		if(i1.width()!=i2.width() || i1.height()!=i2.height() || !s)
			return std::numeric_limits<RT>::max();

		boost::int32_t iv_width_step1=i1.row_begin(1)-i1.row_begin(0);
		integral_box<iv_channel_t> iv1_sampler=
			integral_box<iv_channel_t>(
				math::point2i(0,0),
				math::size2i(1,1),
				iv_width_step1,
				math::constant::one<iv_channel_t>()
			).scaled(float(s),iv_width_step1);

		boost::int32_t iv_width_step2=i2.row_begin(1)-i2.row_begin(0);
		integral_box<iv_channel_t> iv2_sampler=
			integral_box<iv_channel_t>(
				math::point2i(0,0),
				math::size2i(1,1),
				iv_width_step2,
				math::constant::one<iv_channel_t>()
			).scaled(float(s),iv_width_step2);

		RT const inv_width=math::constant::one<RT>()/RT((i1.width()-1)/s);
		RT const inv_height=math::constant::one<RT>()/RT((i1.height()-1)/s);

		
		RT norm_val=math::constant::zero<RT>();
		for(std::size_t y=0;y<static_cast<std::size_t>(i1.height()-1);y+=s)
		{
			RT row_norm_val=math::constant::zero<RT>();

			iv_channel_t const *row_1=reinterpret_cast<iv_channel_t const *>(i1.row_begin(y));
			iv_channel_t const *row_2=reinterpret_cast<iv_channel_t const *>(i2.row_begin(y));
						
			for(std::size_t x=0;x<static_cast<std::size_t>(i1.width()-1);x+=s, row_1+=s, row_2+=s)
			{
				row_norm_val+=std::abs(
					integral_sample<RT,iv_channel_t>(row_1,iv1_sampler)-
					integral_sample<RT,iv_channel_t>(row_2,iv2_sampler)
				);
			}
			norm_val+=row_norm_val*inv_width;
		}
		return norm_val*inv_height;
	}


} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_NORM_H
