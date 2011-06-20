#define BOOST_TEST_MODULE lib_ucv_test

#include <boost/test/unit_test.hpp>

#include <baldzarika/math/config.h>
#include <baldzarika/math/fixed_point.h>
#include <baldzarika/math/matrix.h>
#include <baldzarika/math/svd.h>
#include <baldzarika/math/solve.h>
#include <baldzarika/math/vector.h>

#include <baldzarika/ucv/config.h>
#include <baldzarika/ucv/convert.h>
#include <baldzarika/ucv/warp.h>
#include <baldzarika/ucv/integral.h>
#include <baldzarika/ucv/surf.h>
#include <baldzarika/ucv/homography.h>
#include <baldzarika/ucv/match_feature_points.h>
#include <baldzarika/ucv/klt_tracker.h>
#include <baldzarika/ucv/good_features_detector.h>
#include <baldzarika/ucv/sobel.h>
#include <baldzarika/ucv/gaussian_blur.h>
#include <baldzarika/ucv/canny.h>
#include <baldzarika/ucv/adaptive_treshold.h>
#include <baldzarika/ucv/threshold.h>
#include <baldzarika/ucv/ranged_histogram.h>
#include <baldzarika/ucv/find_otsu_threshold.h>
#include <baldzarika/ucv/perspective_transform.h>
#include <baldzarika/ucv/camera_pose.h>
#include <baldzarika/ucv/pixel_count.h>

#include <boost/date_time.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/features2d/features2d.hpp>

#define OPENCV_WND_NAME  "ucv"
#define OPENCV_WND_NAME2  "ucv2"

using namespace boost;

BOOST_AUTO_TEST_CASE( create_open_cv_window )
{
	cv::namedWindow(OPENCV_WND_NAME);
	//cv::namedWindow(OPENCV_WND_NAME2);
}

BOOST_AUTO_TEST_CASE( test_convert )
{
	namespace ucv=baldzarika::ucv;
	namespace math=baldzarika::math;

	typedef math::fixed_point<20, 11> real_t;

	typedef ucv::gil::pixel<real_t, ucv::gil::gray_layout_t> gray_real_pixel_t;
	typedef ucv::gil::image< gray_real_pixel_t, false, std::allocator<unsigned char> > gray_real_image_t;
	typedef gray_real_image_t::view_t gray_real_view_t;

	cv::Mat cv_img=cv::imread("test_img.png", CV_LOAD_IMAGE_GRAYSCALE);
	cv::imshow(OPENCV_WND_NAME, cv_img);
	cv::waitKey();
	
	gray_real_image_t gray_img(cv_img.cols, cv_img.rows, 4);
	
	ucv::convert(
		ucv::gil::interleaved_view(
			cv_img.cols, cv_img.rows,
			reinterpret_cast<ucv::gil::gray8_pixel_t*>(cv_img.data),
			cv_img.step
		),
		ucv::gil::view(gray_img),
		ucv::detail::grayscale_convert()
	);

	ucv::convert(
		ucv::gil::view(gray_img),
		ucv::gil::interleaved_view(
			cv_img.cols, cv_img.rows,
			reinterpret_cast<ucv::gil::gray8_pixel_t*>(cv_img.data),
			cv_img.step 
		),
		ucv::detail::grayscale_convert()
	);
	
	cv::imshow(OPENCV_WND_NAME, cv_img);
	cv::waitKey();
}
 


BOOST_AUTO_TEST_CASE( test_integral )
{
	namespace ucv=baldzarika::ucv;
	namespace math=baldzarika::math;

	typedef math::fixed_point<20, 11> integral_t;

	typedef ucv::gil::pixel<integral_t, ucv::gil::gray_layout_t> integral_pixel_t;
	typedef ucv::gil::image< integral_pixel_t, false, std::allocator<unsigned char> > integral_image_t;
	typedef integral_image_t::view_t integral_view_t;

	cv::Mat cv_img=cv::imread("test_img2.png", CV_LOAD_IMAGE_GRAYSCALE);
	cv::imshow(OPENCV_WND_NAME, cv_img);
	cv::waitKey();

	integral_image_t gray_img(cv_img.cols, cv_img.rows, 4);

	ucv::convert(
		ucv::gil::interleaved_view(
			cv_img.cols, cv_img.rows,
			reinterpret_cast<ucv::gil::gray8_pixel_t*>(cv_img.data),
			cv_img.step
		),
		ucv::gil::view(gray_img),
		ucv::detail::grayscale_convert()
	);

	integral_image_t integral_img(cv_img.cols, cv_img.rows, 4);

	ucv::integral(
		gil::view(gray_img),
		gil::view(integral_img)
	);

	cv::imshow(OPENCV_WND_NAME, cv::Mat(cv_img.rows, cv_img.cols, CV_32SC1, &gil::view(integral_img)[0][0]));
	cv::waitKey();
}



