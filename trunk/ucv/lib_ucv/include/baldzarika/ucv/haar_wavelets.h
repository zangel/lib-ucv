#ifndef BALDZARIKA_UCV_HAAR_WAVELETS_H
#define BALDZARIKA_UCV_HAAR_WAVELETS_H

#include <baldzarika/ucv/integral.h>

namespace baldzarika { namespace ucv {

	template <typename IVT, typename RT>
	static inline RT haar2d_x(IVT const &iv, boost::int32_t x, boost::int32_t y, boost::uint32_t s)
	{
		return	box_integral<IVT,RT>(iv, x, y-(s>>1), s>>1, s)-
				box_integral<IVT,RT>(iv, x-(s>>1), y-(s>>1), s>>1, s);		
	}

	template <typename IVT, typename RT>
	static inline RT haar2d_y(IVT const &iv, boost::int32_t x, boost::int32_t y, boost::uint32_t s)
	{
		return	box_integral<IVT,RT>(iv, x-(s>>1), y, s, s>>1)-
				box_integral<IVT,RT>(iv, x-(s>>1), y-(s>>1), s, s>>1);
	}

	template <typename IVT, typename RT>
	static inline RT haar2d_x(IVT const &iv, math::point2i const &p, boost::uint32_t s)
	{
		return haar2d_x<IVT,RT>(iv, p.x(), p.y(), s);
	}

	template <typename IVT, typename RT>
	static inline RT haar2d_y(IVT const &iv, math::point2i const &p, boost::uint32_t s)
	{
		return haar2d_y<IVT,RT>(iv, p.x(), p.y(), s);
	}

} //namespace ucv
} //namespace baldzarika


#endif //BALDZARIKA_UCV_HAAR_WAVELETS_H