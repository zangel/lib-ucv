#ifndef BALDZARIKA_UCV_UBLAS_FIXED_POINT_TYPE_TRAITS_H
#define BALDZARIKA_UCV_UBLAS_FIXED_POINT_TYPE_TRAITS_H

namespace boost { namespace numeric { namespace ublas {

	//using baldzarika::ucv::fixed_point;

	template < boost::uint32_t I, boost::uint32_t F >
	struct type_traits< baldzarika::ucv::fixed_point<I,F> >
	{
		typedef baldzarika::ucv::fixed_point<I,F> fixed_point_type;
		typedef type_traits<fixed_point_type> self_type;

		typedef fixed_point_type value_type;

		typedef const fixed_point_type &const_reference;
		typedef fixed_point_type &reference;

		typedef fixed_point_type real_type;
		typedef real_type precision_type;

		static const unsigned plus_complexity = 1;
		static const unsigned multiplies_complexity = 1;


		static BOOST_UBLAS_INLINE real_type real (const_reference t){ return t; }

		static BOOST_UBLAS_INLINE real_type imag (const_reference /*t*/){ return baldzarika::ucv::detail::constant::zero<fixed_point_type>(); }

		static BOOST_UBLAS_INLINE value_type conj (const_reference t){ return t; }

		static BOOST_UBLAS_INLINE real_type type_abs (const_reference t){ return std::abs(t); }

		static BOOST_UBLAS_INLINE value_type type_sqrt (const_reference t){ return std::sqrt(t); }

		static BOOST_UBLAS_INLINE real_type norm_1 (const_reference t){ return std::abs(t); }

		static BOOST_UBLAS_INLINE real_type norm_2 (const_reference t){ return std::abs(t); }

		static BOOST_UBLAS_INLINE real_type norm_inf (const_reference t){ return std::abs(t); }

		static BOOST_UBLAS_INLINE bool equals (const_reference t1, const_reference t2){ return t1==t2; }
	};

} // namespace ublas
} // namespace numeric
} // namespace boost


#endif //BALDZARIKA_UCV_UBLAS_FIXED_POINT_TYPE_TRAITS_H