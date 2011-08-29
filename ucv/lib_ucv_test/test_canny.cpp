#include <boost/test/unit_test.hpp>
#include <baldzarika/ucv/config.h>
#include <baldzarika/ucv/convert.h>
#include <baldzarika/ucv/warp.h>
#include <baldzarika/ucv/sobel.h>
#include <baldzarika/ucv/gaussian_blur.h>
#include <baldzarika/ucv/canny.h>
#include <baldzarika/ucv/adaptive_treshold.h>
#include <baldzarika/ucv/threshold.h>
#include <baldzarika/ucv/ranged_histogram.h>
#include <baldzarika/ucv/find_otsu_threshold.h>
#include <baldzarika/ucv/perspective_transform.h>
#include <baldzarika/ucv/pixel_count.h>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#define OPENCV_WND_NAME  "ucv"
#define OPENCV_WND_NAME2  "ucv2"

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
	//ucv::gil::png_read_and_convert_image("fiducial_test.png", gray8_img);
	ucv::gil::png_read_and_convert_image("qr_code_test.png", gray8_img);


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

	cv::imshow(OPENCV_WND_NAME, cv::Mat(gray8_img.height(), gray8_img.width(), CV_8UC1, &ucv::gil::view(gray8_img)[0][0]));

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

	cv::imshow(OPENCV_WND_NAME, cv::Mat(dx_img.height(), dx_img.width(), CV_32FC1, &ucv::gil::view(gray32f_img)[0][0]));
	cv::waitKey();



	ucv::convert(
		ucv::gil::const_view(gray_img),
		ucv::gil::view(gray32f_img),
		ucv::detail::convert()
	);

	cv::imshow(OPENCV_WND_NAME, cv::Mat(dx_img.height(), dx_img.width(), CV_32FC1, &ucv::gil::view(gray32f_img)[0][0]));
	cv::waitKey();

	//cv::Mat image=cv::imread("fiducial_test.png", CV_LOAD_IMAGE_GRAYSCALE);
	cv::Mat image=cv::imread("qr_code_test.png", CV_LOAD_IMAGE_GRAYSCALE);


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
	cv::imshow(OPENCV_WND_NAME, cv::Mat(dx_img.height(), dx_img.width(), CV_32FC1, &ucv::gil::view(gray32f_img)[0][0]));
	cv::waitKey();

	//cv::imshow(OPENCV_WND_NAME, image_dx);
	//cv::waitKey();

	ucv::convert(
		ucv::gil::const_view(dy_img),
		ucv::gil::view(gray32f_img),
		ucv::detail::convert()
	);
	cv::imshow(OPENCV_WND_NAME, cv::Mat(dy_img.height(), dy_img.width(), CV_32FC1, &ucv::gil::view(gray32f_img)[0][0]));
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

			if(!ucv::perspective_transform(src,dst,pmf)) continue;

			
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

		cv::imshow(OPENCV_WND_NAME, cv::Mat(edge_img.height(), edge_img.width(), CV_32FC1, &ucv::gil::view(gray32f_img)[0][0]));
		cv::waitKey();
	}

	cv::Canny(image, image, 30, 90);
	cv::imshow(OPENCV_WND_NAME, image);
	cv::waitKey();
	
}