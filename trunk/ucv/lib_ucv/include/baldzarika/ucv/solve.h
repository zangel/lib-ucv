#ifndef BALDZARIKA_UCV_SOLVE_H
#define BALDZARIKA_UCV_SOLVE_H

#include <baldzarika/ucv/matrix.h>
#include <baldzarika/ucv/vector.h>
#include <baldzarika/ucv/svd.h>

namespace baldzarika { namespace ucv {

	template < typename T, boost::uint32_t MN >
	bool solve(matrix<T, MN, MN> const &a, vector<T, MN> const &b, vector<T, MN> &x)
	{
		matrix<T, MN, MN> u, v;
		vector<T, MN> w;

		if(!svd(a,u,w,v))
			return false;

		for(boost::uint32_t iw=0;iw<MN;++iw)
		{
			T inv_w=w[iw];
			
			inv_w=(inv_w==detail::constant::zero<T>()?
				detail::constant::zero<T>():
				detail::constant::one<T>()/inv_w
			);

			for(boost::uint32_t r=0;r<MN;++r)
				v(r,iw)*=inv_w;
		}
		x=v*(u.transposed()*b);
		return true;
	}

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_SOLVE_H