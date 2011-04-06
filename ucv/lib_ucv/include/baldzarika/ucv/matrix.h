#ifndef BALDZARIKA_UCV_MATRIX_H
#define BALDZARIKA_UCV_MATRIX_H

namespace baldzarika { namespace ucv {

	template < typename T, boost::uint32_t R, boost::uint32_t C >
	class matrix
		: protected ublas::c_matrix< T, R, C >
	{
	public:
		typedef ublas::c_matrix< T, R, C > base_t;
		template < typename T2, boost::uint32_t R2, boost::uint32_t C2> friend class matrix;
		template < typename T2, boost::uint32_t R2, boost::uint32_t C2> friend matrix<T2,R2,C2> invert_matrix(matrix<T2,R2,C2> const &);
	
		using base_t::operator();
		using base_t::data;

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
		inline matrix& operator =(matrix<RT,R,C> const &rhs)
		{
			base_t::operator =(rhs);
			return *this;
		}

		template < typename RT >
		inline matrix& operator +=(matrix<RT,R,C> const &rhs)
		{
			base_t::operator+=(rhs)
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
			base_t::operator-=(rhs)
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
		inline matrix& operator*=(RT const &v)
		{
			base_t::operator*=(v);
			return *this;
		}

		template < typename RT >
		inline matrix operator *(RT const &v) const
		{
			return matrix(*this)*=v;
		}

		template < typename RT >
		inline matrix& operator /=(RT const &v)
		{
			base_t::operator/=(v);
			return *this;
		}

		template < typename RT >
		inline matrix operator /(RT const &v) const
		{
			return matrix(*this)/=v;
		}

		inline matrix inverse() const { return details::invert_matrix(*this); }
	};


	namespace details {

		template < typename T, boost::uint32_t R, boost::uint32_t C>
		static inline matrix< T, R, C > invert_matrix( matrix< T, R, C > const &m) { BOOST_STATIC_ASSERT(false); return matrix< T, R, C >; }

		template < typename T >
		static inline matrix< T, 3, 3 > invert_matrix( matrix< T, 3, 3 > const &m)
		{
			typename matrix< T, 3, 3 >::base_t::const_pointer md=m.data();
					
			T det=
					md[0]*(md[4]*md[8]-md[5]*md[7])-
					md[1]*(md[3]*md[8]-md[5]*md[6])+
					md[2]*(md[3]*md[7]-md[4]*md[6]);

			det=detail::constants::one<T>()/det;

			matrix<T,3,3> tmp;
			typename matrix< T, 3, 3 >::base_t::pointer mt=tmp.data();

			mt[0]=(md[4]*md[8]-md[5]*md[7])*det;
			mt[1]=(md[2]*md[7]-md[1]*md[8])*det;
			mt[2]=(md[1]*md[5]-md[2]*md[4])*det;

			mt[3]=(md[5]*md[6]-md[3]*md[8])*det;
			mt[4]=(md[0]*md[8]-md[2]*md[6])*det;
			mt[5]=(md[2]*md[3]-md[0]*md[5])*det;

			mt[6]=(md[3]*md[7]-md[4]*md[6])*det;
			mt[7]=(md[1]*md[6]-md[0]*md[7])*det;
			mt[8]=(md[0]*md[4]-md[1]*md[3])*det;
			
			return tmp;
		}

	} //namespace details
	

	typedef matrix<float, 3, 3> matrix33f;



} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_MATRIX_H