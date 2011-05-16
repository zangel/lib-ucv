#include <baldzarika/ar/config.h>
#include <baldzarika/ar/fiducial/bch_marker_model.h>

namespace baldzarika { namespace ar { namespace fiducial {

	bch_marker_model::bch_marker_model()
	{
	}

	bch_marker_model::~bch_marker_model()
	{

	}

	bool bch_marker_model::detect_markers(gray_const_view_t img, std::list<contour_t> const &cnts, std::list<detect_info> &dis) const
	{
		for(std::list<contour_t>::const_iterator ic=cnts.begin();ic!=cnts.end();++ic)
		{
			contour_t const &contour=*ic;

			if(!contour.m_is_closed || contour.m_points.size()>4 || !contour.m_is_convex)
				continue;



		}
		return true;
	}

} //namespace fiducial
} //namespace ar
} //namespace baldzarika
