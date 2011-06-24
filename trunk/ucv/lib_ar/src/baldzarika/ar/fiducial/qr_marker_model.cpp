#include <baldzarika/ar/config.h>
#include <baldzarika/ar/fiducial/qr_marker_model.h>

namespace baldzarika { namespace ar { namespace fiducial {

	namespace {
		
		template < typename T, boost::int32_t D >
		struct helper1_neighbours
		{
			static math::point2<T> const HELPER1[4];
			static math::point2<T> const HELPER2_TL[4];
			static math::point2<T> const HELPER2_TR[4];
			static math::point2<T> const HELPER2_BL[4];
		};

		template < typename T, boost::int32_t D >
		math::point2<T> const helper1_neighbours< T, D >::HELPER1[4]=
		{
			math::point2<T>(
				T((7+D+3)*qr_marker_model::HELPER1_CELL_SIZE)+T(qr_marker_model::HELPER1_CELL_SIZE)*math::constant::half<T>(),
				T(3*qr_marker_model::HELPER1_CELL_SIZE)+T(qr_marker_model::HELPER1_CELL_SIZE)*math::constant::half<T>()
			),
			math::point2<T>(
				T(3*qr_marker_model::HELPER1_CELL_SIZE)+T(qr_marker_model::HELPER1_CELL_SIZE)*math::constant::half<T>(),
				T(-(D+4)*qr_marker_model::HELPER1_CELL_SIZE)+T(qr_marker_model::HELPER1_CELL_SIZE)*math::constant::half<T>()
			),
			math::point2<T>(
				T(-(D+4)*qr_marker_model::HELPER1_CELL_SIZE)+T(qr_marker_model::HELPER1_CELL_SIZE)*math::constant::half<T>(),
				T(3*qr_marker_model::HELPER1_CELL_SIZE)+T(qr_marker_model::HELPER1_CELL_SIZE)*math::constant::half<T>()
			),
			math::point2<T>(
				T(3*qr_marker_model::HELPER1_CELL_SIZE)+T(qr_marker_model::HELPER1_CELL_SIZE)*math::constant::half<T>(),
				T((7+D+3)*qr_marker_model::HELPER1_CELL_SIZE)+T(qr_marker_model::HELPER1_CELL_SIZE)*math::constant::half<T>()
			)
		};

		template < typename T, boost::int32_t D >
		math::point2<T> const helper1_neighbours< T, D >::HELPER2_TL[4]=
		{
			math::point2<T>(
				T((qr_marker_model::HELPER1_CELLS+D)*qr_marker_model::HELPER1_CELL_SIZE)+T(qr_marker_model::HELPER1_CELL_SIZE)*math::constant::half<T>(),
				T((qr_marker_model::HELPER1_CELLS+D)*qr_marker_model::HELPER1_CELL_SIZE)+T(qr_marker_model::HELPER1_CELL_SIZE)*math::constant::half<T>()
			),
			math::point2<T>(
				T((qr_marker_model::HELPER1_CELLS+D)*qr_marker_model::HELPER1_CELL_SIZE)+T(qr_marker_model::HELPER1_CELL_SIZE)*math::constant::half<T>(),
				T(-(D+1)*qr_marker_model::HELPER1_CELL_SIZE)+T(qr_marker_model::HELPER1_CELL_SIZE)*math::constant::half<T>()
			),
			math::point2<T>(
				T(-(D+1)*qr_marker_model::HELPER1_CELL_SIZE)+T(qr_marker_model::HELPER1_CELL_SIZE)*math::constant::half<T>(),
				T(-(D+1)*qr_marker_model::HELPER1_CELL_SIZE)+T(qr_marker_model::HELPER1_CELL_SIZE)*math::constant::half<T>()
				
			),
			math::point2<T>(
				T(-(D+1)*qr_marker_model::HELPER1_CELL_SIZE)+T(qr_marker_model::HELPER1_CELL_SIZE)*math::constant::half<T>(),
				T((qr_marker_model::HELPER1_CELLS+D)*qr_marker_model::HELPER1_CELL_SIZE)+T(qr_marker_model::HELPER1_CELL_SIZE)*math::constant::half<T>()
			)
		};

