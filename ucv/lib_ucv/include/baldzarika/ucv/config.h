#ifndef BALDZARIKA_UCV_CONFIG_H
#define BALDZARIKA_UCV_CONFIG_H

#define BALDZARIKA_UCV_FIXED_POINT_TRIGONO_USE_FPU


#include <boost/config.hpp>
#include <boost/math/special_functions.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/typedefs.hpp>
#include <boost/gil/extension/numeric/sampler.hpp>
#include <boost/gil/extension/numeric/resample.hpp>

#include <boost/smart_ptr.hpp>
#include <boost/noncopyable.hpp>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/random_access_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>

#define BOOST_UBLAS_SHALLOW_ARRAY_ADAPTOR
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>

#include <boost/random.hpp>


#if defined(HAVE_GLES2)
#include <GLES2/GL2.h>
#include <GLES2/gl2ext.h>
#endif //HAVE_GLES2


#include <vector>

namespace baldzarika { namespace ucv {
	
	namespace gil=::boost::gil;
	namespace ublas=::boost::numeric::ublas;

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_CONFIG_H