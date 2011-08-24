#ifndef BALDZARIKA_UCV_SURF_DESCRIBER_H
#define BALDZARIKA_UCV_SURF_DESCRIBER_H

#include <baldzarika/ucv/make_gaussian_kernel.h>
#include <baldzarika/ucv/surf/feature_point.h>

namespace baldzarika { namespace ucv { namespace surf {

	namespace detail {

		template < typename T , boost::uint32_t NB, boost::uint32_t NSB >
		class describer_helper
		{
		public:
			static boost::uint32_t const KERNEL_SIZE=NB*NSB;
		private:
			describer_helper()
			{
				T g_k[KERNEL_SIZE];
				make_gaussian_kernel(g_k, T(3.3));
				for(boost::uint32_t y=0;y<KERNEL_SIZE;++y)
				{
					for(boost::uint32_t x=0;x<KERNEL_SIZE;++x)
					{
						m_gauss33[y][x]=g_k[y]*g_k[x];
					}
				}
			}

		public:
			static inline describer_helper const& get()
			{
				static describer_helper _describer_helper;
				return _describer_helper;
			}
		public:
			T		m_gauss33[KERNEL_SIZE][KERNEL_SIZE];
		};


	} //namespace detail

	template < typename T, boost::uint32_t NB, boost::uint32_t NSB >
	class describer
	{
	public:

		typedef feature_point<T, NB> feature_point_t;
		typedef detail::describer_helper<T,NB,NSB> describer_helper_t;

		typedef typename gil::pixel<T, gil::gray_layout_t> win_pixel_t;
		typedef typename gil::image< win_pixel_t, false, std::allocator<unsigned char> > win_image_t;
		typedef typename win_image_t::view_t win_view_t;
		typedef typename win_image_t::const_view_t win_const_view_t;


		describer()
		{
		}

