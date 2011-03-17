#ifndef BALDZARIKA_UCV_FEATURE_POINT_H
#define BALDZARIKA_UCV_FEATURE_POINT_H

#include <baldzarika/ucv/point2.h>


namespace baldzarika { namespace ucv {

	template < typename T,  typename DT >
	class feature_point
		: public point2<T>
	{
	public:
		typedef T value_type;
		typedef DT desc_value_type;
		typedef point2<T> point2_t;

		feature_point(point2_t const &p, value_type s) : point2_t(p) , m_scale(s) , m_orientation(0.0f) { }
		feature_point(feature_point const &that)
			: point2_t(that)
			, m_scale(that.m_scale)
			, m_orientation(that.m_orientation)
		{
		}

		value_type operator -(feature_point const &rhs) const
		{
			value_type sum=0;
			for(boost::uint32_t d=0;d<64;++d)
			{
				value_type diff=m_desc[d]-rhs.m_desc[d];
				sum+=diff*diff;
			}
			return sqrt(sum);
		}

		value_type		m_scale;
		value_type		m_orientation;
		desc_value_type	m_desc[64];
	};
	
} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_FEATURE_POINT_H