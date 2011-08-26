#ifndef BALDZARIKA_UCV_HOMOGRAPHY_H
#define BALDZARIKA_UCV_HOMOGRAPHY_H

//#include <baldzarika/ucv/surf/feature_point.h>

namespace baldzarika { namespace ucv {

	class homography
	{
	public:
		typedef boost::uniform_int<boost::uint32_t> uint32_dist_t;
		typedef boost::variate_generator<boost::mt19937&, uint32_dist_t> uint32_rng_t;
		
		homography(std::vector<math::point2f> const &obj_pts, std::vector<math::point2f> const &img_pts);
		~homography();

	protected:
		static bool		normalize_points(std::vector<math::point2f> const &pts, math::point2f &tra, math::point2f &sca, math::matrix33f &m33, bool inv=false);
		bool			get_sub_set(std::vector<math::point2f> &obj_pts, std::vector<math::point2f> &img_pts, std::size_t ma);
		bool			check_sub_set(std::vector<math::point2f> const &pts, std::size_t n);
		std::size_t		inliers_count(math::matrix33f &ms, float t);
		void			compute_reproj_error(math::matrix33f &ms, std::vector<float> &err);


	protected:
		std::vector<math::point2f> const &m_obj_pts;
		std::vector<math::point2f> const &m_img_pts;

		boost::mt19937				m_rng;
		uint32_rng_t				m_rng_gen;
	};

	class ransac_homography
		: public homography
	{
	public:
		ransac_homography(std::vector<math::point2f> const &obj_pts, std::vector<math::point2f> const &img_pts);
		~ransac_homography();

		bool			operator()(math::matrix33f &h, float t, float c=0.99f, std::size_t mi=2000);

	protected:
		std::size_t		update_num_iters(float p, float ep, std::size_t mi, std::size_t nmp=4);
	};

#if 0
	template < typename T, typename DT, boost::uint32_t BS, typename MT >
	bool find_homography_ransac(
		std::vector< feature_point<T,DT,BS> > const &ops,
		std::vector< feature_point<T,DT,BS> > const &ips,
		std::vector< std::pair<std::size_t, std::size_t> > const &oim,
		math::matrix<MT, 3, 3> &hm,
		float rt=3.0f
	)
	{
		BOOST_ASSERT(hm.size1()==3 && hm.size2()==3);
		
		std::vector<math::point2f> obj_pts(oim.size()), img_pts(oim.size());
		for(std::size_t p=0;p<oim.size();++p)
		{
			obj_pts[p]=math::point2f(
				static_cast<float>(ops[oim[p].first].x()),
				static_cast<float>(ops[oim[p].first].y())
			);

			img_pts[p]=math::point2f(
				static_cast<float>(ips[oim[p].second].x()),
				static_cast<float>(ips[oim[p].second].y())
			);
		}
		math::matrix33f hmf;
		if(ransac_homography(obj_pts,img_pts)(hmf,rt))
		{
			hm=hmf;
			return true;
		}
		return false;
	}

#endif

	template < typename I1, typename I2 , typename MT>
	bool find_homography_ransac(
		std::vector< std::pair<I1, I2> > const &oim,
		math::matrix<MT, 3, 3> &hm,
		bool swap_dir=false,
		float rt=3.0f
		)
	{
		std::vector<math::point2f> obj_pts(oim.size()), img_pts(oim.size());
		for(std::size_t p=0;p<oim.size();++p)
		{
			obj_pts[p]=math::point2f(
				static_cast<float>(swap_dir?oim[p].second->x():oim[p].first->x()),
				static_cast<float>(swap_dir?oim[p].second->y():oim[p].first->y())
			);

			img_pts[p]=math::point2f(
				static_cast<float>(swap_dir?oim[p].first->x():oim[p].second->x()),
				static_cast<float>(swap_dir?oim[p].first->y():oim[p].second->y())
			);
		}
		math::matrix33f hmf;
		if(ransac_homography(obj_pts,img_pts)(hmf,rt))
		{
			hm=hmf;
			return true;
		}
		return false;
	}

	template < typename OI, typename II, typename MT >
	bool find_homography(std::vector< std::pair<OI, II> > const &oim, math::size2ui const &ms, math::matrix<MT,3,3> &hm)
	{
		typedef typename boost::iterator_value<OI>::type obj_point_t;
		typedef typename boost::iterator_value<II>::type img_point_t;

		//BOOST_MPL_ASSERT( (boost::is_same<typename obj_point_t::value_type, typename img_point_t::value_type>) );

		typedef typename obj_point_t::value_type point2_value_t;
		typedef math::point2<point2_value_t> point2_t;
										
		typedef std::vector< std::pair< boost::uint32_t, point2_value_t> > averaged_corner_t;

		averaged_corner_t averaged_corners[4];
		
		point2_value_t const inv_mwidth=
			math::constant::one<point2_value_t>()/
			point2_value_t(ms.width());

		point2_value_t const inv_mheight=
			math::constant::one<point2_value_t>()/
			point2_value_t(ms.height());

		point2_t corners[4]=
		{
			point2_t(
				math::constant::zero<point2_value_t>(),
				math::constant::zero<point2_value_t>()
			),
			point2_t(
				math::constant::one<point2_value_t>(),
				math::constant::zero<point2_value_t>()
			),
			point2_t(
				math::constant::one<point2_value_t>(),
				math::constant::one<point2_value_t>()
			),
			point2_t(
				math::constant::zero<point2_value_t>(), 
				math::constant::one<point2_value_t>()
			)
		};

		
		for(boost::uint32_t m=0;m<oim.size();++m)
		{
			point2_t normalized_pt(
				oim[m].first->x()*inv_mwidth,
				oim[m].first->y()*inv_mheight
			);

			for(boost::uint32_t c=0;c<4;++c)
			{
				point2_value_t corner_dist=(normalized_pt-corners[c]).length();

				if(averaged_corners[c].empty())
					averaged_corners[c].push_back( std::make_pair(m,corner_dist) );
				else
				{
					averaged_corner_t::iterator iac=averaged_corners[c].begin();
					while(iac!=averaged_corners[c].end() && corner_dist>=iac->second) iac++;

					if(iac==averaged_corners[c].end())
					{
						if(averaged_corners[c].size()<3)
							averaged_corners[c].push_back( std::make_pair(m,corner_dist) );
					}
					else
						averaged_corners[c].insert(iac, std::make_pair(m,corner_dist));

					while(averaged_corners[c].size()>3)
						averaged_corners[c].pop_back();
				}
			}
		}

		std::vector< point2_t > marker_points(4), image_points(4);
		for(boost::uint32_t c=0;c<4;++c)
		{
			point2_value_t const inv_size=
				math::constant::one<point2_value_t>()/
				point2_value_t(averaged_corners[c].size());

			point2_t avg_mpoint(point2_t::zero());
			point2_t avg_ipoint(point2_t::zero());


			for(boost::uint32_t i=0;i<averaged_corners[c].size();++i)
			{
				avg_mpoint+=*oim[averaged_corners[c][i].first].first;
				avg_ipoint+=*oim[averaged_corners[c][i].first].second;
			}
			avg_mpoint/=point2_value_t(averaged_corners[c].size());
			avg_ipoint/=point2_value_t(averaged_corners[c].size());
			marker_points[c]=avg_mpoint;
			image_points[c]=avg_ipoint;
		}

		return ucv::perspective_transform(marker_points, image_points, hm);
	}


} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_HOMOGRAPHY_H
