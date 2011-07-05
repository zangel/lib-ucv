#ifndef BALDZARIKA_MATH_EC_GALOIS_FIELD_POLYNOMIAL_H
#define BALDZARIKA_MATH_EC_GALOIS_FIELD_POLYNOMIAL_H

#include <baldzarika/math/ec/galois/field_symbol.h>
#include <baldzarika/math/ec/galois/field.h>
#include <baldzarika/math/ec/galois/field_element.h>

namespace baldzarika { namespace math { namespace ec { namespace galois {

	template < boost::uint32_t PWR, boost::uint32_t PP >
	class field_polynomial
	{
	public:
		typedef field_symbol_traits<PWR> field_symbol_traits_t;
		typedef typename field_symbol_traits_t::field_symbol_t field_symbol_t;
		typedef field_element<PWR,PP> field_element_t;
		typedef field<PWR,PP> field_t;


		static inline field_polynomial const &one()
		{
			static field_polynomial _one(field_element_t(1));
			return _one;
		}

		static inline field_polynomial const &zero()
		{
			static field_polynomial _zero(field_element_t(0));
			return _zero;
		}
		
		field_polynomial()
		{
		}

		field_polynomial(boost::uint32_t degree)
		{
			 m_poly.resize(degree+1,field_element_t(0));
		}

		field_polynomial(boost::uint32_t degree, field_element_t monomial)
		{
			 m_poly.resize(degree+1,field_element_t(0));
			 m_poly[degree]=monomial;
		}

		template < boost::int32_t N >
		field_polynomial(field_element_t(&elements)[N], bool reverse=false)
		{
			boost::uint32_t poly_size=N;

			while(poly_size>0 && !elements[(reverse?(N-poly_size):(poly_size-1))]) poly_size--;
			if(poly_size)
			{
				m_poly.resize(poly_size);
				for(boost::uint32_t i=0;i<poly_size;++i)
					m_poly[i]=reverse?elements[N-1-i]:elements[i];
			}
			else
				m_poly.resize(1,field_element_t::zero());
		}

