#ifndef BALDZARIKA_UCV_KLT_TRACKER_H
#define BALDZARIKA_UCV_KLT_TRACKER_H

namespace baldzarika { namespace ucv {

	template < boost::uint32_t I, boost::uint32_t F >
	class klt_tracker
	{
	public:
		typedef fixed_point<I, F> integral_t;
		typedef gil::pixel<integral_t, ucv::gil::gray_layout_t> integral_pixel_t;
		typedef gil::image< integral_pixel_t, false, std::allocator<unsigned char> > integral_image_t;
		typedef integral_image_t::view_t integral_view_t;
		
		klt_tracker(size2ui const &ws, integral_view_t prev, integral_view_t curr)
			: m_win_size(ws)
			, m_window((ws.width()+2)*4, ws.height()+2)
			, m_prev(prev)
			, m_curr(curr)
		{
		}

		integral_view_t prev_dx()
		{
			return gil::subimage_view(m_window, 0*(2+m_win_size.width()), 0, 2+m_win_size.width(), 2+m_win_size.height());
		}

		integral_view_t prev_dy()
		{
			return gil::subimage_view(m_window, 1*(2+m_win_size.width()), 0, 2+m_win_size.width(), 2+m_win_size.height());
		}

		integral_view_t curr_dx()
		{
			return gil::subimage_view(m_window, 2*(2+m_win_size.width()), 0, 2+m_win_size.width(), 2+m_win_size.height());
		}

		integral_view_t curr_dy()
		{
			return gil::subimage_view(m_window, 3*(2+m_win_size.width()), 0, 2+m_win_size.width(), 2+m_win_size.height());
		}

	private:
		size2ui				m_win_size;
		integral_image_t	m_window;
		integral_view_t		m_prev;
		integral_view_t		m_curr;
	};

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_KLT_TRACKER_H