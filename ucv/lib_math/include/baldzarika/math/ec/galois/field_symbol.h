#ifndef BALDZARIKA_MATH_EC_GALOIS_FIELD_SYMBOL_H
#define BALDZARIKA_MATH_EC_GALOIS_FIELD_SYMBOL_H

namespace baldzarika { namespace math { namespace ec { namespace galois {

	template < boost::int32_t PWR >
	struct field_symbol_traits
	{
		typedef boost::int32_t field_symbol_t;
		
		static inline field_symbol_t invalid_symbol()
		{
			return -1;			
		}
	};

} //namespace galois
} //namespace ec
} //namespace math
} //namespace baldzarika

#endif //BALDZARIKA_MATH_EC_GALOIS_FIELD_SYMBOL_H