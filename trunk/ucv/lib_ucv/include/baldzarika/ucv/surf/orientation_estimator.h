#ifndef BALDZARIKA_UCV_SURF_ORIENTATION_ESTIMATOR_H
#define BALDZARIKA_UCV_SURF_ORIENTATION_ESTIMATOR_H

#include <baldzarika/ucv/make_gaussian_kernel.h>

namespace baldzarika { namespace ucv { namespace surf {

	namespace detail {

		template < typename T, boost::uint32_t R >
		struct orientation_helper {};


		template <typename T>
		struct orientation_helper<T,6>
		{
		public:
			enum { N=109 };
		private:
			orientation_helper()
			{
				T g_k[2*6+1];
				make_gaussian_kernel(g_k, 0.25);

				boost::int32_t idx=0;
				for(boost::int32_t y=-6;y<=6;++y)
				{
					for(boost::int32_t x=-6;x<=6;++x)
					{
						if(x*x+y*y<36)
						{
							m_indices[idx][0]=x;
							m_indices[idx][1]=y;
							m_gauss25[idx++]=g_k[x+6]*g_k[y+6];
						}
					}
				}
			}

		public:
			static inline rotation_helper const& get()
			{
				static rotation_helper _rotation_helper;
				return _rotation_helper;
			}
		public:
			boost::int32_t	m_indices[N][2];
			T				m_gauss25[N];
		};
	}



} //namespace surf
} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_SURF_ORIENTATION_ESTIMATOR_H