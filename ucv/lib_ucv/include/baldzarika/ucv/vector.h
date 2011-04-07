#ifndef BALDZARIKA_UCV_VECTOR_H
#define BALDZARIKA_UCV_VECTOR_H

#include <baldzarika/ucv/point2.h>

namespace baldzarika { namespace ucv {

	template < typename T, boost::uint32_t D >
	class vector
		: protected ublas::c_vector< T, D >
	{
	public:
		typedef ublas::c_vector< T, D > base_t;
		template < typename T2, boost::uint32_t R2, boost::uint32_t C2> friend class matrix;

		using base_t::operator();
		using base_t::operator[];
		using base_t::data;
	
	protected:
		template < class AE >
		inline vector(ublas::vector_expression<AE> const &ae)
			: base_t(ae)
		{
			BOOST_ASSERT(size()==D);
		}

	public:

		static inline vector const& zero()
		{
			static vector zero_=ublas::zero_vector<T>(D);
			return zero_;
		}

		template < boost::uint32_t A >
		static inline vector const& unit()
		{
			BOOST_STATIC_ASSERT(A<D);
			static vector unit_=ublas::unit_vector<T>(D,A);
			return unit_;
		}

		inline vector()
			: base_t(D)
		{
		}


		inline vector(T const &v)
			: base_t(D, v)
		{
		}

		template < typename RT >
		inline vector(vector<RT, D> const &that)
			: base_t(that)
		{
		}

		template < typename RT >
		inline vector(point2<RT> const &p2)
			: base_t(D)
		{
			BOOST_STATIC_ASSERT(D==2);
			typename base_t::pointer pd=data();
			pd[0]=p2.x;
			pd[1]=p2.y;
		}

		template < typename RT >
		inline vector& operator =(vector< RT, D > const &rhs)
		{
			base_t::operator =(rhs);
			return *this;
		}

		template < typename RT >
		inline vector& operator+=(vector< RT, D > const &rhs)
		{
			base_t::operator+=(rhs)
			return *this;
		}

		template < typename RT >
		inline vector operator +(vector< RT, D > const &rhs) const
		{
			return vector(*this)+=rhs;
		}

		template < typename RT >
		inline vector& operator-=(vector< RT, D > const &rhs)
		{
			base_t::operator-=(rhs)
			return *this;
		}

		template < typename RT >
		inline vector operator -(vector< RT, D > const &rhs) const
		{
			return vector(*this)-=rhs;
		}

		template < typename RT >
		inline vector& operator*=(RT const &v)
		{
			base_t::operator*=(v);
			return *this;
		}

		template < typename RT >
		inline vector operator *(RT const &v) const
		{
			return vector(*this)*=v;
		}

		template < typename RT >
		inline vector& operator/=(RT const &v)
		{
			base_t::operator/=(v);
			return *this;
		}

		template < typename RT >
		inline vector operator /(RT const &v) const
		{
			return vector(*this)/=v;
		}

		inline T sum() const
		{
			return ublas::sum(*this);
		}

		inline T norm_1() const
		{
			return ublas::norm_1(*this);
		}

		inline T norm_2() const
		{
			return ublas::norm_2(*this);
		}

		template < typename RT >
		inline T dot(vector<RT, D> const &rhs) const
		{
			return ublas::inner_prod(*this, rhs);
		}

		inline T length() const
		{
			return ublas::norm_2(*this);
		}

		inline vector& normalize()
		{
			*this/=std::max<T>(std::numeric_limits<T>::epsilon(), norm_2());
		}

		inline vector normalized() const
		{
			return vector(*this)/=norm_2();
		}

		template < typename RT >
		inline vector& operator*=(vector< RT, D > const &rhs)
		{
			typename base_t::pointer td=data();
			typename vector< RT, D >::base_t::const_pointer rd=rhs.data();

			for(std::size_t d=0;d<D;++d)
				td[d]*=rd[d];
			return *this;
		}

		template < typename RT >
		inline vector operator *(vector< RT, D > const &rhs) const
		{
			return vector(*this)*=rhs;
		}

		template < typename RT >
		inline vector& operator/=(vector< RT, D > const &rhs)
		{
			typename base_t::pointer td=data();
			typename vector< RT, D >::base_t::const_pointer rd=rhs.data();

			for(std::size_t d=0;d<D;++d)
				td[d]/=rd[d];
			return *this;
		}

		template < typename RT >
		inline vector operator /(vector< RT, D > const &rhs) const
		{
			return vector(*this)/=rhs;
		}

	};

	typedef vector<float, 2> vector2f;
	typedef vector<float, 3> vector3f;

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_VECTOR_H