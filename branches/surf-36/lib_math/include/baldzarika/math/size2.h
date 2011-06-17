#ifndef BALDZARIKA_MATH_SIZE2_H
#define BALDZARIKA_MATH_SIZE2_H

#include <baldzarika/math/vector.h>
#include <baldzarika/math/constant.h>

namespace baldzarika { namespace math {

	template < typename T >
	class size2
		: public vector<T,2>
	{
	public:
		typedef vector<T, 2> base_type;
		typedef T value_type;

		size2()
		{
			base_type::m_data[0]=base_type::m_data[1]=constant::zero<T>();
		}

		size2(T w, T h)
		{
			base_type::m_data[0]=w;
			base_type::m_data[1]=h;
		}

		inline size2(vector<T,2> const &that)
			: vector<T,2>(that)
		{
		}

		inline bool operator==(size2 const &rhs) const
		{
			return base_type::m_data[0]==rhs.m_data[0] && base_type::m_data[1]==rhs.m_data[1];
		}

		inline bool operator!=(size2 const &rhs) const
		{
			return base_type::m_data[0]!=rhs.m_data[0] || base_type::m_data[1]!=rhs.m_data[1];
		}
		
		inline T& width() { return base_type::m_data[0]; }
		inline T const& width() const { return base_type::m_data[0]; }

		inline T& height() { return base_type::m_data[1]; }
		inline T const& height() const { return base_type::m_data[1]; }

		inline T area() const { return base_type::m_data[0]*base_type::m_data[1]; }
		inline bool empty() const { return base_type::m_data[0]*base_type::m_data[1]==constant::zero<T>(); }
	};

	typedef size2<float>			size2f;
	typedef size2<boost::uint32_t>	size2ui;
	typedef size2<boost::int32_t>	size2i;

} //namespace math
} //namespace baldzarika

#endif //BALDZARIKA_MATH_SIZE2_H