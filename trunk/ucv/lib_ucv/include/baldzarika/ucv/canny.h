#ifndef BALDZARIKA_UCV_CANNY_H
#define BALDZARIKA_UCV_CANNY_H

#include <baldzarika/ucv/sobel.h>

namespace baldzarika { namespace ucv {

	template < typename PT, boost::uint32_t AS >
	class canny
	{
	public:

		typedef sobel< PT, AS, 1 > sobel_t;

		typedef typename sobel_t::gray_t gray_t;
		typedef typename sobel_t::gray_pixel_t gray_pixel_t;
		typedef typename sobel_t::gray_image_t gray_image_t;
		typedef typename sobel_t::gray_view_t gray_view_t;
		typedef typename sobel_t::gray_const_view_t gray_const_view_t;

		typedef gil::gray8_image_t	map_image_t;
		typedef typename map_image_t::view_t map_view_t;
		typedef typename map_image_t::const_view_t map_const_view_t;
		typedef typename map_view_t::value_type map_pixel_t;
		typedef typename gil::channel_type<map_pixel_t>::type map_t;

		typedef std::stack< map_t*, std::vector< map_t* > >	map_stack_t;
		
		canny()
			: m_frame_size(0,0)
		{
		}

		canny(size2ui const &fs)
			: m_frame_size(0,0)
		{
			set_frame_size(fs);
			m_sobel.set_frame_size(fs);
		}

		~canny()
		{
		}

		size2ui const& get_frame_size() const
		{
			return m_frame_size;
		}

		void set_frame_size(size2ui const &fs)
		{
			if(m_frame_size==fs)
				return;
			m_frame_size=fs;
			m_sobel.set_frame_size(fs);
			m_dx_dy_img.recreate(fs.width(), fs.height()*2);
			m_mag_ring_img.recreate(fs.width()+2, 3);
		}

		bool operator()(gray_const_view_t img)
		{
			static gray_t const tan_22_5=0.4142135623730950488016887242097;
			static gray_t const tan_67_5=2.4142135623730950488016887242097;

			if(	img.width()!=m_frame_size.width() || img.height()!=m_frame_size.height())
				return false;
			
			if(!m_sobel(img, get_dx_view(), get_dy_view()))
				return false;

			gray_const_view_t dx_view=get_dx_const_view();
			gray_const_view_t dy_view=get_dy_const_view();

			gray_view_t mag_buf_view[3]=
			{
				get_mag_view<0>(),
				get_mag_view<1>(),
				get_mag_view<2>()
			};

			gray_t *mag_buf[3]=
			{
				reinterpret_cast<gray_t *>(mag_buf_view[0].row_begin(0)),
				reinterpret_cast<gray_t *>(mag_buf_view[1].row_begin(0)),
				reinterpret_cast<gray_t *>(mag_buf_view[2].row_begin(0))
			};

			std::fill(mag_buf[0], mag_buf[0]+m_frame_size.width()+2, detail::constant::zero<gray_t>());

			map_view_t map_view=gil::view(m_map_img);

			{
				map_t *map_first_row=reinterpret_cast<map_t *>(map_view.row_begin(0));
				map_t *map_last_row=reinterpret_cast<map_t *>(map_view.row_begin(map_view.height()-1));
				std::fill(map_first_row, map_first_row+m_frame_size.width()+2, detail::constant::one<map_t>());
				std::fill(map_last_row, map_last_row+m_frame_size.width()+2, detail::constant::one<map_t>());
			}
			

			for(boost::uint32_t y=0;y<=m_frame_size.height();++y)
			{
				gray_t *mag=mag_buf[(y>0?1:2)]+1;
				gray_t const *dx_row=reinterpret_cast<gray_t const *>(dx_view.row_begin(y));
				gray_t const *dy_row=reinterpret_cast<gray_t const *>(dy_view.row_begin(y));

				if(y<m_frame_size.height())
				{
					mag[-1]=mag[m_frame_size.width()]=detail::constant::zero<gray_t>();
					for(boost::uint32_t x=0;x<m_frame_size.width();++x)
						mag[j]=abs(dx_row[x])+abs(dy_row[x]);
				}
				else
				{
					std::fill(mag, mag+m_frame_size.width()+2, detail::constant::zero<gray_t>());
				}

				if(y==0) continue;

				map_t *map=reinterpret_cast<map_t *>(map_view.row_begin(y))+1;
				map[-1]=map[m_frame_size.width()]=detail::constant::one<map_t>();

				mag=mag_buf[1]+1;
				dx_row=reinterpret_cast<gray_t const *>(dx_view.row_begin(y-1));
				dy_row=reinterpret_cast<gray_t const *>(dy_view.row_begin(y-1));
								
				gray_t *tmp_mag_buf=mag_buf_view[0];
				mag_buf_view[0]=mag_buf_view[1];
				mag_buf_view[1]=mag_buf_view[2];
				mag_buf_view[2]=tmp_mag_buf;
			}
			return true;
		}

	protected:
		gray_const_view_t get_dx_const_view() const
		{
			return gil::subimage_view(
				gil::const_view(m_dx_dy_img),
				0, 0,
				m_frame_size.width(), m_frame_size.height()
			);
		}

		gray_view_t get_dx_view()
		{
			return gil::subimage_view(
				gil::view(m_dx_dy_img),
				0, 0,
				m_frame_size.width(), m_frame_size.height()
			);
		}

		gray_const_view_t get_dy_const_view() const
		{
			return gil::subimage_view(
				gil::const_view(m_dx_dy_img),
				0, m_frame_size.height(),
				m_frame_size.width(), m_frame_size.height()
			);
		}

		gray_view_t get_dy_view()
		{
			return gil::subimage_view(
				gil::view(m_dx_dy_img),
				0, m_frame_size.height(),
				m_frame_size.width(), m_frame_size.height()
			);
		}

		template < boost::uint32_t I >
		gray_const_view_t get_mag_const_view() const
		{
			BOOST_STATIC_ASSERT(I>=0 && I<3);
			return gil::subimage_view(
				gil::const_view(m_mag_ring_img),
				0, I,
				m_frame_size.width()+2, 1
			);
		}

		template < boost::uint32_t I >
		gray_view_t get_mag_view()
		{
			BOOST_STATIC_ASSERT(I>=0 && I<3);
			return gil::subimage_view(
				gil::view(m_mag_ring_img),
				0, I,
				m_frame_size.width()+2, 1
			);
		}
	
	private:
		size2ui				m_frame_size;
		sobel_t				m_sobel;
		gray_image_t		m_dx_dy_img;
		gray_image_t		m_mag_ring_img;
		map_image_t			m_map_img;
	};

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_CANNY_H