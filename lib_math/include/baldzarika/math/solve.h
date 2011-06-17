#ifndef BALDZARIKA_MATH_SOLVE_H
#define BALDZARIKA_MATH_SOLVE_H

#include <baldzarika/math/matrix.h>
#include <baldzarika/math/vector.h>
#include <baldzarika/math/svd.h>
#include <baldzarika/math/jacobi.h>

namespace baldzarika { namespace math {

	namespace detail {

		struct svd_solver {};
		struct jacobi_solver {};

	} //namespace detail

	template < typename T, boost::uint32_t MN >
	bool solve(matrix<T, MN, MN> const &a, vector<T, MN> const &b, vector<T, MN> &x, detail::svd_solver)
	{
		matrix<T, MN, MN> u, v;
		vector<T, MN> w;

		if(!svd(a,u,w,v))
			return false;

		for(boost::uint32_t iw=0;iw<MN;++iw)
		{
			T inv_w=w[iw];
			
			inv_w=(inv_w==constant::zero<T>()?
				constant::zero<T>():
				constant::one<T>()/inv_w
			);

			for(boost::uint32_t r=0;r<MN;++r)
				v(r,iw)*=inv_w;
		}
		x=v*(u.transposed()*b);
		return true;
	}

	template < typename T, boost::uint32_t MN >
	bool solve(matrix<T, MN, MN> const &a, vector<T, MN> const &b, vector<T, MN> &x, detail::jacobi_solver)
	{
		return false;
	}


} //namespace math
} //namespace baldzarika

#endif //BALDZARIKA_MATH_SOLVE_H