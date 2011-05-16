#ifndef BALDZARIKA_UCV_PERSPECTIVE_TRANSFORM_H
#define BALDZARIKA_UCV_PERSPECTIVE_TRANSFORM_H

#include <baldzarika/ucv/point2.h>
#include <baldzarika/ucv/matrix.h>
#include <baldzarika/ucv/vector.h>
#include <baldzarika/ucv/jacobi.h>
#include <baldzarika/ucv/homography.h>

namespace baldzarika { namespace ucv {

	namespace detail {
		
		template < typename PT, typename MT >
		bool normalize_points(std::vector< point2<PT> > const &pts, point2<PT> &translate, point2<PT> &scale, matrix<MT, 3, 3> &m3x3, bool inv)
		{
			PT const inv_size=constant::one<PT>()/PT(pts.size());
						
			translate.x=translate.y=scale.x=scale.y=constant::zero<PT>();

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

			if(std::abs(scale.x)<std::numeric_limits<PT>::epsilon() || std::abs(scale.y)<std::numeric_limits<PT>::epsilon())
				return false;

			

			m3x3(0,0)=inv?constant::one<MT>()/MT(scale.x):MT(scale.x);
			m3x3(1,1)=inv?constant::one<MT>()/MT(scale.y):MT(scale.y);
			m3x3(2,2)=constant::one<MT>();

			m3x3(0,2)=inv?MT(-constant::one<PT>()/scale.x*translate.x):MT(translate.x);
			m3x3(1,2)=inv?MT(-constant::one<PT>()/scale.y*translate.y):MT(translate.y);

			m3x3(0,1)=m3x3(1,0)=m3x3(2,0)=m3x3(2,1)=constant::zero<MT>();
			return true;
		}

	} //namespace detail

	template < typename PT , typename MT >
	bool perspective_transform(std::vector< point2<PT> > const &src, std::vector< point2<PT> > const &dst, matrix<MT, 3, 3> &pm)
	{
		if(src.size()!=4 || dst.size()!=4)
			return false;

		point2<PT> src_translate, src_scale;
		matrix<MT, 3, 3> src_correct;
		detail::normalize_points(src, src_translate, src_scale, src_correct, true);

		point2<PT> dst_translate, dst_scale;
		matrix<MT, 3, 3> dst_correct;
		detail::normalize_points(dst, dst_translate, dst_scale, dst_correct, false);

		matrix<MT, 9, 9> LtL=matrix<MT, 9, 9>::zero();

		for(std::size_t i=0;i<4;++i)
		{
			PT src_x=(src[i].x-src_translate.x)/src_scale.x;
			PT src_y=(src[i].y-src_translate.y)/src_scale.y;

			PT dst_x=(dst[i].x-dst_translate.x)/dst_scale.x;
			PT dst_y=(dst[i].y-dst_translate.y)/dst_scale.y;
			PT Lx[9]=
			{
				src_x,							src_y,							detail::constant::one<PT>(),
				detail::constant::zero<PT>(),	detail::constant::zero<PT>(),	detail::constant::zero<PT>(),
				-dst_x*src_x,					-dst_x*src_y,					-dst_x
			};

			PT Ly[9]=
			{
				detail::constant::zero<PT>(),	detail::constant::zero<PT>(),	detail::constant::zero<PT>(),
				src_x,							src_y,							detail::constant::one<PT>(),
				-dst_y*src_x,					-dst_y*src_y,					-dst_y
			};

			for(std::size_t j=0;j<9;++j)
				for(std::size_t k=j;k<9;++k)
					LtL(j,k)+=MT(Lx[j]*Lx[k]+Ly[j]*Ly[k]);

		}

		for(std::size_t i=0;i<9;++i)
		{
			for(std::size_t j=0;j<i;++j)
			{
				LtL(i,j)=LtL(j,i);
			}
		}

		matrix<MT, 9, 9> V;
		vector<MT, 9> W;
		jacobi(LtL, W, V);

		
		pm(0,0)=V(8,0); pm(0,1)=V(8,1); pm(0,2)=V(8,2);
		pm(1,0)=V(8,3); pm(1,1)=V(8,4); pm(1,2)=V(8,5);
		pm(2,0)=V(8,6); pm(2,1)=V(8,7); pm(2,2)=V(8,8);

		// reverse normalization
		pm=(dst_correct*pm)*src_correct;
		pm.scale(detail::constant::one<MT>()/pm(2,2));
		return true;
	}

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_PERSPECTIVE_TRANSFORM_H