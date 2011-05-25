#include <baldzarika/ar/config.h>
#include <baldzarika/ar/fiducial/bch_marker_model.h>

namespace baldzarika { namespace ar { namespace fiducial {

	namespace {

		template < typename BCGV >
		static inline boost::uint32_t marker_hamming_distance(BCGV bcv)
		{
			BOOST_ASSERT(bcv.width()==bcv.height() && bcv.width()==5);

			boost::uint32_t dist(0);

			for(boost::uint32_t y=0;y<5;y++)
			{
				typename BCGV::x_iterator bc_row=bcv.x_at(0,y);
				boost::uint32_t min_sum=std::numeric_limits<boost::uint32_t>::max();
				for(boost::uint32_t p=0;p<4;p++)
				{
					boost::uint32_t sum=0;
					for(boost::uint32_t x=0;x<5;x++)
						sum+=bc_row[x]==bch_marker_model::HAMMING_CODEWORDS[p][x]?0:1;
					min_sum=std::min(min_sum,sum);
				}
				dist+=min_sum;
			}
			return dist;
		}

		template < typename BCGV >
		static inline marker_id_t marker_hamming_id(BCGV bcv)
		{
			marker_id_t mid(0);
			for(boost::uint32_t y=0;y<5;y++)
			{
				typename BCGV::x_iterator bc_row=bcv.x_at(0,y);
				
				mid<<=1;
				if(bc_row[1]) mid|=1;
				mid<<=1;
				if(bc_row[3]) mid|=1;
			}
			return mid;
		}

		template < boost::uint32_t R >
		inline ucv::matrix33f const& homography_rotation();

		template<>
		inline ucv::matrix33f const& homography_rotation<0>()
		{
			return ucv::matrix33f::identity();
		}

		template<>
		inline ucv::matrix33f const& homography_rotation<1>()
		{
			static float const matd[3][3]=
			{
				{  0.0f, -1.0f, float(bch_marker_model::MARKER_SIZE.height()) },
				{  1.0f,  0.0f, 0.0f },
				{  0.0f,  0.0f, 1.0f }
			};

			static ucv::matrix33f mat(matd);
			return mat;
		}

		template<>
		inline ucv::matrix33f const& homography_rotation<2>()
		{
			static float const matd[3][3]=
			{
				{ -1.0f,  0.0f, float(bch_marker_model::MARKER_SIZE.height()) },
				{  0.0f, -1.0f, float(bch_marker_model::MARKER_SIZE.width()) },
				{  0.0f,  0.0f, 1.0f }
			};

			static ucv::matrix33f mat(matd);
			return mat;
		}

		template<>
		inline ucv::matrix33f const& homography_rotation<3>()
		{
			static float const matd[3][3]=
			{
				{  0.0f,  1.0f, 0.0f },
				{ -1.0f,  0.0f, float(bch_marker_model::MARKER_SIZE.width()) },
				{  0.0f,  0.0f, 1.0f }
			};

			static ucv::matrix33f mat(matd);
			return mat;
		}
	}

	ucv::size2ui const bch_marker_model::MARKER_CELL_SIZE=ucv::size2ui(7,7);
	ucv::size2ui const bch_marker_model::MARKER_SIZE=ucv::size2ui(MARKER_CELL_SIZE.width()*7, MARKER_CELL_SIZE.height()*7);
		
	ucv::decimal_t const bch_marker_model::DEFAULT_ECCENTRICITY=0.5;
	ucv::decimal_t const bch_marker_model::DEFAULT_MIN_SIDE_LENGTH=20.0;

	boost::uint8_t const bch_marker_model::HAMMING_CODEWORDS[4][5]=
	{
		{1,0,0,0,0},
		{1,0,1,1,1},
		{0,1,0,0,1},
		{0,1,1,1,0}
	};

	boost::uint8_t const bch_marker_model::HAMMING_ERR_CORRECTION[4][5]=
	{
		{0,0,0,0,0},
		{0,0,1,1,1},
		{1,1,0,0,1},
		{1,1,1,1,0}
	};


	bch_marker_model::marker_id_rot_t bch_marker_model::find_marker_id_and_rot(ucv::gil::gray8c_view_t bcv)
	{
		boost::uint32_t rot=0;
		boost::uint32_t min_dist=marker_hamming_distance(bcv);

		boost::uint32_t dist=marker_hamming_distance(ucv::gil::rotated90ccw_view(bcv));
		if(dist<min_dist) { min_dist=dist; rot=1; }

		dist=marker_hamming_distance(ucv::gil::rotated180_view(bcv));
		if(dist<min_dist) { min_dist=dist; rot=2; }

		dist=marker_hamming_distance(ucv::gil::rotated90cw_view(bcv));
		if(dist<min_dist) { min_dist=dist; rot=3; }

		if(min_dist==0)
		{
			switch(rot)
			{
			case 0:
				return marker_id_rot_t(marker_hamming_id(bcv),0);
			case 1:
				return marker_id_rot_t(marker_hamming_id(ucv::gil::rotated90ccw_view(bcv)),1);
			case 2:
				return marker_id_rot_t(marker_hamming_id(ucv::gil::rotated180_view(bcv)),2);
			case 3:
				return marker_id_rot_t(marker_hamming_id(ucv::gil::rotated90cw_view(bcv)),3);
			}
		}
		
		return marker_id_rot_t(NULL_MARKER_ID,0);
	}
				
