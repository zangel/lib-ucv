#ifndef BALDZARIKA_AR_FIDUCIAL_BCH_MARKER_MODEL_H
#define BALDZARIKA_AR_FIDUCIAL_BCH_MARKER_MODEL_H

#include <baldzarika/ar/fiducial/marker_model.h>

namespace baldzarika { namespace ar { namespace fiducial {

	class bch_marker_model
		: public marker_model
	{
	public:
		typedef std::pair<marker_id_t, boost::uint32_t> marker_id_rot_t;

		static math::size2ui const MARKER_CELL_SIZE;
		static math::size2ui const MARKER_SIZE;
		static math::real_t const DEFAULT_ECCENTRICITY;
		static math::real_t const DEFAULT_MIN_SIDE_LENGTH;

		static boost::uint8_t const HAMMING_CODEWORDS[4][5];
		static boost::uint8_t const HAMMING_ERR_CORRECTION[4][5];

		
		static marker_id_rot_t	find_marker_id_and_rot(ucv::gil::gray8c_view_t bcv);

		bch_marker_model();
		virtual ~bch_marker_model();
		math::size2ui 			get_marker_size(marker_id_t mid) const;
		bool					detect_markers(gray_const_view_t img, std::list<contour_t> const &contours, std::list<detect_info> &dis) const;

		

	
	protected:
		bool					is_candidate(contour_t const &cont) const;
		void					find_candidates(std::list<contour_t> const &contours, std::list<std::list<contour_t>::const_iterator> &candidates) const;
		void					filter_candidates(std::list<std::list<contour_t>::const_iterator> &candidates) const;
		marker_id_t				find_marker_id(math::matrix33f &homography) const;
		bool					detect_markers(gray_const_view_t img, std::list<std::list<contour_t>::const_iterator> const &candidates, std::list<detect_info> &dis) const;

		
	private:
		mutable gray_image_t	m_warped_img;
		math::real_t			m_eccentricity;
		math::real_t			m_min_side_length;
	};

} //namespace fiducial
} //namespace ar
} //namespace baldzarika

#endif //BALDZARIKA_AR_FIDUCIAL_BCH_MODEL_H