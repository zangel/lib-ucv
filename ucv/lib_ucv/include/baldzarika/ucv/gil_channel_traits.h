#ifndef BALDZARIKA_UCV_FP_GIL_FP_CHANNEL_TRAITS_H
#define BALDZARIKA_UCV_FP_GIL_FP_CHANNEL_TRAITS_H

#include <baldzarika/ucv/fixed_point.h>

namespace boost { namespace gil {

	namespace detail {
	
		template < boost::uint32_t I, boost::uint32_t F >
		struct channel_traits_impl< baldzarika::ucv::fixed_point<I, F>, true>
		{
			typedef baldzarika::ucv::fixed_point<I, F> fp_type;

			typedef fp_type								value_type;
			typedef fp_type&							reference;
			typedef fp_type*							pointer;
			typedef fp_type const&						const_reference;
			typedef fp_type const*						const_pointer;

			BOOST_STATIC_CONSTANT(bool, is_mutable=true);

			static value_type min_value() { return std::numeric_limits<fp_type>::lowest(); }
			static value_type max_value() { return std::numeric_limits<fp_type>::max(); }
		};

	} //namespace detail

} //namespace gil
} //namespace boost


#endif //BALDZARIKA_UCV_FP_GIL_FP_CHANNEL_TRAITS_H