BOOST_AUTO_TEST_CASE( test_surf )
{
	namespace ucv=baldzarika::ucv;
	namespace math=baldzarika::math;

	cv::Mat cv_img=cv::imread("test_img.png", CV_LOAD_IMAGE_GRAYSCALE);
	cv::imshow(OPENCV_WND_NAME, cv_img);
	cv::waitKey();

	ucv::gil::gray8_image_t gil_gray_img(cv_img.cols/2, cv_img.rows/2);
	ucv::gil::resize_view(
		ucv::gil::interleaved_view(
			cv_img.cols, cv_img.rows,
			reinterpret_cast<ucv::gil::gray8_pixel_t*>(cv_img.data),
			cv_img.step
		),
		gil::view(gil_gray_img),
		gil::bilinear_sampler()
	);

	cv::Mat cv_gray_img(gil_gray_img.height(), gil_gray_img.width(), CV_8UC1, &ucv::gil::view(gil_gray_img)[0][0]);
	cv::imshow(OPENCV_WND_NAME, cv_gray_img);
	cv::waitKey();

	ucv::surf::integral_image_t gray_img(gil_gray_img.width(), gil_gray_img.height());
	ucv::surf::integral_t median;
	ucv::convert(
		ucv::gil::view(gil_gray_img),
		ucv::gil::view(gray_img),
		ucv::detail::grayscale_convert_and_median<ucv::surf::integral_t>(
			median,
			gil_gray_img.width(),
			gil_gray_img.height()
		)
	);

	ucv::surf::integral_image_t integral_img(gil_gray_img.width(), gil_gray_img.height());
	ucv::integral(ucv::gil::const_view(gray_img), ucv::gil::view(integral_img), median);

	ucv::surf the_surf(math::size2ui(gray_img.width(), gray_img.height()), 3, 4, 2, 4.0e-4f);

	posix_time::ptime start=posix_time::microsec_clock::local_time();
	the_surf.set_integral_view(ucv::gil::const_view(integral_img));
	the_surf.build_response_layers();
	std::vector<ucv::surf::feature_point_t> fps;
	the_surf.detect(fps);
	the_surf.describe(fps);
	posix_time::ptime finish=posix_time::microsec_clock::local_time();
	

	std::cout << "surf::(update+detect+describe)=(" << (finish-start).total_microseconds() << ")" << std::endl;
	for(std::size_t ifp=0;ifp<fps.size();++ifp)
	{
		cv::circle(cv_img,
			cv::Point(2.0f*static_cast<float>(fps[ifp].x()), 2.0f*static_cast<float>(fps[ifp].y())),
			2,
			cv::Scalar(255.0)
		);
	}
	cv::imshow(OPENCV_WND_NAME, cv_img);
	cv::waitKey();
}



