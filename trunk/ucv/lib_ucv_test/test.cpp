#define BOOST_TEST_MODULE lib_ucv_test

#include <boost/test/unit_test.hpp>
#include <baldzarika/ucv/config.h>
#include <baldzarika/ucv/fixed_point.h>
#include <baldzarika/ucv/convert_scale.h>
#include <baldzarika/ucv/integral.h>
#include <baldzarika/ucv/surf.h>
#include <boost/date_time.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#define OPENCV_WND_NAME  "ucv"

using namespace boost;

BOOST_AUTO_TEST_CASE( create_open_cv_window )
{
	cv::namedWindow(OPENCV_WND_NAME);
}

#if 0
BOOST_AUTO_TEST_CASE( test_fixed_point )
{
	using namespace baldzarika::ucv;

	typedef fixed_point<15,16> fixed_t;
	
	BOOST_CHECK_EQUAL(1, static_cast<int>(fixed_t(3)/fixed_t(2)) );
	BOOST_CHECK_EQUAL(1.5f, static_cast<float>(fixed_t(3)/fixed_t(2)));

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
	
	BOOST_CHECK_CLOSE( static_cast<float>(atan2(fixed_point<15,16>(y),fixed_point<15,16>(x))), atan2(y, x), 7.2e-2f);
	BOOST_CHECK_CLOSE( static_cast<float>(atan2(fixed_point<15,16>(y),fixed_point<15,16>(-x))), atan2(y, -x), 7.2e-2f);
	BOOST_CHECK_CLOSE( static_cast<float>(atan2(fixed_point<15,16>(-y),fixed_point<15,16>(x))), atan2(-y, x), 7.2e-2f);
	BOOST_CHECK_CLOSE( static_cast<float>(atan2(fixed_point<15,16>(-y),fixed_point<15,16>(-x))), atan2(-y, -x), 7.2e-2f);
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
#endif
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

	ucv::surf the_surf(ucv::size2ui(gray_img.width(), gray_img.height()), 3, 4, 2, 1.0e-6f);

	posix_time::ptime start=posix_time::microsec_clock::local_time();
	the_surf.update(ucv::gil::view(gray_img));
	std::vector<ucv::feature_point> fps;
	the_surf.detect(fps);
	posix_time::ptime finish=posix_time::microsec_clock::local_time();

	std::cout << "surf::update+detect=" << (finish-start).total_microseconds() << std::endl;

	for(std::size_t ifp=0;ifp<fps.size();++ifp)
	{
		cv::circle(cv_img, cv::Point(2.0f*fps[ifp].x,2.0f*fps[ifp].y),2, cv::Scalar(255.0));
	}
	cv::imshow(OPENCV_WND_NAME, cv_img);
	cv::waitKey();
}