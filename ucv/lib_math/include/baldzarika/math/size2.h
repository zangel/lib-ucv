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
		size2()
		{
			m_data[0]=m_data[1]=constant::zero<T>();
		}

		size2(T w, T h)
		{
			m_data[0]=w;
			m_data[1]=h;
		}

		inline T& width() { return m_data[0]; }
		inline T const& width() const { return m_data[0]; }

		inline T& height() { return m_data[1]; }
		inline T const& height() const { return m_data[1]; }

		inline T area() const { return m_data[0]*m_data[1]; }
		inline bool empty() const { return m_data[0]*m_data[1]==constant::zero<T>(); }
	};

	typedef size2<float>			size2f;
	typedef size2<boost::uint32_t>	size2ui;
	typedef size2<boost::int32_t>	size2i;

} //namespace math
} //namespace baldzarika

#endif //BALDZARIKA_MATH_SIZE2_H