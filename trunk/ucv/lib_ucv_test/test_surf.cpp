#include <boost/test/unit_test.hpp>

#include <baldzarika/ucv/config.h>
#include <baldzarika/ucv/convert.h>
#include <baldzarika/ucv/integral.h>
#include <baldzarika/ucv/surf/hessian_detector.h>
#include <baldzarika/ucv/homography.h>
#include <baldzarika/ucv/surf/feature_point.h>
#include <baldzarika/ucv/surf/match_feature_points.h>
#include <baldzarika/ucv/perspective_transform.h>
#include <baldzarika/ucv/surf/orientation_estimator.h>
#include <baldzarika/ucv/surf/describer.h>

#include <boost/date_time.hpp>
#include <opencv2/highgui/highgui.hpp>

#define OPENCV_WND_NAME  "ucv"
#define OPENCV_WND_NAME2  "ucv2"

template < typename T, boost::uint32_t I, boost::uint32_t NB, boost::uint32_t NSB >
void do_surf_performance_test()
{
	namespace ucv=baldzarika::ucv;
	namespace math=baldzarika::math;

	typedef T gray_t;
	typedef ucv::gil::pixel<gray_t, ucv::gil::gray_layout_t> gray_pixel_t;
	typedef ucv::gil::image< gray_pixel_t, false, std::allocator<unsigned char> > gray_image_t;
	typedef gray_image_t::view_t gray_view_t;
	typedef gray_image_t::const_view_t gray_const_view_t;
	typedef ucv::surf::hessian_detector<T> hessian_detector_t;
	typedef ucv::surf::orientation_estimator<T, 6, 10, 10 > orientation_estimator_t;
	typedef ucv::surf::describer<T, NB, NSB> describer_t;
	typedef ucv::surf::feature_point<T, NB> feature_point_t;


	ucv::gil::gray8_image_t gray8_img;
	ucv::gil::png_read_and_convert_image("test_img2_match_bn.png", gray8_img);

	gray_image_t gray_img(gray8_img.width(), gray8_img.height());

	gray_t median;
	ucv::convert(
		ucv::gil::view(gray8_img),
		ucv::gil::view(gray_img),
		ucv::detail::grayscale_convert_and_median<gray_t>(
			median,
			gray8_img.width(),
			gray8_img.height()
		)
	);

	gray_image_t integral_img(gray8_img.width()+1, gray8_img.height()+1);
	ucv::integral(ucv::gil::const_view(gray_img), ucv::gil::view(integral_img), median);

	hessian_detector_t hd(math::size2ui(gray8_img.width(),gray8_img.height()),2,2,2);
	orientation_estimator_t oe;
	describer_t desc;
	
	struct points_collector
	{
		void add_pts(math::point2<T> const &p, boost::int32_t s, bool lap)
		{
			_points.push_back( feature_point_t(p,s,lap) );
		}

		std::vector< feature_point_t > _points;
	};


	boost::posix_time::ptime start_ud=boost::posix_time::microsec_clock::local_time();
	for(boost::uint32_t i=0;i<I;++i)
	{
		points_collector pc;
		hd.update(ucv::gil::const_view(integral_img));
		boost::function<void (math::point2<T> const &, boost::int32_t, bool)> dcb(boost::bind(&points_collector::add_pts, &pc, _1, _2, _3));
		hd.detect(1.0e-2f, dcb);
	}
	boost::posix_time::ptime finish_ud=boost::posix_time::microsec_clock::local_time();
	std::cout << "hessian_response<" << typeid(T).name() << "> update+detect time=" <<
		float((finish_ud-start_ud).total_milliseconds())/float(I) << " ms" << std::endl;

	{
		points_collector pc;
		hd.update(ucv::gil::const_view(integral_img));
		boost::function<void (math::point2<T> const &, boost::int32_t, bool)> dcb(boost::bind(&points_collector::add_pts, &pc, _1, _2, _3));
		hd.detect(1.0e-2f, dcb);
		
		gray_const_view_t iv=ucv::gil::const_view(gray_img);
		boost::posix_time::ptime start_oe=boost::posix_time::microsec_clock::local_time();
		for(boost::uint32_t i=0;i<I;++i)
		{
			
			for(boost::uint32_t ifp=0;ifp<pc._points.size();++ifp)
				oe.estimate(iv, pc._points[ifp]);
		}
		boost::posix_time::ptime finish_oe=boost::posix_time::microsec_clock::local_time();
		std::cout << "orientation_estimator<" << typeid(T).name() << ">::estimate time=" <<
			float((finish_oe-start_oe).total_milliseconds())/float(I) << " ms" << std::endl;
	}

	{
		points_collector pc;
		hd.update(ucv::gil::const_view(integral_img));
		boost::function<void (math::point2<T> const &, boost::int32_t, bool)> dcb(boost::bind(&points_collector::add_pts, &pc, _1, _2, _3));
		hd.detect(1.0e-2f, dcb);

		gray_const_view_t iv=ucv::gil::const_view(gray_img);

		for(boost::uint32_t ifp=0;ifp<pc._points.size();++ifp)
			oe.estimate(iv, pc._points[ifp]);

		
		boost::posix_time::ptime start_desc=boost::posix_time::microsec_clock::local_time();
		for(boost::uint32_t i=0;i<I;++i)
		{
			for(boost::uint32_t ifp=0;ifp<pc._points.size();++ifp)
				desc.describe(iv, pc._points[ifp]);
		}
		boost::posix_time::ptime finish_desc=boost::posix_time::microsec_clock::local_time();
		std::cout << "describer<" << typeid(T).name() << ">::describe time=" <<
			float((finish_desc-start_desc).total_milliseconds())/float(I) << " ms" << std::endl;
	}
}