		template < typename IVT >
		void describe(IVT const &iv, feature_point_t &fp) const
		{
			typedef typename IVT::value_type	iv_pixel_t;
			typedef typename gil::channel_type<iv_pixel_t>::type iv_channel_t;

			boost::int32_t fp_scale=fp.m_scale;
			T s=static_cast<T>(fp_scale*1.2f/9.0f);
			boost::int32_t win_size=math::iround(T(describer_helper_t::KERNEL_SIZE+1)*s);
			BOOST_ASSERT(m_win_image.width()==m_win_image.height());
			if(m_win_image.width()<win_size+1) m_win_image.recreate(win_size+1,win_size+1);

			win_view_t win_view=gil::subimage_view(
				gil::view(m_win_image),
				0, 0,
				win_size+1, win_size+1
			);

			T sin_ori=std::sin(fp.m_orientation);
            T cos_ori=std::cos(fp.m_orientation) ;
            
			T win_offset=-T(win_size-1)*math::constant::half<T>();

            T start_x=fp.x()+win_offset*cos_ori+win_offset*sin_ori;
			T start_y=fp.y()-win_offset*sin_ori+win_offset*cos_ori;


			//build integral window
			T *win_row=reinterpret_cast<T*>(win_view.row_begin(0));

			for(boost::int32_t j=0;j<=win_size;++j)
				win_row[j]=math::constant::zero<T>();

			win_row++;

			for(boost::int32_t i=0;i<win_size;++i,start_x+=sin_ori,start_y+=cos_ori)
            {
                T pixel_x=start_x;
				T pixel_y=start_y;
				
				T *prev_win_row=win_row;
				win_row=reinterpret_cast<T*>(win_view.row_begin(i+1))+1;

				T sum=win_row[-1]=math::constant::zero<T>();
								
				for(boost::int32_t j=0;j<win_size;j++,pixel_x+=cos_ori,pixel_y-=sin_ori)
                {
					boost::int32_t x=std::min(std::max(math::iround(pixel_x),0),iv.width()-1);
                    boost::int32_t y=std::min(std::max(math::iround(pixel_y),0),iv.height()-1);
					win_row[j]=prev_win_row[j]+(sum+=T(*(reinterpret_cast<iv_channel_t const *>(iv.row_begin(y))+x)));
				}
            }

			//compute gradients using integral window
			for(boost::uint32_t i=0;i<describer_helper_t::KERNEL_SIZE;++i)
			{
				for(boost::uint32_t j=0;j<describer_helper_t::KERNEL_SIZE;++j)
				{
					m_dx[i][j]=m_dy[i][j]=math::constant::zero<T>();
				}
			}

			T win_scale=T(win_size)/T(describer_helper_t::KERNEL_SIZE+1);
			boost::int32_t ww_step=win_view.row_begin(1)-win_view.row_begin(0);
			integral_box<T> ib=
				integral_box<T>(
					math::point2i(0,0),
					math::size2i(1,1),
					ww_step,
					math::constant::one<T>()
				).scaled(win_scale,ww_step);

			
			T const *cwin_row=reinterpret_cast<T const *>(win_view.row_begin(0));
			//top left
			T w_tl=integral_sample<T,T>(
				cwin_row,
				ib
			);
			m_dx[0][0]-=w_tl;
			m_dy[0][0]-=w_tl;
			
			//top row
			T wx=win_scale;
			for(boost::uint32_t x=0;x<describer_helper_t::KERNEL_SIZE;++x, wx+=win_scale)
			{
				T w=integral_sample<T,T>(
					cwin_row+math::iround(wx),
					ib
				);
				m_dx[0][x-1]+=w;
				m_dx[0][x]-=w;
				m_dy[0][x-1]-=w;
				m_dy[0][x]-=w;
			}

			//top right
			T w_tr=integral_sample<T,T>(
				reinterpret_cast<T const *>(cwin_row+math::iround(T(describer_helper_t::KERNEL_SIZE)*win_scale)),
				ib
			);
			m_dx[0][describer_helper_t::KERNEL_SIZE-1]+=w_tr;
			m_dy[0][describer_helper_t::KERNEL_SIZE-1]-=w_tr;

			T wy=win_scale;
			for(boost::uint32_t y=1;y<describer_helper_t::KERNEL_SIZE;++y, wy+=win_scale)
			{
				cwin_row=reinterpret_cast<T const *>(win_view.row_begin(math::iround(wy)));

				//left
				T w_l=integral_sample<T,T>(
					cwin_row,
					ib
				);
				
				m_dx[y-1][0]-=w_l;
				m_dx[y][0]-=w_l;
				m_dy[y-1][0]+=w_l;
				m_dy[y][0]-=w_l;

				wx=win_scale;
				for(boost::uint32_t x=1;x<describer_helper_t::KERNEL_SIZE;++x, wx+=win_scale)
				{
					T w=integral_sample<T,T>(
						cwin_row+math::iround(wx),
						ib
					);
					m_dx[y-1][x-1]+=w;
					m_dx[y-1][x]-=w;
					m_dx[y][x-1]+=w;
					m_dx[y][x]-=w;

					m_dy[y-1][x-1]+=w;
					m_dy[y-1][x]+=w;
					m_dy[y][x-1]-=w;
					m_dy[y][x]-=w;
				}

				//right
				T w_r=integral_sample<T,T>(
					cwin_row+math::iround(T(describer_helper_t::KERNEL_SIZE)*win_scale),
					ib
				);
				m_dx[y-1][describer_helper_t::KERNEL_SIZE-1]+=w_r;
				m_dx[y][describer_helper_t::KERNEL_SIZE-1]+=w_r;

				m_dy[y-1][describer_helper_t::KERNEL_SIZE-1]+=w_r;
				m_dy[y][describer_helper_t::KERNEL_SIZE-1]-=w_r;
			}

			cwin_row=reinterpret_cast<T const *>(win_view.row_begin(math::iround(T(describer_helper_t::KERNEL_SIZE)*win_scale)));

			//bottom left
			T w_bl=integral_sample<T,T>(
				cwin_row,
				ib
			);
			m_dx[describer_helper_t::KERNEL_SIZE-1][0]-=w_bl;
			m_dy[describer_helper_t::KERNEL_SIZE-1][0]+=w_bl;
			
			//bottom row
			wx=win_scale;
			for(boost::uint32_t x=1;x<describer_helper_t::KERNEL_SIZE;++x, wx+=win_scale)
			{
				T w=integral_sample<T,T>(
					cwin_row+math::iround(wx),
					ib
				);

				m_dx[describer_helper_t::KERNEL_SIZE-1][x-1]+=w_bl;
				m_dx[describer_helper_t::KERNEL_SIZE-1][x]-=w_bl;

				m_dy[describer_helper_t::KERNEL_SIZE-1][x-1]+=w_bl;
				m_dy[describer_helper_t::KERNEL_SIZE-1][x]+=w_bl;
			}


			//bottom right
			T w_br=integral_sample<T,T>(
				cwin_row+math::iround(T(describer_helper_t::KERNEL_SIZE)*win_scale),
				ib
			);
			m_dx[describer_helper_t::KERNEL_SIZE-1][describer_helper_t::KERNEL_SIZE-1]+=w_br;
			m_dy[describer_helper_t::KERNEL_SIZE-1][describer_helper_t::KERNEL_SIZE-1]+=w_br;


		}
	
	private:
		mutable win_image_t		m_win_image;
		mutable	T				m_dx[describer_helper_t::KERNEL_SIZE][describer_helper_t::KERNEL_SIZE];
		mutable	T				m_dy[describer_helper_t::KERNEL_SIZE][describer_helper_t::KERNEL_SIZE];
	};

} //namespace surf
} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_SURF_DESCRIBER_H