#ifndef BALDZARIKA_UCV_CONTOUR_H
#define BALDZARIKA_UCV_CONTOUR_H

#include <baldzarika/ucv/approximate_polygon.h>

namespace baldzarika { namespace ucv {

	template < typename T >
	class contour
	{
	public:
		typedef math::box2<T> box2_t;
		typedef math::point2<T> point2_t;
		typedef std::vector<point2_t> points_t;

		static bool check_is_closed(points_t const &pts, T const &eps)
		{
			if(pts.size()>4)
				return (math::vector<T,2>(pts.back()-pts.front()).length()<=eps);
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

		static void compute_orientation_and_convexity(points_t const &pts, box2_t const &bbox, bool is_closed, bool &cw, bool &convex)
		{
			boost::int32_t const count=boost::int32_t(pts.size());
			T const inv_width=math::constant::one<T>()/bbox.width();
			T const inv_height=math::constant::one<T>()/bbox.height();

			T cross_sum=math::constant::zero<T>();
			bool all_positive=true;
			bool all_negative=true;

			math::point2<T> prev_pt=is_closed?pts.back():pts.front();
			math::point2<T> curr_pt=is_closed?pts.front():pts[1];

			for(boost::int32_t p=(is_closed?0:1);p<(count-(is_closed?0:1));++p)
			{
				math::point2<T> next_pt=pts[(p+1)%count];

				math::point2<T> a(next_pt-curr_pt);
				a.x()*=inv_width;
				a.y()*=inv_height;

				math::point2<T> b(prev_pt-curr_pt);
				b.x()*=inv_width;
				b.y()*=inv_height;

				T cross_prod=a.x()*b.y()-a.y()*b.x();
				cross_sum+=cross_prod;
				all_positive=all_positive && cross_prod>=math::constant::zero<T>();
				all_negative=all_negative && cross_prod<=math::constant::zero<T>();

				prev_pt=curr_pt;
				curr_pt=next_pt;
			}

			cw=cross_sum>=math::constant::zero<T>();
			convex=
				(cross_sum>=math::constant::zero<T>() && all_positive) ||
				(cross_sum<=math::constant::zero<T>() && all_negative);
		}

		contour()
			: m_is_closed(false)
			, m_is_clockwise(false)
			, m_is_convex(false)
		{
		}

		contour(points_t const &pts, box2_t const &bbox, bool is_closed, bool is_clockwise, bool is_convex)
			: m_points(pts)
			, m_bbox(bbox)
			, m_is_closed(is_closed)
			, m_is_clockwise(is_clockwise)
			, m_is_convex(is_convex)
		{
		}

		void aproximate(T const &eps)
		{
			approximate_polygon(m_points, m_bbox, m_is_closed, eps);
			compute_orientation_and_convexity(m_points, m_bbox, m_is_closed, m_is_clockwise, m_is_convex);
		}

		points_t		m_points;
		box2_t			m_bbox;
		bool			m_is_closed;
		bool			m_is_clockwise;
		bool			m_is_convex;
	};

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_CONTOUR_H
