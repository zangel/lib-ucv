#ifndef BALDZARIKA_MATH_MATRIX_H
#define BALDZARIKA_MATH_MATRIX_H

#include <baldzarika/math/constant.h>

namespace baldzarika { namespace math {

	template < typename T, boost::uint32_t R, boost::uint32_t C >
	class matrix
	{
	public:
		template < typename T2, boost::uint32_t R2, boost::uint32_t C2 > friend class matrix;

		enum { RXC=R*C };

		static inline matrix const& zero() { return matrix_ops<T, R, C>::zero(); }
		static inline matrix const& identity() { return matrix_ops<T, R, C>::identity(); }

		inline matrix()
		{
		}

		inline matrix(matrix const &that)
		{
			*this=that;
		}

		inline matrix(T(&aa)[R][C])
		{
			*this=*reinterpret_cast< matrix const *>(aa);
		}

		template < typename RT >
		inline matrix(matrix<RT,R,C> const &that)
		{
			*this=that;
		}

		template < typename RT >
		inline matrix(RT(&aa)[R][C])
		{
			*this=*reinterpret_cast< matrix<RT,R,C> const *>(aa);
		}

		inline matrix& operator=(matrix const &rhs)
		{
			std::copy(rhs.m._1d, rhs.m._1d+RXC, m._1d);
			return *this;
		}

		template < typename RT >
		inline matrix& operator=(matrix<RT,R,C> const &rhs)
		{
			for(boost::uint32_t i=0;i<RXC;++i)
				m._1d[i]=static_cast<T>(rhs.m._1d[i]);
			return *this;
		}

		inline T& operator()(boost::int32_t r, boost::uint32_t c)
		{
			return m._2d[r][c];
		}

		inline T const& operator()(boost::int32_t r, boost::uint32_t c) const
		{
			return m._2d[r][c];
		}

		inline T& operator[](boost::int32_t n)
		{
			return m._1d[n];
		}

		inline T const& operator[](boost::int32_t n) const
		{
			return m._1d[n];
		}

	private:
		union
		{
			T	_2d[R][C];
			T	_1d[RXC];
		} m;
	};


} //namespace math
} //namespace baldzarika

#include <baldzarika/math/matrix_ops.h>

#endif //BALDZARIKA_MATH_MATRIX_H