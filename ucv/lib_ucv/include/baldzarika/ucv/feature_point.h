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
			static value_type const s_coeff=1000;
			static value_type const s_icoeff=1.0e-3f;
			value_type sum=0;
			for(boost::uint32_t d=0;d<64;++d)
			{
				value_type diff=m_desc[d]-rhs.m_desc[d];
				//float diff_=diff;
				//float diff_diff_=diff*diff;
				sum+=(s_coeff*diff)*diff;
			}
			//float sum_=sum;
			return sqrt(s_icoeff*sum);
		}

		value_type		m_scale;
		value_type		m_orientation;
		desc_value_type	m_desc[64];
	};
	
} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_FEATURE_POINT_H