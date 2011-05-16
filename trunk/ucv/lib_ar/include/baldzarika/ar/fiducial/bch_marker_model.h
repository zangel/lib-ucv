#ifndef BALDZARIKA_AR_FIDUCIAL_BCH_MARKER_MODEL_H
#define BALDZARIKA_AR_FIDUCIAL_BCH_MARKER_MODEL_H

#include <baldzarika/ar/fiducial/marker_model.h>

namespace baldzarika { namespace ar { namespace fiducial {

	class bch_marker_model
		: public marker_model
	{
	public:
		static ucv::size2ui const MARKER_SIZE;
		
		bch_marker_model();
		virtual ~bch_marker_model();
		
		bool	detect_markers(gray_const_view_t img, std::list<contour_t> const &cnts, std::list<detect_info> &dis) const;
	
	protected:
		
	private:
		gray_image_t			m_warped_img;
		ucv::gil::gray8_image_t	m_binary_img;
	};

} //namespace fiducial
} //namespace ar
} //namespace baldzarika

#endif //BALDZARIKA_AR_FIDUCIAL_BCH_MODEL_H