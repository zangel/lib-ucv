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
			static boost::uint32_t const PATCH_SIZE=NB*NSB;
		private:
			describer_helper()
			{
				T g_k[PATCH_SIZE];
				make_gaussian_kernel(g_k, T(3.3));
				for(boost::uint32_t y=0;y<PATCH_SIZE;++y)
				{
					for(boost::uint32_t x=0;x<PATCH_SIZE;++x)
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
			T		m_gauss33[PATCH_SIZE][PATCH_SIZE];
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
			boost::int32_t win_size=static_cast<boost::int32_t>(T(describer_helper_t::PATCH_SIZE+1)*s);
			BOOST_ASSERT(m_win_image.width()==m_win_image.height());
			if(m_win_image.width()<win_size) m_win_image.recreate(win_size,win_size);

			win_view_t win_view=gil::subimage_view(
				gil::view(m_win_image),
				0, 0,
				win_size, win_size
			);

			T sin_ori=std::sin(fp.m_orientation);
            T cos_ori=std::cos(fp.m_orientation) ;
            
			T win_offset=-T(win_size-1)*math::constant::half<T>();

            T start_x=fp.x()+win_offset*cos_ori+win_offset*sin_ori;
			T start_y=fp.y()-win_offset*sin_ori+win_offset*cos_ori;

			for(boost::int32_t i=0;i<win_size;++i,start_x+=sin_ori,start_y+=cos_ori)
            {
                T pixel_x=start_x;
				T pixel_y=start_y;
				T *win_row=reinterpret_cast<T*>(win_view.row_begin(i));
				
				for(boost::int32_t j=0;j<win_size;j++,pixel_x+=cos_ori,pixel_y-=sin_ori)
                {
					boost::int32_t x=std::min(std::max(math::iround(pixel_x),0),iv.width()-1);
                    boost::int32_t y=std::min(std::max(math::iround(pixel_y),0),iv.height()-1);
                    *win_row++=T(*(reinterpret_cast<iv_channel_t const *>(iv.row_begin(y))+x));
                }
            }
		}
	
	private:
		mutable win_image_t		m_win_image;
	};

} //namespace surf
} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_SURF_DESCRIBER_H