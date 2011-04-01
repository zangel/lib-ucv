#ifndef BALDZARIKA_UCV_FEATURE_POINT_H
#define BALDZARIKA_UCV_FEATURE_POINT_H

#include <baldzarika/ucv/point2.h>


namespace baldzarika { namespace ucv {

	template < typename T,  typename DT >
	class feature_point
		: public point2<T>
	{
	public:
		static boost::uint32_t const DESCRIPTOR_SIZE=64;
		typedef T value_type;
		typedef point2<value_type> point2_t;
		typedef DT desc_value_type;

		struct position_accessor
		{
			typedef value_type result_type;
			result_type operator()(feature_point const &t, size_t k) const { return t[k]; }
			result_type operator()(point2_t const &t, size_t k) const { return t[k]; }
		};

		struct description_accessor
		{
			typedef desc_value_type result_type;
			result_type operator()(feature_point const &t, size_t k) const { return t.m_desc[k]; }
		};
		
		feature_point(point2_t const &p, value_type s) : point2_t(p) , m_scale(s) , m_orientation(0.0f) { }
		feature_point(feature_point const &that)
			: point2_t(that)
			, m_scale(that.m_scale)
			, m_orientation(that.m_orientation)
		{
			::memcpy(m_desc, that.m_desc, sizeof(m_desc));
		}

		value_type operator -(feature_point const &rhs) const
		{
			static desc_value_type const s_coeff=10.0f;
			//static desc_value_type const s_coeff=pow(detail::constants::two<desc_value_type>(), desc_value_type(typename desc_value_type::IS-1));
			static desc_value_type const s_icoeff=0.1f;//detail::constants::one<desc_value_type>()/s_coeff;
			desc_value_type sum=0;
			for(boost::uint32_t d=0;d<64;++d)
			{
				desc_value_type diff=s_coeff(m_desc[d]-rhs.m_desc[d]);
				//float diff_=diff;
				//float diff_diff_=diff*diff;
				sum+=diff*diff;
			}
			//float sum_=sum;
			return s_icoeff*sqrt(sum);
		}

		desc_value_type		m_scale;
		desc_value_type		m_orientation;
		desc_value_type		m_desc[DESCRIPTOR_SIZE];
	};

	
} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_FEATURE_POINT_H