BOOST_AUTO_TEST_CASE( test_surf_match )
{
	namespace ucv=baldzarika::ucv;
	namespace math=baldzarika::math;

	typedef math::fixed_point<10, 21> gray_t;
	typedef gil::pixel<gray_t, ucv::gil::gray_layout_t> gray_pixel_t;
	typedef gil::image< gray_pixel_t, false, std::allocator<unsigned char> > gray_image_t;
	typedef gray_image_t::view_t gray_view_t;



	//img 1
	cv::Mat cv_img1=cv::imread("test_img2.png", CV_LOAD_IMAGE_GRAYSCALE);
	
	ucv::gil::gray8_image_t gil_gray_img1(cv_img1.cols, cv_img1.rows);
	ucv::gil::resize_view(
		ucv::gil::interleaved_view(
			cv_img1.cols, cv_img1.rows,
			reinterpret_cast<ucv::gil::gray8_pixel_t*>(cv_img1.data),
			cv_img1.step
		),
		gil::view(gil_gray_img1),
		gil::bilinear_sampler()
	);

	cv::Mat cv_gray_img1(gil_gray_img1.height(), gil_gray_img1.width(), CV_8UC1, &ucv::gil::view(gil_gray_img1)[0][0]);

	gray_image_t gray_img1(gil_gray_img1.width(), gil_gray_img1.height());
	ucv::convert(
		ucv::gil::view(gil_gray_img1),
		ucv::gil::view(gray_img1),
		ucv::detail::grayscale_convert()
	);

	ucv::surf::integral_image_t int_img1(gil_gray_img1.width(), gil_gray_img1.height());
	ucv::integral(ucv::gil::view(gray_img1), ucv::gil::view(int_img1));
	
	//img2
	cv::Mat cv_img2=cv::imread("test_img2_match.png", CV_LOAD_IMAGE_GRAYSCALE);
	
	ucv::gil::gray8_image_t gil_gray_img2(cv_img2.cols, cv_img2.rows);
	ucv::gil::resize_view(
		ucv::gil::interleaved_view(
			cv_img2.cols, cv_img2.rows,
			reinterpret_cast<ucv::gil::gray8_pixel_t*>(cv_img2.data),
			cv_img2.step
		),
		gil::view(gil_gray_img2),
		gil::bilinear_sampler()
	);

	gray_image_t gray_img2(gil_gray_img2.width(), gil_gray_img2.height(), 4);
	ucv::convert(
		ucv::gil::view(gil_gray_img2),
		ucv::gil::view(gray_img2),
		ucv::detail::grayscale_convert()
	);

	ucv::surf::integral_image_t int_img2(gil_gray_img2.width(), gil_gray_img2.height());
	ucv::integral(ucv::gil::view(gray_img2), ucv::gil::view(int_img2));
	
	ucv::surf the_surf1(math::size2ui(gray_img1.width(), gray_img1.height()), 3, 4, 2, 4.0e-4f);

	the_surf1.set_integral_view(ucv::gil::const_view(int_img1));
	the_surf1.build_response_layers();
	std::vector<ucv::surf::feature_point_t> fps1;
	boost::posix_time::ptime start=boost::posix_time::microsec_clock::local_time();
	for(int i=0;i<100;++i)
		the_surf1.detect(fps1);
	boost::posix_time::ptime finish=boost::posix_time::microsec_clock::local_time();
	std::cout << "detect<vec>: " << float((finish-start).total_microseconds())/100.0f << std::endl;


	ucv::surf::fps_by_pos_tree_t ffps1;
	boost::posix_time::ptime start2=boost::posix_time::microsec_clock::local_time();
	for(int i=0;i<100;++i)
		the_surf1.detect(ffps1);
	boost::posix_time::ptime finish2=boost::posix_time::microsec_clock::local_time();
	std::cout << "detect<pos>: " << float((finish2-start2).total_microseconds())/100.0f << std::endl;

	ucv::surf::fps_by_desc_tree_t ffps2;
	boost::posix_time::ptime start3=boost::posix_time::microsec_clock::local_time();
	for(int i=0;i<100;++i)
	the_surf1.detect(ffps2);
	boost::posix_time::ptime finish3=boost::posix_time::microsec_clock::local_time();
	std::cout << "detect<desc>: " << float((finish3-start3).total_microseconds())/100.0f << std::endl;

	boost::posix_time::ptime start4=boost::posix_time::microsec_clock::local_time();
	for(int i=0;i<100;++i)
		the_surf1.describe(ffps2);
	boost::posix_time::ptime finish4=boost::posix_time::microsec_clock::local_time();
	std::cout << "describe<desc>: " << float((finish4-start4).total_microseconds())/100.0f << std::endl;

	std::cout << "detect<vec>.size()=" << fps1.size() << " detect<pos>.size()=" << ffps1.size() << " detect<desc>.size()=" << ffps2.size() <<std::endl;
	
	std::pair<ucv::surf::fps_by_desc_tree_t::iterator, ucv::surf::fps_by_desc_tree_t::distance_type> res=ffps2.find_nearest(*ffps2.begin());
	std::list< std::pair<ucv::surf::fps_by_desc_tree_t::iterator, ucv::surf::fps_by_desc_tree_t::distance_type> > res2;
	ffps2.find_k_nearest(*ffps2.begin(),res2, 3);

	the_surf1.describe(fps1);

	ucv::surf the_surf2(math::size2ui(gray_img2.width(), gray_img2.height()), 3, 4, 2, 4.0e-4f);

	the_surf2.set_integral_view(ucv::gil::const_view(int_img2));
	the_surf2.build_response_layers();
	std::vector<ucv::surf::feature_point_t> fps2;
	the_surf2.detect(fps2);
	the_surf2.describe(fps2);


	boost::posix_time::ptime match_vec_start=boost::posix_time::microsec_clock::local_time();
	for(std::size_t i=0;i<10;++i)
	{
		std::vector< std::pair< std::vector< ucv::surf::feature_point_t >::const_iterator, std::vector< ucv::surf::feature_point_t >::const_iterator > > matches_vec_vec;
		ucv::match_feature_points<ucv::surf::feature_point_t, std::vector<ucv::surf::feature_point_t>, std::vector< ucv::surf::feature_point_t> >(fps1, fps2, matches_vec_vec);
	}
	boost::posix_time::ptime match_vec_finish=boost::posix_time::microsec_clock::local_time();
	std::cout << "matches_vec_vec: " << float((match_vec_finish-match_vec_start).total_microseconds())/10.0f << std::endl;

	ffps2.optimise();
	boost::posix_time::ptime match_kdtree_start=boost::posix_time::microsec_clock::local_time();
	for(std::size_t i=0;i<10;++i)
	{
		std::vector< std::pair< std::vector< ucv::surf::feature_point_t >::const_iterator, ucv::surf::fps_by_desc_tree_t::const_iterator > > matches_vec_tree;
		ucv::match_feature_points(fps2, ffps2, matches_vec_tree);
	}
	boost::posix_time::ptime match_kdtree_finish=boost::posix_time::microsec_clock::local_time();
	std::cout << "matches_vec_tree: " << float((match_kdtree_finish-match_kdtree_start).total_microseconds())/10.0f << std::endl;



	std::vector< std::pair< std::vector< ucv::surf::feature_point_t >::const_iterator, ucv::surf::fps_by_desc_tree_t::const_iterator > > matches;
	ucv::match_feature_points(fps2, ffps2, matches);

	
	std::cout << "found " << matches.size() << " matches!" << std::endl;

	cv::Mat cv_img1_rgb=cv::imread("test_img2.png");
	cv::Mat cv_img2_rgb=cv::imread("test_img2_match.png");
	
	std::vector<cv::Point2f> pts1,pts2;

	for(std::size_t ifp=0;ifp<matches.size();++ifp)
	{
		pts1.push_back(cv::Point2f(matches[ifp].second->x(),matches[ifp].second->y()));
		pts2.push_back(cv::Point2f(matches[ifp].first->x(),matches[ifp].first->y()));

		cv::circle(cv_img1_rgb,
			cv::Point(static_cast<boost::int32_t>(matches[ifp].second->x()), static_cast<boost::int32_t>(matches[ifp].second->y())),
			3,
			cv::Scalar(0.0, 255.0, 0.0),
			-1
		);

		cv::circle(cv_img2_rgb,
			cv::Point(static_cast<boost::int32_t>(matches[ifp].first->x()), static_cast<boost::int32_t>(matches[ifp].first->y())),
			3,
			cv::Scalar(0.0, 255.0, 0.0),
			-1
		);
	}

	math::matrix33f hm;

	ucv::find_homography_ransac(matches,hm,true);

	float marker_corners[4][3]=
	{
		{
			0.0f,
			0.0f,
			1.0f
		},
		{
			static_cast<float>(gil_gray_img1.width()),
			0.0f,
			1.0f
		},
		{
			static_cast<float>(gil_gray_img1.width()),
			static_cast<float>(gil_gray_img1.height()),
			1.0f
		},
		{
			0.0f,
			static_cast<float>(gil_gray_img1.height()),
			1.0f
		}
	};

	math::vector3f image_corners[4]=
	{
		(hm*math::vector3f(marker_corners[0])).homogenized(),
		(hm*math::vector3f(marker_corners[1])).homogenized(),
		(hm*math::vector3f(marker_corners[2])).homogenized(),
		(hm*math::vector3f(marker_corners[3])).homogenized()
	};

	for(int p=0;p<4;++p)
	{
		math::vector3f const &p0=image_corners[p];
		math::vector3f const &p1=image_corners[(p+1)%4];

		cv::Point cvp0(
			boost::math::round<int>(p0(0)),
			boost::math::round<int>(p0(1))
		);

		cv::Point cvp1(
			boost::math::round<int>(p1(0)),
			boost::math::round<int>(p1(1))
		);

		cv::line(cv_img2_rgb, cvp0, cvp1, cv::Scalar(0.0, 255.0, 0.0));
	}

	
	cv::namedWindow(OPENCV_WND_NAME2);
	cv::imshow(OPENCV_WND_NAME2, cv_img1_rgb);
	cv::imshow(OPENCV_WND_NAME, cv_img2_rgb);
	
	cv::waitKey();
}

