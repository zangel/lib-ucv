#ifndef BALDZARIKA_MATH_MATRIX_H
#define BALDZARIKA_MATH_MATRIX_H

#include <baldzarika/math/constant.h>

namespace baldzarika { namespace math {

	template < typename T, boost::uint32_t D > class vector;

	template < typename T, boost::uint32_t R, boost::uint32_t C >
	class matrix
	{
	public:
		template < typename T2, boost::uint32_t R2, boost::uint32_t C2 > friend class matrix;
		template < typename T2, boost::uint32_t R2, boost::uint32_t C2 > friend struct matrix_ops;

		enum { ROWS=R };
		enum { COLS=C };
		enum { RXC=R*C };


		static inline matrix const& zero() { return matrix_ops<T, R, C>::zero(); }
		static inline matrix const& identity() { return matrix_ops<T, R, C>::identity(); }

		inline matrix()
		{
		}

		template < typename RT >
		inline matrix(matrix<RT,R,C> const &that)
		{
			matrix_ops<T,R,C>::assign(*this,that);
		}

		template < typename RT >
		inline matrix(RT(&ma)[R][C])
		{
			matrix_ops<T,R,C>::assign(*this,*reinterpret_cast< matrix<RT,R,C> const *>(ma));
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

		inline vector<T,C> get_row(boost::int32_t r) const
		{
			return matrix_ops<T,R,C>::get_row(*this, r);
		}

		inline vector<T,R> get_col(boost::int32_t c) const
		{
			return matrix_ops<T,R,C>::get_col(*this, r);
		}

		template < typename RVT >
		inline matrix& set_row(boost::int32_t r, vector<RVT,C> const &row_vec)
		{
			return matrix_ops<T,R,C>::set_row(*this, r, row_vec);
		}

		template < typename CVT >
		inline matrix& set_col(boost::int32_t c, vector<CVT,R> const &col_vec)
		{
			return matrix_ops<T,R,C>::set_col(*this, c, col_vec);
		}

		template < typename RT >
		inline matrix& operator=(matrix<RT,R,C> const &rhs)
		{
			return matrix_ops<T,R,C>::assign(*this, rhs);
		}
		
		template < typename RT >
		inline matrix& operator+=(matrix<RT,R,C> const &rhs)
		{
			return matrix_ops<T,R,C>::plus_assign(*this, rhs);
		}

		template < typename RT >
		inline matrix operator +(matrix<RT,R,C> const &rhs) const
		{
			return matrix(*this)+=rhs;
		}

		template < typename RT >
		inline matrix& operator-=(matrix<RT,R,C> const &rhs)
		{
			return matrix_ops<T,R,C>::minus_assign(*this, rhs);
		}

		template < typename RT >
		inline matrix operator -(matrix<RT,R,C> const &rhs) const
		{
			return matrix(*this)-=rhs;
		}

		template < typename ST >
		inline matrix& operator*=(ST const &s)
		{
			return matrix_ops<T,R,C>::scalar_multiply_assign(*this, s);
		}

		template < typename ST >
		inline matrix operator *(ST const &s) const
		{
			return matrix(*this)*=v;
		}

		template < typename VT >
		inline vector<VT,R> operator *(vector<VT,C> const &vec) const
		{
			return matrix_ops<T,R,C>::vector_multiply(*this, vec);
		}

		template < typename RT >
		inline matrix& operator*=(matrix<RT,R,C> const &rhs)
		{
			return matrix_ops<T,R,C>::assign(*this, matrix_ops<T,R,C>::product(*this, rhs));
		}

		template < typename RT, boost::uint32_t RR, boost::uint32_t RC >
		inline
		typename matrix_ops<T,R,C>::template product_result<RT,RR,RC>::type
				operator *(matrix<RT,RR,RC> const &rhs) const
		{
			return matrix_ops<T,R,C>::product(*this, rhs);
		}
		
		inline matrix& tanspose()
		{
			return matrix_ops<T,R,C>::assign(*this, matrix_ops<T,R,C>::transpose(*this));
		}

		inline typename matrix_ops<T,C,R> tansposed() const
		{
			return matrix_ops<T,R,C>::transpose(*this);
		}

		inline T determinant() const
		{
			return matrix_ops<T,R,C>::determinant(*this);
		}

		inline matrix& invert()
		{
			return matrix_ops<T,R,C>::assign(*this, matrix_ops<T,R,C>::invert(*this));
		}

		inline matrix<T,C,R> inverted() const
		{
			return matrix_ops<T,R,C>::invert(*this);
		}
	
	private:
		union
		{
			T	_2d[R][C];
			T	_1d[RXC];
		} m;
	};

	typedef matrix<float,3,3> matrix33f;


} //namespace math
} //namespace baldzarika

#include <baldzarika/math/matrix_ops.h>

#endif //BALDZARIKA_MATH_MATRIX_H