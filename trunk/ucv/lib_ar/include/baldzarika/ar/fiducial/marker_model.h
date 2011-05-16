#ifndef BALDZARIKA_AR_FIDUCIAL_MARKER_MODEL_H
#define BALDZARIKA_AR_FIDUCIAL_MARKER_MODEL_H

#include <baldzarika/ar/fiducial/typedefs.h>

namespace baldzarika { namespace ar { namespace fiducial {

	class descriptor;

	class marker_model
		: public boost::enable_shared_from_this<marker_model>
	{
	public:

		typedef boost::uint32_t marker_id_t;

		static marker_id_t const NULL_MARKER_ID;

		class detect_info
		{
		public:
			detect_info();
			detect_info(detect_info const &rhs);
			detect_info(marker_id_t mid, ucv::size2ui const &ms, ucv::matrix33f &hm);

			marker_id_t		m_marker_id;
			ucv::size2ui	m_marker_size;
			ucv::matrix33f	m_homography;
		};

		marker_model();
		virtual ~marker_model();

		virtual bool	detect_markers(gray_const_view_t img, std::list<contour_t> const &cnts, std::list<detect_info> &dis) const=0;
	};

} //namespace fiducial
} //namespace ar
} //namespace baldzarika

#endif //BALDZARIKA_AR_FIDUCIAL_MARKER_MODEL_H
