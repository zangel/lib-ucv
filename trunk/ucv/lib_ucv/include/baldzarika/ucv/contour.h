#ifndef BALDZARIKA_UCV_CONTOUR_H
#define BALDZARIKA_UCV_CONTOUR_H

#include <baldzarika/ucv/point2.h>
#include <baldzarika/ucv/vector.h>
#include <baldzarika/ucv/box2.h>
#include <baldzarika/ucv/approximate_polygon.h>

namespace baldzarika { namespace ucv {

	template < typename T >
	class contour
	{
	public:
		typedef enum
		{
			kOriCCW=-1,
			kOriInvalid=0,
			kOriCW=1
		} eOrientation;

		typedef box2<T> box2_t;
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
		
		static box2_t compute_bounding_box(points_t const &pts)
		{
			if(pts.empty())
				return box2_t();

			T min_x=std::numeric_limits<T>::max();
			T max_x=std::numeric_limits<T>::lowest();

			T min_y=std::numeric_limits<T>::max();
			T max_y=std::numeric_limits<T>::lowest();

			for(boost::uint32_t p=0;p<pts.size();++p)
			{
				point2_t const &pt=pts[p];
				min_x=std::min(min_x,pt.x);
				max_x=std::max(max_x,pt.x);
				min_y=std::min(min_y,pt.y);
				max_y=std::max(max_y,pt.y);
			}

			return box2_t(
				point2_t(min_x,min_y),
				point2_t(max_x,max_y)
			);
		}

		static eOrientation compute_orientation(points_t const &pts, box2_t const &bbox, bool is_closed)
		{
			return kOriInvalid;
		}

		contour()
			: m_is_closed(false)
		{
		}

		contour(points_t const &pts, box2_t const &bbox, bool is_closed, eOrientation ori)
			: m_points(pts)
			, m_bbox(bbox)
			, m_is_closed(is_closed)
			, m_orientation(ori)
		{
		}

		contour(points_t const &pts, T const &eps)
			: m_points(pts)
			, m_bbox(compute_bounding_box(pts))
			, m_is_closed(check_is_closed(pts,eps))
			, m_orientation()
		{
			if(m_is_closed)
				m_points.pop_back();
		}

		void aproximate(T const &eps)
		{
			approximate_polygon(m_points, m_bbox, m_is_closed, eps);
		}

		points_t		m_points;
		box2_t			m_bbox;
		bool			m_is_closed;
		eOrientation	m_orientation;
	};

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_CONTOUR_H