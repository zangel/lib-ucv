#ifndef BALDZARIKA_AR_FIDUCIAL_QR_MARKER_MODEL_H
#define BALDZARIKA_AR_FIDUCIAL_QR_MARKER_MODEL_H

#include <baldzarika/ar/fiducial/marker_model.h>

namespace baldzarika { namespace ar { namespace fiducial {

	class qr_marker_model
		: public marker_model
	{
	public:
		static boost::uint32_t const CELL_SIZE;
		static boost::uint32_t const MARKER_CELLS;
		static boost::uint32_t const MARKER_SIZE;
		static boost::uint32_t const HELPER1_CELLS;
		static boost::uint32_t const HELPER2_CELLS;
		static boost::uint32_t const HELPER_CELLS;
		static boost::uint32_t const HELPER1_CELL_SIZE;
		static boost::uint32_t const HELPER2_CELL_SIZE;
		static boost::uint32_t const HELPER_SIZE;
		
		static math::real_t const DEFAULT_HELPER_ECCENTRICITY;
		static math::real_t const DEFAULT_HELPER_MIN_AREA;

		typedef enum
		{
			HELPER_INVALID=0,
			HELPER_1,
			HELPER_2
		} eHelperType;

				
		qr_marker_model();
		virtual ~qr_marker_model();

		bool					begin(math::size2ui const &fs) const;
		bool					end() const;


		math::size2ui 			get_marker_size(marker_id_t mid) const;
		bool					detect_markers(gray_const_view_t img, std::list<contour_t> const &contours, std::list<detect_info> &dis) const;
		
	
	protected:
		bool					is_helper_candidate(gray_const_view_t img, contour_t const &cont) const;
		void					find_helper_candidates(gray_const_view_t img, std::list<contour_t> const &contours, std::list<std::list<contour_t>::const_iterator> &helper_candidates) const;
		void					filter_helper_candidates(std::list<std::list<contour_t>::const_iterator> &helper_candidates) const;

		bool					detect_helpers(gray_const_view_t img, std::list<std::list<contour_t>::const_iterator> const &helper_candidates, std::list< math::point2f > &helpers1, std::list< math::point2f > &helpers2) const;
		eHelperType				detect_helper(gray_const_view_t wimg) const;

	private:
		mutable gray_image_t	m_helper_warped_img;
		mutable gray_image_t	m_marker_warped_img;
		math::real_t			m_helper_eccentricity;
		math::real_t			m_helper_min_area;
	};

} //namespace fiducial
} //namespace ar
} //namespace baldzarika

#endif //BALDZARIKA_AR_FIDUCIAL_QR_MARKER_MODEL_H