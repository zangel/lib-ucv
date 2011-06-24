#ifndef BALDZARIKA_AR_FIDUCIAL_QR_MARKER_MODEL_H
#define BALDZARIKA_AR_FIDUCIAL_QR_MARKER_MODEL_H

#include <baldzarika/ar/fiducial/marker_model.h>

namespace baldzarika { namespace ar { namespace fiducial {

	class qr_marker_model
		: public marker_model
	{
	public:

		template <typename T>
		struct helper_detect_info
		{
			helper_detect_info(std::list<contour_t>::const_iterator cn, math::point2<T> const &cr, math::matrix<T,3,3> const &h, math::matrix<T,3,3> const &ih);

			std::list<contour_t>::const_iterator	m_contour;
			math::point2<T>							m_center;
			math::matrix<T,3,3>						m_homography;
			math::matrix<T,3,3>						m_inv_homography;
		};

		template < typename T >
		struct helper_detect_info_triplet
		{
			helper_detect_info_triplet(typename std::list< helper_detect_info<T> >::iterator tl, boost::int32_t tlr, typename std::list< helper_detect_info<T> >::iterator tr, boost::int32_t trr, typename std::list< helper_detect_info<T> >::iterator bl, boost::int32_t blr);
			
			typename std::list< helper_detect_info <T> >::iterator	m_top_left;
			boost::int32_t											m_top_left_rot;

			typename std::list< helper_detect_info <T> >::iterator	m_top_right;
			boost::int32_t											m_top_right_rot;

			typename std::list< helper_detect_info<T> >::iterator	m_bottom_left;
			boost::int32_t											m_bottom_left_rot;
		};




		static boost::int32_t const CELL_SIZE;
		static boost::int32_t const MAX_MARKER_CELLS;
		static boost::int32_t const MAX_MARKER_SIZE;
		static boost::int32_t const HELPER1_CELLS;
		static boost::int32_t const HELPER2_CELLS;
		static boost::int32_t const HELPER_CELLS;
		static boost::int32_t const HELPER1_CELL_SIZE;
		static boost::int32_t const HELPER2_CELL_SIZE;
		static boost::int32_t const HELPER_SIZE;
		
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

		
		template < typename T >
		bool					detect_helpers(gray_const_view_t img, std::list< std::list<contour_t>::const_iterator > const &helper_candidates, std::list< helper_detect_info<T> > &helpers1, std::list< helper_detect_info<T> > &helpers2) const;
		eHelperType				detect_helper(gray_const_view_t wimg) const;
		template < typename T >
		bool					detect_markers(gray_const_view_t img, std::list< helper_detect_info<T> > &helpers1, std::list< helper_detect_info<T> > &helpers2, std::list<detect_info> &dis) const;
		template < typename T, boost::int32_t D >
		void					find_helper_detect_info_triplets(std::list< helper_detect_info<T> > &helpers1, std::list< helper_detect_info_triplet<T> > &triplets) const;
		template < typename T, boost::int32_t D >
		bool					detect_v1_markers(gray_const_view_t img, std::list< helper_detect_info<T> > &helpers1, std::list<detect_info> &dis) const;
		template < typename T, boost::int32_t D >
		bool					detect_v2_4_markers(gray_const_view_t img, std::list< helper_detect_info<T> > &helpers1, std::list< helper_detect_info<T> > &helpers2, std::list<detect_info> &dis) const;
	
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