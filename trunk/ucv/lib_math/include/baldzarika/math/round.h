#ifndef BALDZARIKA_MATH_ROUND_H
#define BALDZARIKA_MATH_ROUND_H

namespace baldzarika { namespace math {

	template < typename T >
	static inline T round(T v)
	{
		return v< math::constant::zero<T>()?
			static_cast<T>(std::ceil(v-math::constant::half<T>())):
			static_cast<T>(std::floor(v+math::constant::half<T>()));
	}

	template < typename T >
	static inline boost::int32_t iround(T v)
	{
		return static_cast<boost::int32_t>(round(v));
	}

} //namespace math
} //namespace baldzarika

#endif //BALDZARIKA_MATH_ROUND_H