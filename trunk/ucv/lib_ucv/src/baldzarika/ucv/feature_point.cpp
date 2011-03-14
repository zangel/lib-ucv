#include <baldzarika/ucv/config.h>
#include <baldzarika/ucv/feature_point.h>

namespace baldzarika { namespace ucv {

	feature_point::feature_point(point2f const &p, float s)
		: point2f(p)
		, m_scale(s)
		, m_orientation(0.0f)
	{
	}

	feature_point::feature_point(feature_point const &that)
		: point2f(that)
		, m_scale(that.m_scale)
		, m_orientation(that.m_orientation)
	{
	}


} //namespace ucv
} //namespace baldzarika