#ifndef BALDZARIKA_MATH_CONFIG_H
#define BALDZARIKA_MATH_CONFIG_H

#include <boost/config.hpp>
#include <boost/math/special_functions.hpp>

//#define BALDZARIKA_MATH_FIXED_POINT_TRIGONO_USE_FPU_ATAN2
//#define BALDZARIKA_MATH_FIXED_POINT_TRIGONO_USE_FPU_EXP
//#define BALDZARIKA_MATH_FIXED_POINT_TRIGONO_USE_FPU_COS
//#define BALDZARIKA_MATH_FIXED_POINT_TRIGONO_USE_FPU_SIN
#define BALDZARIKA_MATH_FIXED_POINT_TRIGONO_USE_FPU_SQRT


#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>


namespace baldzarika { namespace math {
	
	namespace ublas=::boost::numeric::ublas;

} //namespace math
} //namespace math


#endif //BALDZARIKA_MATH_CONFIG_H