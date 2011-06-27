#ifndef BALDZARIKA_UCV_CONFIG_H
#define BALDZARIKA_UCV_CONFIG_H

#include <baldzarika/math/config.h>
#include <baldzarika/math/fixed_point.h>
#include <baldzarika/math/point2.h>
#include <baldzarika/math/size2.h>
#include <baldzarika/math/box2.h>
#include <baldzarika/math/matrix.h>
#include <baldzarika/math/vector.h>

#include <boost/config.hpp>
#include <boost/math/special_functions.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/typedefs.hpp>
#include <boost/gil/extension/numeric/sampler.hpp>
#include <boost/gil/extension/numeric/resample.hpp>
#define png_infopp_NULL (png_infopp)0
#define int_p_NULL (int*)0
#include <boost/gil/extension/io/png_io.hpp>

#include <boost/smart_ptr.hpp>
#include <boost/noncopyable.hpp>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/random_access_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>

#define BOOST_UBLAS_SHALLOW_ARRAY_ADAPTOR

#include <boost/random.hpp>

#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/dynamic_bitset.hpp>


#if defined(HAVE_GLES2)
#include <GLES2/GL2.h>
#include <GLES2/gl2ext.h>
#endif //HAVE_GLES2


#include <vector>
#include <stack>
#include <list>

#include <kdtree++/kdtree.hpp>

namespace baldzarika { namespace ucv {
	
	namespace gil=::boost::gil;

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_CONFIG_H