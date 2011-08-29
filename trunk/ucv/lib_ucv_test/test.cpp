#define BOOST_TEST_MODULE lib_ucv_test

#include <boost/test/unit_test.hpp>

#include <baldzarika/ucv/config.h>
#include <baldzarika/ucv/convert.h>
#include <baldzarika/ucv/integral.h>

#include <opencv2/highgui/highgui.hpp>



#define OPENCV_WND_NAME  "ucv"
#define OPENCV_WND_NAME2  "ucv2"

BOOST_AUTO_TEST_CASE( create_open_cv_window )
{
	cv::namedWindow(OPENCV_WND_NAME);
	cv::namedWindow(OPENCV_WND_NAME2);
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
	integral_t median;
	ucv::convert(
		ucv::gil::interleaved_view(
			cv_img.cols, cv_img.rows,
			reinterpret_cast<ucv::gil::gray8_pixel_t*>(cv_img.data),
			cv_img.step
		),
		ucv::gil::view(gray_img),
		ucv::detail::grayscale_convert_and_median<integral_t>(
			median,
			gray_img.width(),
			gray_img.height()
		)
	);

	integral_image_t integral_img(cv_img.cols+1, cv_img.rows+1, 4);

	ucv::integral(
		ucv::gil::const_view(gray_img),
		ucv::gil::view(integral_img),
		median
	);

	cv::imshow(OPENCV_WND_NAME, cv::Mat(cv_img.rows+1, cv_img.cols+1, CV_32SC1, &ucv::gil::view(integral_img)[0][0]));
	cv::waitKey();
}
