#ifndef BALDZARIKA_UCV_EXTRACT_SURF_DESCRIPTOR_H
#define BALDZARIKA_UCV_EXTRACT_SURF_DESCRIPTOR_H

#include <baldzarika/ucv/feature_point.h>
#include 

namespace baldzarika { namespace ucv {

	template < typename IVT, typename FPT,  typename FPDT, boost::uint32_t FPBS, boost::uint32_t SPB >
	void extract_surf_descriptor(IVT const &iv, feature_point<FPT,FPDT,FPBS> &fp)
	{
	}

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_EXTRACT_SURF_DESCRIPTOR_H