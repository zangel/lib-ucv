#include <baldzarika/ar/config.h>
#include <baldzarika/ar/fiducial/qr_marker_model.h>

namespace baldzarika { namespace ar { namespace fiducial {

	boost::uint32_t const qr_marker_model::CELL_SIZE=3;
	boost::uint32_t const qr_marker_model::MARKER_CELLS=25;
	boost::uint32_t const qr_marker_model::MARKER_SIZE=MARKER_CELLS*CELL_SIZE;
	boost::uint32_t const qr_marker_model::HELPER1_CELLS=7;
	boost::uint32_t const qr_marker_model::HELPER2_CELLS=3;
	boost::uint32_t const qr_marker_model::HELPER_CELLS=HELPER1_CELLS*HELPER2_CELLS;
	boost::uint32_t const qr_marker_model::HELPER1_CELL_SIZE=HELPER_CELLS/HELPER1_CELLS*CELL_SIZE;
	boost::uint32_t const qr_marker_model::HELPER2_CELL_SIZE=HELPER_CELLS/HELPER2_CELLS*CELL_SIZE;
	boost::uint32_t const qr_marker_model::HELPER_SIZE=HELPER_CELLS*CELL_SIZE;
		
	math::real_t const qr_marker_model::DEFAULT_HELPER_ECCENTRICITY=0.70710678118654752440084436210485;;
	math::real_t const qr_marker_model::DEFAULT_HELPER_MIN_AREA=0.05;

	qr_marker_model::qr_marker_model()
		: m_helper_warped_img(HELPER_SIZE, HELPER_SIZE)
		, m_marker_warped_img(MARKER_SIZE, MARKER_SIZE)
		, m_helper_eccentricity(DEFAULT_HELPER_ECCENTRICITY)
		, m_helper_min_area(DEFAULT_HELPER_MIN_AREA)
	{
	}

	qr_marker_model::~qr_marker_model()
	{
	}

	bool qr_marker_model::begin(math::size2ui const &fs) const
	{
		return true;
	}

	bool qr_marker_model::end() const
	{
		return true;
	}

	math::size2ui qr_marker_model::get_marker_size(marker_id_t mid) const
	{
		return math::size2ui(MARKER_SIZE,MARKER_SIZE);
	}

	bool qr_marker_model::detect_markers(gray_const_view_t img, std::list<contour_t> const &contours, std::list<detect_info> &dis) const
	{
		std::list<std::list<contour_t>::const_iterator> helper_candidates;
		find_helper_candidates(img, contours, helper_candidates);
		filter_helper_candidates(helper_candidates);

		std::list< math::point2f > helpers1, helpers2;
		if(!detect_helpers(img, helper_candidates, helpers1, helpers2))
			return false;
		return true;
	}

	bool qr_marker_model::is_helper_candidate(gray_const_view_t img, contour_t const &contour) const
	{
		if(!(contour.m_is_closed && contour.m_points.size()==4 && contour.m_is_convex))
			return false;

		math::real_t bbox_frame_ratio=std::sqrt(
			(math::real_t(contour.m_bbox.width())/math::real_t(img.width()))*
			(math::real_t(contour.m_bbox.height())/math::real_t(img.height()))
		);

		float fr=bbox_frame_ratio;
		float min_fr=m_helper_min_area;


		if(bbox_frame_ratio<m_helper_min_area)
			return false;

		math::real_t inv_contour_scale=math::constant::one<math::real_t>()/
			math::real_t(std::sqrt(contour.m_bbox.width()*contour.m_bbox.height()));
		
		float finv_scale=inv_contour_scale;

		for(boost::uint32_t p=0;p<4;++p)
		{
			math::vector<math::real_t, 2> prev=((contour.m_points[(p-1)%4]-contour.m_points[p])*inv_contour_scale).normalized();
			math::vector<math::real_t, 2> next=((contour.m_points[(p+1)%4]-contour.m_points[p])*inv_contour_scale).normalized();
			
			if(std::abs(next.normalized().dot(prev))>m_helper_eccentricity)
				return false;
		}
		return true;
	}


	void qr_marker_model::find_helper_candidates(gray_const_view_t img, std::list<contour_t> const &contours, std::list<std::list<contour_t>::const_iterator> &helper_candidates) const
	{
		for(std::list<contour_t>::const_iterator helper_candidate=contours.begin();helper_candidate!=contours.end();++helper_candidate)
		{
			contour_t const &contour=*helper_candidate;
			if(!is_helper_candidate(img, contour)) continue;
			helper_candidates.push_back(helper_candidate);
		}
	}

	void qr_marker_model::filter_helper_candidates(std::list<std::list<contour_t>::const_iterator> &helper_candidates) const
	{
		std::list<std::list<contour_t>::const_iterator>::iterator first_hcand=helper_candidates.begin();

		while(first_hcand!=helper_candidates.end())
		{
			contour_t const &first_hcont=*(*first_hcand);
			std::list<std::list<contour_t>::const_iterator>::iterator second_hcand=first_hcand;
			second_hcand++;

			while(second_hcand!=helper_candidates.end())
			{
				contour_t const &second_hcont=*(*second_hcand);

				if(first_hcont.m_bbox.intersects(second_hcont.m_bbox))
				{
					if(first_hcont.m_bbox.size().area()>=second_hcont.m_bbox.size().area())
					{
						second_hcand=helper_candidates.erase(second_hcand);
						continue;
					}
					else
					{
						first_hcand=helper_candidates.erase(first_hcand);
						break;
					}
				}
				second_hcand++;
			}
			if(second_hcand==helper_candidates.end())
				first_hcand++;
		}
	}

