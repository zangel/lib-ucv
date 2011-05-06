#define BOOST_TEST_MODULE lib_ucv_test

#include <boost/test/unit_test.hpp>
#include <baldzarika/ucv/config.h>
#include <baldzarika/ucv/fixed_point.h>
#include <baldzarika/ucv/matrix.h>
#include <baldzarika/ucv/vector.h>
#include <baldzarika/ucv/convert_scale.h>
#include <baldzarika/ucv/integral.h>
#include <baldzarika/ucv/surf.h>
#include <baldzarika/ucv/homography.h>
#include <baldzarika/ucv/match_feature_points.h>
#include <baldzarika/ucv/klt_tracker.h>
#include <baldzarika/ucv/good_features_detector.h>
#include <baldzarika/ucv/sobel.h>
#include <baldzarika/ucv/gaussian_blur.h>
#include <boost/date_time.hpp>
#define png_infopp_NULL (png_infopp)0
#define int_p_NULL (int*)0
#include <boost/gil/extension/io/png_io.hpp>

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
#if 0

BOOST_AUTO_TEST_CASE( test_fixed_point )
{
	using namespace baldzarika::ucv;

	typedef fixed_point<15,16> fixed_t;


	BOOST_CHECK_EQUAL(static_cast<float>(fixed_t(fixed_point<0,15>(0.5f))), 0.5f);
	BOOST_CHECK_EQUAL(static_cast<float>(fixed_point<0,15>(fixed_t(0.5f))), 0.5f);
	
	BOOST_CHECK_EQUAL(1, static_cast<int>(fixed_t(3)/fixed_t(2)) );
	BOOST_CHECK_EQUAL(1.5f, static_cast<float>(fixed_t(3)/fixed_t(2)));

	//sqrt
	BOOST_CHECK_EQUAL(static_cast<int>(sqrt(fixed_t(144))), 12);
	BOOST_CHECK_LT(fabs(static_cast<float>(sqrt(fixed_t(2)))-sqrt(2.0f)), 1.0e-3f);

	BOOST_CHECK_LT(fabs(static_cast<float>(sqrt(fixed_t(0.00001f)))-sqrt(0.00001f)), 1.0e-6f);


	//sin
	BOOST_CHECK_LT( fabs(static_cast<float>(sin(baldzarika::ucv::detail::constants::zero<fixed_t>()))-sin(0.0f)), 1.0e-3f);
	BOOST_CHECK_LT( fabs(static_cast<float>(sin(baldzarika::ucv::detail::constants::pi_i2<fixed_t>()))-sin(3.1415926535897932384626433832795f/2.0f)), 1.0e-3f);

	//cos
	BOOST_CHECK_LT( fabs(static_cast<float>(cos(baldzarika::ucv::detail::constants::zero<fixed_t>()))-cos(0.0f)), 1.0e-3f);
	BOOST_CHECK_LT( fabs(static_cast<float>(cos(baldzarika::ucv::detail::constants::pi_i2<fixed_t>()))-cos(3.1415926535897932384626433832795f/2.0f)), 1.0e-3f);
	
	
	//fabs
	BOOST_CHECK( (fabs( fixed_point<15,16>(1) ) == fixed_point<15,16>(1)) );
	BOOST_CHECK( (fabs( fixed_point<15,16>(0) ) == fixed_point<15,16>(0)) );
	BOOST_CHECK( (fabs( fixed_point<15,16>(-1) ) == fixed_point<15,16>(1)) );

	//floor
	BOOST_CHECK( (floor( fixed_point<15,16>(1.5) ) == fixed_point<15,16>(1.0)) );
	BOOST_CHECK( (floor( fixed_point<15,16>(-1.5) ) == fixed_point<15,16>(-2.0)) );

	//ceil
	BOOST_CHECK( (ceil( fixed_point<15,16>(1.5) ) == fixed_point<15,16>(2.0)) );
	BOOST_CHECK( (ceil( fixed_point<15,16>(-1.5) ) == fixed_point<15,16>(-1.0)) );

	//fmod
	BOOST_CHECK( (fmod( fixed_point<15,16>(2.5), fixed_point<15,16>(2.0) ) == fixed_point<15,16>(0.5)) );
	BOOST_CHECK( (fmod( fixed_point<15,16>(1.5), fixed_point<15,16>(2.0) ) == fixed_point<15,16>(1.5)) );


	//modf
	fixed_point<15,16> ip;
	BOOST_CHECK( (modf( fixed_point<15,16>(10.5), &ip ) == fixed_point<15,16>(0.5)) );
	BOOST_CHECK( (ip == fixed_point<15,16>(10.0)) );

	//atan2
	float y=sin(3.14f/2.0f);
	float x=cos(3.14f/2.0f);
	
	BOOST_CHECK_LT( fabs(static_cast<float>(atan2(fixed_point<15,16>(y),fixed_point<15,16>(x)))-atan2(y, x)), 7.2e-2f);
	BOOST_CHECK_LT( fabs(static_cast<float>(atan2(fixed_point<15,16>(y),fixed_point<15,16>(-x)))-atan2(y, -x)), 7.2e-2f);
	BOOST_CHECK_LT( fabs(static_cast<float>(atan2(fixed_point<15,16>(-y),fixed_point<15,16>(x)))-atan2(-y, x)), 7.2e-2f);
	BOOST_CHECK_LT( fabs(static_cast<float>(atan2(fixed_point<15,16>(-y),fixed_point<15,16>(-x)))-atan2(-y, -x)), 7.2e-2f);
}