		template < typename T, boost::int32_t D >
		math::point2<T> const helper1_neighbours< T, D >::HELPER2_TR[4]=
		{
			math::point2<T>(
				T(qr_marker_model::HELPER1_CELL_SIZE)*math::constant::half<T>(),
				T((qr_marker_model::HELPER1_CELLS+D)*qr_marker_model::HELPER1_CELL_SIZE)+T(qr_marker_model::HELPER1_CELL_SIZE)*math::constant::half<T>()
			),
			math::point2<T>(
				T((qr_marker_model::HELPER1_CELLS+D)*qr_marker_model::HELPER1_CELL_SIZE)+T(qr_marker_model::HELPER1_CELL_SIZE)*math::constant::half<T>(),
				T((qr_marker_model::HELPER1_CELLS-1)*qr_marker_model::HELPER1_CELL_SIZE)+T(qr_marker_model::HELPER1_CELL_SIZE)*math::constant::half<T>()
			),
			math::point2<T>(
				T((qr_marker_model::HELPER1_CELLS-1)*qr_marker_model::HELPER1_CELL_SIZE)+T(qr_marker_model::HELPER1_CELL_SIZE)*math::constant::half<T>(),
				T(-(D+1)*qr_marker_model::HELPER1_CELL_SIZE)+T(qr_marker_model::HELPER1_CELL_SIZE)*math::constant::half<T>()
			),
			math::point2<T>(
				T(-(D+1)*qr_marker_model::HELPER1_CELL_SIZE)+T(qr_marker_model::HELPER1_CELL_SIZE)*math::constant::half<T>(),
				T(qr_marker_model::HELPER1_CELL_SIZE)*math::constant::half<T>()
			)
		};

		template < typename T, boost::int32_t D >
		math::point2<T> const helper1_neighbours< T, D >::HELPER2_BL[4]=
		{
			math::point2<T>(
				T((qr_marker_model::HELPER1_CELLS+D)*qr_marker_model::HELPER1_CELL_SIZE)+T(qr_marker_model::HELPER1_CELL_SIZE)*math::constant::half<T>(),
				T(qr_marker_model::HELPER1_CELL_SIZE)*math::constant::half<T>()
			),
			math::point2<T>(
				T(qr_marker_model::HELPER1_CELL_SIZE)*math::constant::half<T>(),
				T(-(D+1)*qr_marker_model::HELPER1_CELL_SIZE)+T(qr_marker_model::HELPER1_CELL_SIZE)*math::constant::half<T>()
			),
			math::point2<T>(
				T(-(D+1)*qr_marker_model::HELPER1_CELL_SIZE)+T(qr_marker_model::HELPER1_CELL_SIZE)*math::constant::half<T>(),
				T((qr_marker_model::HELPER1_CELLS-1)*qr_marker_model::HELPER1_CELL_SIZE)+T(qr_marker_model::HELPER1_CELL_SIZE)*math::constant::half<T>()
			),
			math::point2<T>(
				T((qr_marker_model::HELPER1_CELLS-1)*qr_marker_model::HELPER1_CELL_SIZE)+T(qr_marker_model::HELPER1_CELL_SIZE)*math::constant::half<T>(),
				T((qr_marker_model::HELPER1_CELLS+D)*qr_marker_model::HELPER1_CELL_SIZE)+T(qr_marker_model::HELPER1_CELL_SIZE)*math::constant::half<T>()
			)
		};

	} //namespace anonymous
	
	template < typename T >
	qr_marker_model::helper_detect_info<T>::helper_detect_info(std::list<contour_t>::const_iterator cn, math::point2<T> const &cr, math::matrix<T,3,3> const &h, math::matrix<T,3,3> const &ih)
		: m_contour(cn)
		, m_center(cr)
		, m_homography(h)
		, m_inv_homography(ih)
	{
	}

