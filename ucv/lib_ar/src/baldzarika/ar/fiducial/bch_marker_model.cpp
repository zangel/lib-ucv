#include <baldzarika/ar/config.h>
#include <baldzarika/ar/fiducial/bch_marker_model.h>

namespace baldzarika { namespace ar { namespace fiducial {

	ucv::size2ui const bch_marker_model::MARKER_SIZE=ucv::size2ui(50,50);
	ucv::decimal_t const bch_marker_model::DEFAULT_ECCENTRICITY=0.5;
	ucv::decimal_t const bch_marker_model::DEFAULT_MIN_SIDE_LENGTH=20.0;
		
	bch_marker_model::bch_marker_model()
		: m_warped_img(MARKER_SIZE.width(), MARKER_SIZE.height())
		, m_eccentricity(DEFAULT_ECCENTRICITY)
		, m_min_side_length(DEFAULT_MIN_SIDE_LENGTH)
	{
	}

	bch_marker_model::~bch_marker_model()
	{

	}

	bool bch_marker_model::detect_markers(gray_const_view_t img, std::list<contour_t> const &contours, std::list<detect_info> &dis) const
	{
		std::list<std::list<contour_t>::const_iterator> candidates;
		find_candidates(contours,candidates);
		filter_candidates(candidates);
		return detect_markers(img, candidates, dis);
	}

	bool bch_marker_model::is_candidate(contour_t const &contour) const
	{
		if(!(contour.m_is_closed && contour.m_points.size()==4 && contour.m_is_convex))
			return false;

		for(boost::uint32_t p=0;p<4;++p)
		{
			ucv::vector<ucv::decimal_t, 2> prev=contour.m_points[(p-1)%4]-contour.m_points[p];
			ucv::decimal_t prev_length=prev.length();
			if(prev_length<m_min_side_length)
				return false;
			
			prev/=prev_length;
			ucv::vector<ucv::decimal_t, 2> next=contour.m_points[(p+1)%4]-contour.m_points[p];
			if(std::abs(next.normalized().dot(prev))>m_eccentricity)
				return false;
		}
		return true;
	}

	void bch_marker_model::find_candidates(std::list<contour_t> const &contours, std::list<std::list<contour_t>::const_iterator> &candidates) const
	{
		for(std::list<contour_t>::const_iterator candidate=contours.begin();candidate!=contours.end();++candidate)
		{
			contour_t const &contour=*candidate;
			if(!is_candidate(contour)) continue;
			candidates.push_back(candidate);
		}
	}

	void bch_marker_model::filter_candidates(std::list<std::list<contour_t>::const_iterator> &candidates) const
	{
		std::list<std::list<contour_t>::const_iterator>::iterator first_cand=candidates.begin();

		while(first_cand!=candidates.end())
		{
			contour_t const &first_cont=*(*first_cand);
			std::list<std::list<contour_t>::const_iterator>::iterator second_cand=first_cand;
			second_cand++;

			while(second_cand!=candidates.end())
			{
				contour_t const &second_cont=*(*second_cand);

				if(first_cont.m_bbox.intersects(second_cont.m_bbox))
				{
					if(first_cont.m_bbox.size().area()>=second_cont.m_bbox.size().area())
					{
						second_cand=candidates.erase(second_cand);
						continue;
					}
					else
					{
						first_cand=candidates.erase(first_cand);
						break;
					}
				}
				second_cand++;
			}
			if(second_cand==candidates.end())
				first_cand++;
		}
	}

	marker_model::marker_id_t bch_marker_model::find_marker_id(ucv::matrix33f &homography) const
	{
		return NULL_MARKER_ID;
	}
	

	bool bch_marker_model::detect_markers(gray_const_view_t img, std::list<std::list<contour_t>::const_iterator> const &candidates, std::list<detect_info> &dis) const
	{
		std::vector<contour_t::point2_t> src_pts(4), dst_pts(4);

		src_pts[0]=contour_t::point2_t(	0,						0 );
		src_pts[1]=contour_t::point2_t(	MARKER_SIZE.width(),	0);
		src_pts[2]=contour_t::point2_t( MARKER_SIZE.width(),	MARKER_SIZE.height());
		src_pts[3]=contour_t::point2_t( 0,						MARKER_SIZE.height());
		
		for(std::list<std::list<contour_t>::const_iterator>::const_iterator candidate=candidates.begin();candidate!=candidates.end();++candidate)
		{
			contour_t const &contour=*(*candidate);

			for(boost::uint32_t p=0;p<4;++p)
				dst_pts[p]=contour.m_points[contour.m_is_clockwise?p:3-p];
			
			ucv::matrix33f homgraphy;
			if(!ucv::perspective_transform(src_pts,dst_pts,homgraphy)) continue;

			//use fixed_point based warp for performance reason
			ucv::matrix<ucv::decimal_t, 3, 3> fp_homgraphy=homgraphy;
			if(!ucv::warp(img, ucv::gil::view(m_warped_img), fp_homgraphy, true))
				continue;

			if(	!ucv::threshold(
					ucv::gil::const_view(m_warped_img),
					ucv::gil::view(m_warped_img),
					ucv::detail::normal_binary_threshold<gray_t,gray_t>(
						ucv::find_otsu_threshold<gray_const_view_t,20>(
							ucv::gil::const_view(m_warped_img),
							0,
							1
						),
						1.0
					)
				)
			) continue;

			marker_id_t marker_id=find_marker_id(homgraphy);
			if(marker_id==NULL_MARKER_ID) continue;
			dis.push_back(
				detect_info(
					marker_id,
					MARKER_SIZE,
					homgraphy
				)
			);
		}
		return true;
	}

} //namespace fiducial
} //namespace ar
} //namespace baldzarika