BOOST_AUTO_TEST_CASE( test_klt_tracker )
{
	namespace ucv=baldzarika::ucv;
	namespace math=baldzarika::math;

	typedef math::fixed_point<10, 21> gray_t;
	typedef gil::pixel<gray_t, ucv::gil::gray_layout_t> gray_pixel_t;
	typedef gil::image< gray_pixel_t, false, std::allocator<unsigned char> > gray_image_t;
	typedef gray_image_t::view_t gray_view_t;


	ucv::gil::gray8_image_t marker_img;
	ucv::gil::png_read_and_convert_image("test_img2.png", marker_img);

	ucv::gil::gray8_image_t shifted_marker_img(marker_img.width(),marker_img.height());
	
	{
		ucv::gil::matrix3x2<float> shift_mat=ucv::gil::matrix3x2<float>::get_translate(ucv::gil::point2<float>(5.0f,-2.0f));
		ucv::gil::resample_pixels(
			ucv::gil::const_view(marker_img),
			ucv::gil::view(shifted_marker_img),
			shift_mat,
			ucv::gil::nearest_neighbor_sampler()
		);
	}


	gray_image_t gray_img(marker_img.width(), marker_img.height()), shifted_gray_img(marker_img.width(), marker_img.height());
	ucv::convert(ucv::gil::view(marker_img), ucv::gil::view(gray_img), ucv::detail::grayscale_convert());
	ucv::convert(ucv::gil::view(shifted_marker_img), ucv::gil::view(shifted_gray_img), ucv::detail::grayscale_convert());
	
	ucv::surf::integral_image_t integral_img(marker_img.width(), marker_img.height()), shifted_integral_img(marker_img.width(), marker_img.height());
	ucv::integral(ucv::gil::view(gray_img), ucv::gil::view(integral_img));
	ucv::integral(ucv::gil::view(shifted_gray_img), ucv::gil::view(shifted_integral_img));

	typedef ucv::good_features_detector<ucv::surf::integral_t::IS,ucv::surf::integral_t::FS> good_features_detector_t;

	good_features_detector_t gfd(
		math::size2ui(marker_img.width(), marker_img.height())
	);


	for(int fd=0;fd<9;++fd)
	{
		boost::posix_time::ptime detect_start,detect_finish;
		std::string detector_name;
		std::vector<ucv::surf::feature_point_t::point2_t> prev_pts, next_pts;
		if(fd<7)
		{
			boost::scoped_ptr<cv::FeatureDetector> feature_detector;

			switch(fd)
			{
			case 0: feature_detector.reset( new cv::FastFeatureDetector() ); detector_name="FAST"; break;
			case 1: feature_detector.reset( new cv::GoodFeaturesToTrackDetector() ); detector_name="GFTT"; break;
			case 2: feature_detector.reset( new cv::MserFeatureDetector() ); detector_name="MSER"; break;
			case 3: feature_detector.reset( new cv::StarFeatureDetector() ); detector_name="STAR"; break;
			case 4: feature_detector.reset( new cv::SiftFeatureDetector() ); detector_name="SIFT"; break;
			case 5: feature_detector.reset( new cv::SurfFeatureDetector() ); detector_name="SURF"; break;
			case 6: feature_detector.reset( new cv::DenseFeatureDetector() ); detector_name="DENSE"; break;
			}

			std::vector<cv::KeyPoint> key_points;
			detect_start=boost::posix_time::microsec_clock::local_time();
			for(int i=0;i<10;++i)
			{
				feature_detector->detect(
					cv::Mat(
						marker_img.height(),
						marker_img.width(),
						CV_8UC1,
						ucv::gil::view(marker_img).row_begin(0)
					),
					key_points
				);
			}
			detect_finish=boost::posix_time::microsec_clock::local_time();
				

			for(std::size_t ifp=0;ifp<key_points.size();++ifp)
				prev_pts.push_back(
					ucv::surf::feature_point_t::point2_t(
						key_points[ifp].pt.x,
						key_points[ifp].pt.y
					)
				);
		}
		else
		if(fd==7)
		{
			
			detector_name="fpSURF";
			ucv::surf surf_detector(math::size2ui(integral_img.width(), integral_img.height()), 3, 4, 2, 1.0e-4f);
			std::vector<ucv::surf::feature_point_t> key_points;

			detect_start=boost::posix_time::microsec_clock::local_time();
			for(int i=0;i<10;++i)
			{
				surf_detector.set_integral_view(ucv::gil::const_view(integral_img));
				surf_detector.build_response_layers();
			
				surf_detector.detect(key_points);
			}
			detect_finish=boost::posix_time::microsec_clock::local_time();

			for(std::size_t ifp=0;ifp<key_points.size();++ifp)
				prev_pts.push_back(
					ucv::surf::feature_point_t::point2_t(
						key_points[ifp].x(),
						key_points[ifp].y()
					)
				);
		}
		else
		if(fd==8)
		{
			detector_name="fpGFTT";
			good_features_detector_t gfd(
				math::size2ui(marker_img.width(), marker_img.height())
			);


			std::vector<ucv::surf::feature_point_t::point2_t> key_points;
			std::vector<cv::Point2f> good_features;
			detect_start=boost::posix_time::microsec_clock::local_time();
			for(int i=0;i<10;++i)
				gfd(ucv::gil::view(integral_img), key_points);
			detect_finish=boost::posix_time::microsec_clock::local_time();
			for(std::size_t ifp=0;ifp<key_points.size();++ifp)
				prev_pts.push_back(
					ucv::surf::feature_point_t::point2_t(
						key_points[ifp].x(),
						key_points[ifp].y()
					)
				);
		}

		typedef ucv::klt_tracker<ucv::surf::integral_t::IS,ucv::surf::integral_t::FS> klt_tracker_t;
		klt_tracker_t feature_point_tracker(math::size2ui(integral_img.width(), integral_img.height()), math::size2ui(7,7), 4, 100);
		feature_point_tracker.set_integral_views(
			ucv::gil::const_view(integral_img),
			ucv::gil::const_view(shifted_integral_img)
		);

		std::vector<bool> status;
		boost::posix_time::ptime klt_track_start=boost::posix_time::microsec_clock::local_time();
		for(int i=0;i<10;++i)
			feature_point_tracker(prev_pts, next_pts, status);
		boost::posix_time::ptime klt_track_finish=boost::posix_time::microsec_clock::local_time();

		float er=0.0f;
		for(std::size_t ifp=0;ifp<next_pts.size();++ifp)
		{
			float dx=std::abs(static_cast<float>(next_pts[ifp].x()-prev_pts[ifp].x())+5.0f);
			float dy=std::abs(static_cast<float>(next_pts[ifp].y()-prev_pts[ifp].y())-2.0f);

			er+=dx*dx+dy*dy;
		}

		std::cout << detector_name<< ": " << "n=" << prev_pts.size() <<
			" dt=" <<float((detect_finish-detect_start).total_microseconds())/(10.0f) <<
			" (" << float((detect_finish-detect_start).total_microseconds())/(10.0f*prev_pts.size()) << ")" <<
			" tt=" << float((klt_track_finish-klt_track_start).total_microseconds())/(10.0f) <<
			" (" << float((klt_track_finish-klt_track_start).total_microseconds())/(10.0f*prev_pts.size()) << ")" <<
			" e=" << er/float(prev_pts.size()) << std::endl;
	}
}


