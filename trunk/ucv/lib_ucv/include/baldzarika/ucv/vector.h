#ifndef BALDZARIKA_UCV_VECTOR_H
#define BALDZARIKA_UCV_VECTOR_H

#include <baldzarika/ucv/point2.h>
#include <baldzarika/ucv/fixed_point.h>
#include <baldzarika/ucv/ublas_fixed_point_type_traits.h>

namespace baldzarika { namespace ucv {

	template < typename T, boost::uint32_t D >
	class vector
		: protected ublas::c_vector< T, D >
	{
	public:
		typedef ublas::c_vector< T, D > base_t;

		template < typename T2, boost::uint32_t D2 > friend class vector;
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
			: base_t(ublas::scalar_vector<T>(D,v))
		{
		}

		template < typename RT >
		inline vector(RT(&a)[D])
			: base_t(D)
		{
			for(std::size_t d=0;d<D;++d)
				data()[d]=a[d];
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
			BOOST_STATIC_ASSERT(D>=2);
			data()[0]=static_cast<T>(p2.x);
			data()[1]=static_cast<T>(p2.y);
			for(std::size_t d=2;d<D-1;++d)
				data()[d]=detail::constant::zero<T>();
			if(D>2) data()[D-1]=detail::constant::one<T>();
		}

		template < typename RT >
		inline operator point2<RT> () const
		{
			BOOST_STATIC_ASSERT(D>=2);
			return point2<RT>(data()[0], data()[1]);
		}
		
		template < typename RT >
		inline vector& operator =(vector< RT, D > const &rhs)
		{
			base_t::operator =(rhs);
			return *this;
		}

		template < typename RT >
		inline vector& operator =(RT(&a)[D])
		{
			for(std::size_t d=0;d<D;++d)
				data()[d]=a[d];
			return *this;
		}

		template < typename RT >
		inline vector& operator+=(vector< RT, D > const &rhs)
		{
			base_t::operator+=(rhs);
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
			base_t::operator-=(rhs);
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

		inline vector& homogenize()
		{
			T inv_last=detail::constant::one<T>()/data()[D-1];
			for(std::size_t d=0;d<D;++d)
				data()[d]*=inv_last;
			return *this;
			
		}

		inline vector homogenized() const
		{
			return vector(*this).homogenize();		
		}


		inline vector& normalize()
		{
			*this/=std::max<T>(std::numeric_limits<T>::epsilon(), norm_2());
			return *this;
		}

		inline vector normalized() const
		{
			return vector(*this)/=norm_2();
		}

		template < typename RT >
		inline vector& operator*=(vector< RT, D > const &rhs)
		{
			for(std::size_t d=0;d<D;++d)
				data()[d]*=rhs.data()[d];
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
			for(std::size_t d=0;d<D;++d)
				data()[d]/=rhs.data()[d];
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
	typedef vector<float, 4> vector4f;
	typedef vector<float, 9> vector9f;

	template < typename T >
	vector<T,3> cross_product(vector<T,3> const &a, vector<T,3> const &b)
	{
		T res[3]=
		{
			a[1]*b[2]-b[1]*a[2],
			a[2]*b[0]-b[2]*a[0],
			a[0]*b[1]-b[0]*a[1]
		};
		return vector<T,3>(res);
	}

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_VECTOR_H
