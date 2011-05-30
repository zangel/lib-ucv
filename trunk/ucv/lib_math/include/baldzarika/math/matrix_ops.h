#ifndef BALDZARIKA_MATH_MATRIX_OPS_H
#define BALDZARIKA_MATH_MATRIX_OPS_H

#include <baldzarika/math/constant.h>

namespace baldzarika { namespace math {

	template < typename T , boost::uint32_t R, boost::uint32_t C > struct matrix_ops;


	template < typename T >
	struct matrix_ops<T, 2, 2>
	{
		static inline matrix<T,2,2> const& zero()
		{
			static T const _zero[2][2]=
			{
				{ constant::zero<T>(), constant::zero<T>() },
				{ constant::zero<T>(), constant::zero<T>() }
			};
			return *(reinterpret_cast<matrix<T,2,2> const *>(&_zero[0][0]));
		}

		static inline matrix<T,2,2> const& identity()
		{
			static T const _identity[2][2]=
			{
				{ constant::one<T>(),	constant::zero<T>() },
				{ constant::zero<T>(),	constant::one<T>()  }
			};
			return *(reinterpret_cast<matrix<T,2,2> const *>(&_identity[0][0]));
		}
	};


	template < typename T >
	struct matrix_ops<T, 3, 3>
	{
		static inline matrix<T,3,3> const& zero()
		{
			static T const _zero[3][3]=
			{
				{ constant::zero<T>(), constant::zero<T>(), constant::zero<T>() },
				{ constant::zero<T>(), constant::zero<T>(), constant::zero<T>() },
				{ constant::zero<T>(), constant::zero<T>(), constant::zero<T>() }
			};
			return *(reinterpret_cast<matrix<T,3,3> const *>(&_zero[0][0]));
		}

		static inline matrix<T,3,3> const& identity()
		{
			static T const _identity[3][3]=
			{
				{ constant::one<T>(),	constant::zero<T>(),	constant::zero<T>() },
				{ constant::zero<T>(),	constant::one<T>(),		constant::zero<T>() },
				{ constant::zero<T>(),	constant::zero<T>(),	constant::one<T>()  }
			};
			return *(reinterpret_cast<matrix<T,3,3> const *>(&_identity[0][0]));
		}
	};

	template < typename T >
	struct matrix_ops<T, 4, 4>
	{
		static inline matrix<T,4,4> const& zero()
		{
			static T const _zero[4][4]=
			{
				{ constant::zero<T>(), constant::zero<T>(), constant::zero<T>(), constant::zero<T>() },
				{ constant::zero<T>(), constant::zero<T>(), constant::zero<T>(), constant::zero<T>() },
				{ constant::zero<T>(), constant::zero<T>(), constant::zero<T>(), constant::zero<T>() },
				{ constant::zero<T>(), constant::zero<T>(), constant::zero<T>(), constant::zero<T>() }
			};
			return *(reinterpret_cast<matrix<T,4,4> const *>(&_zero[0][0]));
		}

		static inline matrix<T,4,4> const& identity()
		{
			static T const _identity[4][4]=
			{
				{ constant::one<T>(),	constant::zero<T>(),	constant::zero<T>(),	constant::zero<T>() },
				{ constant::zero<T>(),	constant::one<T>(),		constant::zero<T>(),	constant::zero<T>() },
				{ constant::zero<T>(),	constant::zero<T>(),	constant::one<T>(),		constant::zero<T>() },
				{ constant::zero<T>(),	constant::zero<T>(),	constant::zero<T>(),	constant::one<T>() }
			};
			return *(reinterpret_cast<matrix<T,4,4> const *>(&_identity[0][0]));
		}
	};


} //namespace math
} //namespace baldzarika

#endif //BALDZARIKA_MATH_MATRIX_OPS_H