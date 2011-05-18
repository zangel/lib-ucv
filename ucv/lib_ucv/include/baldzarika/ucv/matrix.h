#ifndef BALDZARIKA_UCV_MATRIX_H
#define BALDZARIKA_UCV_MATRIX_H

#include <baldzarika/ucv/point2.h>
#include <baldzarika/ucv/vector.h>
#include <baldzarika/ucv/fixed_point.h>


namespace baldzarika { namespace ucv {

	template < typename T, boost::uint32_t R, boost::uint32_t C > class matrix;

	namespace detail {

		template < typename T >
		static inline matrix< T, 3, 3 > invert_matrix( matrix< T, 3, 3 > const &);

	} //namespace details


	template < typename T, boost::uint32_t R, boost::uint32_t C >
	class matrix
		: protected ublas::c_matrix< T, R, C >
	{
	public:
		typedef ublas::c_matrix< T, R, C > base_t;
		template < typename T2, boost::uint32_t R2, boost::uint32_t C2> friend class matrix;
		template < typename T2, boost::uint32_t R2, boost::uint32_t C2> friend matrix<T2,R2,C2> invert_matrix(matrix<T2,R2,C2> const &);
	
		using base_t::size1;
		using base_t::size2;
		using base_t::operator();
		using base_t::data;
		using base_t::at_element;

		typedef typename base_t::pointer pointer;
		typedef typename base_t::const_pointer const_pointer;

	protected:
		template < class AE >
		matrix(ublas::matrix_expression<AE> const &ae) : base_t(ae)
		{
			BOOST_ASSERT(size1()==R && size2()==C);
		}
	
	public:
		static inline matrix const& identity()
		{
			static matrix identity_=ublas::identity_matrix<T>(R,C);
			return identity_;
		}

		static inline matrix const& zero()
		{
			static matrix zero_=ublas::zero_matrix<T>(R,C);
			return zero_;
		}

		inline matrix()
			: base_t(R,C)
		{
		}

		inline matrix(T const &v)
			: base_t(R, C, v)
		{
		}

		template < typename RT >
		inline matrix(matrix<RT,R,C> const &that)
			: base_t(that)
		{
		}

		template < typename RT >
		inline matrix(RT(&mat)[R][C])
		{
			for(boost::uint32_t r=0;r<R;++r)
				for(boost::uint32_t c=0;c<C;++c)
					at_element(r,c)=mat[r][c];
		}

		template < typename RT >
		inline matrix& operator =(matrix<RT,R,C> const &rhs)
		{
			base_t::operator =(rhs);
			return *this;
		}

		template < typename RT >
		inline matrix& operator +=(matrix<RT,R,C> const &rhs)
		{
			base_t::operator+=(rhs);
			return *this;
		}

		template < typename RT >
		inline matrix operator +(matrix<RT,R,C> const &rhs) const
		{
			return matrix(*this)+=rhs;
		}

		template < typename RT >
		inline matrix& operator -=(matrix<RT,R,C> const &rhs)
		{
			base_t::operator-=(rhs);
				return *this;
		}

		template < typename RT >
		inline matrix operator -(matrix<RT,R,C> const &rhs) const
		{
			return matrix(*this)-=rhs;
		}
		
		template < typename RT >
		inline matrix& operator*=(matrix<RT,R,C> const &rhs)
		{
			return *this=ublas::prod(*this, rhs);
		}

		template < typename RT >
		inline matrix operator *(matrix<RT,R,C> const &rhs) const
		{
			return matrix(*this)*=rhs;
		}

		template < typename RT >
		inline matrix& scale(RT const &v)
		{
			base_t::operator*=(v);
			return *this;
		}

		template < typename RT >
		inline matrix scaled(RT const &v) const
		{
			return matrix(*this).scale(v);
		}

		inline matrix inverse() const
		{
			return detail::invert_matrix<T>(*this);
		}

		inline matrix<T,C,R> transposed() const
		{
			return matrix<T,C,R>(ublas::trans(*this));
		}

		template < typename VT >
		inline vector< VT, R > operator *(vector< VT, C > const &v) const
		{
			return vector< VT, R >(ublas::prod(*this, vector<T,C>(v)));
		}

		template < typename PT >
		inline point2<PT> operator *(point2< PT > const &p) const
		{
			return vector< T, R >(ublas::prod(*this, vector< T, C >(p))).homogenized();
		}
	};


	namespace detail {

		template < typename T >
		static inline matrix< T, 3, 3 > invert_matrix( matrix< T, 3, 3 > const &m)
		{
			T det=
					m.data()[0]*(m.data()[4]*m.data()[8]-m.data()[5]*m.data()[7])-
					m.data()[1]*(m.data()[3]*m.data()[8]-m.data()[5]*m.data()[6])+
					m.data()[2]*(m.data()[3]*m.data()[7]-m.data()[4]*m.data()[6]);

			det=detail::constant::one<T>()/det;

			matrix<T,3,3> tmp;
			typename matrix< T, 3, 3 >::base_t::pointer mt=tmp.data();

			tmp.data()[0]=(m.data()[4]*m.data()[8]-m.data()[5]*m.data()[7])*det;
			tmp.data()[1]=(m.data()[2]*m.data()[7]-m.data()[1]*m.data()[8])*det;
			tmp.data()[2]=(m.data()[1]*m.data()[5]-m.data()[2]*m.data()[4])*det;

			tmp.data()[3]=(m.data()[5]*m.data()[6]-m.data()[3]*m.data()[8])*det;
			tmp.data()[4]=(m.data()[0]*m.data()[8]-m.data()[2]*m.data()[6])*det;
			tmp.data()[5]=(m.data()[2]*m.data()[3]-m.data()[0]*m.data()[5])*det;

			tmp.data()[6]=(m.data()[3]*m.data()[7]-m.data()[4]*m.data()[6])*det;
			tmp.data()[7]=(m.data()[1]*m.data()[6]-m.data()[0]*m.data()[7])*det;
			tmp.data()[8]=(m.data()[0]*m.data()[4]-m.data()[1]*m.data()[3])*det;
			
			return tmp;
		}

	} //namespace detail
	

	typedef matrix<float, 3, 3> matrix33f;
	typedef matrix<float, 9, 9> matrix99f;



} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_MATRIX_H
