#ifndef BALDZARIKA_UCV_SURF_DESCRIBER_H
#define BALDZARIKA_UCV_SURF_DESCRIBER_H

#include <baldzarika/ucv/make_gaussian_kernel.h>
#include <baldzarika/ucv/surf/feature_point.h>

namespace baldzarika { namespace ucv { namespace surf {

	template < typename T, boost::uint32_t NB, boost::uint32_t NSB >
	class describer
	{
	public:
		typedef feature_point<T, NB> feature_point_t;

		typedef typename gil::pixel<T, gil::gray_layout_t> win_pixel_t;
		typedef typename gil::image< win_pixel_t, false, std::allocator<unsigned char> > win_image_t;
		typedef typename win_image_t::view_t win_view_t;
		typedef typename win_image_t::const_view_t win_const_view_t;

		static boost::uint32_t const PATCH_SIZE=NB*NSB;
	private:

		struct win_sample
		{
			boost::uint32_t m_win;
			boost::uint32_t m_patch;
			T				m_wt;
		};

		class describer_helper
		{
		public:
			
		private:
			describer_helper()
			{
				T g_k[PATCH_SIZE];
				make_gaussian_kernel(g_k, T(3.3));
				for(boost::uint32_t y=0;y<PATCH_SIZE;++y)
					for(boost::uint32_t x=0;x<PATCH_SIZE;++x)
						m_gauss33[y][x]=g_k[y]*g_k[x];
			}

		public:
			static inline describer_helper const& get()
			{
				static describer_helper _describer_helper;
				return _describer_helper;
			}
		public:
			T		m_gauss33[PATCH_SIZE][PATCH_SIZE];
		};

	public:

		

		describer()
			: m_win_samples_size(0)
		{
		}

