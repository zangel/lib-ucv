#ifndef BALDZARIKA_UCV_PERSPECTIVE_TRANSFORM_H
#define BALDZARIKA_UCV_PERSPECTIVE_TRANSFORM_H

#include <baldzarika/ucv/point2.h>
#include <baldzarika/ucv/matrix.h>
#include <baldzarika/ucv/vector.h>
#include <baldzarika/ucv/solve.h>

namespace baldzarika { namespace ucv {

	template < typename PT , typename MT >
	bool perspective_transform(std::vector< point2<PT> > const &src, std::vector< point2<PT> > const &dst, matrix<MT, 3, 3> &pm)
	{
		if(src.size()!=4 || dst.size()!=4)
			return false;

		matrix<MT, 8, 8> a;
		vector<MT, 8> b;
		for(boost::uint32_t i=0;i<4;++i)
		{
			a(i,0)=a(i+4,3)=static_cast<MT>(src[i].x);
			a(i,1)=a(i+4,4)=static_cast<MT>(src[i].y);
			a(i,2)=a(i+4,5)=detail::constant::one<MT>();
			a(i,3)=a(i,4)=a(i,5)=a(i+4,0)=a(i+4,1)=a(i+4,2)=detail::constant::zero<MT>();
			a(i,6)=-static_cast<MT>(src[i].x*dst[i].x);
			a(i,7)=-static_cast<MT>(src[i].y*dst[i].x);
			a(i+4,6)=-static_cast<MT>(src[i].x*dst[i].y);
			a(i+4,7)=-static_cast<MT>(src[i].y*dst[i].y);
			b[i]=static_cast<MT>(dst[i].x);
			b[i+4]=static_cast<MT>(dst[i].y);
		}
		vector<MT, 8> x;
		if(!solve(a,b,x))
			return false;
		
		for(boost::uint32_t i=0;i<8;++i)
			pm.data()[i]=x[i];
		pm.data()[8]=detail::constant::one<MT>();
		return true;
	}

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_PERSPECTIVE_TRANSFORM_H