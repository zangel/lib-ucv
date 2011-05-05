#ifndef BALDZARIKA_UCV_GAUSSIAN_BLUR_H
#define BALDZARIKA_UCV_GAUSSIAN_BLUR_H

namespace baldzarika { namespace ucv {

	namespace detail {

		template < typename PT, boost::uint32_t KS >
		struct gaussian_blur_filter_traits { };


		template < typename PT >
		struct gaussian_blur_filter_traits< PT, 3 >
		{
			static PT const SEPARABLE_KERNEL[2][3];
		};


		template < typename PT >
		PT const gaussian_blur_filter_traits< PT, 3 >::SEPARABLE_KERNEL[2][3]={ {-1.0, 0.0, 1.0}, { 1.0, 2.0, 1.0} };
	}


	template < typename PT, boost::uint32_t KS >
	class gaussian_blur
	{
	public:
	};


} //namespace ucv
} //namespace baldzarika


#endif //BALDZARIKA_UCV_GAUSSIAN_BLUR_H
