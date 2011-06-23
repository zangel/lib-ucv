#include <baldzarika/ar/config.h>
#include <baldzarika/ar/fiducial/qr_marker_model.h>

namespace baldzarika { namespace ar { namespace fiducial {
	
	qr_marker_model::helper_detect_info::helper_detect_info(std::list<contour_t>::const_iterator cn, math::point2f const &cr, math::matrix33f const &h, math::matrix33f const &ih)
		: m_contour(cn)
		, m_center(cr)
		, m_homography(h)
		, m_inv_homography(ih)
	{
	}

	qr_marker_model::paired_helper_detect_infos::paired_helper_detect_infos(std::list<helper_detect_info>::iterator f, std::list<helper_detect_info>::iterator s, boost::uint32_t frot, boost::uint32_t srot)
		: m_first(f)
		, m_second(s)
		, m_first_rot(frot)
		, m_second_rot(srot)
	{
	}

	boost::int32_t const qr_marker_model::CELL_SIZE=3;
	boost::int32_t const qr_marker_model::MARKER_CELLS=25;
	boost::int32_t const qr_marker_model::MARKER_SIZE=MARKER_CELLS*CELL_SIZE;
	boost::int32_t const qr_marker_model::HELPER1_CELLS=7;
	boost::int32_t const qr_marker_model::HELPER2_CELLS=3;
	boost::int32_t const qr_marker_model::HELPER_CELLS=HELPER1_CELLS*HELPER2_CELLS;
	boost::int32_t const qr_marker_model::HELPER1_CELL_SIZE=HELPER_CELLS/HELPER1_CELLS*CELL_SIZE;
	boost::int32_t const qr_marker_model::HELPER2_CELL_SIZE=HELPER_CELLS/HELPER2_CELLS*CELL_SIZE;
	boost::int32_t const qr_marker_model::HELPER_SIZE=HELPER_CELLS*CELL_SIZE;
		
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

		std::list< helper_detect_info > helpers1, helpers2;
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

	bool qr_marker_model::detect_helpers(gray_const_view_t img, std::list<std::list<contour_t>::const_iterator> const &helper_candidates, std::list< helper_detect_info > &helpers1, std::list< helper_detect_info > &helpers2) const
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

#if 1
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
			//inv_homography*=1.0f/inv_homography(2,2);
			
			math::point2f helper_center(
				(dst_pts[0].x()+dst_pts[1].x()+dst_pts[2].x()+dst_pts[3].x())*0.25f,
				(dst_pts[0].y()+dst_pts[1].y()+dst_pts[2].y()+dst_pts[3].y())*0.25f
			);

			if(helper_type==HELPER_1)
				helpers1.push_back( helper_detect_info(*helper_candidate, helper_center, homography, inv_homography) );
			else
			if(helper_type==HELPER_2)
				helpers2.push_back( helper_detect_info(*helper_candidate, helper_center, homography, inv_homography) );
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


