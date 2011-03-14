#ifndef BALDZARIKA_UCV_FEATURE_POINT_H
#define BALDZARIKA_UCV_FEATURE_POINT_H

#include <baldzarika//ucv/point2.h>

namespace baldzarika { namespace ucv {

	class feature_point
		: public point2f
	{
	public:
		feature_point(point2f const &p, float s);
		feature_point(feature_point const &that);

		float		m_scale;
		float		m_orientation;
	};
} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_FEATURE_POINT_H