#ifndef BALDZARIKA_MATH_UTILS_H
#define BALDZARIKA_MATH_UTILS_H

#include <baldzarika/math/constant.h>

namespace baldzarika { namespace math { namespace utils {

	template < typename T >
	static inline T sign(T const &a, T const &b)
	{
		return (b<constant::zero<T>()?-std::abs(a):std::abs(a));
	}

	template < typename T >
	static inline T sqr(T const &x)
	{
		return x*x;
	}

	template < typename T >
	static inline T hypot(T const &a, T const &b)
	{
		T const absa=std::abs(a);
		T const absb=std::abs(b);

		return (absa>absb?
			absa*std::sqrt(constant::one<T>()+sqr(absb/absa)):
			(absb==constant::zero<T>()?
				constant::zero<T>():
				absb*std::sqrt(constant::one<T>()+sqr(absa/absb))
			)
		);
	}
	
} //namespace utils
} //namespace math
} //namespace baldzarika

#endif //BALDZARIKA_MATH_UTILS_H