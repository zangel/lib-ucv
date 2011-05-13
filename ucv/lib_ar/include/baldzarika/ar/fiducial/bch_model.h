#ifndef BALDZARIKA_AR_FIDUCIAL_BCH_MODEL_H
#define BALDZARIKA_AR_FIDUCIAL_BCH_MODEL_H

#include <baldzarika/ar/fiducial/model.h>

namespace baldzarika { namespace ar { namespace fiducial {

	class bch_model
		: public model
	{
	public:
		static ucv::size2ui const MARKER_SIZE;
		
		bch_model();
		virtual ~bch_model();
		
		ucv::size2ui const&	get_marker_size() const;
		bool				detect(gray_const_view_t img, contour_t const &cont, ucv::matrix33f &hm, boost::int32_t &id) const;

	protected:
		
	private:
		gray_image_t			m_warped_img;
		ucv::gil::gray8_image_t	m_binary_img;
	};

} //namespace fiducial
} //namespace ar
} //namespace baldzarika

#endif //BALDZARIKA_AR_FIDUCIAL_BCH_MODEL_H