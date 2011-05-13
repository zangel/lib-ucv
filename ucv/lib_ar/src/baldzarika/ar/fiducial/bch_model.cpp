#include <baldzarika/ar/config.h>
#include <baldzarika/ar/fiducial/bch_model.h>

namespace baldzarika { namespace ar { namespace fiducial {

	bch_model::bch_model()
	{
	}

	bch_model::~bch_model()
	{

	}

	bool bch_model::detect(gray_const_view_t img, contour_t const &cont, ucv::matrix33f &hm, boost::int32_t &id) const
	{
		if(cont.m_points.size()!=4 || !cont.m_is_closed || !cont.m_is_convex)
			return false;


		return false;
	}

} //namespace fiducial
} //namespace ar
} //namespace baldzarika