BOOST_AUTO_TEST_CASE( test_fixed_point_atan2_speed )
{
	using namespace baldzarika::ucv;

	float f_res;
	fixed_point<15,16> fp_res;
	float y=sin(2.5f);
	float x=cos(2.5f);
	fixed_point<15,16> fp_x(x), fp_y(y);

	posix_time::ptime cp_1=posix_time::microsec_clock::local_time();
	
	for(uint32_t i=0;i<10000000;++i)
	{
		f_res=atan2(y, x);
	}
	
	posix_time::ptime cp_2=posix_time::microsec_clock::local_time();

	for(uint32_t i=0;i<10000000;++i)
	{
		fp_res=atan2(fixed_point<15,16>(fp_y), fixed_point<15,16>(fp_x));
	}
	posix_time::ptime cp_3=posix_time::microsec_clock::local_time();

	std::cout << "fixed_point speed=" << float((cp_3-cp_2).total_microseconds())/float((cp_2-cp_1).total_microseconds()) << " " << f_res << " " << static_cast<float>(fp_res) << std::endl;
}



BOOST_AUTO_TEST_CASE( test_convert_scale )
{
	namespace ucv=baldzarika::ucv;

	typedef ucv::fixed_point<20, 11> real_t;

	typedef ucv::gil::pixel<real_t, ucv::gil::gray_layout_t> gray_real_pixel_t;
	typedef ucv::gil::image< gray_real_pixel_t, false, std::allocator<unsigned char> > gray_real_image_t;
	typedef gray_real_image_t::view_t gray_real_view_t;

	cv::Mat cv_img=cv::imread("test_img.png", CV_LOAD_IMAGE_GRAYSCALE);
	cv::imshow(OPENCV_WND_NAME, cv_img);
	cv::waitKey();
	
	gray_real_image_t gray_img(cv_img.cols, cv_img.rows, 4);
	
	ucv::convert_scale(
		ucv::gil::interleaved_view(
			cv_img.cols, cv_img.rows,
			reinterpret_cast<ucv::gil::gray8_pixel_t*>(cv_img.data),
			cv_img.step
		),
		ucv::gil::view(gray_img),
		real_t(1.0f/255.0f)
	);

	ucv::convert_scale(
		ucv::gil::view(gray_img),
		ucv::gil::interleaved_view(
			cv_img.cols, cv_img.rows,
			reinterpret_cast<ucv::gil::gray8_pixel_t*>(cv_img.data),
			cv_img.step
		),
		real_t(255.0f)
	);
	
	cv::imshow(OPENCV_WND_NAME, cv_img);
	cv::waitKey();
}

