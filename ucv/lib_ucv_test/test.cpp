#define BOOST_TEST_MODULE lib_ucv_test

#include <boost/test/unit_test.hpp>
//#define BALDZARIKA_UCV_FIXED_POINT_TRIGONO_USE_FPU
#include <baldzarika/ucv/config.h>
#include <baldzarika/ucv/fixed_point.h>
#include <baldzarika/ucv/convert_scale.h>
#include <baldzarika/ucv/integral.h>
#include <baldzarika/ucv/surf.h>
#include <baldzarika/ucv/homography.h>
#include <boost/date_time.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>

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


#if 1
BOOST_AUTO_TEST_CASE( test_surf_match )
{
	namespace ucv=baldzarika::ucv;

	//img 1
	cv::Mat cv_img1=cv::imread("test_img.png", CV_LOAD_IMAGE_GRAYSCALE);
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

	ucv::surf::gray_image_t gray_img1(gil_gray_img1.width(), gil_gray_img1.height(), 4);
	ucv::convert_scale(
		ucv::gil::view(gil_gray_img1),
		ucv::gil::view(gray_img1),
		ucv::surf::integral_t(1.0f/255.0f)
	);


	//img2
	cv::Mat cv_img2=cv::imread("test_img_match.png", CV_LOAD_IMAGE_GRAYSCALE);
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

	cv::Mat cv_gray_img2(gil_gray_img2.height(), gil_gray_img2.width(), CV_8UC1, &ucv::gil::view(gil_gray_img2)[0][0]);
	//cv::imshow(OPENCV_WND_NAME, cv_gray_img2);
	//cv::waitKey();

	ucv::surf::gray_image_t gray_img2(gil_gray_img2.width(), gil_gray_img2.height(), 4);
	ucv::convert_scale(
		ucv::gil::view(gil_gray_img2),
		ucv::gil::view(gray_img2),
		ucv::surf::integral_t(1.0f/255.0f)
	);

	ucv::surf the_surf1(ucv::size2ui(gray_img1.width(), gray_img1.height()), 3, 4, 2, 1.0e-4f);

	the_surf1.update(ucv::gil::view(gray_img1));
	std::vector<ucv::surf::feature_point_t> fps1;
	the_surf1.detect(fps1);
	the_surf1.describe(fps1);

	ucv::surf the_surf2(ucv::size2ui(gray_img2.width(), gray_img2.height()), 3, 4, 2, 1.0e-4f);

	the_surf2.update(ucv::gil::view(gray_img2));
	std::vector<ucv::surf::feature_point_t> fps2;
	the_surf2.detect(fps2);
	the_surf2.describe(fps2);



	std::vector< std::pair<std::size_t, std::size_t> > matches;
	ucv::surf::match_feature_points(fps1, fps2, matches, 0.65f);

	cv::Mat cv_img1_rgb=cv::imread("test_img.png");
	cv::Mat cv_img2_rgb=cv::imread("test_img_match.png");
	


	std::vector<cv::Point2f> pts1,pts2;

	for(std::size_t ifp=0;ifp<matches.size();++ifp)
	{
		pts1.push_back(cv::Point2f(fps1[matches[ifp].first].x,fps1[matches[ifp].first].y));
		pts2.push_back(cv::Point2f(fps2[matches[ifp].second].x,fps2[matches[ifp].second].y));

		cv::circle(cv_img1_rgb,
			cv::Point(static_cast<float>(fps1[matches[ifp].first].x), static_cast<float>(fps1[matches[ifp].first].y)),
			3,
			cv::Scalar(0.0, 255.0, 0.0),
			-1
		);

		cv::circle(cv_img2_rgb,
			cv::Point(static_cast<float>(fps2[matches[ifp].second].x), static_cast<float>(fps2[matches[ifp].second].y)),
			3,
			cv::Scalar(0.0, 255.0, 0.0),
			-1
		);
	}

	ucv::homography::matrix_t hm(3,3);

	ucv::find_homography_ransac(fps1, fps2, matches, hm);
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

	ucv::homography::vector_t marker_corners[4];

	ucv::homography::vector_t image_corners[4];

	marker_corners[0].resize(3);
	marker_corners[0](0)=0; marker_corners[0](1)=0;marker_corners[0](2)=1;
	image_corners[0]=ucv::ublas::prod(hm, marker_corners[0]);
	image_corners[0](0)/=image_corners[0](2); image_corners[0](1)/=image_corners[0](2);


	marker_corners[1].resize(3);
	marker_corners[1](0)=gil_gray_img1.width(); marker_corners[1](1)=0;marker_corners[1](2)=1;
	image_corners[1]=ucv::ublas::prod(hm, marker_corners[1]);
	image_corners[1](0)/=image_corners[1](2); image_corners[1](1)/=image_corners[1](2);

	marker_corners[2].resize(3);
	marker_corners[2](0)=gil_gray_img1.width(); marker_corners[2](1)=gil_gray_img1.height();marker_corners[2](2)=1;
	image_corners[2]=ucv::ublas::prod(hm, marker_corners[2]);
	image_corners[2](0)/=image_corners[2](2); image_corners[2](1)/=image_corners[2](2);

	marker_corners[3].resize(3);
	marker_corners[3](0)=0; marker_corners[3](1)=gil_gray_img1.height();marker_corners[3](2)=1;
	image_corners[3]=ucv::ublas::prod(hm, marker_corners[3]);
	image_corners[3](0)/=image_corners[3](2); image_corners[3](1)/=image_corners[3](2);

	for(int p=0;p<4;++p)
	{
		ucv::homography::vector_t const &p0=image_corners[p];
		ucv::homography::vector_t const &p1=image_corners[(p+1)%4];

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