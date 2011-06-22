#ifndef BALDZARIKA_AR_FIDUCIAL_MARKER_MODEL_H
#define BALDZARIKA_AR_FIDUCIAL_MARKER_MODEL_H

#include <baldzarika/ar/fiducial/typedefs.h>

namespace baldzarika { namespace ar { namespace fiducial {

	class marker_model
		: public boost::enable_shared_from_this<marker_model>
		, private boost::noncopyable
	{
	public:

		class detect_info
		{
		public:
			detect_info();
			detect_info(detect_info const &rhs);
			detect_info(marker_id_t mid, math::size2ui const &ms, math::matrix33f &hm);

			marker_id_t		m_marker_id;
			math::size2ui	m_marker_size;
			math::matrix33f	m_homography;
		};

		marker_model();
		virtual ~marker_model();

		virtual bool				begin(math::size2ui const &fs) const=0;
		virtual bool				end() const=0;

		virtual math::size2ui		get_marker_size(marker_id_t mid) const=0;
		virtual bool				detect_markers(gray_const_view_t img, std::list<contour_t> const &cnts, std::list<detect_info> &dis) const=0;
	
	public:
		mutable boost::any			m_any_data;
	};

} //namespace fiducial
} //namespace ar
} //namespace baldzarika

#endif //BALDZARIKA_AR_FIDUCIAL_MARKER_MODEL_H
