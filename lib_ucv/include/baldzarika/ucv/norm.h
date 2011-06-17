#ifndef BALDZARIKA_UCV_NORM_H
#define BALDZARIKA_UCV_NORM_H

#include <baldzarika/ucv/integral.h>

namespace baldzarika { namespace ucv {

	template < typename RT, typename IVT >
	RT norm_l1(IVT i1, IVT i2, boost::uint32_t s)
	{
		if(i1.width()!=i2.width() || i1.height()!=i2.height() || !s)
			return std::numeric_limits<RT>::max();
		
		math::size2ui const sample_size(s,s);

		RT const inv_s_area=math::constant::one<RT>()/RT(s*s);


		RT const inv_width=math::constant::one<RT>()/RT(i1.width()/s);
		RT const inv_height=math::constant::one<RT>()/RT(i1.height()/s);
		
		RT norm_val=math::constant::zero<RT>();
		for(std::size_t y=0;y<static_cast<std::size_t>(i1.height());y+=s)
		{
			RT row_norm_val=math::constant::zero<RT>();

			for(std::size_t x=0;x<static_cast<std::size_t>(i1.width());x+=s)
			{
				math::point2i sample_at(x,y);
				row_norm_val+=std::abs(box_integral<IVT,RT>(i1,sample_at,sample_size)-box_integral<IVT,RT>(i2,sample_at,sample_size));
			}
			norm_val+=row_norm_val*inv_width;
		}
		return norm_val*inv_height*inv_s_area;
	}


} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_NORM_H
