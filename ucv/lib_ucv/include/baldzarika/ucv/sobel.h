#ifndef BALDZARIKA_UCV_SOBEL_H
#define BALDZARIKA_UCV_SOBEL_H

namespace baldzarika { namespace ucv {

	namespace detail {

		template < typename PT, boost::uint32_t KS, boost::uint32_t O >
		struct sobel_filter_traits { };


		template < typename PT >
		struct sobel_filter_traits< PT, 3, 1 >
		{
			static PT const SEPARABLE_X_KERNEL[2][3];
			static PT const SEPARABLE_Y_KERNEL[2][3];
		};


		template < typename PT >
		PT const sobel_filter_traits< PT, 3, 1 >::SEPARABLE_X_KERNEL[2][3]={ {-1.0, 0.0, 1.0}, { 1.0, 2.0, 1.0} };
		
		template < typename PT >
		PT const sobel_filter_traits< PT, 3, 1 >::SEPARABLE_Y_KERNEL[2][3]={ { 1.0, 2.0, 1.0}, {-1.0, 0.0, 1.0} };
	}

	
	
	template < typename PT, boost::uint32_t KS , boost::uint32_t O >
	class sobel
	{
	public:
		static boost::uint32_t const KERNEL_SIZE=KS;
		static boost::uint32_t const HALF_KERNEL_SIZE=KS/2;

		typedef PT gray_t;
		typedef gil::pixel< gray_t, ucv::gil::gray_layout_t > gray_pixel_t;
		typedef gil::image< gray_pixel_t, false, std::allocator< unsigned char > > gray_image_t;
		typedef typename gray_image_t::view_t gray_view_t;
		typedef typename gray_image_t::const_view_t gray_const_view_t;


		typedef detail::sobel_filter_traits<PT, KS, O> filter_tratis_t;

		sobel()
			: m_frame_size(0,0)
		{
		}

		sobel(size2ui const &fs)
			: m_frame_size(0,0)
		{
			set_frame_size(fs);
		}

		size2ui const &get_frame_size() const
		{
			return m_frame_size;
		}

		void set_frame_size(size2ui const &fs)
		{
			if(m_frame_size==fs)
				return;
			m_frame_size=fs;
			m_ring_buffer_img.recreate(m_frame_size.width(), KERNEL_SIZE*2);
		}

		bool operator ()(gray_const_view_t img, gray_view_t dx, gray_view_t dy)
		{
			if(	img.width()!=m_frame_size.width() || img.height()!=m_frame_size.height() ||
				img.width()!=dx.width() || img.height()!=dx.height() ||
				img.width()!=dy.width() || img.height()!=dy.height()
			) return false;

			gray_view_t ring_dx_view=gil::subimage_view(gil::view(m_ring_buffer_img), 0, 0*KERNEL_SIZE, m_frame_size.width(), KERNEL_SIZE);
			gray_view_t ring_dy_view=gil::subimage_view(gil::view(m_ring_buffer_img), 0, 1*KERNEL_SIZE, m_frame_size.width(), KERNEL_SIZE);

			boost::int32_t ring_buff_pos=-boost::int32_t(KERNEL_SIZE);

			for(boost::uint32_t r=0;r<m_frame_size.height();++r)
			{
				for(;ring_buff_pos<boost::int32_t(r);++ring_buff_pos)
				{
					boost::int32_t sr=ring_buff_pos+HALF_KERNEL_SIZE+1;
					sr=std::abs(sr);
					sr-=(sr/m_frame_size.height())*2*((sr+1)%m_frame_size.height());


					gray_t const *img_row=reinterpret_cast<gray_t const *>(img.row_begin(sr));

					gray_t *ring_dx_row=reinterpret_cast<gray_t *>(ring_dx_view.row_begin((ring_buff_pos-1)%KERNEL_SIZE));
					gray_t *ring_dy_row=reinterpret_cast<gray_t *>(ring_dy_view.row_begin((ring_buff_pos-1)%KERNEL_SIZE));
										
					for(boost::int32_t c=0;c<boost::int32_t(m_frame_size.width());++c)
					{
						gray_t dx=detail::constant::zero<gray_t>();
						gray_t dy=detail::constant::zero<gray_t>();
						for(boost::uint32_t f=0;f<KERNEL_SIZE;++f)
						{
							boost::int32_t img_col=std::max<boost::int32_t>(0, std::min<boost::int32_t>(m_frame_size.width()-1, (c+f)-HALF_KERNEL_SIZE));
							dx+=img_row[img_col]*filter_tratis_t::SEPARABLE_X_KERNEL[0][f];
							dy+=img_row[img_col]*filter_tratis_t::SEPARABLE_Y_KERNEL[0][f];
						}
						ring_dx_row[c]=dx;
						ring_dy_row[c]=dy;
					}
				}

				gray_t const *ring_dx_rows[KERNEL_SIZE];
				gray_t const *ring_dy_rows[KERNEL_SIZE];

				for(boost::uint32_t rr=0;rr<KERNEL_SIZE;++rr)
				{
					ring_dx_rows[rr]=reinterpret_cast<gray_t const *>(ring_dx_view.row_begin((r+rr)%KERNEL_SIZE));
					ring_dy_rows[rr]=reinterpret_cast<gray_t const *>(ring_dy_view.row_begin((r+rr)%KERNEL_SIZE));
				}

				gray_t *dx_row=reinterpret_cast<gray_t *>(dx.row_begin(r));
				gray_t *dy_row=reinterpret_cast<gray_t *>(dy.row_begin(r));
				
				for(boost::uint32_t c=0;c<m_frame_size.width();++c)
				{
					gray_t dx=detail::constant::zero<gray_t>();
					gray_t dy=detail::constant::zero<gray_t>();

					for(boost::uint32_t f=0;f<KERNEL_SIZE;++f)
					{
						dx+=ring_dx_rows[f][c]*filter_tratis_t::SEPARABLE_X_KERNEL[1][f];
						dy+=ring_dy_rows[f][c]*filter_tratis_t::SEPARABLE_Y_KERNEL[1][f];
					}
					dx_row[c]=dx;
					dy_row[c]=dy;
				}
			}
			return true;
		}
	
	private:
		size2ui			m_frame_size;
		gray_image_t	m_ring_buffer_img;
	};

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_SOBEL_H