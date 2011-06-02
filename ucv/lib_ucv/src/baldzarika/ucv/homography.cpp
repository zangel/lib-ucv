#include <baldzarika/ucv/config.h>
#include <baldzarika/ucv/homography.h>
#include <baldzarika/ucv/perspective_transform.h>

namespace baldzarika { namespace ucv {

	namespace {

		static inline float hypot(float a, float b)
		{
			a=std::abs(a);
			b=std::abs(b);
			if(a>b)
			{
				float f=b/a;
				return a*std::sqrt(1.0f+f*f);
			}
			if(b<std::numeric_limits<float>::epsilon())
				return 0.0f;
			float f=a/b;
			return b*std::sqrt(1.0f+f*f);
		}
	
	} //namespace anonymous



	homography::homography(std::vector<math::point2f> const &obj_pts, std::vector<math::point2f> const &img_pts)
		: m_obj_pts(obj_pts)
		, m_img_pts(img_pts)
		, m_rng()
		, m_rng_gen(m_rng, uint32_dist_t(0, m_obj_pts.size()-1))
	{
		BOOST_ASSERT(m_obj_pts.size()==m_img_pts.size());
	}

	homography::~homography()
	{
	}


	bool homography::get_sub_set(std::vector<math::point2f> &obj_pts, std::vector<math::point2f> &img_pts, std::size_t ma)
	{
		BOOST_ASSERT(obj_pts.size()==img_pts.size());
		
		boost::uint32_t idx[4];
		std::size_t it=0, i=0;
		for(; it<ma;++it)
		{
			for(i=0;i<4 && it<ma;)
			{
				boost::uint32_t idx_i=idx[i]=m_rng_gen();
				std::size_t j=0;
				for(;j<i;++j)
					if(idx_i==idx[j])
						break;
				if(j<i)
					continue;

				obj_pts[i]=m_obj_pts[idx_i];
				img_pts[i]=m_img_pts[idx_i];
				i++;
			}
			if(i==4 && (!check_sub_set(obj_pts,i) || !check_sub_set(img_pts,i)))
				continue;
			break;
		}
		return i==4 && it<ma;
	}

	bool homography::check_sub_set(std::vector<math::point2f> const &pts, std::size_t n)
	{
		std::size_t i=0;
		for(;i<=(n-1);++i)
		{
			std::size_t j=0;
			for(;j<i;j++)
			{
				float dx1=pts[j].x()-pts[i].x();
				float dy1=pts[j].y()-pts[i].y();
				std::size_t k=0;
				for(;k<j;k++)
				{
					float dx2=pts[k].x()-pts[i].x();
					float dy2=pts[k].y()-pts[i].y();
					if(std::abs(dx2*dy1-dy2*dx1)<=std::numeric_limits<float>::epsilon()*(std::abs(dx1)+std::abs(dy1)+std::abs(dx2)+std::abs(dy2)))
						break;
				}
				if(k<j)
					break;
			}
			if(j<i)
				break;
		}
		return i>=n-1;
	}

	void homography::compute_reproj_error(math::matrix33f &ms, std::vector<float> &err)
	{
		BOOST_ASSERT(m_obj_pts.size()==m_img_pts.size());
		
		for(std::size_t i=0;i<m_obj_pts.size();++i)
		{
			float ww=1.0f/(ms[6]*m_obj_pts[i].x()+ms[7]*m_obj_pts[i].y()+1.0f);
			float dx=(ms[0]*m_obj_pts[i].x()+ms[1]*m_obj_pts[i].y()+ms[2])*ww-m_img_pts[i].x();
			float dy=(ms[3]*m_obj_pts[i].x()+ms[4]*m_obj_pts[i].y()+ms[5])*ww-m_img_pts[i].y();
			err[i]=(dx*dx + dy*dy);
		}
	}