	template < typename T >
	qr_marker_model::helper_detect_info_triplet<T>::helper_detect_info_triplet(typename std::list< helper_detect_info<T> >::iterator tl, boost::int32_t tlr, typename std::list< helper_detect_info<T> >::iterator tr, boost::int32_t trr, typename std::list< helper_detect_info<T> >::iterator bl, boost::int32_t blr)
		: m_top_left(tl)
		, m_top_left_rot(tlr)
		, m_top_right(tr)
		, m_top_right_rot(trr)
		, m_bottom_left(bl)
		, m_bottom_left_rot(blr)
	{
	}

	boost::int32_t const qr_marker_model::CELL_SIZE=3;
	boost::int32_t const qr_marker_model::MAX_MARKER_CELLS=33;
	boost::int32_t const qr_marker_model::MAX_MARKER_SIZE=MAX_MARKER_CELLS*CELL_SIZE;
	boost::int32_t const qr_marker_model::HELPER1_CELLS=7;
	boost::int32_t const qr_marker_model::HELPER2_CELLS=3;
	boost::int32_t const qr_marker_model::HELPER_CELLS=HELPER1_CELLS*HELPER2_CELLS;
	boost::int32_t const qr_marker_model::HELPER1_CELL_SIZE=HELPER_CELLS/HELPER1_CELLS*CELL_SIZE;
	boost::int32_t const qr_marker_model::HELPER2_CELL_SIZE=HELPER_CELLS/HELPER2_CELLS*CELL_SIZE;
	boost::int32_t const qr_marker_model::HELPER_SIZE=HELPER_CELLS*CELL_SIZE;
		
	math::real_t const qr_marker_model::DEFAULT_HELPER_ECCENTRICITY=0.70710678118654752440084436210485;
	math::real_t const qr_marker_model::DEFAULT_HELPER_MIN_AREA=0.05;

	qr_marker_model::qr_marker_model()
		: m_helper_warped_img(HELPER_SIZE, HELPER_SIZE)
		, m_marker_warped_img(MAX_MARKER_SIZE, MAX_MARKER_SIZE)
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
		return math::size2ui(MAX_MARKER_SIZE,MAX_MARKER_SIZE);
	}