	bool qr_marker_model::detect_markers(gray_const_view_t img, std::list< helper_detect_info > &helpers1, std::list< helper_detect_info > &helpers2, std::list<detect_info> &dis) const
	{

		std::list< paired_helper_detect_infos > v4_pairs;
		find_paired_helper_detect_infos(helpers1, 15, v4_pairs);


#if 0
		static math::point2f const helper21_1[4]=
		{
			math::point2f(4*HELPER2_CELL_SIZE+HELPER2_CELL_SIZE*0.5f, -14*HELPER2_CELL_SIZE+HELPER2_CELL_SIZE*0.5f),
			math::point2f(-14*HELPER2_CELL_SIZE+HELPER2_CELL_SIZE*0.5f, -2*HELPER2_CELL_SIZE+HELPER2_CELL_SIZE*0.5f),
			math::point2f(-2*HELPER2_CELL_SIZE+HELPER2_CELL_SIZE*0.5f, 16*HELPER2_CELL_SIZE+HELPER2_CELL_SIZE*0.5f),
			math::point2f(16*HELPER2_CELL_SIZE+HELPER2_CELL_SIZE*0.5f, 4*HELPER2_CELL_SIZE+HELPER2_CELL_SIZE*0.5f)
		};

		static math::point2f const helper21_2[4]=
		{
			math::point2f(-14*HELPER2_CELL_SIZE+HELPER2_CELL_SIZE*0.5f, 4*HELPER2_CELL_SIZE+HELPER2_CELL_SIZE*0.5f),
			math::point2f(4*HELPER2_CELL_SIZE+HELPER2_CELL_SIZE*0.5f, 16*HELPER2_CELL_SIZE+HELPER2_CELL_SIZE*0.5f),
			math::point2f(16*HELPER2_CELL_SIZE+HELPER2_CELL_SIZE*0.5f, -2*HELPER2_CELL_SIZE+HELPER2_CELL_SIZE*0.5f),
			math::point2f(-2*HELPER2_CELL_SIZE+HELPER2_CELL_SIZE*0.5f, -14*HELPER2_CELL_SIZE+HELPER2_CELL_SIZE*0.5f)
		};

		static math::point2f const helper21_3[4]=
		{
			math::point2f(-14*HELPER2_CELL_SIZE+HELPER2_CELL_SIZE*0.5f, -14*HELPER2_CELL_SIZE+HELPER2_CELL_SIZE*0.5f),
			math::point2f(-14*HELPER2_CELL_SIZE+HELPER2_CELL_SIZE*0.5f, 16*HELPER2_CELL_SIZE+HELPER2_CELL_SIZE*0.5f),
			math::point2f(16*HELPER2_CELL_SIZE+HELPER2_CELL_SIZE*0.5f, 16*HELPER2_CELL_SIZE+HELPER2_CELL_SIZE*0.5f),
			math::point2f(16*HELPER2_CELL_SIZE+HELPER2_CELL_SIZE*0.5f, -14*HELPER2_CELL_SIZE+HELPER2_CELL_SIZE*0.5f)
		};
		
		static math::point2f const marker_corners[4]=
		{
			math::point2f(18.0f*CELL_SIZE+CELL_SIZE*0.5f, 18.0f*CELL_SIZE+CELL_SIZE*0.5f),
			math::point2f(21.0f*CELL_SIZE+CELL_SIZE*0.5f, 3.0f*CELL_SIZE+CELL_SIZE*0.5f),
			math::point2f(3.0f*CELL_SIZE+CELL_SIZE*0.5f, 21.0f*CELL_SIZE+CELL_SIZE*0.5f),
			math::point2f(3.0f*CELL_SIZE+CELL_SIZE*0.5f, 3.0f*CELL_SIZE+CELL_SIZE*0.5f)
		};


		static std::vector< math::point2f > src_pts(marker_corners, marker_corners+4);
		
		std::list< helper_detect_info >::const_iterator ihelper2=helpers2.begin();
		
		while(ihelper2!=helpers2.end())
		{
			contour_t const &contour_helper2=*(ihelper2->m_contour);

			std::list< helper_detect_info >::iterator helpers21[3];

			bool helpers1_found=false;
			boost::uint32_t r=0;
			for(;r<4;++r)
			{
				math::point2f helper1_centers[3]=
				{
					ihelper2->m_homography*math::vector3f(helper21_1[r]),
					ihelper2->m_homography*math::vector3f(helper21_2[r]),
					ihelper2->m_homography*math::vector3f(helper21_3[r])
				};

				for(std::list< helper_detect_info >::iterator ihelper11=helpers1.begin();!helpers1_found && ihelper11!=helpers1.end();++ihelper11)
				{
					helpers21[0]=helpers1.end();
					math::point2f _center1=helper1_centers[0].transformed(ihelper11->m_inv_homography);

					if(helper_bbox2.contains(_center1))
					{
						helpers21[0]=ihelper11;
						for(std::list< helper_detect_info >::iterator ihelper12=helpers1.begin();!helpers1_found && ihelper12!=helpers1.end();++ihelper12)
						{
							helpers21[1]=helpers1.end();
							if(ihelper12==ihelper11) continue;
							math::point2f _center2=helper1_centers[1].transformed(ihelper12->m_inv_homography);

							if(helper_bbox2.contains(_center2))
							{
								helpers21[1]=ihelper12;
								for(std::list< helper_detect_info >::iterator ihelper13=helpers1.begin();!helpers1_found && ihelper13!=helpers1.end();++ihelper13)
								{
									helpers21[2]=helpers1.end();
									if(ihelper13==ihelper12 || ihelper13==ihelper11) continue;

									math::point2f _center3=helper1_centers[2].transformed(ihelper13->m_inv_homography);
									
									if(helper_bbox2.contains(_center3))
									{
										helpers21[2]=ihelper13;
										helpers1_found=true;
									}
								}
							}
						}
					}
				}
			}
			
			if(helpers1_found)
			{
				BOOST_ASSERT(helpers21[0]!=helpers1.end());
				BOOST_ASSERT(helpers21[1]!=helpers1.end());
				BOOST_ASSERT(helpers21[2]!=helpers1.end());

				std::vector< math::point2f > dst_pts(4);

				dst_pts[0]=ihelper2->m_center;
				dst_pts[1]=helpers21[0]->m_center;
				dst_pts[2]=helpers21[1]->m_center;
				dst_pts[3]=helpers21[2]->m_center;

				math::matrix33f homography;
				if(ucv::perspective_transform(src_pts,dst_pts,homography))
				{
					math::matrix<math::real_t, 3, 3> fp_homography=homography;
					if(ucv::warp(img, ucv::gil::view(m_marker_warped_img), fp_homography, true))
					{
						{
							ucv::gil::gray8_image_t save_img(m_marker_warped_img.width(),m_marker_warped_img.height());
							ucv::convert(
								ucv::gil::const_view(m_marker_warped_img),
								ucv::gil::view(save_img),
								ucv::detail::grayscale_convert()
							);
				
							ucv::gil::png_write_view("warped_image.png", ucv::gil::const_view(save_img));
						}
					}
					
				}
				
				helpers1.erase(helpers21[0]);
				helpers1.erase(helpers21[1]);
				helpers1.erase(helpers21[2]);

				ihelper2=helpers2.erase(ihelper2);
				continue;
			}
			ihelper2++;
		}
#endif
		return true;
	}