	std::size_t homography::inliers_count(math::matrix33f &ms, float t)
	{
		BOOST_ASSERT(m_obj_pts.size()==m_img_pts.size());
		
		float treshold=t*t;

		std::vector<float> err(m_obj_pts.size());
		compute_reproj_error(ms, err);

		std::size_t n_inliers=0;
		for(std::size_t i=0;i<m_obj_pts.size();++i)
			n_inliers+=err[i]<treshold?1:0;
		return n_inliers;
	}

	
	bool homography::normalize_points(std::vector<math::point2f> const &pts, math::point2f &tra, math::point2f &sca, math::matrix33f &m33, bool inv)
	{
		float const inv_size=1.0f/static_cast<float>(pts.size());

		math::point2f translate(0.0f,0.0f), scale(0.0f,0.0f);

		for(std::size_t p=0;p<pts.size();++p)
		{
			translate.x()+=pts[p].x();
			translate.y()+=pts[p].y();

			scale.x()+=pts[p].x()*pts[p].x();
			scale.y()+=pts[p].y()*pts[p].y();
		}

		translate.x()*=inv_size;
		translate.y()*=inv_size;
		scale.x()*=inv_size;
		scale.y()*=inv_size;

		scale.x()=std::sqrt(scale.x()-translate.x()*translate.x());
		scale.y()=std::sqrt(scale.y()-translate.y()*translate.y());

		if(std::abs(scale.x())<std::numeric_limits<float>::epsilon() || std::abs(scale.y())<std::numeric_limits<float>::epsilon())
			return false;

		m33=math::matrix33f::zero();
		m33(2,2)=1.0f;

		m33(0,0)=inv?1.0f/scale.x():scale.x();
		m33(1,1)=inv?1.0f/scale.y():scale.y();

		m33(0,2)=inv?-1.0f/scale.x()*translate.x():translate.x();
		m33(1,2)=inv?-1.0f/scale.y()*translate.y():translate.y();
		tra=translate;
		sca=scale;
		return true;
	}

	ransac_homography::ransac_homography(std::vector<math::point2f> const &obj_pts, std::vector<math::point2f> const &img_pts)
		: homography(obj_pts,img_pts)
	{

	}

	ransac_homography::~ransac_homography()
	{

	}

	std::size_t ransac_homography::update_num_iters(float p, float ep, std::size_t mi, std::size_t nmp)
	{
		p=std::max(p,0.0f);
		p=std::min(p,1.0f);
		ep=std::max(ep,0.0f);
		ep=std::min(ep,1.0f);
		
		float num=std::max(1.0f-p, std::numeric_limits<float>::min());
		float denom=1.0f-std::pow(1.0f-ep, static_cast<float>(nmp));
		if(denom<std::numeric_limits<float>::min())
			return 0;
		num=std::log(num);
		denom=std::log(denom);
		return denom>=0.0f || -num>=mi*(-denom)?
			mi:
			static_cast<std::size_t>(boost::math::round(num/denom));
	}

	bool ransac_homography::operator()(math::matrix33f &h, float t, float c, std::size_t mi)
	{
		if(m_obj_pts.size()<4 || m_obj_pts.size()!=m_img_pts.size())
			return false;
		
		std::vector<math::point2f> obj_pts(4), img_pts(4);
		std::size_t n_iters=m_img_pts.size()==4?1:mi;

		if(m_obj_pts.size()==4)
		{
			n_iters=1;
			for(std::size_t m=0;m<m_obj_pts.size();++m)
			{
				obj_pts[m]=m_obj_pts[m];
				img_pts[m]=m_img_pts[m];
			}
		}

		math::matrix33f ms;

		std::size_t max_good_count=0;
		for(std::size_t it=0;it<n_iters;++it)
		{
			if(m_obj_pts.size()>4)
			{
				if(!get_sub_set(obj_pts,img_pts, 300))
				{
					if(!it) return false;
					break;
				}
			}
			if(!perspective_transform(obj_pts, img_pts, ms))
				continue;
			std::size_t good_count=inliers_count(ms,t);
			if(good_count>std::max(max_good_count,std::size_t(3)))
			{
				max_good_count=good_count;
				h=ms;
				n_iters=update_num_iters(
					c,
					static_cast<float>(m_obj_pts.size()-good_count)/static_cast<float>(m_obj_pts.size()),
					n_iters
				);
			}
		}
		return max_good_count>0;
	}

} //namespace ucv
} //namespace baldzarika