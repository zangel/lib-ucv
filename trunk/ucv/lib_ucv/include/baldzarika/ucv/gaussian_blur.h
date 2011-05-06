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
		PT const gaussian_blur_filter_traits< PT, 3 >::SEPARABLE_KERNEL[2][3]={ {0.25, 0.5, 0.25}, { 0.25, 0.5, 0.25} };
	}

	template < typename PT, boost::uint32_t KS >
	class gaussian_blur
	{
	public:
		static boost::uint32_t const KERNEL_SIZE=KS;
		static boost::uint32_t const HALF_KERNEL_SIZE=KS/2;

		typedef PT gray_t;
		typedef gil::pixel< gray_t, ucv::gil::gray_layout_t > gray_pixel_t;
		typedef gil::image< gray_pixel_t, false, std::allocator< unsigned char > > gray_image_t;
		typedef typename gray_image_t::view_t gray_view_t;
		typedef typename gray_image_t::const_view_t gray_const_view_t;


		typedef detail::gaussian_blur_filter_traits<PT, KS> filter_tratis_t;

		gaussian_blur()
			: m_frame_size(0,0)
		{
		}

		gaussian_blur(size2ui const &fs)
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
			m_ring_buffer_img.recreate(m_frame_size.width(), KERNEL_SIZE);
		}

		bool operator ()(gray_const_view_t src, gray_view_t dst)
		{
			if(	src.width()!=m_frame_size.width() || src.height()!=m_frame_size.height() ||
				src.width()!=dst.width() || src.height()!=dst.height()
			) return false;

			gray_view_t ring_view=gil::view(m_ring_buffer_img);

			boost::int32_t ring_buff_pos=-boost::int32_t(KERNEL_SIZE);

			for(boost::uint32_t r=0;r<m_frame_size.height();++r)
			{
				for(;ring_buff_pos<boost::int32_t(r);++ring_buff_pos)
				{
					boost::int32_t sr=ring_buff_pos+HALF_KERNEL_SIZE+1;
					sr=std::abs(sr);
					sr-=(sr/m_frame_size.height())*2*((sr+1)%m_frame_size.height());


					gray_t const *src_row=reinterpret_cast<gray_t const *>(src.row_begin(sr));

					gray_t *ring_row=reinterpret_cast<gray_t *>(ring_view.row_begin((ring_buff_pos-1)%KERNEL_SIZE));
					
					for(boost::int32_t c=0;c<boost::int32_t(m_frame_size.width());++c)
					{
						gray_t dst_val=detail::constant::zero<gray_t>();
						for(boost::uint32_t f=0;f<KERNEL_SIZE;++f)
						{
							boost::int32_t img_col=std::max<boost::int32_t>(0, std::min<boost::int32_t>(m_frame_size.width()-1, (c+f)-HALF_KERNEL_SIZE));
							dst_val+=src_row[img_col]*filter_tratis_t::SEPARABLE_KERNEL[0][f];
						}
						ring_row[c]=dst_val;
					}
				}

				gray_t const *ring_rows[KERNEL_SIZE];
				
				for(boost::uint32_t rr=0;rr<KERNEL_SIZE;++rr)
					ring_rows[rr]=reinterpret_cast<gray_t const *>(ring_view.row_begin((r+rr)%KERNEL_SIZE));

				gray_t *dst_row=reinterpret_cast<gray_t *>(dst.row_begin(r));
				
				for(boost::uint32_t c=0;c<m_frame_size.width();++c)
				{
					gray_t dst_val=detail::constant::zero<gray_t>();
					
					for(boost::uint32_t f=0;f<KERNEL_SIZE;++f)
						dst_val+=ring_rows[f][c]*filter_tratis_t::SEPARABLE_KERNEL[1][f];
					dst_row[c]=dst_val;
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


#endif //BALDZARIKA_UCV_GAUSSIAN_BLUR_H
