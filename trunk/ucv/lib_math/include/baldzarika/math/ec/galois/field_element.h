#ifndef BALDZARIKA_MATH_EC_GALOIS_FIELD_ELEMENT_H
#define BALDZARIKA_MATH_EC_GALOIS_FIELD_ELEMENT_H

#include <baldzarika/math/ec/galois/field.h>
#include <baldzarika/math/ec/galois/field_symbol.h>

namespace baldzarika { namespace math { namespace ec { namespace galois {

	template < boost::uint32_t PWR, boost::uint32_t PP >
	class field_element
	{
	public:
		typedef field<PWR,PP> field_t;
		typedef field_symbol_traits<PWR> field_symbol_traits_t;
		typedef typename field_symbol_traits_t::field_symbol_t field_symbol_t;

		field_element()
			: m_poly_value(field_symbol_traits_t::invalid_symbol())
		{
		}

		field_element(field_symbol_t v)
			: m_poly_value(v)
		{
		}

		field_element(field_element const &that)
			: m_poly_value(that.m_poly_value)
		{
		}

		inline field_element& operator=(field_element const &rhs)
		{
			m_poly_value=rhs.m_poly_value;
			return *this;
		}

		inline field_element& operator=(field_symbol_t const &v)
		{
			m_poly_value=v & field_t::mask();
			return *this;
		}

		inline field_element& operator+=(field_element const &rhs)
		{
			m_poly_value^=rhs.m_poly_value;
			return *this;
		}

		inline field_element& operator+=(field_symbol_t const &v)
		{
			m_poly_value^=v;
			return *this;
		}

		inline field_element& operator-=(field_element const &rhs)
		{
			return *this+=rhs;
		}

		inline field_element& operator-=(field_symbol_t const &v)
		{
			return *this+=v;
		}

		inline field_element& operator*=(field_element const &rhs)
		{
			m_poly_value=field_t::get().mul(m_poly_value,rhs.m_poly_value);
			return *this;
		}

		inline field_element& operator*=(field_symbol_t const &v)
		{
			m_poly_value=field_t::get().mul(m_poly_value,v);
			return *this;
		}

		inline field_element& operator/=(field_element const &rhs)
		{
			m_poly_value=field_t::get().div(m_poly_value,rhs.m_poly_value);
			return *this;
		}

		inline field_element& operator/=(field_symbol_t const &v)
		{
			m_poly_value=field_t::get().div(m_poly_value,v);
			return *this;
		}

		inline field_element& operator^=(boost::int32_t n)
		{
			m_poly_value=field_t::get().exp(m_poly_value, n);
			return *this;
		}

		inline bool operator==(field_element const &rhs) const
		{
			return m_poly_value==rhs.m_poly_value;
		}

		inline bool operator==(field_symbol_t const &v) const
		{
			return m_poly_value==v;
		}

		inline bool operator!=(field_element const &rhs) const
		{
			return m_poly_value!=rhs.m_poly_value;
		}

		inline bool operator!=(field_symbol_t const &v) const
		{
			return m_poly_value!=v;
		}

		inline bool operator<(field_element const &rhs)
		{
			return m_poly_value<rhs.m_poly_value;
		}

		inline bool operator<(field_symbol_t const &v)
		{
			return m_poly_value<v;
		}

		inline bool operator>(field_element const &rhs)
		{
			return m_poly_value>rhs.m_poly_value;
		}

		inline bool operator>(field_symbol_t const &v)
		{
			return m_poly_value>v;
		}

		inline field_symbol_t index() const
		{
			return field_t::get().index(m_poly_value);
		}

		inline field_symbol_t poly() const
		{
			return m_poly_value;
		}

		inline field_symbol_t inverse() const
		{
			return field_t::get().inverse(m_poly_value);
		}

		inline field_element& normalize()
		{
			m_poly_value&=field_t::mask();
			return *this;
		}
	
	private:
		field_symbol_t	m_poly_value;
	};


	template < boost::uint32_t PWR, boost::uint32_t PP >
	inline field_element<PWR,PP> operator +(field_element<PWR,PP> const &a, field_element<PWR,PP> const &b)
	{
		return field_element<PWR,PP>(a)+=b;
	}

	template < boost::uint32_t PWR, boost::uint32_t PP >
	inline field_element<PWR,PP> operator -(field_element<PWR,PP> const &a, field_element<PWR,PP> const &b)
	{
		return field_element<PWR,PP>(a)-=b;
	}

	template < boost::uint32_t PWR, boost::uint32_t PP >
	inline field_element<PWR,PP> operator *(field_element<PWR,PP> const &a, field_element<PWR,PP> const &b)
	{
		return field_element<PWR,PP>(a)*=b;
	}

	template < boost::uint32_t PWR, boost::uint32_t PP >
	inline field_element<PWR,PP> operator *(field_element<PWR,PP> const &a, typename field_element<PWR,PP>::field_symbol_t const &b)
	{
		return field_element<PWR,PP>(a)*=b;
	}

	template < boost::uint32_t PWR, boost::uint32_t PP >
	inline field_element<PWR,PP> operator *(typename field_element<PWR,PP>::field_symbol_t const &a, field_element<PWR,PP> const &b)
	{
		return field_element<PWR,PP>(b)*=a;
	}

	template < boost::uint32_t PWR, boost::uint32_t PP >
	inline field_element<PWR,PP> operator /(field_element<PWR,PP> const &a, field_element<PWR,PP> const &b)
	{
		return field_element<PWR,PP>(a)/=b;
	}

	template < boost::uint32_t PWR, boost::uint32_t PP >
	inline field_element<PWR,PP> operator ^(field_element<PWR,PP> const &a, boost::int32_t b)
	{
		return field_element<PWR,PP>(a)^=b;
	}

} //namespace galois
} //namespace ec
} //namespace math
} //namespace baldzarika

#endif //BALDZARIKA_MATH_EC_GALOIS_FIELD_ELEMENT_H