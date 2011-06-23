#ifndef BALDZARIKA_MATH_BOX2_H
#define BALDZARIKA_MATH_BOX2_H

#include <baldzarika/math/point2.h>
#include <baldzarika/math/size2.h>
#include <baldzarika/math/constant.h>

namespace baldzarika { namespace math {

	template < typename T >
	class box2
	{
	public:
		typedef point2<T> point2_t;
		typedef size2<T> size2_t;

		box2()
			: m_top_left(constant::zero<T>(), constant::zero<T>())
			, m_size(constant::zero<T>(), constant::zero<T>())
		{
		}

		box2(box2 const &that)
			: m_top_left(that.m_top_left)
			, m_size(that.m_size)
		{
		}

		box2(point2_t const &min_corner, point2_t const &max_corner)
			: m_top_left(min_corner)
			, m_size(max_corner-min_corner)
		{
		}

		box2(point2_t const &top_left, size2_t const &size)
			: m_top_left(top_left)
			, m_size(size)
		{
		}

		T width() const
		{
			return m_size.width();
		}

		T height() const
		{
			return m_size.height();
		}

		size2_t const& size() const
		{
			return m_size;
		}

		bool empty() const
		{
			return m_size.empty();
		}

		bool intersects(box2 const &rhs) const
		{
			T tlx=std::max(m_top_left.x(), rhs.m_top_left.x());
			T tly=std::max(m_top_left.y(), rhs.m_top_left.y());

			T brx=std::min(m_top_left.x()+m_size.width(), rhs.m_top_left.x()+rhs.m_size.width());
			T bry=std::min(m_top_left.y()+m_size.height(), rhs.m_top_left.y()+rhs.m_size.height());

			return tlx<=brx && tly<=bry;
		}

		bool contains(point2_t const &p) const
		{
			return p.x()>=m_top_left.x() && p.x()<m_top_left.x()+m_size.width() &&
				p.y()>=m_top_left.y() && p.y()<m_top_left.y()+m_size.height();
		}

		point2_t	m_top_left;
		size2_t		m_size;
	};

	typedef box2<float> box2f;

} //namespace math
} //namespace baldzarika

#endif //BALDZARIKA_MATH_BOX2_H