BOOST_AUTO_TEST_CASE( surf_performance_test )
{
	namespace math=baldzarika::math;
	namespace ucv=baldzarika::ucv;

	do_surf_performance_test< float, 500, 3, 5 >();
	//do_surf_performance_test< math::fixed_point<10,21>, 500, 3, 5 >();
}

BOOST_AUTO_TEST_CASE( surf_match_test )
{
	namespace ucv=baldzarika::ucv;
	namespace math=baldzarika::math;

	typedef float gray_t;
	typedef ucv::gil::pixel<gray_t, ucv::gil::gray_layout_t> gray_pixel_t;
	typedef ucv::gil::image< gray_pixel_t, false, std::allocator<unsigned char> > gray_image_t;
	typedef gray_image_t::view_t gray_view_t;
	typedef gray_image_t::const_view_t gray_const_view_t;
	typedef ucv::surf::hessian_detector<float> hessian_detector_t;
	typedef ucv::surf::orientation_estimator<float, 6, 10, 10 > orientation_estimator_t;
	typedef ucv::surf::describer<float, 3, 5> describer_t;
	typedef describer_t::feature_point_t feature_point_t;
	
	orientation_estimator_t oe;
	describer_t desc;



	//first image
	ucv::gil::gray8_image_t gray8_img_1;
	ucv::gil::png_read_and_convert_image("test_img2.png", gray8_img_1);
	gray_image_t gray_img_1(gray8_img_1.width(), gray8_img_1.height());
	
	gray_t median_1;
	ucv::convert(
		ucv::gil::view(gray8_img_1),
		ucv::gil::view(gray_img_1),
		ucv::detail::grayscale_convert_and_median<gray_t>(
			median_1,
			gray8_img_1.width(),
			gray8_img_1.height()
		)
	);
	gray_image_t integral_img_1(gray8_img_1.width()+1, gray8_img_1.height()+1);
	ucv::integral(ucv::gil::const_view(gray_img_1), ucv::gil::view(integral_img_1), median_1);

	
	std::vector<feature_point_t> fps_1;

	hessian_detector_t hd_1(math::size2ui(gray8_img_1.width(),gray8_img_1.height()), 2, 2, 2);
	hd_1.update(ucv::gil::const_view(integral_img_1));
	hd_1.detect<float>(1.0e-2f,
		boost::bind(
			&ucv::surf::add_feature_point<feature_point_t::value_type,feature_point_t::NUM_BLOCKS>,
			boost::ref(fps_1),
			_1,
			_2,
			_3
		)
	);
	for(boost::uint32_t i=0;i<fps_1.size();++i)
	{
		oe.estimate(ucv::gil::const_view(integral_img_1), fps_1[i]);
		desc.describe(ucv::gil::const_view(gray_img_1), fps_1[i]);
	}

	//second image
	ucv::gil::gray8_image_t gray8_img_2;
	ucv::gil::png_read_and_convert_image("test_img2_match.png", gray8_img_2);
	gray_image_t gray_img_2(gray8_img_2.width(), gray8_img_2.height());

	gray_t median_2;
	ucv::convert(
		ucv::gil::view(gray8_img_2),
		ucv::gil::view(gray_img_2),
		ucv::detail::grayscale_convert_and_median<gray_t>(
			median_2,
			gray8_img_2.width(),
			gray8_img_2.height()
		)
	);
	gray_image_t integral_img_2(gray8_img_2.width()+1, gray8_img_2.height()+1);
	ucv::integral(ucv::gil::const_view(gray_img_2), ucv::gil::view(integral_img_2), median_2);

	std::vector<feature_point_t> fps_2;
	
	hessian_detector_t hd_2(math::size2ui(gray8_img_2.width(),gray8_img_2.height()), 2, 2, 2);
	hd_2.update(ucv::gil::const_view(integral_img_2));
	hd_2.detect<float>(1.0e-2f,
		boost::bind(
			&ucv::surf::add_feature_point<feature_point_t::value_type, feature_point_t::NUM_BLOCKS>,
			boost::ref(fps_2),
			_1,
			_2,
			_3
		)
	);
	for(boost::uint32_t i=0;i<fps_2.size();++i)
	{
		oe.estimate(ucv::gil::const_view(integral_img_2), fps_2[i]);
		desc.describe(ucv::gil::const_view(gray_img_2), fps_2[i]);
	}

	std::vector<
		std::pair<
			std::vector<feature_point_t>::const_iterator,
			std::vector<feature_point_t>::const_iterator
		>
	> matches;

	ucv::surf::match_feature_points<
		feature_point_t,
		std::vector<feature_point_t>,
		std::vector<feature_point_t>
	>(fps_1, fps_2, matches, 0.6f);

	cv::Mat cv_img1_rgb=cv::imread("test_img2.png");
	cv::Mat cv_img2_rgb=cv::imread("test_img2_match.png");
	
	std::vector< feature_point_t::base_type > pts1(4), pts2(4);

	for(std::size_t ifp=0;ifp<matches.size();++ifp)
	{
	
		if(ifp<4)
		{

			pts1[ifp]=*matches[ifp].first;
			pts2[ifp]=*matches[ifp].second;
		}

		cv::circle(cv_img1_rgb,
			cv::Point(static_cast<boost::int32_t>(matches[ifp].first->x()), static_cast<boost::int32_t>(matches[ifp].first->y())),
			3,
			cv::Scalar(0.0, 255.0, 0.0),
			-1
		);

		cv::circle(cv_img2_rgb,
			cv::Point(static_cast<boost::int32_t>(matches[ifp].second->x()), static_cast<boost::int32_t>(matches[ifp].second->y())),
			3,
			cv::Scalar(0.0, 255.0, 0.0),
			-1
		);
	}

	math::matrix33f hm;

	ucv::find_homography_ransac(matches,hm,false,2.5);
	//ucv::perspective_transform(pts1,pts2,hm);
	//ucv::find_homography(matches, math::size2ui(gray8_img_1.width(),gray8_img_1.height()), hm);


	float marker_corners[4][3]=
	{
		{
			0.0f,
			0.0f,
			1.0f
		},
		{
			static_cast<float>(gray8_img_1.width()),
			0.0f,
			1.0f
		},
		{
			static_cast<float>(gray8_img_1.width()),
			static_cast<float>(gray8_img_1.height()),
			1.0f
		},
		{
			0.0f,
			static_cast<float>(gray8_img_1.height()),
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
			math::iround(p0(0)),
			math::iround(p0(1))
		);

		cv::Point cvp1(
			math::iround(p1(0)),
			math::iround(p1(1))
		);

		cv::line(cv_img2_rgb, cvp0, cvp1, cv::Scalar(0.0, 255.0, 0.0));
	}

	
	cv::imshow(OPENCV_WND_NAME2, cv_img1_rgb);
	cv::imshow(OPENCV_WND_NAME, cv_img2_rgb);
	
	cv::waitKey();

}