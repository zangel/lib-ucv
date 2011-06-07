#ifndef BALDZARIKA_MATH_POINT2_H
#define BALDZARIKA_MATH_POINT2_H

#include <baldzarika/math/vector.h>
#include <baldzarika/math/matrix.h>


namespace baldzarika { namespace math {

	template < typename T >
	class point2
		: public vector<T, 2>
	{
	public:
		typedef T value_type;
		point2()
		{
			m_data[0]=m_data[1]=constant::zero<T>();
		}

		point2(T px, T py)
		{
			m_data[0]=px;
			m_data[1]=py;
		}

		point2(vector<T,3> const &v3)
			: vector<T,2>(v3)
		{
		}
		
		template < typename MT >
		inline point2 transformed(matrix<MT,3,3> const &tm) const
		{
			return point2((tm*vector<T,3>(*this)).homogenized());
		}


		inline T& x() { return m_data[0]; }
		inline T const& x() const { return m_data[0]; }

		inline T& y() { return m_data[1]; }
		inline T const& y() const { return m_data[1]; }
	};

	typedef point2<float>			point2f;
	typedef point2<boost::uint32_t>	point2ui;
	typedef point2<boost::int32_t>	point2i;

} //namespace math
} //namespace baldzarika

#endif //BALDZARIKA_MATH_POINT2_H