#ifndef BALDZARIKA_AR_CONFIG_H
#define BALDZARIKA_AR_CONFIG_H

#include <baldzarika/ucv/config.h>
#include <baldzarika/ucv/gil_channel_traits.h>
#include <baldzarika/ucv/convert.h>
#include <baldzarika/ucv/resize.h>
#include <baldzarika/ucv/integral.h>
#include <baldzarika/ucv/norm.h>
#include <baldzarika/ucv/contour.h>
#include <baldzarika/ucv/surf.h>
#include <baldzarika/ucv/match_feature_points.h>
#include <baldzarika/ucv/klt_tracker.h>
#include <baldzarika/ucv/homography.h>
#include <baldzarika/ucv/perspective_transform.h>
#include <baldzarika/ucv/camera_pose.h>
#include <baldzarika/ucv/warp.h>
#include <baldzarika/ucv/corners_subpixel.h>
#include <baldzarika/ucv/find_otsu_threshold.h>
#include <baldzarika/ucv/threshold.h>
#include <baldzarika/ucv/gaussian_blur.h>
#include <baldzarika/ucv/canny.h>
#include <baldzarika/ucv/pixel_count.h>
#include <baldzarika/ucv/binarize.h>


#include <boost/enable_shared_from_this.hpp>
#include <boost/thread.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/deadline_timer.hpp>
#include <boost/dynamic_bitset.hpp>
#include <boost/foreach.hpp>
#include <boost/noncopyable.hpp>
#include <boost/signals2.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/algorithm/string/predicate.hpp>



namespace baldzarika { namespace ar {

} //namespace ar
} //namespace baldzarika

#endif //BALDZARIKA_AR_CONFIG_H
