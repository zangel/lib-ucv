#ifndef BALDZARIKA_MATH_VECTOR_H
#define BALDZARIKA_MATH_VECTOR_H

namespace baldzarika { namespace math {

	template < typename T, boost::uint32_t R, boost::uint32_t C> class matrix;
	template < typename T2, boost::uint32_t D2 > struct vector_ops;
	template < typename T2, boost::uint32_t R2, boost::uint32_t C2 > struct matrix_ops;
	
	template < typename T, boost::uint32_t D >
	class vector
	{
	public:
		template < typename T2, boost::uint32_t D2 > friend class vector;
		template < typename T2, boost::uint32_t D2 > friend struct vector_ops;
		template < typename T2, boost::uint32_t R2, boost::uint32_t C2 > friend struct matrix_ops;

		static inline vector const& zero() { return vector_ops<T,D>::zero(); }

		template < boost::uint32_t A >
		static inline vector const& unit() { return vector_ops<T,D>::template unit<A>(); }

		inline vector()
		{
		}

		template < typename RT >
		inline vector(vector<RT,D> const &that)
		{
			vector_ops<T, D>::assign(*this,that);
		}

		template < typename RT, boost::uint32_t RD >
		inline vector(vector<RT,RD> const &that)
		{
			vector_ops<T, D>::assign(*this,that);
		}

		template < typename RT >
		inline vector(RT(&va)[D])
		{
			vector_ops<T,D>::assign(*this,*reinterpret_cast< vector<RT,D> const *>(va));
		}

		inline T& operator[](boost::int32_t e)
		{
			return m_data[e];
		}

		inline T const& operator[](boost::int32_t e) const
		{
			return m_data[e];
		}

		inline T& operator()(boost::int32_t e)
		{
			return m_data[e];
		}

		inline T const& operator()(boost::int32_t e) const
		{
			return m_data[e];
		}

		template < boost::uint32_t SS >
		inline vector<T, SS>& segment(boost::uint32_t offset)
		{
			BOOST_ASSERT(offset+SS<=D);
			return *reinterpret_cast< vector<T,SS> *>(&m_data[offset]);
		}

		template < boost::uint32_t SS >
		inline vector<T, SS> const& segment(boost::uint32_t offset) const
		{
			BOOST_ASSERT(offset+SS<=D);
			return *reinterpret_cast< vector<T,SS> const *>(&m_data[offset]);
		}

		template < typename RT >
		inline vector& operator =(vector< RT, D > const &rhs)
		{
			return vector_ops<T,D>::assign(*this,rhs);
		}
		
		template < typename RT >
		inline vector& operator+=(vector<RT,D> const &rhs)
		{
			return vector_ops<T,D>::plus_assign(*this,rhs);
		}

		template < typename RT >
		inline vector operator +(vector<RT,D> const &rhs) const
		{
			return vector(*this)+=rhs;
		}

		template < typename RT >
		inline vector& operator-=(vector<RT,D> const &rhs)
		{
			return vector_ops<T,D>::minus_assign(*this,rhs);
		}

		template < typename RT >
		inline vector operator -(vector<RT,D> const &rhs) const
		{
			return vector(*this)-=rhs;
		}

		template < typename RT >
		inline vector& operator*=(vector<RT,D> const &rhs)
		{
			return vector_ops<T,D>::vector_multiply_assign(*this,rhs);
		}

		template < typename RT >
		inline vector operator *(vector<RT,D> const &rhs) const
		{
			return vector(*this)*=rhs;
		}

		template < typename RT >
		inline vector& operator/=(vector<RT,D> const &rhs)
		{
			return vector_ops<T,D>::vector_divide_assign(*this,rhs);
		}

		template < typename RT >
		inline vector operator /(vector<RT,D> const &rhs) const
		{
			return vector(*this)*=rhs;
		}

		template < typename RT >
		inline vector& operator*=(RT const &rhs)
		{
			return vector_ops<T,D>::scalar_multiply_assign(*this,rhs);
		}

		template < typename RT >
		inline vector operator *(RT const &rhs) const
		{
			return vector(*this)*=rhs;
		}


		template < typename MT >
		inline vector& operator*=(matrix<MT,D,D> const &mat)
		{
			return vector_ops<T,D>::assign(
				*this,
				matrix_ops<MT,D,D>::vector_multiply(mat, *this)
			);
		}

		template < typename RT >
		inline vector& operator/=(RT const &rhs)
		{
			return vector_ops<T,D>::scalar_divide_assign(*this,rhs);
		}

		template < typename RT >
		inline vector operator /(RT const &rhs) const
		{
			return vector(*this)/=rhs;
		}

		template < typename RT >
		inline T dot(vector<RT,D> const &rhs) const
		{
			return vector_ops<T,D>::dot_product(*this,rhs);
		}

		template < typename RT >
		inline typename vector_ops<T,D>::cross_product_result_t cross(vector<RT,D> const &rhs) const
		{
			return vector_ops<T,D>::cross_product(*this,rhs);
		}

		inline T norm_1() const
		{
			return vector_ops<T,D>::norm_1(*this);
		}

		inline T norm_2() const
		{
			return vector_ops<T,D>::norm_2(*this);
		}

		inline T length() const
		{
			return vector_ops<T,D>::norm_2(*this);
		}

		inline vector& homogenize()
		{
			return *this/=m_data[D-1];
		}

		inline vector homogenized() const
		{
			return vector(*this).homogenize();
		}

		inline vector& normalize()
		{
			if(T n=norm_2())
				return *this/=n;
			return vector_ops<T,D>::assign(*this, vector_ops<T,D>::zero());
		}

		inline vector normalized() const
		{
			return vector(*this).normalize();
		}
	
	public:
		T m_data[D];
	};

	typedef vector<float,2> vector2f;
	typedef vector<float,3> vector3f;
	typedef vector<float,4> vector4f;


} //namespace math
} //namespace baldzarika

#include <baldzarika/math/vector_ops.h>
#include <baldzarika/math/matrix_ops.h>


#endif //BALDZARIKA_MATH_VECTOR_H