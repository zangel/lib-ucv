#ifndef BALDZARIKA_MATH_EC_GALOIS_FIELD_POLYNOMIAL_H
#define BALDZARIKA_MATH_EC_GALOIS_FIELD_POLYNOMIAL_H

#include <baldzarika/math/ec/galois/field_symbol.h>
#include <baldzarika/math/ec/galois/field_element.h>

namespace baldzarika { namespace math { namespace ec { namespace galois {

	template < boost::uint32_t PWR, boost::uint32_t PP >
	class field_polynomial
	{
	public:
		typedef field_element<PWR,PP> field_element_t;
		typedef field_symbol_traits<PWR> field_symbol_traits_t;
		typedef typename field_symbol_traits_t::field_symbol_t field_symbol_t;

		field_polynomial()
		{
		}

		field_polynomial(boost::uint32_t degree)
		{
			 m_poly.resize(degree+1,field_element_t(0));
		}

		field_polynomial(boost::uint32_t degree, field_element_t elements[])
		{
			if(elements)
				for(boost::uint32_t i=0;i<=degree;++i)
					m_poly.push_back(elements[i]);
            else
				m_poly.resize(degree+1,field_element_t(0));
		}

		field_polynomial(field_polynomial const &that)
			: m_poly(that.m_poly)
		{
		}

		field_polynomial(field_element_t const &fe)
		{
			m_poly.resize(1,fe);
		}

		inline field_polynomial& operator=(field_polynomial const &rhs)
		{
			if(this==&rhs)
				return *this;
			m_poly=rhs.m_poly;
			return *this;
		}

		inline field_polynomial& operator=(field_element_t const &fe)
		{
			m_poly.resize(1,fe);
			return *this;
		}

		inline field_polynomial& operator+=(field_polynomial const &rhs)
		{
			if (m_poly.size()<rhs.m_poly.size())
			{
				const_poly_iterator it0=rhs.m_poly.begin();
				for(poly_iterator it1=m_poly.begin();it1!=m_poly.end();++it0,++it1)
					(*it1) += (*it0);
					
				while(it0!=rhs.m_poly.end())
				{
					m_poly.push_back(*it0);
					++it0;
				}
			}
			else
			{
				poly_iterator it0=m_poly.begin();
				for(const_poly_iterator it1=rhs.m_poly.begin();it1!=rhs.m_poly.end();++it0,++it1)
					(*it0) += (*it1);
			}
			simplify(*this);
			return *this;
		}

		inline field_polynomial& operator+=(field_element_t const &fe)
		{
			m_poly[0]+=fe;
			return *this;
		}

		inline field_polynomial& operator-=(field_polynomial const &rhs)
		{
			return *this+=rhs;
		}

		inline field_polynomial& operator-=(field_element_t const &fe)
		{
			m_poly[0]-=fe;
			return *this;
		}

		inline field_polynomial& operator*=(field_polynomial const &rhs)
		{
			field_polynomial product(degree()+rhs.degree()+1);
			
			poly_iterator result_it=product.m_poly.begin();
			for(poly_iterator it0=m_poly.begin();it0!=m_poly.end();++it0)
			{
				poly_iterator current_result_it=result_it;
				for(const_poly_iterator it1=rhs.m_poly.begin();it1!=rhs.m_poly.end();++it1)
				{
					(*current_result_it)+=(*it0)*(*it1);
					current_result_it++;
				}
				result_it++;
			}

			simplify(product);
			m_poly=product.m_poly;
			return *this;
		}

		inline field_polynomial& operator*=(field_element_t const &fe)
		{
			for(poly_iterator it=m_poly.begin();it!=m_poly.end();++it)
				(*it)*=fe;
			return *this;
		}

		inline bool valid() const
		{
			return m_poly.size()>0;
		}

		inline boost::int32_t degree() const
		{
			return static_cast<boost::int32_t>(m_poly.size())-1;
				
		}

		inline void set_degree(boost::uint32_t d)
		{
			m_poly.resize(d+1,field_element_t(0));
		}

	private:
		typedef typename std::vector<field_element_t>::iterator poly_iterator;
		typedef typename std::vector<field_element_t>::const_iterator const_poly_iterator;

		std::vector<field_element_t> m_poly;
	};

} //namespace galois
} //namespace ec
} //namespace math
} //namespace baldzarika

#endif //BALDZARIKA_MATH_EC_GALOIS_FIELD_POLYNOMIAL_H