		template < typename IVT >
		void describe(IVT const &iv, feature_point_t &fp) const
		{
			typedef typename IVT::value_type	iv_pixel_t;
			typedef typename gil::channel_type<iv_pixel_t>::type iv_channel_t;

			static describer_helper const &desc_helper=describer_helper::get();

			T s=static_cast<T>(fp.m_scale*1.2f/9.0f)*T(4)/T(NB)*T(0.9625f);
			//T s=static_cast<T>(fp.m_scale*1.2f/9.0f)*T(4)/T(NB);
			T win_ss=math::constant::one<T>()/s;
			T win_sa=win_ss*win_ss;

			BOOST_ASSERT(win_ss<math::constant::one<T>());
			
			T patch_real_size=T(PATCH_SIZE+1);
			T win_real_size=T(PATCH_SIZE+1)*s;
			
			boost::uint32_t win_size=std::ceil(win_real_size);
			boost::uint32_t win_samples_size=win_size+PATCH_SIZE+1;
			
			if(m_win_samples_size<win_samples_size)
			{
				m_win_samples.reset( new win_sample[win_samples_size] );
				m_win_samples_size=win_samples_size;
			}

			win_sample *pwin_samples=m_win_samples.get();
			
			boost::uint32_t win_samples_count=0;
			for(boost::uint32_t iw=0;iw<win_size;++iw)
			{
				T ps1=T(iw)*win_ss;
				T ps2=T(iw+1)*win_ss;
				boost::uint32_t ips1=static_cast<boost::uint32_t>(ps1);
				boost::uint32_t ips2=static_cast<boost::uint32_t>(ps2);

				if(ips1<ips2)
				{
					pwin_samples[win_samples_count].m_win=iw;
					pwin_samples[win_samples_count].m_patch=ips1;
					pwin_samples[win_samples_count++].m_wt=(T(ips2)-ps1)*s;
					if(ips2<PATCH_SIZE+1)
					{
						pwin_samples[win_samples_count].m_win=iw;
						pwin_samples[win_samples_count].m_patch=ips2;
						pwin_samples[win_samples_count++].m_wt=(ps2-T(ips2))*s;
					}
				}
				else
				{
					pwin_samples[win_samples_count].m_win=iw;
					pwin_samples[win_samples_count].m_patch=ips1;
					pwin_samples[win_samples_count++].m_wt=math::constant::one<T>();
				}
			}

			for(boost::uint32_t y=0;y<PATCH_SIZE+1;++y)
				for(boost::uint32_t x=0;x<PATCH_SIZE+1;++x)
					m_patch[y][x]=math::constant::zero<T>();
			
						
			T sin_ori=std::sin(fp.m_orientation);
            T cos_ori=std::cos(fp.m_orientation);
            
			T win_offset=-T(win_size-1)*math::constant::half<T>();

            T start_x=fp.x()+win_offset*cos_ori+win_offset*sin_ori;
			T start_y=fp.y()-win_offset*sin_ori+win_offset*cos_ori;


			
			for(boost::uint32_t ys=0;ys<win_samples_count;start_x+=sin_ori,start_y+=cos_ori)
            {
				for(boost::uint32_t i=0;i<PATCH_SIZE+1;++i)
					m_patch_buff[i]=math::constant::zero<T>();

                T pixel_x=start_x;
				T pixel_y=start_y;
				
				for(boost::uint32_t xs=0;xs<win_samples_count;pixel_x+=cos_ori,pixel_y-=sin_ori)
                {
					boost::int32_t x=std::min(std::max(math::iround(pixel_x),0),iv.width()-1);
                    boost::int32_t y=std::min(std::max(math::iround(pixel_y),0),iv.height()-1);
					T win_sample=T(*(reinterpret_cast<iv_channel_t const *>(iv.row_begin(y))+x))*win_sa;

					boost::uint32_t xwin=pwin_samples[xs].m_win;
					while(xs<win_samples_count && pwin_samples[xs].m_win==xwin)
					{
						m_patch_buff[pwin_samples[xs].m_patch]+=win_sample*pwin_samples[xs].m_wt;
						xs++;
					}
				}

				boost::uint32_t ywin=pwin_samples[ys].m_win;
				while(ys<win_samples_count && pwin_samples[ys].m_win==ywin)
				{
					for(boost::uint32_t p=0;p<PATCH_SIZE+1;++p)
						m_patch[pwin_samples[ys].m_patch][p]+=m_patch_buff[p]*pwin_samples[ys].m_wt;
					ys++;
				}
            }
#if 0
			ucv::gil::gray8_image_t patch_img(PATCH_SIZE+1,PATCH_SIZE+1);
			ucv::convert(
				ucv::gil::interleaved_view(
					PATCH_SIZE+1, PATCH_SIZE+1,
					reinterpret_cast<win_pixel_t const *>(&m_patch[0][0]),
					(PATCH_SIZE+1)*sizeof(T)
				),
				ucv::gil::view(patch_img),
				ucv::detail::grayscale_convert()
			);
			ucv::gil::png_write_view("patch.png", ucv::gil::const_view(patch_img));
#endif

			for(boost::uint32_t y=0;y<PATCH_SIZE;++y)
			{
				for(boost::uint32_t x=0;x<PATCH_SIZE;++x)
                {
                    T gw=desc_helper.m_gauss33[y][x];
					m_dx[y][x]=(m_patch[y][x+1]-m_patch[y][x]+m_patch[y+1][x+1]-m_patch[y+1][x])*gw;
					m_dy[y][x]=(m_patch[y+1][x]-m_patch[y][x]+m_patch[y+1][x+1]-m_patch[y][x+1])*gw;
                }
			}


#if 0
			for(boost::uint32_t i=0;i<PATCH_SIZE;++i)
			{
				for(boost::uint32_t j=0;j<PATCH_SIZE;++j)
				{
					m_dx[i][j]=0.5f*(1.0f+50.0f*m_dx[i][j]);
					m_dy[i][j]=0.5f*(1.0f+50.0f*m_dy[i][j]);
				}
			}
			ucv::gil::gray8_image_t grad_img(PATCH_SIZE,PATCH_SIZE);
			ucv::convert(
				ucv::gil::interleaved_view(
					PATCH_SIZE, PATCH_SIZE,
					reinterpret_cast<win_pixel_t const *>(&m_dx[0][0]),
					PATCH_SIZE*sizeof(T)
				),
				ucv::gil::view(grad_img),
				ucv::detail::grayscale_convert()
			);
			ucv::gil::png_write_view("dx.png", ucv::gil::const_view(grad_img));

			ucv::convert(
				ucv::gil::interleaved_view(
					PATCH_SIZE, PATCH_SIZE,
					reinterpret_cast<win_pixel_t const *>(&m_dy[0][0]),
					PATCH_SIZE*sizeof(T)
				),
				ucv::gil::view(grad_img),
				ucv::detail::grayscale_convert()
			);
			ucv::gil::png_write_view("dy.png", ucv::gil::const_view(grad_img));
#endif
			//compute the descriptor
			fp.m_descriptor=feature_point_t::descriptor_type::zero();
			for(boost::uint32_t i=0;i<NB;++i)
			{
				for(boost::uint32_t j=0;j<NB;++j)
				{
					math::vector<T,4> &vec=fp.m_descriptor.segment<4>((i*NB+j)<<2);
					
					for(boost::uint32_t y=i*NSB;y<i*NSB+NSB;++y)
					{
						for(boost::uint32_t x=j*NSB;x<j*NSB+5;++x)
						{
							T tx=m_dx[y][x];
							T ty=m_dy[y][x];
							vec[0]+=tx;
							vec[1]+=ty;
							vec[2]+=std::abs(tx);
							vec[3]+=std::abs(ty);
						}
					}
				}
			}
			fp.m_descriptor.normalize();
		}
	
	private:
		mutable boost::scoped_array<win_sample>		m_win_samples;
		mutable	boost::uint32_t						m_win_samples_size;

		mutable	T									m_patch_buff[PATCH_SIZE+1];
		mutable	T									m_patch[PATCH_SIZE+1][PATCH_SIZE+1];
		mutable	T									m_dx[PATCH_SIZE][PATCH_SIZE];
		mutable	T									m_dy[PATCH_SIZE][PATCH_SIZE];
	};

} //namespace surf
} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_SURF_DESCRIBER_H