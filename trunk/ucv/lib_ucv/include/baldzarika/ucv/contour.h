#ifndef BALDZARIKA_UCV_CONTOUR_H
#define BALDZARIKA_UCV_CONTOUR_H

#include <baldzarika/ucv/point2.h>
#include <baldzarika/ucv/vector.h>
#include <baldzarika/ucv/approximate_polygon.h>

namespace baldzarika { namespace ucv {

	template < typename T >
	class contour
	{
	public:
		typedef point2<T> point2_t;
		typedef std::vector<point2_t> points_t;

		static bool check_is_closed(points_t const &pts, T const &eps)
		{
			if(pts.size()>3)
			{
				float dist=vector<T,2>(pts.back()-pts.front()).length();
				return (vector<T,2>(pts.back()-pts.front()).length()<=eps);
			}
			return false;
		}

		contour()
			: m_is_closed(false)
		{
		}

		contour(points_t const &pts, bool is_closed)
			: m_is_closed(is_closed)
			, m_points(pts)
		{
		}

		contour(points_t const &pts, T const &eps)
			: m_is_closed(check_is_closed(pts,eps))
			, m_points(pts)
		{
			if(m_is_closed)
				m_points.pop_back();
		}

		void aproximate(T const &eps)
		{
			approximate_polygon(m_points, m_is_closed, eps);
		}

		bool		m_is_closed;
		points_t	m_points;
	};

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_CONTOUR_H