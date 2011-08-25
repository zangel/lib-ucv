#ifndef BALDZARIKA_UCV_MAKE_GAUSSIAN_KERNEL_H
#define BALDZARIKA_UCV_MAKE_GAUSSIAN_KERNEL_H

namespace baldzarika { namespace ucv {

	template < typename T, boost::int32_t N >
	void make_gaussian_kernel(T(&kernel)[N], T sigma, bool normalize=true)
	{
		T _sigma=-math::constant::half<T>()/(sigma*sigma);
		T sum=math::constant::zero<T>();
		for(boost::int32_t n=0;n<N;++n)
		{
			T x=T(n)-T(N-1)*math::constant::half<T>();
			T t=std::exp(_sigma*x*x);
			kernel[n]=t;
			sum+=t;
		}
		if(normalize)
		{
			sum=math::constant::one<T>()/sum;
			for(boost::int32_t n=0;n<N;++n)
				kernel[n]*=sum;
		}
	}


} //namespace baldzarika
} //namespace ucv


#endif //BALDZARIKA_UCV_MAKE_GAUSSIAN_KERNEL_H