	bch_marker_model::bch_marker_model()
		: m_warped_img(MARKER_SIZE.width(), MARKER_SIZE.height())
		, m_eccentricity(DEFAULT_ECCENTRICITY)
		, m_min_side_length(DEFAULT_MIN_SIDE_LENGTH)
	{
	}

	bch_marker_model::~bch_marker_model()
	{

	}

	ucv::size2ui bch_marker_model::get_marker_size(marker_id_t mid) const
	{
		return MARKER_SIZE;
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

		ucv::decimal_t inv_scale=ucv::detail::constant::one<ucv::decimal_t>()/
			ucv::decimal_t(std::max(contour.m_bbox.width(),contour.m_bbox.height()));

		ucv::decimal_t min_side_length=m_min_side_length*inv_scale;

		for(boost::uint32_t p=0;p<4;++p)
		{
			ucv::vector<ucv::decimal_t, 2> prev=contour.m_points[(p-1)%4]-contour.m_points[p];
			prev*=inv_scale;

			ucv::decimal_t prev_length=prev.length();
			if(prev_length<min_side_length)
				return false;
			
			prev/=prev_length;
			ucv::vector<ucv::decimal_t, 2> next=contour.m_points[(p+1)%4]-contour.m_points[p];
			next*=inv_scale;

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

	marker_id_t bch_marker_model::find_marker_id(ucv::matrix33f &homography) const
	{
		gray_const_view_t warped_view=ucv::gil::const_view(m_warped_img);
		
		for(boost::uint32_t cy=0;cy<7;++cy)
		{
			boost::uint32_t cx_inc=(cy==0 || cy==6)?1:6;
			for(boost::uint32_t cx=0;cx<7;cx+=cx_inc)
			{
				boost::uint32_t zero_count=ucv::pixel_count(
					ucv::gil::subimage_view(warped_view,
						cx*MARKER_CELL_SIZE.width(), cy*MARKER_CELL_SIZE.height(),
						MARKER_CELL_SIZE.width(),MARKER_CELL_SIZE.height()
					),
					ucv::detail::is_zero()
				);

				if(zero_count<MARKER_CELL_SIZE.area()/2)
					return NULL_MARKER_ID;
			}
		}

		ucv::gil::gray8_image_t binary_code(5,5);
		ucv::gil::gray8_view_t binary_code_view=ucv::gil::view(binary_code);

		for(boost::uint32_t y=0;y<5;++y)
		{
			ucv::gil::gray8_view_t::x_iterator bc_row=binary_code_view.x_at(0,y);
			for(boost::uint32_t x=0;x<5;++x)
			{

				boost::uint32_t non_zero_count=ucv::pixel_count(
					ucv::gil::subimage_view(warped_view,
						(x+1)*MARKER_CELL_SIZE.width(), (y+1)*MARKER_CELL_SIZE.height(),
						MARKER_CELL_SIZE.width(),MARKER_CELL_SIZE.height()
					),
					ucv::detail::is_non_zero()
				);
				bc_row[x]=(non_zero_count<MARKER_CELL_SIZE.area()/2)?0:1;
			}
		}
		marker_id_rot_t mid_and_rot=find_marker_id_and_rot(ucv::gil::const_view(binary_code));
		
		if(mid_and_rot.first==NULL_MARKER_ID)
			return NULL_MARKER_ID;

#if 1
		switch(mid_and_rot.second)
		{
		case 0:
			homography*=homography_rotation<0>(); break;
		case 1:
			homography*=homography_rotation<1>(); break;
		case 2:
			homography*=homography_rotation<2>(); break;
		case 3:
			homography*=homography_rotation<3>(); break;
		}
#endif
		homography.scale(1.0f/homography(2,2));
		return mid_and_rot.first;
	}
	
	bool bch_marker_model::detect_markers(gray_const_view_t img, std::list<std::list<contour_t>::const_iterator> const &candidates, std::list<detect_info> &dis) const
	{
		std::vector<ucv::point2f> src_pts(4), dst_pts(4);

		src_pts[0]=ucv::point2f( 0.0f,							0.0f );
		src_pts[1]=ucv::point2f( float(MARKER_SIZE.width()),	0.0f);
		src_pts[2]=ucv::point2f( float(MARKER_SIZE.width()),	float(MARKER_SIZE.height()));
		src_pts[3]=ucv::point2f( 0.0f,							float(MARKER_SIZE.height()));
		
		for(std::list<std::list<contour_t>::const_iterator>::const_iterator candidate=candidates.begin();candidate!=candidates.end();++candidate)
		{
			contour_t const &contour=*(*candidate);

			for(boost::uint32_t p=0;p<4;++p)
			{
				contour_t::point2_t const &pt=contour.m_points[contour.m_is_clockwise?p:3-p];
				dst_pts[p]=ucv::point2f(pt.x, pt.y);
			}
			
			ucv::matrix33f homography;
			if(!ucv::perspective_transform(src_pts,dst_pts,homography))
				continue;

			//use fixed_point based warp for performance reason
			ucv::matrix<ucv::decimal_t, 3, 3> fp_homography=homography;
			if(!ucv::warp(img, ucv::gil::view(m_warped_img), fp_homography, true))
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



			marker_id_t marker_id=find_marker_id(homography);
			if(marker_id==NULL_MARKER_ID) continue;
			
			dis.push_back(
				detect_info(
					marker_id,
					MARKER_SIZE,
					homography
				)
			);
		}
		return true;
	}

} //namespace fiducial
} //namespace ar
} //namespace baldzarika
