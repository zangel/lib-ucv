#ifndef BALDZARIKA_UCV_FIND_OTSU_THRESHOLD_H
#define BALDZARIKA_UCV_FIND_OTSU_THRESHOLD_H

#include <baldzarika/ucv/ranged_histogram.h>

namespace baldzarika { namespace ucv {

	namespace detail {
#if 0
		template
		<
			boost::uint32_t HBS
			typename SVT,
			template
			<
				typename gil::channel_type<typename SVT::value_type>::type,
				typename DT
			> class THTB
		>
		struct otsu_treshold
			: THTB<typename gil::channel_type<typename SVT::value_type>::type,DT>
		{
			typedef typename gil::channel_type<typename SVT::value_type>::type src_channel_t;

		};

#endif

	} //namespace detail

	template < typename VT, boost::uint32_t HBS >
	typename gil::channel_type<typename VT::value_type>::type find_otsu_threshold(VT iv,
		typename gil::channel_type<typename VT::value_type>::type min_val,
		typename gil::channel_type<typename VT::value_type>::type max_val)
	{
		typedef typename gil::channel_type<typename VT::value_type>::type channel_t;

		channel_t const max_range=(max_val-min_val)/channel_t(HBS);

		boost::array<channel_t, HBS> histogram;
		if(!ranged_histogram(iv,histogram, min_val, max_val))
			return max_val;

		channel_t mu=detail::constant::zero<channel_t>();

		for(boost::uint32_t i=0;i<HBS;++i)
			mu+=channel_t(i)*histogram[i];

		channel_t mu1=detail::constant::zero<channel_t>();
		channel_t q1=detail::constant::zero<channel_t>();
		channel_t max_sigma=detail::constant::zero<channel_t>();
		boost::uint32_t max_val_id=0;

		for(boost::uint32_t i=0;i<HBS;i++)
		{
			mu1*=q1;
			q1+=histogram[i];
			channel_t q2=detail::constant::one<channel_t>()-q1;

			if(std::min(q1,q2)<std::numeric_limits<channel_t>::epsilon() || std::max(q1,q2)>(detail::constant::one<channel_t>()-std::numeric_limits<channel_t>::epsilon()))
				continue;

			mu1=(mu1+channel_t(i)*histogram[i])/q1;
			channel_t mu2=(mu-q1*mu1)/q2;
			channel_t sigma=q1*q2*(mu1-mu2)*(mu1-mu2);
			if(sigma>max_sigma)
			{
				max_sigma=sigma;
				max_val_id=i;
			}
		}
		return min_val+channel_t(max_val_id)*max_range;
	}

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_FIND_OTSU_THRESHOLD_H