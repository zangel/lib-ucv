#ifndef BALDZARIKA_MATH_EC_GALOIS_FIELD_H
#define BALDZARIKA_MATH_EC_GALOIS_FIELD_H

#include <baldzarika/math/ec/galois/field_symbol.h>

namespace baldzarika { namespace math { namespace ec { namespace galois {

	template < boost::uint32_t PRIM_POLY >
	struct primitive_polynomial;

	template <>
	struct primitive_polynomial<5>
	{
		static boost::int32_t const DEGREE=9;
		static boost::uint32_t const POLYNOMIAL[DEGREE];
	};
	boost::uint32_t const primitive_polynomial<5>::POLYNOMIAL[primitive_polynomial<5>::DEGREE]={1, 0, 1, 1, 1, 0, 0, 0, 1};

	template <>
	struct primitive_polynomial<6>
	{
		static boost::int32_t const DEGREE=9;
		static boost::uint32_t const POLYNOMIAL[DEGREE];
	};
	boost::uint32_t const primitive_polynomial<6>::POLYNOMIAL[primitive_polynomial<6>::DEGREE]={1, 1, 1, 0, 0, 0, 0, 1, 1};

	

	template < boost::uint32_t PWR, boost::uint32_t PP >
	class field
	{
	public:
		typedef field_symbol_traits<PWR> field_symbol_traits_t;
		typedef typename field_symbol_traits_t::field_symbol_t field_symbol_t;

		typedef primitive_polynomial<PP> primitive_polynomial_t;

		static boost::uint32_t const POWER=PWR;
		static boost::uint32_t const DEGREE=primitive_polynomial_t::DEGREE;

		static boost::uint32_t const FIELD_SIZE=(1<<POWER)-1;
		static boost::uint32_t const INIT_PRIM_POLY_HASH=0xAAAAAAAA;

		static inline field const& get() { static field _field; return _field; }
		
		static inline field_symbol_t const& mask(){ static field_symbol_t _mask(FIELD_SIZE); return _mask; }

		template < boost::uint32_t PWR_, boost::uint32_t PP_ >
		friend class field_element;

	private:
		field()
			: m_prim_poly_hash(INIT_PRIM_POLY_HASH)
		{
			for(boost::int32_t i=0;i<DEGREE;++i)
				m_prim_poly[i]=primitive_polynomial_t::POLYNOMIAL[i];
			m_prim_poly[DEGREE]=0;

			for(boost::int32_t i=0;i<DEGREE+1;++i)
				m_prim_poly_hash+=((i&1)==0)?((m_prim_poly_hash<<7)^m_prim_poly[i]^(m_prim_poly_hash>>3)):
					(~((m_prim_poly_hash<<11)^m_prim_poly[i]^(m_prim_poly_hash>>5)));
			generate_field();
		}

		inline void generate_field()
		{
			field_symbol_t mask=1;
			m_alpha_to[POWER]=0;
			for(field_symbol_t i=0;i<static_cast<field_symbol_t>(POWER);++i)
			{
				m_alpha_to[i]=mask;
				m_index_of[m_alpha_to[i]]=i;
				if(primitive_polynomial_t::POLYNOMIAL[i]!=0)
					m_alpha_to[POWER]^=mask;
				mask<<=1;
            }
			m_index_of[m_alpha_to[POWER]]=POWER;
			mask>>=1;
			for(field_symbol_t i=POWER+1;i<static_cast<field_symbol_t>(FIELD_SIZE);++i)
			{
				if(m_alpha_to[i-1]>=mask)
					m_alpha_to[i]=m_alpha_to[POWER]^((m_alpha_to[i-1]^mask)<<1);
				else
					m_alpha_to[i]=m_alpha_to[i-1]<<1;
				m_index_of[m_alpha_to[i]]=i;
			}

			m_index_of[0]=-1;
			m_alpha_to[FIELD_SIZE]=1;
			for(field_symbol_t i=0;i<static_cast<field_symbol_t>(FIELD_SIZE+1);++i)
			{
				for(field_symbol_t j=0;j<static_cast<field_symbol_t>(FIELD_SIZE+1);++j)
				{
					m_mul_table[i][j]=gen_mul(i,j);
					m_div_table[i][j]=gen_div(i,j);
					m_exp_table[i][j]=gen_exp(i,j);
				}
			}
			for(field_symbol_t i=0;i<static_cast<field_symbol_t>(FIELD_SIZE+1);++i)
			{
				m_mul_inverse[i]=gen_inverse(i);
				m_mul_inverse[i+FIELD_SIZE+1]=m_mul_inverse[i];
			}
		}

		inline field_symbol_t gen_mul(field_symbol_t const &a, field_symbol_t const &b) const
		{
			return ((a==0)||(b==0))?
				0:m_alpha_to[normalize(m_index_of[a]+m_index_of[b])];
		}

		inline field_symbol_t gen_div(field_symbol_t const &a, field_symbol_t const &b) const
		{
			return ((a == 0)||(b == 0))?
				0:m_alpha_to[normalize(m_index_of[a]-m_index_of[b]+FIELD_SIZE)];
		}

		inline field_symbol_t gen_exp(field_symbol_t const &a, boost::uint32_t n) const
		{
			return (a!=0)?
				((n==0)?1:m_alpha_to[normalize(m_index_of[a]*static_cast<field_symbol_t>(n))]):0;
		}

		inline field_symbol_t gen_inverse(field_symbol_t const &val) const
		{
			return m_alpha_to[normalize(FIELD_SIZE-m_index_of[val])];
		}
		
		inline boost::int32_t normalize(field_symbol_t x) const
		{
			while(x<0) x+=static_cast<field_symbol_t>(FIELD_SIZE);
		
			while(x>=static_cast<field_symbol_t>(FIELD_SIZE))
			{
				x-=static_cast<field_symbol_t>(FIELD_SIZE);
				x=(x>>POWER)+(x&FIELD_SIZE);
			}
            return x;
         }

	protected:

		inline field_symbol_t index(field_symbol_t const &v) const
		{
			return m_index_of[v];
		}

		inline field_symbol_t inverse(field_symbol_t const &v) const
		{
			return m_mul_inverse[v];
		}

		inline field_symbol_t mul(field_symbol_t const &a, field_symbol_t const &b) const
		{
			return m_mul_table[a][b];
		}

		inline field_symbol_t div(field_symbol_t const &a, field_symbol_t const &b) const
		{
			return m_div_table[a][b];
		}

		inline field_symbol_t exp(field_symbol_t const &a, boost::int32_t n) const
		{
			if(n<0)
			{
				while(n<0) n+=FIELD_SIZE;
				return ((n==0)?1:m_exp_table[a][n]);
			}
			return m_exp_table[a][n & mask()];
		}



	private:
		boost::uint32_t m_prim_poly_hash;
		boost::uint32_t	m_prim_poly[DEGREE+1];
		
		field_symbol_t	m_alpha_to[FIELD_SIZE+1];
		field_symbol_t	m_index_of[FIELD_SIZE+1];
		field_symbol_t	m_mul_inverse[(FIELD_SIZE+1)*2];
		field_symbol_t	m_mul_table[(FIELD_SIZE+1)][(FIELD_SIZE+1)];
		field_symbol_t	m_div_table[(FIELD_SIZE+1)][(FIELD_SIZE+1)];
		field_symbol_t	m_exp_table[(FIELD_SIZE+1)][(FIELD_SIZE+1)];
	};



} //namespace galois
} //namespace ec
} //namespace math
} //namespace baldzarika

#endif //BALDZARIKA_MATH_EC_GALOIS_FIELD_H