BOOST_AUTO_TEST_CASE( perspective_transform_test )
{
	namespace ucv=baldzarika::ucv;
	namespace math=baldzarika::math;
	typedef math::point2<float> point_t;
	typedef math::vector<float,3> vector_t;
	typedef math::matrix<float,3,3> matrix_t;

	std::vector<point_t> src(4),dst(4);

	dst[0]=point_t( 219.0f, 316.0f ); src[0]=point_t(   0.0f,   0.0f );
	dst[1]=point_t( 317.0f, 313.0f ); src[1]=point_t(  50.0f,   0.0f );
	dst[2]=point_t( 325.0f, 396.0f ); src[2]=point_t(  50.0f,  50.0f );
	dst[3]=point_t( 213.0f, 400.0f ); src[3]=point_t(   0.0f,  50.0f );


	matrix_t pm;
	BOOST_CHECK(ucv::perspective_transform(src,dst,pm));

	matrix_t identity=pm*pm.inverted();

	BOOST_CHECK_LT( std::abs((pm*vector_t(src[0]))[0]-dst[0][0]), 5.0e-2f);
	BOOST_CHECK_LT( std::abs((pm*vector_t(src[0]))[1]-dst[0][1]), 5.0e-2f);

}

BOOST_AUTO_TEST_CASE( warp_test )
{
	namespace ucv=baldzarika::ucv;
	namespace math=baldzarika::math;
	

	typedef math::fixed_point<10,21> gray_t;
	typedef ucv::gil::pixel< gray_t, ucv::gil::gray_layout_t > gray_pixel_t;
	typedef ucv::gil::image< gray_pixel_t, false, std::allocator< unsigned char > > gray_image_t;
	typedef gray_image_t::view_t gray_view_t;


	ucv::gil::gray8_image_t src8_img;
	ucv::gil::png_read_and_convert_image("fiducial_test.png", src8_img);

	gray_image_t src_img(src8_img.width(), src8_img.height());

	ucv::convert(
		ucv::gil::const_view(src8_img),
		ucv::gil::view(src_img),
		ucv::detail::grayscale_convert()
	);

	typedef math::point2<float> point_t;
	typedef math::vector<float,3> vector_t;
	typedef math::matrix<float,3,3> matrix_t;

	std::vector<point_t> src(4),dst(4);

	dst[0]=point_t( 219.0f, 316.0f ); src[0]=point_t(   0.0f,   0.0f );
	dst[1]=point_t( 317.0f, 313.0f ); src[1]=point_t(  50.0f,   0.0f );
	dst[2]=point_t( 325.0f, 396.0f ); src[2]=point_t(  50.0f,  50.0f );
	dst[3]=point_t( 213.0f, 400.0f ); src[3]=point_t(   0.0f,  50.0f );


	matrix_t pm;
	BOOST_CHECK(ucv::perspective_transform(src,dst,pm));

	for(boost::uint32_t p=0;p<4;p++)
	{
		point_t delta=src[p].transformed(pm)-dst[p];
		BOOST_CHECK_LT(delta.length(), 1.0e-3f);
	}

	gray_image_t dst_img(50, 50);

	BOOST_CHECK(ucv::warp(ucv::gil::const_view(src_img), ucv::gil::view(dst_img), pm, true));

	ucv::gil::gray8_image_t dst8_img(50, 50);

	BOOST_CHECK(ucv::convert(
		ucv::gil::const_view(dst_img),
		ucv::gil::view(dst8_img),
		ucv::detail::grayscale_convert()
	));

	ucv::gil::png_write_view("warped-fiducial_test.png", ucv::gil::const_view(dst8_img));
}