	void qr_marker_model::find_paired_helper_detect_infos(std::list< helper_detect_info > &helpers1, boost::uint32_t dist, std::list<paired_helper_detect_infos> &paired_infos) const
	{
		static math::box2f const helper_bbox2=math::box2f(
			math::point2f(0.0f, 0.0f),
			math::size2f(HELPER_SIZE,HELPER_SIZE)
		);

		math::point2f const neigbours[4]=
		{
			math::point2f((7+dist+3)*HELPER1_CELL_SIZE+HELPER1_CELL_SIZE*0.5f, 3*HELPER1_CELL_SIZE+HELPER1_CELL_SIZE*0.5f),
			math::point2f(3*HELPER1_CELL_SIZE+HELPER1_CELL_SIZE*0.5f, -(dist+4)*HELPER1_CELL_SIZE+HELPER1_CELL_SIZE*0.5f),
			math::point2f(-(dist+4)*HELPER1_CELL_SIZE+HELPER1_CELL_SIZE*0.5f, 3*HELPER1_CELL_SIZE+HELPER1_CELL_SIZE*0.5f),
			math::point2f(3*HELPER1_CELL_SIZE+HELPER1_CELL_SIZE*0.5f, (7+dist+3)*HELPER1_CELL_SIZE+HELPER1_CELL_SIZE*0.5f)
		};

		std::list< helper_detect_info >::iterator ihelper1_first=helpers1.begin();
		while(ihelper1_first!=helpers1.end())
		{
			math::point2f const rotated_centers[4]=
			{
				neigbours[0].transformed(ihelper1_first->m_homography),
				neigbours[1].transformed(ihelper1_first->m_homography),
				neigbours[2].transformed(ihelper1_first->m_homography),
				neigbours[3].transformed(ihelper1_first->m_homography)
			};


			bool paired=false;
			std::list< helper_detect_info >::iterator ihelper1_second=ihelper1_first;
			ihelper1_second++;
			while(ihelper1_second!=helpers1.end())
			{
				bool paired_fs(false);
				boost::uint32_t fr=0;
				while(!paired_fs && fr<4)
				{
					math::point2f p=rotated_centers[fr].transformed(ihelper1_second->m_inv_homography);
					if(paired_fs=helper_bbox2.contains(p)) break;

					fr++;
				}

				if(paired_fs)
				{
					bool paired_sf(false);
					boost::uint32_t sr=0;

					while(!paired_sf && sr<4)
					{
						math::point2f p=neigbours[sr].transformed(ihelper1_second->m_homography).
							transformed(ihelper1_first->m_inv_homography);

						if(paired_sf=helper_bbox2.contains(p)) break;
						sr++;
					}
					if(paired_sf)
						paired_infos.push_back(
							paired_helper_detect_infos(
								ihelper1_first, ihelper1_second,
								fr, sr
							)
						);
				}
				ihelper1_second++;
			}
			ihelper1_first++;
		}
	}

} //namespace fiducial 
} //namespace ar
} //namespace baldzarika