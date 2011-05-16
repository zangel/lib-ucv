#include <baldzarika/ar/config.h>
#include <baldzarika/ar/fiducial/marker_model.h>

namespace baldzarika { namespace ar { namespace fiducial {

	marker_model::marker_id_t const marker_model::NULL_MARKER_ID=marker_model::marker_id_t(-1);

	marker_model::detect_info::detect_info()
		: m_marker_id(NULL_MARKER_ID)
	{

	}

	marker_model::detect_info::detect_info(detect_info const &rhs)
		: m_marker_id(rhs.m_marker_id)
		, m_marker_size(rhs.m_marker_size)
		, m_homography(rhs.m_homography)
	{

	}

	marker_model::detect_info::detect_info(marker_id_t mid, ucv::size2ui const &ms, ucv::matrix33f &hm)
		: m_marker_id(mid)
		, m_marker_size(ms)
		, m_homography(hm)
	{
	}

	marker_model::marker_model()
	{
	}

	marker_model::~marker_model()
	{
	}

	

} //namespace fiducial
} //namespace ar
} //namespace baldzarika