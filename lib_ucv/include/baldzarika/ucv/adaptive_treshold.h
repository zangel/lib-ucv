#ifndef BALDZARIKA_UCV_ADAPTIVE_TRESHOLD_H
#define BALDZARIKA_UCV_ADAPTIVE_TRESHOLD_H

namespace baldzarika { namespace ucv {

	template < typename PT, boost::uint32_t MS , bool IBT=false >
	class adaptive_treshold
	{
	public:

		typedef gaussian_blur< PT, MS > gaussian_blur_t;

		typedef typename gaussian_blur_t::gray_t gray_t;
		typedef typename gaussian_blur_t::gray_pixel_t gray_pixel_t;
		typedef typename gaussian_blur_t::gray_image_t gray_image_t;
		typedef typename gaussian_blur_t::gray_view_t gray_view_t;
		typedef typename gaussian_blur_t::gray_const_view_t gray_const_view_t;
		
		adaptive_treshold()
			: m_frame_size(0,0)
			, m_max_value(detail::constant::one<gray_t>())
			, m_delta(0.2)
		{
		}

		adaptive_treshold(math::size2ui const &fs, gray_t const &max_value, gray_t const &delta=gray_t(0.2) )
			: m_frame_size(0,0)
			, m_max_value(max_value)
			, m_delta(delta)
		{
			set_frame_size(fs);
		}

		~adaptive_treshold()
		{
		}

		math::size2ui const& get_frame_size() const
		{
			return m_frame_size;
		}

		void set_frame_size(math::size2ui const &fs)
		{
			if(m_frame_size==fs)
				return;
			m_frame_size=fs;
			m_median_img.recreate(m_frame_size.width(), m_frame_size.height());
			m_gaussian_blur.set_frame_size(fs);
		}

		bool operator()(gray_const_view_t src, gray_view_t dst)
		{
			if(	src.width()!=m_frame_size.width() || src.height()!=m_frame_size.height() ||
				src.width()!=dst.width() || src.height()!=dst.height()
			) return false;

			gray_view_t median_view=gil::view(m_median_img);
			
			if(!m_gaussian_blur(src, median_view))
				return false;

			for(boost::uint32_t y=0;y<m_frame_size.height();++y)
			{
				gray_t const *src_row=reinterpret_cast<gray_t const *>(src.row_begin(y));
				gray_t const *median_row=reinterpret_cast<gray_t const *>(median_view.row_begin(y));
				gray_t *dst_row=reinterpret_cast<gray_t *>(dst.row_begin(y));

				for(boost::uint32_t x=0;x<m_frame_size.width();++x)
					dst_row[x]=treshold<IBT>(src_row[x]-median_row[x]);
			}
			
			return true;
		}

	protected:
		template < bool INV >
		inline gray_t const& treshold(gray_t const &d) const;

		template <>
		inline gray_t const& treshold<false>(gray_t const &d) const
		{
			return d>=-m_delta?m_max_value:math::constant::zero<gray_t>();
		}

		template <>
		inline gray_t const& treshold<true>(gray_t const &d) const
		{
			return d<-m_delta?m_max_value:math::constant::zero<gray_t>();
		}

	private:
		math::size2ui				m_frame_size;
		gray_t				m_max_value;
		gray_t				m_delta;
		gray_image_t		m_median_img;
		gaussian_blur_t		m_gaussian_blur;
	};

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_ADAPTIVE_TRESHOLD_H