BOOST_AUTO_TEST_CASE( test_integral )
{
	namespace ucv=baldzarika::ucv;

	typedef ucv::fixed_point<20, 11> integral_t;

	typedef ucv::gil::pixel<integral_t, ucv::gil::gray_layout_t> integral_pixel_t;
	typedef ucv::gil::image< integral_pixel_t, false, std::allocator<unsigned char> > integral_image_t;
	typedef integral_image_t::view_t integral_view_t;

	cv::Mat cv_img=cv::imread("test_img.png", CV_LOAD_IMAGE_GRAYSCALE);
	cv::imshow(OPENCV_WND_NAME, cv_img);
	cv::waitKey();

	integral_image_t gray_img(cv_img.cols, cv_img.rows, 4);

	ucv::convert_scale(
		ucv::gil::interleaved_view(
			cv_img.cols, cv_img.rows,
			reinterpret_cast<ucv::gil::gray8_pixel_t*>(cv_img.data),
			cv_img.step
		),
		ucv::gil::view(gray_img),
		integral_t(1.0f/255.0f)
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

	ucv::surf::gray_image_t gray_img(gil_gray_img.width(), gil_gray_img.height(), 4);
	ucv::convert_scale(
		ucv::gil::view(gil_gray_img),
		ucv::gil::view(gray_img),
		ucv::surf::integral_t(1.0f/255.0f)
	);

	ucv::surf the_surf(ucv::size2ui(gray_img.width(), gray_img.height()), 3, 4, 2, 4.0e-4f);

	posix_time::ptime start=posix_time::microsec_clock::local_time();
	the_surf.update(ucv::gil::view(gray_img));
	the_surf.build_response_layers();
	std::vector<ucv::surf::feature_point_t> fps;
	the_surf.detect(fps);
	the_surf.describe(fps);
	posix_time::ptime finish=posix_time::microsec_clock::local_time();
	

	std::cout << "surf::(update+detect+describe)=(" << (finish-start).total_microseconds() << ")" << std::endl;
	for(std::size_t ifp=0;ifp<fps.size();++ifp)
	{
		cv::circle(cv_img,
			cv::Point(2.0f*static_cast<float>(fps[ifp].x), 2.0f*static_cast<float>(fps[ifp].y)),
			2,
			cv::Scalar(255.0)
		);
	}
	cv::imshow(OPENCV_WND_NAME, cv_img);
	cv::waitKey();
}
#endif


#if 0
BOOST_AUTO_TEST_CASE( test_surf_match )
{
	namespace ucv=baldzarika::ucv;

	typedef ucv::fixed_point<10, 21> gray_t;
	typedef gil::pixel<gray_t, ucv::gil::gray_layout_t> gray_pixel_t;
	typedef gil::image< gray_pixel_t, false, std::allocator<unsigned char> > gray_image_t;
	typedef gray_image_t::view_t gray_view_t;



	//img 1
	//cv::Mat cv_img1=cv::imread("test_img2.png", CV_LOAD_IMAGE_GRAYSCALE);
	cv::Mat cv_img1=cv::imread("box.png", CV_LOAD_IMAGE_GRAYSCALE);
	//cv::imshow(OPENCV_WND_NAME, cv_img1);
	//cv::waitKey();

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
	//cv::imshow(OPENCV_WND_NAME, cv_gray_img1);
	//cv::waitKey();

	gray_image_t gray_img1(gil_gray_img1.width(), gil_gray_img1.height());
	ucv::convert_scale(
		ucv::gil::view(gil_gray_img1),
		ucv::gil::view(gray_img1),
		ucv::surf::integral_t(1.0f/255.0f)
	);

	ucv::surf::integral_image_t int_img1(gil_gray_img1.width(), gil_gray_img1.height());
	ucv::integral(ucv::gil::view(gray_img1), ucv::gil::view(int_img1));
	
	//img2
	//cv::Mat cv_img2=cv::imread("test_img2_match.png", CV_LOAD_IMAGE_GRAYSCALE);
	cv::Mat cv_img2=cv::imread("box_in_scene.png", CV_LOAD_IMAGE_GRAYSCALE);
	//cv::imshow(OPENCV_WND_NAME, cv_img2);
	//cv::waitKey();

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
	ucv::convert_scale(
		ucv::gil::view(gil_gray_img2),
		ucv::gil::view(gray_img2),
		ucv::surf::integral_t(1.0f/255.0f)
	);

	ucv::surf::integral_image_t int_img2(gil_gray_img2.width(), gil_gray_img2.height());
	ucv::integral(ucv::gil::view(gray_img2), ucv::gil::view(int_img2));
	
	ucv::surf the_surf1(ucv::size2ui(gray_img1.width(), gray_img1.height()), 3, 4, 2, 4.0e-4f);

	the_surf1.set_integral_view(ucv::gil::const_view(int_img1));
	the_surf1.build_response_layers();
	std::vector<ucv::surf::feature_point_t> fps1;
	boost::posix_time::ptime start=boost::posix_time::microsec_clock::local_time();
	//for(int i=0;i<100;++i)
		the_surf1.detect(fps1);
	boost::posix_time::ptime finish=boost::posix_time::microsec_clock::local_time();
	std::cout << "detect<vec>: " << float((finish-start).total_microseconds())/100.0f << std::endl;


	ucv::surf::fps_by_pos_tree_t ffps1;
	boost::posix_time::ptime start2=boost::posix_time::microsec_clock::local_time();
	//for(int i=0;i<100;++i)
		the_surf1.detect(ffps1);
	boost::posix_time::ptime finish2=boost::posix_time::microsec_clock::local_time();
	std::cout << "detect<pos>: " << float((finish2-start2).total_microseconds())/100.0f << std::endl;

	ucv::surf::fps_by_desc_tree_t ffps2;
	boost::posix_time::ptime start3=boost::posix_time::microsec_clock::local_time();
	//for(int i=0;i<100;++i)
	the_surf1.detect(ffps2);
	boost::posix_time::ptime finish3=boost::posix_time::microsec_clock::local_time();
	std::cout << "detect<desc>: " << float((finish3-start3).total_microseconds())/100.0f << std::endl;

	boost::posix_time::ptime start4=boost::posix_time::microsec_clock::local_time();
	//for(int i=0;i<100;++i)
		the_surf1.describe(ffps2);
	boost::posix_time::ptime finish4=boost::posix_time::microsec_clock::local_time();
	std::cout << "describe<desc>: " << float((finish4-start4).total_microseconds())/100.0f << std::endl;


	//std::size_t num_points=ffps.size();
	std::cout << "detect<vec>.size()=" << fps1.size() << " detect<pos>.size()=" << ffps1.size() << " detect<desc>.size()=" << ffps2.size() <<std::endl;

	std::pair<ucv::surf::fps_by_desc_tree_t::iterator, ucv::surf::fps_by_desc_tree_t::distance_type> res=ffps2.find_nearest(*ffps2.begin());
	std::list< std::pair<ucv::surf::fps_by_desc_tree_t::iterator, ucv::surf::fps_by_desc_tree_t::distance_type> > res2;
	ffps2.find_k_nearest(*ffps2.begin(),res2, 3);

	


	the_surf1.describe(fps1);

	ucv::surf the_surf2(ucv::size2ui(gray_img2.width(), gray_img2.height()), 3, 4, 2, 1.0e-4f);

	the_surf2.set_integral_view(ucv::gil::const_view(int_img2));
	the_surf2.build_response_layers();
	std::vector<ucv::surf::feature_point_t> fps2;
	the_surf2.detect(fps2);
	the_surf2.describe(fps2);


	//std::vector< std::pair<std::size_t, std::size_t> > matches;
	//ucv::surf::match_feature_points(fps1, fps2, matches, 0.65f);

	boost::posix_time::ptime match_vec_start=boost::posix_time::microsec_clock::local_time();
	//for(std::size_t i=0;i<10;++i)
	{
		std::vector< std::pair< std::vector< ucv::surf::feature_point_t >::const_iterator, std::vector< ucv::surf::feature_point_t >::const_iterator > > matches_vec_vec;
		ucv::match_feature_points<ucv::surf::feature_point_t, std::vector<ucv::surf::feature_point_t>, std::vector< ucv::surf::feature_point_t> >(fps1, fps2, matches_vec_vec);
	}
	boost::posix_time::ptime match_vec_finish=boost::posix_time::microsec_clock::local_time();
	std::cout << "matches_vec_vec: " << float((match_vec_finish-match_vec_start).total_microseconds())/10.0f << std::endl;

	ffps2.optimise();
	boost::posix_time::ptime match_kdtree_start=boost::posix_time::microsec_clock::local_time();
	//for(std::size_t i=0;i<10;++i)
	{
		std::vector< std::pair< std::vector< ucv::surf::feature_point_t >::const_iterator, ucv::surf::fps_by_desc_tree_t::const_iterator > > matches_vec_tree;
		ucv::match_feature_points(fps2, ffps2, matches_vec_tree);
	}
	boost::posix_time::ptime match_kdtree_finish=boost::posix_time::microsec_clock::local_time();
	std::cout << "matches_vec_tree: " << float((match_kdtree_finish-match_kdtree_start).total_microseconds())/10.0f << std::endl;



	std::vector< std::pair< std::vector< ucv::surf::feature_point_t >::const_iterator, ucv::surf::fps_by_desc_tree_t::const_iterator > > matches;
	ucv::match_feature_points(fps2, ffps2, matches);

	
	std::cout << "found " << matches.size() << " matches!" << std::endl;

	//cv::Mat cv_img1_rgb=cv::imread("test_img2.png");
	//cv::Mat cv_img2_rgb=cv::imread("test_img2_match.png");

	cv::Mat cv_img1_rgb=cv::imread("box.png");
	cv::Mat cv_img2_rgb=cv::imread("box_in_scene.png");
	


	std::vector<cv::Point2f> pts1,pts2;

	for(std::size_t ifp=0;ifp<matches.size();++ifp)
	{
		pts1.push_back(cv::Point2f(matches[ifp].second->x,matches[ifp].second->y));
		pts2.push_back(cv::Point2f(matches[ifp].first->x,matches[ifp].first->y));

		cv::circle(cv_img1_rgb,
			cv::Point(static_cast<boost::int32_t>(matches[ifp].second->x), static_cast<boost::int32_t>(matches[ifp].second->y)),
			3,
			cv::Scalar(0.0, 255.0, 0.0),
			-1
		);

		cv::circle(cv_img2_rgb,
			cv::Point(static_cast<boost::int32_t>(matches[ifp].first->x), static_cast<boost::int32_t>(matches[ifp].first->y)),
			3,
			cv::Scalar(0.0, 255.0, 0.0),
			-1
		);
	}

	ucv::matrix33f hm;

	//ucv::find_homography_ransac(fps1, fps2, matches, hm);

	ucv::find_homography_ransac(matches,hm,true);


	
#if 1
	cv::Mat ocvhm=cv::findHomography(cv::Mat(pts1), cv::Mat(pts2), CV_RANSAC);

	float err=0.0f;

	for(std::size_t r=0;r<2;++r)
	{
		for(std::size_t c=0;c<2;++c)
		{
			float d=float(ocvhm.at<double>(r,c))-static_cast<float>(hm(r,c));
			err+=d*d;
		}
	}
#if 0
	hm(0,0)=ocvhm.at<double>(0,0);
	hm(0,1)=ocvhm.at<double>(0,1);
	hm(0,2)=ocvhm.at<double>(0,2);

	hm(1,0)=ocvhm.at<double>(1,0);
	hm(1,1)=ocvhm.at<double>(1,1);
	hm(1,2)=ocvhm.at<double>(1,2);

	hm(2,0)=ocvhm.at<double>(2,0);
	hm(2,1)=ocvhm.at<double>(2,1);
	hm(2,2)=ocvhm.at<double>(2,2);

#endif

#endif

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

	ucv::vector3f image_corners[4]=
	{
		(hm*ucv::vector3f(marker_corners[0])).homogenized(),
		(hm*ucv::vector3f(marker_corners[1])).homogenized(),
		(hm*ucv::vector3f(marker_corners[2])).homogenized(),
		(hm*ucv::vector3f(marker_corners[3])).homogenized()
	};

	for(int p=0;p<4;++p)
	{
		ucv::vector3f const &p0=image_corners[p];
		ucv::vector3f const &p1=image_corners[(p+1)%4];

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

#endif

#if 0
BOOST_AUTO_TEST_CASE( test_klt_tracker )
{
	namespace ucv=baldzarika::ucv;

	typedef ucv::fixed_point<10, 21> gray_t;
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
	ucv::convert_scale(ucv::gil::view(marker_img), ucv::gil::view(gray_img), 1.0f/255.0f);
	ucv::convert_scale(ucv::gil::view(shifted_marker_img), ucv::gil::view(shifted_gray_img), 1.0f/255.0f);
	
	ucv::surf::integral_image_t integral_img(marker_img.width(), marker_img.height()), shifted_integral_img(marker_img.width(), marker_img.height());
	ucv::integral(ucv::gil::view(gray_img), ucv::gil::view(integral_img));
	ucv::integral(ucv::gil::view(shifted_gray_img), ucv::gil::view(shifted_integral_img));

	typedef ucv::good_features_detector<ucv::surf::integral_t::IS,ucv::surf::integral_t::FS> good_features_detector_t;

	good_features_detector_t gfd(
		ucv::size2ui(marker_img.width(), marker_img.height())
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
			ucv::surf surf_detector(ucv::size2ui(integral_img.width(), integral_img.height()), 3, 4, 2, 1.0e-4f);
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
						key_points[ifp].x,
						key_points[ifp].y
					)
				);
		}
		else
		if(fd==8)
		{
			detector_name="fpGFTT";
			good_features_detector_t gfd(
				ucv::size2ui(marker_img.width(), marker_img.height())
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
						key_points[ifp].x,
						key_points[ifp].y
					)
				);
		}

		typedef ucv::klt_tracker<ucv::surf::integral_t::IS,ucv::surf::integral_t::FS> klt_tracker_t;
		klt_tracker_t feature_point_tracker(ucv::size2ui(integral_img.width(), integral_img.height()), ucv::size2ui(7,7), 4, 100);
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
			float dx=std::abs(static_cast<float>(next_pts[ifp].x-prev_pts[ifp].x)+5.0f);
			float dy=std::abs(static_cast<float>(next_pts[ifp].y-prev_pts[ifp].y)-2.0f);

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



BOOST_AUTO_TEST_CASE( test_matrix )
{
	namespace ucv=baldzarika::ucv;

	ucv::matrix33f h1, h2, h3;
	h1(0,0)=1.0f;

	h2=ucv::matrix33f::identity();
	h3=ucv::matrix33f::identity();
	h1*=h3;
	h1=h2*h3;
	h1.scale(2.0f);

	h1=ucv::matrix<int, 3, 3>::identity().scaled(3.0f);

	ucv::matrix< ucv::fixed_point<15,16>, 3, 3 > m1,m2;
	m1=ucv::matrix< ucv::fixed_point<15,16>, 3, 3 >::identity().scaled(ucv::fixed_point<15,16>(3.0f));
	m1=m1.inverse();

	h1=h1.inverse();
}

BOOST_AUTO_TEST_CASE( test_vector )
{
	namespace ucv=baldzarika::ucv;

	typedef ucv::vector< ucv::fixed_point<15,16>, 2> vector_fp2;
	typedef ucv::vector< ucv::fixed_point<15,16>, 3> vector_fp3;

	ucv::vector3f axes[3]=
	{
		ucv::vector3f::unit<0>(),
		ucv::vector3f::unit<1>(),
		ucv::vector3f::unit<2>()
	};

	float d1=axes[0].dot(axes[1]), d2=axes[1].dot(axes[2]), d3=axes[2].dot(axes[0]);
	float d11=axes[0].dot(axes[0]);
	float d22=axes[1].dot(axes[1]);
	float d33=axes[2].dot(axes[2]);

	vector_fp2 v1=vector_fp2::zero();
	vector_fp2 v2=vector_fp2::unit<0>();
	vector_fp2 v3=vector_fp2::unit<1>();
	vector_fp2 v4=v2*v3;

	ucv::matrix33f m1=ucv::matrix33f::identity(), m2=ucv::matrix33f::identity(), m3=m1*m2;


	vector_fp3 v5;
	vector_fp3 v6=m1*v5;
}

#endif

BOOST_AUTO_TEST_CASE( canny_test )
{
	namespace ucv=baldzarika::ucv;

	typedef ucv::fixed_point<10,21> real_t;
	typedef ucv::sobel<real_t, 3, 1> sobel_t;
	typedef ucv::gaussian_blur<real_t, 3> gaussian_blur_t;
	

	ucv::gil::gray8_image_t gray8_img;
	ucv::gil::png_read_and_convert_image("image-test.png", gray8_img);


	gaussian_blur_t::gray_image_t gray_img(gray8_img.width(), gray8_img.height());
	
	ucv::convert_scale(
		ucv::gil::const_view(gray8_img),
		ucv::gil::view(gray_img),
		real_t(1.0f/255.0f)
	);

	gaussian_blur_t gaussian_blur(ucv::size2ui(gray8_img.width(), gray8_img.height()));
	gaussian_blur(ucv::gil::const_view(gray_img), ucv::gil::view(gray_img));

	sobel_t sobel(ucv::size2ui(gray8_img.width(), gray8_img.height()));

	sobel_t::gray_image_t dx_img(gray8_img.width(), gray8_img.height());
	sobel_t::gray_image_t dy_img(gray8_img.width(), gray8_img.height());

	sobel(ucv::gil::const_view(gray_img), ucv::gil::view(dx_img), ucv::gil::view(dy_img));

	cv::imshow(OPENCV_WND_NAME, cv::Mat(gray8_img.height(), gray8_img.width(), CV_8UC1, &gil::view(gray8_img)[0][0]));

	cv::waitKey();
	
	ucv::gil::gray32f_image_t gray32f_img(dx_img.width(), dx_img.height());

	ucv::convert_scale(
		ucv::gil::const_view(gray_img),
		ucv::gil::view(gray32f_img),
		1.0f
	);

	cv::imshow(OPENCV_WND_NAME, cv::Mat(dx_img.height(), dx_img.width(), CV_32FC1, &gil::view(gray32f_img)[0][0]));
	cv::waitKey();

	
	ucv::convert_scale(
		ucv::gil::const_view(dx_img),
		ucv::gil::view(gray32f_img),
		1.0f
	);

	cv::imshow(OPENCV_WND_NAME, cv::Mat(dx_img.height(), dx_img.width(), CV_32FC1, &gil::view(gray32f_img)[0][0]));
	cv::waitKey();
	ucv::convert_scale(
		ucv::gil::const_view(dy_img),
		ucv::gil::view(gray32f_img),
		1.0f
	);
	cv::imshow(OPENCV_WND_NAME, cv::Mat(dy_img.height(), dy_img.width(), CV_32FC1, &gil::view(gray32f_img)[0][0]));
	cv::waitKey();
}