include_directories (include)

set(ucv_headers
  include/baldzarika/ucv/config.h
  include/baldzarika/ucv/adaptive_treshold.h
  include/baldzarika/ucv/approximate_polygon.h
  include/baldzarika/ucv/binarize.h
  include/baldzarika/ucv/camera_pose.h
  include/baldzarika/ucv/canny.h
  include/baldzarika/ucv/contour.h
  include/baldzarika/ucv/corners_subpixel.h
  include/baldzarika/ucv/estimate_rotation.h
  include/baldzarika/ucv/extract_surf_descriptor.h
  include/baldzarika/ucv/feature_point.h
  include/baldzarika/ucv/find_otsu_threshold.h
  include/baldzarika/ucv/gaussian_blur.h
  include/baldzarika/ucv/gil_channel_traits.h
  include/baldzarika/ucv/good_features_detector.h
  include/baldzarika/ucv/haar_wavelets.h
  include/baldzarika/ucv/homography.h
  include/baldzarika/ucv/integral.h
  include/baldzarika/ucv/kalman_filter.h
  include/baldzarika/ucv/klt_tracker.h
  include/baldzarika/ucv/make_gaussian_kernel.h
  include/baldzarika/ucv/masked_region.h
  include/baldzarika/ucv/match_feature_points.h
  include/baldzarika/ucv/norm.h
  include/baldzarika/ucv/perspective_transform.h
  include/baldzarika/ucv/pixel_count.h
  include/baldzarika/ucv/ranged_histogram.h
  include/baldzarika/ucv/resize.h
  include/baldzarika/ucv/sobel.h
  include/baldzarika/ucv/threshold.h
  include/baldzarika/ucv/ublas_fixed_point_type_traits.h
  include/baldzarika/ucv/resize.h
  include/baldzarika/ucv/resize.h
  include/baldzarika/ucv/warp.h
)

set(ucv_sources
	src/baldzarika/ucv/config.cpp
)

add_library(ucv STATIC
	${ucv_headers}
	${ucv_sources}
)

target_link_libraries(ucv math)
