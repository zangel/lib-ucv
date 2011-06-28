#ifndef BALDZARIKA_MATH_EC_REED_SOLOMON_BLOCK_H
#define BALDZARIKA_MATH_EC_REED_SOLOMON_BLOCK_H

#include <baldzarika/math/ec/galois/field_symbol.h>

namespace baldzarika { namespace math { namespace ec { namespace reed_solomon {

	template < boost::int32_t CL, boost::int32_t FECL, boost::int32_t PWR, boost::int32_t PP >
	class block
	{
	public:
		static boost::int32_t const CODE_LENGTH=CL;
		static boost::int32_t const DATA_LENGTH=CL-FECL;
		static boost::int32_t const FEC_LENGTH=FECL;

		typedef galois::field_symbol_traits<PWR> field_symbol_traits;
		typedef typename field_symbol_traits::field_symbol_t field_symbol_t;


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
	};


} //namespace reed_solomon
} //namespace ec
} //namespace math
} //namespace baldzarika