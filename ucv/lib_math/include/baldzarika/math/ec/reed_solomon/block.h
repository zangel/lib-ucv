#ifndef BALDZARIKA_MATH_EC_REED_SOLOMON_BLOCK_H
#define BALDZARIKA_MATH_EC_REED_SOLOMON_BLOCK_H

#include <baldzarika/math/ec/galois/field_symbol.h>

namespace baldzarika { namespace math { namespace ec { namespace reed_solomon {

	template < boost::int32_t PWR, boost::int32_t CL, boost::int32_t FECL >
	class block
	{
	public:
		typedef galois::field_symbol_traits<PWR> field_symbol_traits;
		typedef typename field_symbol_traits::field_symbol_t field_symbol_t;

		template < boost::int32_t PWR_, boost::int32_t PP_, boost::int32_t CL_, boost::int32_t FECL_ >
		friend class decoder;


		static boost::uint32_t const CODE_LENGTH=CL;
		static boost::uint32_t const DATA_LENGTH=CL-FECL;
		static boost::uint32_t const FEC_LENGTH=FECL;


		static inline boost::uint32_t num_blocks(boost::dynamic_bitset<> const &bs)
		{
			return bs.size()/PWR/CODE_LENGTH;
		}

		bool get(boost::dynamic_bitset<> const &bs, boost::uint32_t n)
		{
			if(bs.size()<(n+CODE_LENGTH)*PWR)
				return false;

			for(boost::uint32_t s=0;s<CODE_LENGTH;++s)
			{
				field_symbol_t v=0;
				for(boost::uint32_t b=0;b<PWR;++b)
				{
					v|=bs.test((n+s)*PWR+b)?1:0;
					v<<=1;
				}
				m_data[s]=v;
			}
			return true;
		}

		bool put(boost::dynamic_bitset<> &bs, boost::uint32_t n)
		{
			if(bs.size()<(n+CODE_LENGTH)*PWR)
				return false;

			for(boost::uint32_t s=0;s<CODE_LENGTH;++s)
			{
				for(boost::uint32_t b=0;b<PWR;++b)
					bs.set((n+s)*PWR+b, (m_data[s]&(1<<b))?true:false);
			}
			return true;
		}

		inline field_symbol_t& operator[](boost::int32_t index)
		{
			return m_data[index];
		}

		inline field_symbol_t const& operator[](boost::int32_t index) const
		{
			return m_data[index];
		}

	private:
		field_symbol_t	m_data[CODE_LENGTH];
		boost::uint32_t	m_errors_detected;
		boost::uint32_t	m_errors_corrected;
		bool			m_unrecoverable;
	};


} //namespace reed_solomon
} //namespace ec
} //namespace math
} //namespace baldzarika

#endif //BALDZARIKA_MATH_EC_REED_SOLOMON_BLOCK_H