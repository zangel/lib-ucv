#ifndef BALDZARIKA_MATH_EC_GALOIS_H
#define BALDZARIKA_MATH_EC_GALOIS_H

namespace baldzarika { namespace math { namespace ec { namespace galois {

	typedef boost::int32_t field_symbol;
	static field_symbol const GF_ERROR=-1;

	template < boost::int32_t PRIM_POLY >
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

	

	template <boost::int32_t PWR, boost::int32_t PP >
	class field
	{
	public:
		typedef primitive_polynomial<PP> primitive_polynomial_t;

		static boost::int32_t const POWER=PWR;
		static boost::int32_t const DEGREE=primitive_polynomial_t::DEGREE;

		static boost::int32_t const FIELD_SIZE=(1<<POWER)-1;
		static boost::uint32_t const INIT_PRIM_POLY_HASH=0xAAAAAAAA;


		static inline field const& get()
		{
			static field _field;
			return _field;
		}

		inline field_symbol index(const field_symbol value) const
		{
			return m_index_of[value];
		}

		inline field_symbol alpha(const field_symbol value) const
		{
			return m_alpha_to[value];
		}

		inline field_symbol add(const field_symbol& a, const field_symbol& b) const
        {
			return (a ^ b);
		}

         inline field_symbol sub(const field_symbol& a, const field_symbol& b) const
         {
            return (a ^ b);
         }

         inline field_symbol normalize(field_symbol x) const
         {
            while (x < 0)
            {
               x += static_cast<field_symbol>(field_size_);
            }

            while (x >= static_cast<field_symbol>(field_size_))
            {
               x -= static_cast<field_symbol>(field_size_);
               x  = (x >> power_) + (x & field_size_);
            }
            return x;
         }

         inline field_symbol mul(const field_symbol& a, const field_symbol& b) const
         {
            #if !defined(NO_GFLUT)
               return mul_table_[a][b];
            #else
               if ((a == 0) || (b == 0))
                  return 0;
               else
               {
                  return alpha_to_[normalize(index_of_[a] + index_of_[b])];
               }
            #endif
         }

         inline field_symbol div(const field_symbol& a, const field_symbol& b) const
         {
            #if !defined(NO_GFLUT)
               return div_table_[a][b];
            #else
               if ((a == 0) || (b == 0))
                  return 0;
               else
                  return alpha_to_[normalize(index_of_[a] - index_of_[b] + field_size_)];
            #endif
         }

         inline field_symbol exp(const field_symbol& a, const int& n) const
         {
            #if !defined(NO_GFLUT)
               if (n < 0)
               {
                  int b = n;
                  while (b < 0) b += field_size_;// b could be negative

                  return ((b == 0) ? 1 : exp_table_[a][b]);
               }
               else
                  return exp_table_[a][n & field_size_];
            #else
               if (a != 0)
               {
                  if (n < 0)
                  {
                     int b = n;
                     while (b < 0) b += field_size_; // b could be negative
                     return ((b == 0) ? 1 : alpha_to_[normalize(index_of_[a] * b)]);
                  }
                  else if (n == 0)
                     return 1;
                  else
                     return alpha_to_[normalize(index_of_[a] * static_cast<field_symbol>(n))];
               }
               else
                  return 0;
            #endif
         }

         inline field_symbol inverse(const field_symbol& val) const
         {
            #if !defined(NO_GFLUT)
               return mul_inverse_[val];
            #else
               return alpha_to_[normalize(field_size_ - index_of_[val])];
            #endif
         }

         inline unsigned int prim_poly_term(const unsigned int index) const
         {
            return prim_poly_[index];
         }

	protected:
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
			field_symbol mask=1;
			m_alpha_to[POWER]=0;
			for(field_symbol i=0;i<static_cast<field_symbol>(POWER);++i)
			{
				m_alpha_to[i]=mask;
				m_index_of[m_alpha_to[i]]=i;
				if(primitive_polynomial_t::POLYNOMIAL[i]!=0)
					m_alpha_to[POWER]^=mask;
				mask<<=1;
            }
			m_index_of[m_alpha_to[POWER]]=POWER;
			mask>>=1;
			for(field_symbol i=POWER+1;i<static_cast<field_symbol>(FIELD_SIZE);++i)
			{
				if(m_alpha_to[i-1]>=mask)
					m_alpha_to[i]=m_alpha_to[POWER]^((m_alpha_to[i-1]^mask)<<1);
				else
					m_alpha_to[i]=m_alpha_to[i-1]<<1;
				m_index_of[m_alpha_to[i]]=i;
			}

			m_index_of[0]=GF_ERROR;
			m_alpha_to[FIELD_SIZE]=1;
			for(field_symbol i=0;i<static_cast<field_symbol>(FIELD_SIZE+1);++i)
			{
				for(field_symbol j=0;j<static_cast<field_symbol>(FIELD_SIZE+1);++j)
				{
					m_mul_table[i][j]=gen_mul(i,j);
					m_div_table[i][j]=gen_div(i,j);
					m_exp_table[i][j]=gen_exp(i,j);
				}
			}
			for(field_symbol i=0;i<static_cast<field_symbol>(FIELD_SIZE+1);++i)
			{
				m_mul_inverse[i]=gen_inverse(i);
				m_mul_inverse[i+(FIELD_SIZE+1)]=m_mul_inverse[i];
			}
		}

		inline field_symbol gen_mul(field_symbol const a, field_symbol const b) const
		{
			return ((a==0)||(b==0))?
				0:m_alpha_to[normalize(m_index_of[a]+m_index_of[b])];
		}

		inline field_symbol gen_div(field_symbol const a, field_symbol const b) const
		{
			return ((a == 0)||(b == 0))?
				0:m_alpha_to[normalize(m_index_of[a]-m_index_of[b]+FIELD_SIZE)];
		}

		inline field_symbol gen_exp(field_symbol const a, field_symbol const n) const
		{
			return (a!=0)?
				((n==0)?1:m_alpha_to[normalize(m_index_of[a]*static_cast<field_symbol>(n))]):0;
		}

		inline field_symbol gen_inverse(field_symbol const val) const
		{
			return m_alpha_to[normalize(FIELD_SIZE-m_index_of[val])];
		}
		
		inline field_symbol normalize(field_symbol x) const
		{
			while(x<0) x+=static_cast<field_symbol>(FIELD_SIZE);
		
			while(x>=static_cast<field_symbol>(FIELD_SIZE))
			{
				x-=static_cast<field_symbol>(FIELD_SIZE);
				x=(x>>POWER)+(x&FIELD_SIZE);
			}
            return x;
         }

	private:
		boost::uint32_t m_prim_poly_hash;
		boost::uint32_t	m_prim_poly[DEGREE+1];
		
		field_symbol	m_alpha_to[FIELD_SIZE+1];
		field_symbol	m_index_of[FIELD_SIZE+1];
		field_symbol	m_mul_inverse[(FIELD_SIZE+1)*2];
		field_symbol	m_mul_table[(FIELD_SIZE+1)][(FIELD_SIZE+1)];
		field_symbol	m_div_table[(FIELD_SIZE+1)][(FIELD_SIZE+1)];
		field_symbol	m_exp_table[(FIELD_SIZE+1)][(FIELD_SIZE+1)];
	};



} //namespace galois
} //namespace ec
} //namespace math
} //namespace baldzarika

#endif //BALDZARIKA_MATH_EC_GALOIS_H