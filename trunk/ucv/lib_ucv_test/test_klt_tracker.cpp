#include <boost/test/unit_test.hpp>

#include <baldzarika/ucv/config.h>
#include <baldzarika/ucv/convert.h>
#include <baldzarika/ucv/integral.h>
#include <baldzarika/ucv/homography.h>
#include <baldzarika/ucv/klt_tracker.h>
#include <baldzarika/ucv/good_features_detector.h>
#include <baldzarika/ucv/surf/feature_point.h>
#include <baldzarika/ucv/surf/hessian_detector.h>
#include <baldzarika/ucv/surf/match_feature_points.h>
#include <baldzarika/ucv/surf/orientation_estimator.h>
#include <baldzarika/ucv/surf/describer.h>
#include <boost/date_time.hpp>

#include <opencv2/features2d/features2d.hpp>


BOOST_AUTO_TEST_CASE( test_klt_tracker )
{
	namespace ucv=baldzarika::ucv;
	namespace math=baldzarika::math;

	typedef float gray_t;
	typedef ucv::gil::pixel<gray_t, ucv::gil::gray_layout_t> gray_pixel_t;
	typedef ucv::gil::image< gray_pixel_t, false, std::allocator<unsigned char> > gray_image_t;
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
	gray_t img_median, shifted_img_medain;

	ucv::convert(
		ucv::gil::const_view(marker_img),
		ucv::gil::view(gray_img),
		ucv::detail::grayscale_convert_and_median<gray_t>(
			img_median,
			marker_img.width(),
			marker_img.height()
		)
	);

	ucv::convert(
		ucv::gil::const_view(shifted_marker_img),
		ucv::gil::view(shifted_gray_img),
		ucv::detail::grayscale_convert_and_median<gray_t>(
			shifted_img_medain,
			marker_img.width(),
			marker_img.height()
		)
	);
	
	gray_image_t integral_img(marker_img.width()+1, marker_img.height()+1), shifted_integral_img(marker_img.width()+1, marker_img.height()+1);
	ucv::integral(ucv::gil::const_view(gray_img), ucv::gil::view(integral_img), img_median);
	ucv::integral(ucv::gil::const_view(shifted_gray_img), ucv::gil::view(shifted_integral_img),shifted_img_medain);

	typedef ucv::good_features_detector<gray_t> good_features_detector_t;

	good_features_detector_t gfd(
		math::size2ui(marker_img.width(), marker_img.height())
	);


	for(int fd=0;fd<9;++fd)
	{
		boost::posix_time::ptime detect_start,detect_finish;
		std::string detector_name;
		std::vector<math::point2f> prev_pts, next_pts;
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
					math::point2f(
						key_points[ifp].pt.x,
						key_points[ifp].pt.y
					)
				);
		}
		else
		if(fd==7)
		{
			typedef ucv::surf::hessian_detector<float> hessian_detector_t;
			typedef ucv::surf::orientation_estimator<float, 6, 10, 10 > orientation_estimator_t;
			typedef ucv::surf::describer<float, 3, 5> describer_t;
			typedef ucv::surf::feature_point<float,3> feature_point_t;


			std::vector<feature_point_t> key_points;
		
			detector_name="ucvSURF";
			hessian_detector_t hd(math::size2ui(marker_img.width(),marker_img.height()), 2, 2, 2);
			
			

			detect_start=boost::posix_time::microsec_clock::local_time();
			for(int i=0;i<10;++i)
			{
				key_points.clear();
				hd.update(ucv::gil::const_view(integral_img));
				hd.detect<float>(
					1.0e-2f,
					boost::bind(&ucv::surf::add_feature_point<feature_point_t::value_type, feature_point_t::NUM_BLOCKS> ,
						boost::ref(key_points),
						_1,
						_2,
						_3
					)
				);
			}
			detect_finish=boost::posix_time::microsec_clock::local_time();

			for(std::size_t ifp=0;ifp<key_points.size();++ifp)
				prev_pts.push_back(
					feature_point_t::base_type(
						key_points[ifp].x(),
						key_points[ifp].y()
					)
				);
		}
		else
		if(fd==8)
		{
			detector_name="ucvGFTT";
			good_features_detector_t gfd(
				math::size2ui(marker_img.width(), marker_img.height())
			);


			std::vector<math::point2f> key_points;
			std::vector<cv::Point2f> good_features;
			detect_start=boost::posix_time::microsec_clock::local_time();
			for(int i=0;i<10;++i)
				gfd(ucv::gil::view(integral_img), key_points);
			detect_finish=boost::posix_time::microsec_clock::local_time();
			for(std::size_t ifp=0;ifp<key_points.size();++ifp)
				prev_pts.push_back(key_points[ifp]);

		}

		typedef ucv::klt_tracker<float> klt_tracker_t;
		klt_tracker_t feature_point_tracker(math::size2ui(marker_img.width(), marker_img.height()), math::size2ui(5,5), 4, 2);
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
		boost::uint32_t n_tracked=0;
		for(std::size_t ifp=0;ifp<next_pts.size();++ifp)
		{
			if(status[ifp])
			{
				float dx=std::abs(static_cast<float>(next_pts[ifp].x()-prev_pts[ifp].x())+5.0f);
				float dy=std::abs(static_cast<float>(next_pts[ifp].y()-prev_pts[ifp].y())-2.0f);
				er+=dx*dx+dy*dy;
				n_tracked++;
			}
		}

		std::cout << detector_name<< ": " << "n=" << prev_pts.size() <<
			" dt=" <<float((detect_finish-detect_start).total_microseconds())/(10.0f) <<
			" (" << float((detect_finish-detect_start).total_microseconds())/(10.0f*prev_pts.size()) << ")" <<
			" tt=" << float((klt_track_finish-klt_track_start).total_microseconds())/(10.0f) <<
			" (" << float((klt_track_finish-klt_track_start).total_microseconds())/(10.0f*prev_pts.size()) << ")" <<
			" e=" << er/float(n_tracked) << std::endl;
	}
}