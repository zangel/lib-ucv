#include <baldzarika/ucv/config.h>
#include <baldzarika/ucv/homography.h>

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



	homography::homography(std::vector<point2f> const &obj_pts, std::vector<point2f> const &img_pts)
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


	bool homography::get_sub_set(std::vector<point2f> &obj_pts, std::vector<point2f> &img_pts, std::size_t ma)
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

	bool homography::check_sub_set(std::vector<point2f> const &pts, std::size_t n)
	{
		std::size_t i=0;
		for(;i<=(n-1);++i)
		{
			std::size_t j=0;
			for(;j<i;j++)
			{
				float dx1=pts[j].x-pts[i].x;
				float dy1=pts[j].y-pts[i].y;
				std::size_t k=0;
				for(;k<j;k++)
				{
					float dx2=pts[k].x-pts[i].x;
					float dy2=pts[k].y-pts[i].y;
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

	void homography::compute_reproj_error(matrix33f &ms, std::vector<float> &err)
	{
		BOOST_ASSERT(m_obj_pts.size()==m_img_pts.size());
		matrix33f::const_pointer H=ms.data();
		for(std::size_t i=0;i<m_obj_pts.size();++i)
		{
			float ww=1.0f/(H[6]*m_obj_pts[i].x+H[7]*m_obj_pts[i].y+1.0f);
			float dx=(H[0]*m_obj_pts[i].x+H[1]*m_obj_pts[i].y+H[2])*ww-m_img_pts[i].x;
			float dy=(H[3]*m_obj_pts[i].x+H[4]*m_obj_pts[i].y+H[5])*ww-m_img_pts[i].y;
			err[i]=(dx*dx + dy*dy);
		}
	}

	std::size_t homography::inliers_count(matrix33f &ms, float t)
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

	bool homography::run_kernel(std::vector<point2f> const &obj_pts, std::vector<point2f> const &img_pts, matrix33f &h)
	{
		BOOST_ASSERT(obj_pts.size()==img_pts.size());
		BOOST_ASSERT(obj_pts.size()>=4);

		point2f obj_translate, obj_scale;
		matrix33f obj_correct;
		normalize_points(obj_pts, obj_translate, obj_scale, obj_correct, true);

		point2f img_translate, img_scale;
		matrix33f img_correct;
		normalize_points(img_pts, img_translate, img_scale, img_correct, false);

		matrix99f LtL=matrix99f::zero();

		for(std::size_t i=0;i<obj_pts.size();++i)
		{
			float obj_x=(obj_pts[i].x-obj_translate.x)/obj_scale.x;
			float obj_y=(obj_pts[i].y-obj_translate.y)/obj_scale.y;

			float img_x=(img_pts[i].x-img_translate.x)/img_scale.x;
			float img_y=(img_pts[i].y-img_translate.y)/img_scale.y;
			float Lx[9]=
			{
				obj_x,			obj_y,			1.0f,
				0.0f,			0.0f,			0.0f,
				-img_x*obj_x,	-img_x*obj_y,	-img_x
			};

			float Ly[9]=
			{
				0.0f,			0.0f,			0.0f,
				obj_x,			obj_y,			1.0f,
				-img_y*obj_x,	-img_y*obj_y,	-img_y
			};

			for(std::size_t j=0;j<9;++j)
				for(std::size_t k=j;k<9;++k)
					LtL(j,k)+=Lx[j]*Lx[k]+Ly[j]*Ly[k];

		}
		
		for(std::size_t i=0;i<9;++i)
		{
			for(std::size_t j=0;j<i;++j)
			{
				LtL(i,j)=LtL(j,i);
			}
		}

		matrix99f V;
		vector9f W;
		jacobi(LtL, W, V);

		matrix33f H;
		H(0,0)=V(8,0);H(0,1)=V(8,1);H(0,2)=V(8,2);
		H(1,0)=V(8,3);H(1,1)=V(8,4);H(1,2)=V(8,5);
		H(2,0)=V(8,6);H(2,1)=V(8,7);H(2,2)=V(8,8);

		// reverse normalization
		matrix33f Htemp=img_correct*H;
		H=Htemp*obj_correct;
		h=H.scaled(1.0f/H(2,2));
		return true;
	}

	bool homography::normalize_points(std::vector<point2f> const &pts, point2f &tra, point2f &sca, matrix33f &m33, bool inv)
	{
		float const inv_size=1.0f/static_cast<float>(pts.size());

		point2f translate(0.0f,0.0f), scale(0.0f,0.0f);

		for(std::size_t p=0;p<pts.size();++p)
		{
			translate.x+=pts[p].x;
			translate.y+=pts[p].y;

			scale.x+=pts[p].x*pts[p].x;
			scale.y+=pts[p].y*pts[p].y;
		}

		translate.x*=inv_size;
		translate.y*=inv_size;
		scale.x*=inv_size;
		scale.y*=inv_size;

		scale.x=std::sqrt(scale.x-translate.x*translate.x);
		scale.y=std::sqrt(scale.y-translate.y*translate.y);

		if(std::abs(scale.x)<std::numeric_limits<float>::epsilon() || std::abs(scale.y)<std::numeric_limits<float>::epsilon())
			return false;

		m33=matrix33f::zero();
		m33(2,2)=1.0f;

		m33(0,0)=inv?1.0f/scale.x:scale.x;
		m33(1,1)=inv?1.0f/scale.y:scale.y;

		m33(0,2)=inv?-1.0f/scale.x*translate.x:translate.x;
		m33(1,2)=inv?-1.0f/scale.y*translate.y:translate.y;
		tra=translate;
		sca=scale;
		return true;
	}

	bool homography::jacobi(matrix99f const &S0, vector9f &e, matrix99f &E, bool ce, float eps)
	{
		boost::int32_t max_iters=9*9*30;;
		boost::int32_t i, j, k, m, it;

		vector9f maxSR, maxSC;
		boost::int32_t indR[9], indC[9];

		matrix99f S(S0);

		if(ce) E=matrix99f::identity();

		float mv;

		for(k=0;k<9;++k)
		{
			e(k)=S(k,k);
			if(k<8)
			{
				for(m=k+1,mv=std::abs(S(k,m)),i=k+2;i<9;++i)
				{
					float v=std::abs(S(k,i));
					if(mv<v)
						mv=v,m=i;
				}
				maxSR(k)=mv;
				indR[k]=m;
			}
			if(k>0)
			{
				for(m=0,mv=std::abs(S(0,k)),i=1;i<k;++i)
				{
					float v=std::abs(S(i,k));
					if(mv<v)
						mv=v,m=i;
				}
				maxSC(k)=mv;
				indC[k]=m;
			}
		}

		for(it=0;it<max_iters;++it)
		{
			for(k=0,mv=maxSR(0),i=1;i<8;++i)
			{
				float v=maxSR(i);
				if(mv<v)
					mv=v,k=i;
			}

			boost::int32_t l=indR[k];
			for(i=1;i<9;++i)
			{
				float v=maxSC(i);
				if(mv<v)
					mv=v,k=indC[i],l=i;
			}

			float p=S(k,l);
			if(std::abs(p)<=eps)
				break;
			float y=(e(l)-e(k))*0.5f;
			float t=std::abs(y)+hypot(p,y);
			float s=hypot(p,t);
			float c=t/s;
			s=p/s; t=(p/t)*p;
			if(y<0.0f)
				s=-s,t=-t;

			S(k,l)=0.0f;
			e(k)-=t;
			e(l)+=t;

			float a0, b0;

#undef rotate
#define rotate(v0, v1) a0 = v0, b0 = v1, v0 = a0*c - b0*s, v1 = a0*s + b0*c
			for(i=0;i<k;++i)
				rotate(S(i,k), S(i,l));
			for(i=k+1;i<l;++i)
				rotate(S(k,i), S(i,l));
			for(i=l+1;i<9;++i)
				rotate(S(k,i), S(l,i));
			if(ce)
			{
				for(i=0;i<9;++i)
				{
					rotate(E(k,i), E(l,i));
				}
			}
#undef rotate
			for(j=0;j<2;++j)
			{
				boost::int32_t idx=j==0?k:l;
				if(idx<8)
				{
					for(m=idx+1,mv=std::abs(S(idx,m)),i=idx+2;i<9;++i)
					{
						float v=std::abs(S(idx,i));
						if(mv<v)
							mv=v,m=i;
					}
					maxSR(idx)=mv;
					indR[idx]=m;
				}
				if(idx>0)
				{
					for(m=0,mv=std::abs(S(0,idx)),i=1;i<idx;++i)
					{
						float v=std::abs(S(i,idx));
						if(mv<v)
							mv=v,m=i;
					}
					maxSC(idx)=mv;
					indC[idx]=m;
				}
			}
		}
		for(k=0;k<8;++k)
		{
			m=k;
			for(i=k+1;i<9;++i)
			{
				if(e(m)<e(i))
					m=i;
			}
			if(k!=m)
			{
				std::swap(e(m), e(k));
				if(ce)
				{
					for(i=0;i<9;++i)
						std::swap(E(m,i),E(k,i));
				}
			}
		}
		return true;
	}


	ransac_homography::ransac_homography(std::vector<point2f> const &obj_pts, std::vector<point2f> const &img_pts)
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

	bool ransac_homography::operator()(matrix33f &h, float t, float c, std::size_t mi)
	{
		if(m_obj_pts.size()<4 || m_obj_pts.size()!=m_img_pts.size())
			return false;
		
		std::vector<point2f> obj_pts(4), img_pts(4);
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

		matrix33f ms;

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
			if(!run_kernel(obj_pts, img_pts, ms))
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