		template < boost::int32_t N >
		field_polynomial(field_symbol_t(&symbols)[N], bool reverse=false)
		{
			boost::uint32_t poly_size=N;

			while(poly_size>0 && !symbols[(reverse?(N-poly_size):(poly_size-1))]) poly_size--;
			if(poly_size)
			{
				m_poly.resize(poly_size);
				for(boost::uint32_t i=0;i<poly_size;++i)
					m_poly[i]=reverse?symbols[N-1-i]:symbols[i];
			}
			else
				m_poly.resize(1,field_element_t::zero());
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

		inline field_polynomial& operator/=(field_polynomial const &rhs)
		{
			if(degree()>=rhs.degree() && rhs.degree()>=0)
			{
				field_polynomial quotient(degree()-rhs.deg()+1);
				field_polynomial remainder(rhs.degree()-1);
				
				for(boost::int32_t i=degree();i>=0;--i)
				{
					if(i<=rhs.degree())
					{
						quotient[i]=remainder[remainder.degree()]/rhs[rhs.degree()];
						for(boost::int32_t j=remainder.degree();j>0;--j)
							remainder[j]=remainder[j-1]+(quotient[i]*rhs[j]);
						
						remainder[0]=m_poly[i]+(quotient[i]*rhs[0]);
					}
					else
					{
						for(boost::int32_t j=remainder.degree();j>0;--j)
							remainder[j]=remainder[j-1];
						remainder[0]=m_poly[i];
					}
				}
				simplify(quotient);
				m_poly=quotient.m_poly;
			}
			return *this;
		}

		inline field_polynomial& operator/=(field_element_t const &fe)
		{
			for(poly_iterator it=m_poly.begin();it!=m_poly.end();++it)
				(*it)/=fe;
			return *this;
		}

		inline field_polynomial& operator%=(field_polynomial const &rhs)
		{
			if(degree()>=rhs.degree() && rhs.degree()>=0)
			{
				field_polynomial quotient(degree()-rhs.degree()+1);
				field_polynomial remainder(rhs.degree()-1);

				for(boost::int32_t i=degree();i>=0;--i)
				{
					if(i<=quotient.degree())
					{
						quotient[i]=remainder[remainder.degree()]/rhs[rhs.degree()];
						for (boost::int32_t j=remainder.degree();j>0;--j)
							remainder[j]=remainder[j-1]+(quotient[i]*rhs[j]);
						remainder[0]=m_poly[i]+(quotient[i]*rhs[0]);
					}
					else
					{
						for(boost::int32_t j=remainder.degree();j>0;--j)
							remainder[j]=remainder[j-1];
						remainder[0]=m_poly[i];
					}
				}
				m_poly=remainder.m_poly;
			}
			return *this;
		}

		inline field_polynomial& operator%=(boost::uint32_t x)
		{
			if(m_poly.size()>=x)
			{
				m_poly.resize(x,field_element_t(0));
				simplify(*this);
			}
			return *this;
		}

		inline field_polynomial& operator^=(boost::uint32_t n)
		{
			field_polynomial result=*this;
			for(boost::uint32_t i=0;i<n;++i)
				result*=*this;
			*this=result;
			return *this;
		}

		inline field_polynomial& operator<<=(boost::uint32_t n)
		{
			if(m_poly.size()>0)
			{
				boost::uint32_t initial_size=m_poly.size();
				m_poly.resize(m_poly.size()+n, field_element_t(0));
				
				for(boost::uint32_t i=initial_size-1;static_cast<boost::int32_t>(i)>=0;--i)
					m_poly[i+n]=m_poly[i];
				
				for(boost::uint32_t i=0;i<n;++i)
					m_poly[i]=field_element_t(0);
			}
			return *this;
		}

		inline field_polynomial& operator>>=(boost::uint32_t n)
		{
			if(n<=m_poly.size())
			{
				for(boost::uint32_t i=0;i<=degree()-n;++i)
					m_poly[i]=m_poly[i+n];
				m_poly.resize(m_poly.size()-n, field_element_t(0));
			}
			else
			if(static_cast<boost::int32_t>(n)>=(degree()+1))
				m_poly.resize(0, field_element_t(0));
			return *this;
		}

		inline field_element_t const& operator[](boost::uint32_t idx) const
		{
			BOOST_ASSERT(idx<m_poly.size());
			return m_poly[idx];
		}

		inline field_element_t& operator[](boost::uint32_t idx)
		{
			BOOST_ASSERT(idx<m_poly.size());
			return m_poly[idx];
		}

		inline field_element_t operator()(field_symbol_t const &v) const
		{
			field_element_t result(0);

			if(!m_poly.empty())
			{
				field_symbol_t total_sum=0 ;
				field_symbol_t value_poly_form=v;
				boost::int32_t i=0;
				for(const_poly_iterator it=m_poly.begin();it!=m_poly.end();++it,++i)
					total_sum^=field_t::get().mul(field_t::get().exp(value_poly_form,i),(*it).poly());
				result=total_sum;
			}
			return result;
		}

		inline field_element_t operator()(field_element_t const &v) const
		{
			return operator()(v.poly());
		}

		inline bool is_zero() const
		{
			return m_poly.empty() || m_poly.front()==0;
		}
		
		inline bool operator==(field_polynomial const &rhs) const
		{
			if(m_poly.size()!=rhs.m_poly.size())
				return false;
			else
			{
				const_poly_iterator it0=rhs.m_poly.begin();
				for(const_poly_iterator it1=m_poly.begin();it1!=m_poly.end();++it0,++it1)
					if((*it0)!=(*it1)) return false;
			}
			return true;
		}

		inline bool operator!=(field_polynomial const &rhs) const
		{
			return !(*this==rhs);
		}

		inline field_polynomial derivative() const
		{
			if(m_poly.size()>1)
			{
				field_polynomial deriv(degree());
				for(boost::uint32_t i=0;i<m_poly.size()-1;++i)
				{
					if((i&1)==0)
						deriv.m_poly[i]=0;
					else
						deriv.m_poly[i]=m_poly[i+1];
				}
				simplify(deriv);
				return deriv;
			}
			return field_polynomial(0);
		}

		inline bool monic() const
		{
			return (m_poly[m_poly.size()-1]==static_cast<field_symbol_t>(1));
		}

		inline void simplify()
		{
			simplify(*this);
		}

		static inline void simplify(field_polynomial& polynomial)
		{
			boost::uint32_t poly_size;
			if(((poly_size=polynomial.m_poly.size())>0) && (polynomial.m_poly.back()==0))
			{
				poly_iterator it=polynomial.m_poly.end();
				boost::uint32_t cnt=0;
				while(it!=polynomial.m_poly.begin())
				{
					if((*(--it))==0)
						cnt++;
					else
						break;
				}
				if(cnt!=0)
					polynomial.m_poly.resize(poly_size-cnt,field_element_t(0));
			}
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

		static inline field_polynomial const& get_x()
		{
			static field_element_t _xfes[2]={ field_element_t(0), field_element_t(1) };
			static field_polynomial _x_polynomial(1,_xfes);
			return _x_polynomial;
		}

	private:
		typedef typename std::vector<field_element_t>::iterator poly_iterator;
		typedef typename std::vector<field_element_t>::const_iterator const_poly_iterator;

		std::vector<field_element_t> m_poly;
	};

	template < boost::uint32_t PWR, boost::uint32_t PP >
	inline field_polynomial<PWR,PP> operator +(field_polynomial<PWR,PP> const &a, field_polynomial<PWR,PP> const &b)
	{
		return field_polynomial<PWR,PP>(a)+=b;
	}

	template < boost::uint32_t PWR, boost::uint32_t PP >
	inline field_polynomial<PWR,PP> operator +(field_polynomial<PWR,PP> const &a, typename field_polynomial<PWR,PP>::field_element_t const &b)
	{
		return field_polynomial<PWR,PP>(a)+=b;
	}

	template < boost::uint32_t PWR, boost::uint32_t PP >
	inline field_polynomial<PWR,PP> operator +(typename field_polynomial<PWR,PP>::field_element_t const &a, field_polynomial <PWR,PP> const &b)
	{
		return field_polynomial<PWR,PP>(b)+=a;
	}

	template < boost::uint32_t PWR, boost::uint32_t PP >
	inline field_polynomial<PWR,PP> operator +(field_polynomial<PWR,PP> const &a, typename field_polynomial<PWR,PP>::field_symbol_t const &b)
	{
		return a+field_element(b);
	}

	template < boost::uint32_t PWR, boost::uint32_t PP >
	inline field_polynomial<PWR,PP> operator +(typename field_polynomial<PWR,PP>::field_symbol_t const &a, field_polynomial<PWR,PP> const &b)
	{
		return b+typename field_polynomial<PWR,PP>::field_element_t(a);
	}


	template < boost::uint32_t PWR, boost::uint32_t PP >
	inline field_polynomial<PWR,PP> operator -(field_polynomial<PWR,PP> const &a, field_polynomial<PWR,PP> const &b)
	{
		return field_polynomial<PWR,PP>(a)-=b;
	}

	template < boost::uint32_t PWR, boost::uint32_t PP >
	inline field_polynomial<PWR,PP> operator -(field_polynomial<PWR,PP> const &a, typename field_polynomial<PWR,PP>::field_element_t const &b)
	{
		return field_polynomial<PWR,PP>(a)-=b;
	}

	template < boost::uint32_t PWR, boost::uint32_t PP >
	inline field_polynomial<PWR,PP> operator -(typename field_polynomial<PWR,PP>::field_element_t const &a, field_polynomial<PWR,PP> const &b)
	{
		return field_polynomial<PWR,PP>(b)-=a;
	}

	template < boost::uint32_t PWR, boost::uint32_t PP >
	inline field_polynomial<PWR,PP> operator -(field_polynomial<PWR,PP> const &a, typename field_polynomial<PWR,PP>::field_symbol_t const &b)
	{
		return a-field_polynomial<PWR,PP>::field_element_t(b);
	}

	template < boost::uint32_t PWR, boost::uint32_t PP >
	inline field_polynomial<PWR,PP> operator -(typename field_polynomial<PWR,PP>::field_symbol_t const &a, field_polynomial<PWR,PP> const &b)
	{
		return b-typename field_polynomial<PWR,PP>::field_element_t(a);
	}

	template < boost::uint32_t PWR, boost::uint32_t PP >
	inline field_polynomial<PWR,PP> operator *(field_polynomial<PWR,PP> const &a, field_polynomial<PWR,PP> const &b)
	{
		return field_polynomial<PWR,PP>(a)*=b;
	}

	template < boost::uint32_t PWR, boost::uint32_t PP >
	inline field_polynomial<PWR,PP> operator *(typename field_polynomial<PWR,PP>::field_element_t const &a, field_polynomial<PWR,PP> const &b)
	{
		return field_polynomial<PWR,PP>(b)*=a;
	}

	template < boost::uint32_t PWR, boost::uint32_t PP >
	inline field_polynomial<PWR,PP> operator *(field_polynomial<PWR,PP> const &a, typename field_polynomial<PWR,PP>::field_element_t const &b)
	{
		return field_polynomial<PWR,PP>(a)*=b;
	}

	template < boost::uint32_t PWR, boost::uint32_t PP >
	inline field_polynomial<PWR,PP> operator /(field_polynomial<PWR,PP> const &a, field_polynomial<PWR,PP> const &b)
	{
		return field_polynomial<PWR,PP>(a)/=b;
	}

	template < boost::uint32_t PWR, boost::uint32_t PP >
	inline field_polynomial<PWR,PP> operator /(field_polynomial<PWR,PP> const &a, typename field_polynomial<PWR,PP>::field_element_t const &b)
	{
		return field_polynomial<PWR,PP>(a)/=b;
	}

	template < boost::uint32_t PWR, boost::uint32_t PP >
	inline field_polynomial<PWR,PP> operator %(field_polynomial<PWR,PP> const &a, field_polynomial<PWR,PP> const &b)
	{
		return field_polynomial<PWR,PP>(a)%=b;
	}

	template < boost::uint32_t PWR, boost::uint32_t PP >
	inline field_polynomial<PWR,PP> operator %(field_polynomial<PWR,PP> const &a, boost::uint32_t x)
	{
		return field_polynomial<PWR,PP>(a)%=x;
	}

	template < boost::uint32_t PWR, boost::uint32_t PP >
	inline field_polynomial<PWR,PP> operator ^(field_polynomial<PWR,PP> const &a, boost::int32_t n)
	{
		return field_polynomial<PWR,PP>(a)^=n;
	}

	template < boost::uint32_t PWR, boost::uint32_t PP >
	inline field_polynomial<PWR,PP> operator<<(field_polynomial<PWR,PP> const &a, boost::uint32_t n)
	{
		return field_polynomial<PWR,PP>(a)<<=n;
	}

	template < boost::uint32_t PWR, boost::uint32_t PP >
	inline field_polynomial<PWR,PP> operator>>(field_polynomial<PWR,PP> const &a, boost::uint32_t n)
	{
		return field_polynomial<PWR,PP>(a)>>=n;
	}

	template < boost::uint32_t PWR, boost::uint32_t PP >
	inline field_polynomial<PWR,PP> gcd(field_polynomial<PWR,PP> const &a, field_polynomial<PWR,PP> const &b)
	{
		if(!(a.valid() || b.valid()))
			return field_polynomial<PWR,PP>();
		
		if(!a.valid()) return b;
		if(!b.valid()) return a;

		field_polynomial<PWR,PP> x=a%b;
		field_polynomial<PWR,PP> y=b;
		field_polynomial<PWR,PP> z=x;

		while((z=(y%x)).valid())
		{
			y=x;
			x=z;
		}
		return x;
	}

} //namespace galois
} //namespace ec
} //namespace math
} //namespace baldzarika

#endif //BALDZARIKA_MATH_EC_GALOIS_FIELD_POLYNOMIAL_H