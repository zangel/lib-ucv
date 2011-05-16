#ifndef BALDZARIKA_UCV_UTILS_H
#define BALDZARIKA_UCV_UTILS_H

namespace baldzarika { namespace ucv { namespace utils {

	template < typename T >
	inline T hypot(T const &ha, T const &hb)
	{
		T a=std::abs(ha);
		T b=std::abs(hb);
		if(a>b)
		{
			T f=b/a;
			return a*std::sqrt(detail::constant::one<T>()+f*f);
		}
		if(b<std::numeric_limits<T>::epsilon())
			return detail::constant::zero<T>();
		T f=a/b;
		return b*std::sqrt(detail::constant::one<T>()+f*f);
	}
	
} //namespace utils
} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_UTILS_H