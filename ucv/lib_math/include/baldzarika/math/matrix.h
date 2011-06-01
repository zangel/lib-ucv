#ifndef BALDZARIKA_MATH_MATRIX_H
#define BALDZARIKA_MATH_MATRIX_H

#include <baldzarika/math/constant.h>

namespace baldzarika { namespace math {

	template < typename T, boost::uint32_t R, boost::uint32_t C >
	class matrix
	{
	public:
		template < typename T2, boost::uint32_t R2, boost::uint32_t C2 > friend class matrix;
		template < typename T2, boost::uint32_t R2, boost::uint32_t C2 > friend struct matrix_ops;
		
		enum { RXC=R*C };

		static inline matrix const& zero() { return matrix_ops<T, R, C>::zero(); }
		static inline matrix const& identity() { return matrix_ops<T, R, C>::identity(); }

		inline matrix()
		{
		}

		template < typename RT >
		inline matrix(matrix<RT,R,C> const &that)
		{
			matrix_ops<T, R, C>::assign(*this,that);
		}

		template < typename RT >
		inline matrix(RT(&ma)[R][C])
		{
			matrix_ops<T, R, C>::assign(*this,*reinterpret_cast< matrix<RT,R,C> const *>(ma));
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

		template < typename RT >
		inline matrix& operator=(matrix<RT,R,C> const &rhs)
		{
			return matrix_ops<T, R, C>::assign(*this,rhs);
		}
		
		template < typename RT >
		inline matrix& operator+=(matrix<RT,R,C> const &rhs)
		{
			return matrix_ops<T,R,C>::plus_assign(*this,rhs);
		}

		template < typename RT >
		inline matrix operator +(matrix<RT,R,C> const &rhs) const
		{
			return matrix(*this)+=rhs;
		}

		template < typename RT >
		inline matrix& operator-=(matrix<RT,R,C> const &rhs)
		{
			return matrix_ops<T,R,C>::minus_assign(*this,rhs);
		}

		template < typename RT >
		inline matrix operator -(matrix<RT,R,C> const &rhs) const
		{
			return matrix(*this)-=rhs;
		}

		template < typename RT >
		inline matrix& operator*=(matrix<RT,R,C> const &rhs)
		{
			return matrix_ops<T, R, C>::prod_assign(*this, rhs);
		}

		template < typename RT >
		inline matrix operator *(matrix<RT,R,C> const &rhs) const
		{
			return matrix(*this)*=rhs;
		}

		template < typename RST >
		inline matrix& operator*=(RST const &v)
		{
			return matrix_ops<T,R,C>::scalar_multiply_assign(*this, v);
		}

		template < typename RST >
		inline matrix operator *(RST const &v) const
		{
			return matrix(*this)*=v;
		}

		inline matrix& tanspose()
		{
			return matrix_ops<T,R,C>::transpose(*this);
		}

		inline matrix& tansposed() const
		{
			return matrix(*this).transpose();
		}

		inline matrix& invert()
		{
			return matrix_ops<T,R,C>::invert(*this);
		}

		inline matrix inverted() const
		{
			return matrix(*this).invert();
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