	bool qr_marker_model::detect_helpers(gray_const_view_t img, std::list<std::list<contour_t>::const_iterator> const &helper_candidates, std::list< math::point2f > &helpers1, std::list< math::point2f > &helpers2) const
	{
		std::vector<math::point2f> src_pts(4), dst_pts(4);

		src_pts[0]=math::point2f( 0.0f,					0.0f );
		src_pts[1]=math::point2f( float(MARKER_SIZE),	0.0f);
		src_pts[2]=math::point2f( float(MARKER_SIZE),	float(MARKER_SIZE));
		src_pts[3]=math::point2f( 0.0f,					float(MARKER_SIZE));
		
		for(std::list<std::list<contour_t>::const_iterator>::const_iterator helper_candidate=helper_candidates.begin();helper_candidate!=helper_candidates.end();++helper_candidate)
		{
			contour_t const &helper_contour=*(*helper_candidate);

			for(boost::uint32_t p=0;p<4;++p)
			{
				contour_t::point2_t const &pt=helper_contour.m_points[helper_contour.m_is_clockwise?p:3-p];
				dst_pts[p]=math::point2f(pt.x(), pt.y());
			}

			ucv::corners_subpixel(img,dst_pts);
			
			math::matrix33f homography;
			if(!ucv::perspective_transform(src_pts,dst_pts,homography))
				continue;

			//use fixed_point based warp for performance reason
			math::matrix<math::real_t, 3, 3> fp_homography=homography;
			if(!ucv::warp(img, ucv::gil::view(m_helper_warped_img), fp_homography, true))
				continue;
			
			if(	!ucv::threshold(
					ucv::gil::const_view(m_helper_warped_img),
					ucv::gil::view(m_helper_warped_img),
					ucv::detail::normal_binary_threshold<gray_t,gray_t>(
						ucv::find_otsu_threshold<gray_const_view_t,20>(
							ucv::gil::const_view(m_helper_warped_img),
							0,
							1
						),
						1.0
					)
				)
			) continue;

			eHelperType helper_type=detect_helper(ucv::gil::const_view(m_helper_warped_img));
			if(helper_type==HELPER_INVALID) continue;

			math::point2f helper_center(
				(dst_pts[0].x()+dst_pts[1].x()+dst_pts[2].x()+dst_pts[3].x())*0.25f,
				(dst_pts[0].y()+dst_pts[1].y()+dst_pts[2].y()+dst_pts[3].y())*0.25f
			);

			if(helper_type==HELPER_1)
				helpers1.push_back(helper_center);
			else
			if(helper_type==HELPER_2)
				helpers2.push_back(helper_center);
		}
		return true;
	}

	qr_marker_model::eHelperType qr_marker_model::detect_helper(gray_const_view_t wimg) const
	{
		boost::uint32_t border_zero_count=0;
		border_zero_count+=ucv::pixel_count(
			ucv::gil::subimage_view(wimg,
				0, 0,
				HELPER1_CELLS*HELPER1_CELL_SIZE,HELPER1_CELL_SIZE
			),
			ucv::detail::is_zero()
		);

		border_zero_count+=ucv::pixel_count(
			ucv::gil::subimage_view(wimg,
				0, HELPER1_CELL_SIZE,
				HELPER1_CELL_SIZE,(HELPER1_CELLS-2)*HELPER1_CELL_SIZE
			),
			ucv::detail::is_zero()
		);

		border_zero_count+=ucv::pixel_count(
			ucv::gil::subimage_view(wimg,
				(HELPER1_CELLS-1)*HELPER1_CELL_SIZE, HELPER1_CELL_SIZE,
				HELPER1_CELL_SIZE,(HELPER1_CELLS-2)*HELPER1_CELL_SIZE
			),
			ucv::detail::is_zero()
		);

		border_zero_count+=ucv::pixel_count(
			ucv::gil::subimage_view(wimg,
				0, (HELPER1_CELLS-1)*HELPER1_CELL_SIZE,
				HELPER1_CELLS*HELPER1_CELL_SIZE,HELPER1_CELL_SIZE
			),
			ucv::detail::is_zero()
		);
		
		if(border_zero_count>((24*HELPER1_CELL_SIZE*HELPER1_CELL_SIZE)*1)/2) //helper1
		{
			boost::uint32_t center_zero_count=ucv::pixel_count(
				ucv::gil::subimage_view(wimg,
					2*HELPER1_CELL_SIZE, 2*HELPER1_CELL_SIZE,
					3*HELPER1_CELL_SIZE, 3*HELPER1_CELL_SIZE
				),
				ucv::detail::is_zero()
			);
			if(center_zero_count>((9*HELPER1_CELL_SIZE*HELPER1_CELL_SIZE)*1)/2)
				return HELPER_1;
		}
		else
		if(border_zero_count<(24*HELPER1_CELL_SIZE*HELPER1_CELL_SIZE)/3)//helper2
		{
		}
		return HELPER_INVALID;
	}



} //namespace fiducial 
} //namespace ar
} //namespace baldzarika