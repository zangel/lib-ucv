#ifndef BALDZARIKA_UCV_CONFIG_H
#define BALDZARIKA_UCV_CONFIG_H

#include <boost/config.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/typedefs.hpp>

#include <boost/smart_ptr.hpp>
#include <boost/noncopyable.hpp>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/random_access_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>

#if defined(HAVE_GLES2)
#include <GLES2/GL2.h>
#include <GLES2/gl2ext.h>
#endif //HAVE_GLES2


#include <vector>

namespace baldzarika { namespace ucv {
	
	namespace gil=::boost::gil;

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_CONFIG_H