BOOST_AUTO_TEST_CASE( camera_pose_test )
{
	namespace ucv=baldzarika::ucv;
	namespace math=baldzarika::math;

	typedef math::point2<float> point2_t;
	
	math::size2ui const fs=math::size2ui(100,100);
	math::size2ui const ms=math::size2ui(50,50);
	
	std::vector<point2_t> src(4),dst(4);

	src[0]=point2_t( 0.0f,			0.0f );			dst[0]=point2_t( 0.0f,			0.0f );			
	src[1]=point2_t( ms.width(),	0.0f );			dst[1]=point2_t( fs.width(),	0.0f );			
	src[2]=point2_t( ms.width(),	ms.height() );	dst[2]=point2_t( fs.width(),	fs.height() );
	src[3]=point2_t( 0.0f,			ms.height() );	dst[3]=point2_t( 0.0f,			fs.height() );

	math::matrix33f hm;

	ucv::perspective_transform(src,dst,hm);

	float const fovy=45.0f;
	float ty=std::tan(fovy*0.5f/360.0f*math::constant::pi<float>());
	float fl=fs.height()/(2.0f*ty);
	
	math::matrix44f cam_matrix;
	ucv::camera_pose(fl, fl, point2_t(fs.width()/2,fs.height()/2), hm, cam_matrix);

	math::matrix44f cam_adjust=math::matrix44f::identity();

	cam_adjust(0,0)=ms.width()/2.0f;
	cam_adjust(0,3)=ms.width()/2.0f;

	cam_adjust(1,1)=-(ms.height()/2.0f);
	cam_adjust(1,3)=ms.height()/2.0f;

	cam_matrix*=cam_adjust;
	
	float corners[4][4]=
	{
		{ -1.0f,  1.0f, 0.0f, 1.0f },
		{  1.0f,  1.0f,	0.0f, 1.0f },
		{  1.0f, -1.0f,	0.0f, 1.0f },
		{ -1.0f, -1.0f,	0.0f, 1.0f }
	};
	
	math::vector4f top_left=cam_matrix*math::vector4f(corners[0]);
	math::vector4f top_right=cam_matrix*math::vector4f(corners[1]);
	math::vector4f bottom_right=cam_matrix*math::vector4f(corners[2]);
	math::vector4f bottom_left=cam_matrix*math::vector4f(corners[3]);

	math::matrix44f cam_intr=math::matrix44f::identity();
	cam_intr(0,0)=fl;
	cam_intr(0,2)=fs.width()/2;
	cam_intr(1,1)=fl;
	cam_intr(1,2)=fs.height()/2;

	top_left=cam_intr*top_left; top_left/=top_left[2];
	top_right=cam_intr*top_right; top_right/=top_right[2];
	bottom_right=cam_intr*bottom_right; bottom_right/=bottom_right[2];
	bottom_left=cam_intr*bottom_left; bottom_left/=bottom_left[2];
}

