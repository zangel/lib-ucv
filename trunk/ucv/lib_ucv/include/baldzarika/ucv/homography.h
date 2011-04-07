#ifndef BALDZARIKA_UCV_HOMOGRAPHY_H
#define BALDZARIKA_UCV_HOMOGRAPHY_H

#include <baldzarika/ucv/point2.h>
#include <baldzarika/ucv/feature_point.h>
#include <baldzarika/ucv/matrix.h>
#include <baldzarika/ucv/vector.h>

namespace baldzarika { namespace ucv {

	class homography
	{
	public:
		typedef boost::uniform_int<boost::uint32_t> uint32_dist_t;
		typedef boost::variate_generator<boost::mt19937&, uint32_dist_t> uint32_rng_t;
		
		homography(std::vector<point2f> const &obj_pts, std::vector<point2f> const &img_pts);
		~homography();

	protected:
		static bool		normalize_points(std::vector<point2f> const &pts, point2f &tra, point2f &sca, matrix33f &m33, bool inv=false);
		bool			get_sub_set(std::vector<point2f> &obj_pts, std::vector<point2f> &img_pts, std::size_t ma);
		bool			check_sub_set(std::vector<point2f> const &pts, std::size_t n);
		bool			run_kernel(std::vector<point2f> const &obj_pts, std::vector<point2f> const &img_pts, matrix33f &h);
		std::size_t		inliers_count(matrix33f &ms, float t);
		void			compute_reproj_error(matrix33f &ms, std::vector<float> &err);


	private:
		static bool		jacobi(matrix99f const &S0, vector9f &e, matrix99f &E, bool ce=true, float eps=std::numeric_limits<float>::epsilon());
	
	
	protected:
		std::vector<point2f> const &m_obj_pts;
		std::vector<point2f> const &m_img_pts;

		boost::mt19937				m_rng;
		uint32_rng_t				m_rng_gen;
	};

	class ransac_homography
		: public homography
	{
	public:
		ransac_homography(std::vector<point2f> const &obj_pts, std::vector<point2f> const &img_pts);
		~ransac_homography();

		bool			operator()(matrix33f &h, float t, float c=0.99f, std::size_t mi=2000);

	protected:
		std::size_t		update_num_iters(float p, float ep, std::size_t mi, std::size_t nmp=4);
	};

	template < typename T, typename DT, typename MT >
	bool find_homography_ransac(
		std::vector< feature_point<T,DT> > const &ops,
		std::vector< feature_point<T,DT> > const &ips,
		std::vector< std::pair<std::size_t, std::size_t> > const &oim,
		matrix<MT, 3, 3> &hm,
		float rt=3.0f
	)
	{
		BOOST_ASSERT(hm.size1()==3 && hm.size2()==3);
		
		std::vector<point2f> obj_pts(oim.size()), img_pts(oim.size());
		for(std::size_t p=0;p<oim.size();++p)
		{
			obj_pts[p]=point2f(
				static_cast<float>(ops[oim[p].first].x),
				static_cast<float>(ops[oim[p].first].y)
			);

			img_pts[p]=point2f(
				static_cast<float>(ips[oim[p].second].x),
				static_cast<float>(ips[oim[p].second].y)
			);
		}
		matrix33f hmf;
		if(ransac_homography(obj_pts,img_pts)(hmf,rt))
		{
			hm=hmf;
			return true;
		}
		return false;
	}

	template < typename I1, typename I2 , typename MT>
	bool find_homography_ransac(
		std::vector< std::pair<I1, I2> > const &oim,
		matrix<MT, 3, 3> &hm,
		bool swap_dir=false,
		float rt=3.0f
		)
	{
		BOOST_ASSERT(hm.size1()==3 && hm.size2()==3);

		std::vector<point2f> obj_pts(oim.size()), img_pts(oim.size());
		for(std::size_t p=0;p<oim.size();++p)
		{
			obj_pts[p]=point2f(
				static_cast<float>(swap_dir?oim[p].second->x:oim[p].first->x),
				static_cast<float>(swap_dir?oim[p].second->y:oim[p].first->y)
			);

			img_pts[p]=point2f(
				static_cast<float>(swap_dir?oim[p].first->x:oim[p].second->x),
				static_cast<float>(swap_dir?oim[p].first->y:oim[p].second->y)
			);
		}
		matrix33f hmf;
		if(ransac_homography(obj_pts,img_pts)(hmf,rt))
		{
			hm=hmf;
			return true;
		}
		return false;
	}


} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_HOMOGRAPHY_H