	bool qr_marker_model::detect_markers(gray_const_view_t img, std::list<contour_t> const &contours, std::list<detect_info> &dis) const
	{
		std::list<std::list<contour_t>::const_iterator> helper_candidates;
		find_helper_candidates(img, contours, helper_candidates);
		filter_helper_candidates(helper_candidates);

		std::list< helper_detect_info<math::real_t> > helpers1, helpers2;
		if(!detect_helpers(img, helper_candidates, helpers1, helpers2))
			return false;
		return detect_markers(img,helpers1,helpers2,dis);
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

	template < typename T >
	bool qr_marker_model::detect_helpers(gray_const_view_t img, std::list< std::list<contour_t>::const_iterator > const &helper_candidates, std::list< helper_detect_info<T> > &helpers1, std::list< helper_detect_info<T> > &helpers2) const
	{
		std::vector<math::point2f> src_pts(4), dst_pts(4);

		src_pts[0]=math::point2f( 0.0f,					0.0f );
		src_pts[1]=math::point2f( float(HELPER_SIZE),	0.0f);
		src_pts[2]=math::point2f( float(HELPER_SIZE),	float(HELPER_SIZE));
		src_pts[3]=math::point2f( 0.0f,					float(HELPER_SIZE));
		
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

#if 0
			{
				ucv::gil::gray8_image_t save_img(m_helper_warped_img.width(),m_helper_warped_img.height());
				ucv::convert(
					ucv::gil::const_view(m_helper_warped_img),
					ucv::gil::view(save_img),
					ucv::detail::grayscale_convert()
				);
				
				ucv::gil::png_write_view("warped_image.png", ucv::gil::const_view(save_img));
			}
#endif

			
			
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

			math::matrix33f inv_homography=homography.inverted();
			
			
			math::point2f helper_center(
				(dst_pts[0].x()+dst_pts[1].x()+dst_pts[2].x()+dst_pts[3].x())*0.25f,
				(dst_pts[0].y()+dst_pts[1].y()+dst_pts[2].y()+dst_pts[3].y())*0.25f
			);

			if(helper_type==HELPER_1)
				helpers1.push_back( helper_detect_info<T>(*helper_candidate, math::point2<T>(helper_center), math::matrix<T,3,3>(homography), math::matrix<T,3,3>(inv_homography)) );
			else
			if(helper_type==HELPER_2)
				helpers2.push_back( helper_detect_info<T>(*helper_candidate, math::point2<T>(helper_center), math::matrix<T,3,3>(homography), math::matrix<T,3,3>(inv_homography)) );
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
		
		if(border_zero_count>((24*HELPER1_CELL_SIZE*HELPER1_CELL_SIZE)*2)/3) //helper1
		{
			boost::uint32_t middle_zero_count=ucv::pixel_count(
				ucv::gil::subimage_view(wimg,
					HELPER1_CELL_SIZE, HELPER1_CELL_SIZE,
					(HELPER1_CELLS-2)*HELPER1_CELL_SIZE,HELPER1_CELL_SIZE
				),
				ucv::detail::is_zero()
			);

			middle_zero_count+=ucv::pixel_count(
				ucv::gil::subimage_view(wimg,
					HELPER1_CELL_SIZE, 2*HELPER1_CELL_SIZE,
					HELPER1_CELL_SIZE,(HELPER1_CELLS-4)*HELPER1_CELL_SIZE
				),
				ucv::detail::is_zero()
			);

			middle_zero_count+=ucv::pixel_count(
				ucv::gil::subimage_view(wimg,
					(HELPER1_CELLS-2)*HELPER1_CELL_SIZE, 2*HELPER1_CELL_SIZE,
					HELPER1_CELL_SIZE,(HELPER1_CELLS-4)*HELPER1_CELL_SIZE
				),
				ucv::detail::is_zero()
			);

			middle_zero_count+=ucv::pixel_count(
				ucv::gil::subimage_view(wimg,
					HELPER1_CELL_SIZE, (HELPER1_CELLS-2)*HELPER1_CELL_SIZE,
					(HELPER1_CELLS-2)*HELPER1_CELL_SIZE,HELPER1_CELL_SIZE
				),
				ucv::detail::is_zero()
			);

			if(middle_zero_count<((16*HELPER1_CELL_SIZE*HELPER1_CELL_SIZE)*1)/3)
			{
				boost::uint32_t center_zero_count=ucv::pixel_count(
					ucv::gil::subimage_view(wimg,
						2*HELPER1_CELL_SIZE, 2*HELPER1_CELL_SIZE,
						3*HELPER1_CELL_SIZE, 3*HELPER1_CELL_SIZE
					),
					ucv::detail::is_zero()
				);
				if(center_zero_count>((9*HELPER1_CELL_SIZE*HELPER1_CELL_SIZE)*2)/3)
					return HELPER_1;
			}
		}
		else
		if(border_zero_count<(24*HELPER1_CELL_SIZE*HELPER1_CELL_SIZE)/3)//helper2
		{
			border_zero_count+=ucv::pixel_count(
				ucv::gil::subimage_view(wimg,
					HELPER1_CELL_SIZE, HELPER1_CELL_SIZE,
					(HELPER1_CELLS-2)*HELPER1_CELL_SIZE,HELPER2_CELL_SIZE-HELPER1_CELL_SIZE
				),
				ucv::detail::is_zero()
			);

			border_zero_count+=ucv::pixel_count(
				ucv::gil::subimage_view(wimg,
					HELPER1_CELL_SIZE, HELPER2_CELL_SIZE,
					HELPER2_CELL_SIZE-HELPER1_CELL_SIZE,HELPER2_CELL_SIZE
				),
				ucv::detail::is_zero()
			);

			border_zero_count+=ucv::pixel_count(
				ucv::gil::subimage_view(wimg,
					2*HELPER2_CELL_SIZE, HELPER2_CELL_SIZE,
					HELPER2_CELL_SIZE-HELPER1_CELL_SIZE,HELPER2_CELL_SIZE
				),
				ucv::detail::is_zero()
			);

			border_zero_count+=ucv::pixel_count(
				ucv::gil::subimage_view(wimg,
					HELPER1_CELL_SIZE, (HELPER2_CELLS-1)*HELPER2_CELL_SIZE,
					(HELPER1_CELLS-2)*HELPER1_CELL_SIZE,HELPER2_CELL_SIZE-HELPER1_CELL_SIZE
				),
				ucv::detail::is_zero()
			);
			
			if(border_zero_count<(8*HELPER2_CELL_SIZE*HELPER2_CELL_SIZE)/3)
			{
				boost::uint32_t center_zero_count=ucv::pixel_count(
					ucv::gil::subimage_view(wimg,
						HELPER2_CELL_SIZE, HELPER2_CELL_SIZE,
						HELPER2_CELL_SIZE, HELPER2_CELL_SIZE
					),
					ucv::detail::is_zero()
				);
				if(center_zero_count>((HELPER2_CELL_SIZE*HELPER2_CELL_SIZE)*2)/3)
					return HELPER_2;
			}
		}
		return HELPER_INVALID;
	}

	template < typename T >
	bool qr_marker_model::detect_markers(gray_const_view_t img, std::list< helper_detect_info<T> > &helpers1, std::list< helper_detect_info<T> > &helpers2, std::list<detect_info> &dis) const
	{
		return (
			detect_v1_markers<T,7>(img, helpers1, dis) &&
			detect_v2_4_markers<T,9>(img, helpers1, helpers2, dis) &&
			detect_v2_4_markers<T,15>(img, helpers1, helpers2, dis) &&
			detect_v2_4_markers<T,19>(img, helpers1, helpers2, dis)
		);
	}

	template < typename T, boost::int32_t D >
	void qr_marker_model::find_helper_detect_info_triplets(std::list< helper_detect_info<T> > &helpers1, std::list< helper_detect_info_triplet<T> > &triplets) const
	{
		static math::box2<T> const helper_bbox2=math::box2<T>(
			math::point2<T>(math::constant::zero<T>(), math::constant::zero<T>()),
			math::size2<T>(T(HELPER_SIZE),T(HELPER_SIZE))
		);

		typename std::list< helper_detect_info<T> >::iterator i_top_left=helpers1.begin();
		while(i_top_left!=helpers1.end())
		{
			math::point2<T> const rotated_neighbours[4]=
			{
				helper1_neighbours<T,D>::HELPER1[0].transformed(i_top_left->m_homography),
				helper1_neighbours<T,D>::HELPER1[1].transformed(i_top_left->m_homography),
				helper1_neighbours<T,D>::HELPER1[2].transformed(i_top_left->m_homography),
				helper1_neighbours<T,D>::HELPER1[3].transformed(i_top_left->m_homography)
			};
			
			typename std::list< helper_detect_info<T> >::iterator i_top_right=helpers1.end();
			boost::uint32_t top_right_rot;

			typename std::list< helper_detect_info<T> >::iterator i_bottom_left=helpers1.end();
			boost::uint32_t bottom_left_rot;

			boost::uint32_t rot=0;
			while(rot<4 && (i_top_right==helpers1.end() || i_bottom_left==helpers1.end()))
			{
				math::point2<T> const &top_right_center=rotated_neighbours[rot];
				math::point2<T> const &bottom_left_center=rotated_neighbours[(rot+3)%4];
				
				typename std::list< helper_detect_info<T> >::iterator i_neighbours=helpers1.begin();
				while(i_neighbours!=helpers1.end() && (i_top_right==helpers1.end() || i_bottom_left==helpers1.end()))
				{
					if(i_neighbours==i_top_left)
					{
						i_neighbours++;
						continue;
					}

					if(i_top_right==helpers1.end() && helper_bbox2.contains(top_right_center.transformed(i_neighbours->m_inv_homography)))
					{
						top_right_rot=0;
						bool paired(false);
						while(top_right_rot<4)
						{
							if(paired=helper_bbox2.contains(helper1_neighbours<T,D>::HELPER1[top_right_rot].
								transformed(i_neighbours->m_homography).
								transformed(i_top_left->m_inv_homography))) break;
							top_right_rot++;
						}

						if(paired) i_top_right=i_neighbours;
					}

					if(i_bottom_left==helpers1.end() && helper_bbox2.contains(bottom_left_center.transformed(i_neighbours->m_inv_homography)))
					{
						bottom_left_rot=0;
						bool paired(false);
						while(bottom_left_rot<4)
						{
							if(paired=helper_bbox2.contains(helper1_neighbours<T,D>::HELPER1[bottom_left_rot].
								transformed(i_neighbours->m_homography).
								transformed(i_top_left->m_inv_homography))) break;
							bottom_left_rot++;
						}

						if(paired) i_bottom_left=i_neighbours;
					}
					i_neighbours++;
				}

				if(i_top_right!=helpers1.end() && i_bottom_left!=helpers1.end())
				{
					triplets.push_back(
						helper_detect_info_triplet<T>(
							i_top_left, rot,
							i_top_right, top_right_rot,
							i_bottom_left, bottom_left_rot
						)
					);
					break;
				}
				else
					i_top_right=i_bottom_left=helpers1.end();

				rot++;
			}
			i_top_left++;
		}
	}
	
	template < typename T, boost::int32_t D >
	bool qr_marker_model::detect_v1_markers(gray_const_view_t img, std::list< helper_detect_info<T> > &helpers1, std::list<detect_info> &dis) const
	{
		return true;
	}

	template < typename T, boost::int32_t D >
	bool qr_marker_model::detect_v2_4_markers(gray_const_view_t img, std::list< helper_detect_info<T> > &helpers1, std::list< helper_detect_info<T> > &helpers2, std::list<detect_info> &dis) const
	{
		static math::box2<T> const helper_bbox2=math::box2<T>(
			math::point2<T>(math::constant::zero<T>(), math::constant::zero<T>()),
			math::size2<T>(T(HELPER_SIZE),T(HELPER_SIZE))
		);

		static math::point2f const src_pts_data[4]=
		{
			math::point2f(
				float(HELPER1_CELLS*CELL_SIZE)*math::constant::half<float>(),
				float(HELPER1_CELLS*CELL_SIZE)*math::constant::half<float>()
			),
			math::point2f(
				float((HELPER1_CELLS+D)*CELL_SIZE)+float(HELPER1_CELLS*CELL_SIZE)*math::constant::half<float>(),
				float(HELPER1_CELLS*CELL_SIZE)*math::constant::half<float>()
			),
			math::point2f(
				float((D+HELPER1_CELLS)*CELL_SIZE)+float(CELL_SIZE)*math::constant::half<float>(),
				float((D+HELPER1_CELLS)*CELL_SIZE)+float(CELL_SIZE)*math::constant::half<float>()
			),
			math::point2f(
				float(HELPER1_CELLS*CELL_SIZE)*math::constant::half<float>(),
				float((HELPER1_CELLS+D)*CELL_SIZE)+float(HELPER1_CELLS*CELL_SIZE)*math::constant::half<float>()
			)
		};

		static std::vector<math::point2f> src_pts(src_pts_data, src_pts_data+4);

		std::vector<math::point2f> dst_pts(4);


		std::list< helper_detect_info_triplet<T> > triplets;
		find_helper_detect_info_triplets<T,D>(helpers1, triplets);
		for(typename std::list< helper_detect_info_triplet<T> >::const_iterator itriplet=triplets.begin();itriplet!=triplets.end();++itriplet)
		{
			typename std::list< helper_detect_info<T> >::iterator ihelper2=helpers2.begin();
			while(ihelper2!=helpers2.end())
			{
				bool tl_contains=helper_bbox2.contains(
					helper1_neighbours<T,D>::HELPER2_TL[itriplet->m_top_left_rot].
						transformed(itriplet->m_top_left->m_homography).
						transformed(ihelper2->m_inv_homography)
				);

				bool tr_contains=helper_bbox2.contains(
					helper1_neighbours<T,D>::HELPER2_TR[(itriplet->m_top_right_rot+2)%4].
						transformed(itriplet->m_top_right->m_homography).
						transformed(ihelper2->m_inv_homography)
				);

				bool bl_contains=helper_bbox2.contains(
					helper1_neighbours<T,D>::HELPER2_BL[(itriplet->m_bottom_left_rot+3)%4].
						transformed(itriplet->m_bottom_left->m_homography).
						transformed(ihelper2->m_inv_homography)
				);

				if(tl_contains && tr_contains && bl_contains)
				{
					dst_pts[0]=math::point2f(itriplet->m_top_left->m_center);
					dst_pts[1]=math::point2f(itriplet->m_top_right->m_center);
					dst_pts[2]=math::point2f(ihelper2->m_center);
					dst_pts[3]=math::point2f(itriplet->m_bottom_left->m_center);

					math::matrix33f homography;
					if(ucv::perspective_transform(src_pts,dst_pts,homography))
					{
						math::matrix<math::real_t, 3, 3> fp_homography=homography;
						if(ucv::warp(
							img,
							ucv::gil::subimage_view(
								ucv::gil::view(m_marker_warped_img),
								0, 0,
								(2*HELPER1_CELLS+D)*CELL_SIZE,(2*HELPER1_CELLS+D)*CELL_SIZE
							),
							fp_homography,
							true))
						{
				
#if 0
							{
								ucv::gil::gray8_image_t save_img((2*HELPER1_CELLS+D)*CELL_SIZE,(2*HELPER1_CELLS+D)*CELL_SIZE);
								ucv::convert(
									ucv::gil::subimage_view(
										ucv::gil::const_view(m_marker_warped_img),
										0, 0,
										(2*HELPER1_CELLS+D)*CELL_SIZE,(2*HELPER1_CELLS+D)*CELL_SIZE
									),
									ucv::gil::view(save_img),
									ucv::detail::grayscale_convert()
								);
				
								ucv::gil::png_write_view("warped_marker_image.png", ucv::gil::const_view(save_img));
							}
#endif


							if(ucv::threshold(
								ucv::gil::subimage_view(
									ucv::gil::const_view(m_marker_warped_img),
									0, 0,
									(2*HELPER1_CELLS+D)*CELL_SIZE,(2*HELPER1_CELLS+D)*CELL_SIZE
								),
								ucv::gil::subimage_view(
									ucv::gil::view(m_marker_warped_img),
									0, 0,
									(2*HELPER1_CELLS+D)*CELL_SIZE,(2*HELPER1_CELLS+D)*CELL_SIZE
								),
								ucv::detail::normal_binary_threshold<gray_t,gray_t>(
									ucv::find_otsu_threshold<gray_const_view_t,20>(
										ucv::gil::subimage_view(
											ucv::gil::const_view(m_marker_warped_img),
											0, 0,
											(2*HELPER1_CELLS+D)*CELL_SIZE,(2*HELPER1_CELLS+D)*CELL_SIZE
										),
										0,
										1
									)*gray_t(1.2),
									1.0
								)
							))
							{
								ucv::gil::gray8_image_t binary((2*HELPER1_CELLS+D),(2*HELPER1_CELLS+D));
								if(ucv::binarize(
									ucv::gil::subimage_view(
										ucv::gil::const_view(m_marker_warped_img),
										0, 0,
										(2*HELPER1_CELLS+D)*CELL_SIZE,(2*HELPER1_CELLS+D)*CELL_SIZE
									),
									ucv::gil::view(binary),
									CELL_SIZE,CELL_SIZE,
									ucv::detail::is_non_zero()
								))
								{

								}

							}
						}
					}

					helpers2.erase(ihelper2);
					helpers1.erase(itriplet->m_top_left);
					helpers1.erase(itriplet->m_top_right);
					helpers1.erase(itriplet->m_bottom_left);
					break;
				}
				ihelper2++;
			}
		}
		return true;
	}

} //namespace fiducial 
} //namespace ar
} //namespace baldzarika