BOOST_AUTO_TEST_CASE( canny_test )
{
	namespace ucv=baldzarika::ucv;
	namespace math=baldzarika::math;

	typedef math::real_t real_t;
	typedef ucv::sobel<real_t, 3, 1> sobel_t;
	typedef ucv::gaussian_blur<real_t, 3> gaussian_blur_t;
	typedef ucv::adaptive_treshold<real_t, 5, true > adaptive_treshold_t;


	unsigned int i1=1;
	unsigned int i2=4;
	
	unsigned int c=(i1-i2)%5;


	

	ucv::gil::gray8_image_t gray8_img;
	ucv::gil::png_read_and_convert_image("fiducial_test.png", gray8_img);


	gaussian_blur_t::gray_image_t raw_img(gray8_img.width(), gray8_img.height());
	
	ucv::convert(
		ucv::gil::const_view(gray8_img),
		ucv::gil::view(raw_img),
		ucv::detail::grayscale_convert()
	);


	gaussian_blur_t::gray_image_t gray_img(gray8_img.width(), gray8_img.height());

	gaussian_blur_t gaussian_blur(math::size2ui(gray8_img.width(), gray8_img.height()));
	gaussian_blur(ucv::gil::const_view(raw_img), ucv::gil::view(gray_img));

	sobel_t sobel(math::size2ui(gray8_img.width(), gray8_img.height()));

	sobel_t::gray_image_t dx_img(gray8_img.width(), gray8_img.height());
	sobel_t::gray_image_t dy_img(gray8_img.width(), gray8_img.height());

	sobel(ucv::gil::const_view(gray_img), ucv::gil::view(dx_img), ucv::gil::view(dy_img));

	cv::imshow(OPENCV_WND_NAME, cv::Mat(gray8_img.height(), gray8_img.width(), CV_8UC1, &gil::view(gray8_img)[0][0]));

	cv::waitKey();


	ucv::gil::gray32f_image_t gray32f_img(dx_img.width(), dx_img.height());


	gaussian_blur_t::gray_image_t treshold_img(gray8_img.width(), gray8_img.height());
	adaptive_treshold_t adaptive_treshold(math::size2ui(gray8_img.width(), gray8_img.height()), math::constant::one<real_t>(), real_t(0.01));
	adaptive_treshold(ucv::gil::const_view(raw_img), ucv::gil::view(treshold_img));

	ucv::convert(
		ucv::gil::const_view(treshold_img),
		ucv::gil::view(gray32f_img),
		ucv::detail::convert()
	);

	cv::imshow(OPENCV_WND_NAME, cv::Mat(dx_img.height(), dx_img.width(), CV_32FC1, &gil::view(gray32f_img)[0][0]));
	cv::waitKey();



	ucv::convert(
		ucv::gil::const_view(gray_img),
		ucv::gil::view(gray32f_img),
		ucv::detail::convert()
	);

	cv::imshow(OPENCV_WND_NAME, cv::Mat(dx_img.height(), dx_img.width(), CV_32FC1, &gil::view(gray32f_img)[0][0]));
	cv::waitKey();

	cv::Mat image=cv::imread("fiducial_test.png", CV_LOAD_IMAGE_GRAYSCALE);


	cv::blur(image,image,cv::Size(3,3));

	cv::imshow(OPENCV_WND_NAME, image);
	cv::waitKey();

	cv::Mat image_dx, image_dy;
	cv::Sobel(image,image_dx,CV_8UC1,1,0);
	cv::Sobel(image,image_dy,CV_8UC1,0,1);
	
	
	ucv::convert(
		ucv::gil::const_view(dx_img),
		ucv::gil::view(gray32f_img),
		ucv::detail::convert()
	);
	cv::imshow(OPENCV_WND_NAME, cv::Mat(dx_img.height(), dx_img.width(), CV_32FC1, &gil::view(gray32f_img)[0][0]));
	cv::waitKey();

	//cv::imshow(OPENCV_WND_NAME, image_dx);
	//cv::waitKey();

	ucv::convert(
		ucv::gil::const_view(dy_img),
		ucv::gil::view(gray32f_img),
		ucv::detail::convert()
	);
	cv::imshow(OPENCV_WND_NAME, cv::Mat(dy_img.height(), dy_img.width(), CV_32FC1, &gil::view(gray32f_img)[0][0]));
	cv::waitKey();
	//cv::imshow(OPENCV_WND_NAME, image_dy);
	//cv::waitKey();


	typedef ucv::canny<real_t, 3> canny_t;

	{
		typedef ucv::contour< real_t > contour_t;
		canny_t canny(math::size2ui(gray8_img.width(), gray8_img.height()), 0.4, 1.2);
		std::list< contour_t > contours;

		typedef math::matrix<math::real_t, 3, 3> matrix_t;
		//typedef ucv::point2<ucv::decimal_t> point_t;

		typedef math::point2f point_t;
		
		std::vector<point_t> src(4),dst(4);

		src[0]=point_t(   0.0f,   0.0f );
		src[1]=point_t(  50.0f,   0.0f );
		src[2]=point_t(  50.0f,  50.0f );
		src[3]=point_t(   0.0f,  50.0f );

		gaussian_blur_t::gray_image_t warped_img(50,50);
		ucv::gil::gray8_image_t warped8_img(50,50);

		canny(ucv::gil::const_view(gray_img), contours);
		
		for(std::list<contour_t>::iterator ic=contours.begin();ic!=contours.end();++ic)
		{
			cv::Mat contour_img(dy_img.height(),dy_img.width(),CV_8UC3);
			image.convertTo(contour_img, CV_8UC3);

			contour_t &contour=*ic;
			contour.aproximate(3.0);

			if(!(contour.m_is_closed && contour.m_points.size()==4 && contour.m_is_convex))
				continue;

			dst[0]=point_t(contour.m_points[contour.m_is_clockwise?0:3].x(), contour.m_points[contour.m_is_clockwise?0:3].y());
			dst[1]=point_t(contour.m_points[contour.m_is_clockwise?1:2].x(), contour.m_points[contour.m_is_clockwise?1:2].y());
			dst[2]=point_t(contour.m_points[contour.m_is_clockwise?2:1].x(), contour.m_points[contour.m_is_clockwise?2:1].y());
			dst[3]=point_t(contour.m_points[contour.m_is_clockwise?3:0].x(), contour.m_points[contour.m_is_clockwise?3:0].y());

			math::matrix33f pmf;

			if(!ucv::perspective_transform(src,dst,pmf))
				continue;
			
			matrix_t pm=pmf;

			if(!ucv::warp(ucv::gil::const_view(gray_img), ucv::gil::view(warped_img), pm, true))
				continue;

			boost::array<gaussian_blur_t::gray_t, 20> histogram;
			ucv::ranged_histogram(ucv::gil::const_view(warped_img), histogram, 0, 1);


			gaussian_blur_t::gray_t otsu_treshold_val=ucv::find_otsu_threshold<gaussian_blur_t::gray_const_view_t,20>(ucv::gil::const_view(warped_img), 0, 1)*gaussian_blur_t::gray_t(1.2);

			
			ucv::threshold(
				ucv::gil::const_view(warped_img),
				ucv::gil::view(warped_img),
				ucv::detail::normal_binary_threshold<gaussian_blur_t::gray_t,gaussian_blur_t::gray_t>(otsu_treshold_val, 1.0)
			);

			boost::uint32_t pc=ucv::pixel_count(ucv::gil::const_view(warped_img), ucv::detail::is_non_zero());



			ucv::convert(
				ucv::gil::const_view(warped_img),
				ucv::gil::view(warped8_img),
				ucv::detail::grayscale_convert()
			);
						

			boost::scoped_array<cv::Point> cpts(new cv::Point[contour.m_points.size()]);
			for(boost::uint32_t p=0;p<contour.m_points.size();++p)
				cpts.get()[p]=cv::Point(contour.m_points[p].x(),contour.m_points[p].y());

			int const npts=contour.m_points.size();
			cv::Point const *ppts=cpts.get();

			cv::polylines(
				contour_img,
				&ppts,
				&npts,
				1,
				contour.m_is_closed,
				cv::Scalar(255, 0, 0)
			);

			cv::imshow(OPENCV_WND_NAME, contour_img);
			cv::imshow(OPENCV_WND_NAME2, cv::Mat(50, 50, CV_8UC1, &ucv::gil::view(warped8_img)[0][0]));
			cv::waitKey();

		}
		
	}


	real_t th=0.0;
	for(int i=0;i<10;++i)
	{
		th+=real_t(0.1);
		real_t th_hi=th*real_t(3.0);

		canny_t canny(math::size2ui(gray8_img.width(), gray8_img.height()), th, th_hi);

		gaussian_blur_t::gray_image_t edge_img(gray8_img.width(), gray8_img.height());
	
		canny(ucv::gil::const_view(gray_img), ucv::gil::view(edge_img));

		ucv::convert(
			ucv::gil::const_view(edge_img),
			ucv::gil::view(gray32f_img),
			ucv::detail::convert()
		);

		cv::imshow(OPENCV_WND_NAME, cv::Mat(edge_img.height(), edge_img.width(), CV_32FC1, &gil::view(gray32f_img)[0][0]));
		cv::waitKey();
	}

	cv::Canny(image, image, 30, 90);
	cv::imshow(OPENCV_WND_NAME, image);
	cv::waitKey();
	
}
