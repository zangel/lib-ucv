#ifndef BALDZARIKA_UCV_SURF_ORIENTATION_ESTIMATOR_H
#define BALDZARIKA_UCV_SURF_ORIENTATION_ESTIMATOR_H

#include <baldzarika/ucv/make_gaussian_kernel.h>
#include <baldzarika/ucv/surf/feature_point.h>

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
				make_gaussian_kernel(g_k, T(2.5));

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
			static inline orientation_helper const& get()
			{
				static orientation_helper _orientation_helper;
				return _orientation_helper;
			}

		public:
			boost::int32_t	m_indices[N][2];
			T				m_gauss25[N];
		};

	} //namespace detail

	template < typename T, boost::uint32_t R, boost::uint32_t AS, boost::uint32_t OW >
	class orientation_estimator
	{
	public:
		typedef detail::orientation_helper<T,R> orientation_helper_t;

		orientation_estimator()
		{
		}

		
		template < typename IVT, typename FPI >
		void estimate(IVT iv, FPI first, FPI last) const
		{
			typedef typename boost::iterator_value<FPI>::type feature_point_t;
			BOOST_MPL_ASSERT((boost::is_same<T,typename feature_point_t::value_type>));

			typedef typename IVT::value_type	iv_pixel_t;
			typedef typename gil::channel_type<iv_pixel_t>::type iv_channel_t;

			static orientation_helper_t const &ori_helper=orientation_helper_t::get();

			boost::int32_t const iv_width_step=iv.row_begin(1)-iv.row_begin(0);

			for(FPI fpi=first;fpi!=last;++fpi)
			{
				boost::iterator_reference<FPI>::type fp=*fpi;
				boost::int32_t fp_scale=fp.m_scale;
				T s=static_cast<T>(fp_scale*1.2f/9.0f);
				boost::int32_t grad_wav_size=2*math::iround(s*math::constant::two<T>());

				float const ib_scale=float(grad_wav_size)/4.0f;

				integral_box<iv_channel_t> dx_t[]=
				{
					integral_box<iv_channel_t>(math::point2i(0,0), math::size2i(2,4), iv_width_step, -math::constant::one<iv_channel_t>()).scaled(ib_scale,iv_width_step),
					integral_box<iv_channel_t>(math::point2i(2,0), math::size2i(2,4), iv_width_step,  math::constant::one<iv_channel_t>()).scaled(ib_scale,iv_width_step)
				};

				integral_box<iv_channel_t> dy_t[]=
				{
					integral_box<iv_channel_t>(math::point2i(0,0), math::size2i(4,2), iv_width_step,  math::constant::one<iv_channel_t>()).scaled(ib_scale,iv_width_step),
					integral_box<iv_channel_t>(math::point2i(0,2), math::size2i(4,2), iv_width_step, -math::constant::one<iv_channel_t>()).scaled(ib_scale,iv_width_step)
				};

				boost::uint32_t n_angles=0;
				for(boost::uint32_t i=0;i<orientation_helper_t::N;++i)
				{
					boost::int32_t x=math::iround(fp.x()+T(ori_helper.m_indices[i][0])*s-T(grad_wav_size-1)*math::constant::half<T>());
					boost::int32_t y=math::iround(fp.y()+T(ori_helper.m_indices[i][1])*s-T(grad_wav_size-1)*math::constant::half<T>());
					
					if(	boost::uint32_t(y)>=boost::uint32_t(iv.height()-grad_wav_size) ||
						boost::uint32_t(x)>=boost::uint32_t(iv.width()-grad_wav_size)
					) continue;
					
					iv_channel_t const *piv=reinterpret_cast<iv_channel_t const *>(iv.row_begin(y))+x;
					
					T _xy[]=
					{
						integral_sample<T,iv_channel_t>(piv,dx_t)*ori_helper.m_gauss25[i],
						integral_sample<T,iv_channel_t>(piv,dy_t)*ori_helper.m_gauss25[i]
					};
					m_xy[n_angles]=math::vector<T,2>(_xy);
					m_angle[n_angles++]=std::atan2(_xy[1], _xy[0]);
				}

				math::vector<T,2> best=math::vector<T,2>::zero();
				T desc_mod=math::constant::zero<T>();
				
				for(boost::uint32_t a=0;a<360;a+=AS)
				{
					math::vector<T,2> sum=math::vector<T,2>::zero();
					T temp_mod=math::constant::zero<T>();
					
					for(boost::uint32_t i=0;i<n_angles;++i)
					{
						boost::int32_t d=std::abs(math::iround(m_angle[i]-T(a)));
						if(d<OW/2 || d>360-OW/2)
							sum+=m_xy[i];

					}
					temp_mod=sum.dot(sum);
					if(temp_mod>desc_mod)
					{
						desc_mod=temp_mod;
						best=sum;
					}
				}
				fp.m_orientation=std::atan2(best[1],best[0]);
			}
		}
		
	private:
		mutable math::vector<T,2>	m_xy[orientation_helper_t::N];
		mutable T					m_angle[orientation_helper_t::N];
	};


	
} //namespace surf
} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_SURF_ORIENTATION_ESTIMATOR_H