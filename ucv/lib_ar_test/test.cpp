#define BOOST_TEST_MODULE lib_ucv_test

#include <boost/test/unit_test.hpp>
#include <baldzarika/ar/config.h>
#include <baldzarika/ar/tracker.h>
#include <baldzarika/ar/marker.h>
#include <baldzarika/ar/fiducial/detector.h>
#include <baldzarika/ar/fiducial/bch_marker_model.h>

#if 0

BOOST_AUTO_TEST_CASE( marker_io )
{
	using namespace baldzarika;

	boost::shared_ptr<ar::marker> pmarker=ar::marker::create_from_file("../lib_ucv_test/test_img2.png");
	BOOST_CHECK(pmarker);
	BOOST_CHECK_EQUAL(pmarker->get_size().width(), 256);
	BOOST_CHECK_EQUAL(pmarker->get_size().height(), 256);
}

BOOST_AUTO_TEST_CASE( tracker_start_stop_test )
{
	using namespace baldzarika;

	boost::shared_ptr<ar::tracker> ar_tracker(new ar::tracker(ucv::size2ui(100,100)));

	BOOST_CHECK(!ar_tracker->is_started());
	BOOST_CHECK(ar_tracker->start());
	BOOST_CHECK(ar_tracker->is_started());
	BOOST_CHECK(ar_tracker->stop());
	BOOST_CHECK(!ar_tracker->is_started());
}

BOOST_AUTO_TEST_CASE( tracker_frame_size )
{
	using namespace baldzarika;

	boost::shared_ptr<ar::tracker> ar_tracker(new ar::tracker(ucv::size2ui(100,100)));

	BOOST_CHECK(ar_tracker->get_frame_size()==ucv::size2ui(100,100));
	BOOST_CHECK(ar_tracker->set_frame_size(ucv::size2ui(200,200)));
	BOOST_CHECK(ar_tracker->get_frame_size()==ucv::size2ui(200,200));
	ar_tracker->start();
	BOOST_CHECK(!ar_tracker->set_frame_size(ucv::size2ui(100,100)));
	ar_tracker->stop();
	BOOST_CHECK(ar_tracker->set_frame_size(ucv::size2ui(100,100)));
	BOOST_CHECK(ar_tracker->get_frame_size()==ucv::size2ui(100,100));
}



BOOST_AUTO_TEST_CASE( tracker_detect_marker )
{
	using namespace baldzarika;

	boost::shared_ptr<ar::marker> pmarker=ar::marker::create_from_file("../lib_ucv_test/test_img2.png");

	boost::shared_ptr<ar::tracker> ar_tracker( new ar::tracker(
		ucv::size2ui(
			pmarker->get_size().width(),
			pmarker->get_size().height()
		)
	));


	boost::shared_ptr<ar::tracker::marker_state> pms=ar_tracker->add_marker(pmarker);

	struct _anonymous
	{
		void on_marker_state_changed_signal(boost::shared_ptr<ar::tracker::marker_state> const &ms, ar::tracker::marker_state::eSC sc)
		{
			if(ms && sc==ar::tracker::marker_state::SC_DETECTION && ms->is_detected())
				pdms=ms;
		}
		
		boost::shared_ptr<ar::tracker::marker_state> pdms;
	} anonymous;
	
	ar_tracker->marker_state_changed().connect(boost::bind(&_anonymous::on_marker_state_changed_signal, &anonymous, _1, _2));
	ar_tracker->start();
	ar_tracker->update(ucv::gil::const_view(pmarker->get_image()), pmarker->get_median());
	ar_tracker->stop();

	BOOST_CHECK_EQUAL(anonymous.pdms, pms);
}




BOOST_AUTO_TEST_CASE( fiducial_bch_marker_model )
{
	using namespace baldzarika;
	typedef ucv::gaussian_blur<ar::fiducial::gray_t, 5> gaussian_blur_t;
	typedef ucv::canny<ar::fiducial::gray_t, 3> canny_t;

	ucv::gil::gray8_image_t gray8_img;
	ucv::gil::png_read_and_convert_image("../lib_ucv_test/image-test.png", gray8_img);

	ar::fiducial::gray_image_t gray_img(gray8_img.width(), gray8_img.height());
	
	ucv::convert_scale(
		ucv::gil::const_view(gray8_img),
		ucv::gil::view(gray_img),
		ar::fiducial::gray_t(1.0f/255.0f)
	);

	

	ar::fiducial::gray_image_t gray_blurred_img(gray8_img.width(), gray8_img.height());
	gaussian_blur_t gaussian_blur(ucv::size2ui(gray8_img.width(), gray8_img.height()));
	canny_t canny(ucv::size2ui(gray8_img.width(), gray8_img.height()), 0.4, 1.2);

	boost::posix_time::ptime start_time=boost::posix_time::microsec_clock::local_time();
	for(boost::uint32_t i=0;i<100;++i)
	{

		gaussian_blur(ucv::gil::const_view(gray_img), ucv::gil::view(gray_blurred_img));
			
		std::list< ar::fiducial::contour_t > contours;
	

	
		canny(ucv::gil::const_view(gray_blurred_img), contours);
		for(std::list<ar::fiducial::contour_t>::iterator ic=contours.begin();ic!=contours.end();++ic)
			if(ic->m_is_closed) ic->aproximate(3.0);

		ar::fiducial::bch_marker_model model;

		std::list<ar::fiducial::marker_model::detect_info> d_infos;
		BOOST_CHECK(model.detect_markers(ucv::gil::const_view(gray_blurred_img),contours,d_infos));
		BOOST_CHECK_EQUAL(d_infos.size(),6);
	}
	boost::posix_time::ptime finish_time=boost::posix_time::microsec_clock::local_time();
	std::cout << "fiducial_bch_marker_model performance=" << 100.0f/(1.0e-3f*float((finish_time-start_time).total_milliseconds())) << "fps" << std::endl;
}



BOOST_AUTO_TEST_CASE( fiducial_detector_start_stop_test )
{
	using namespace baldzarika;

	boost::shared_ptr<ar::fiducial::detector> detector(new ar::fiducial::detector(ucv::size2ui(100,100)));

	BOOST_CHECK(!detector->is_started());
	BOOST_CHECK(detector->start());
	BOOST_CHECK(detector->is_starting() || detector->is_started());
	BOOST_CHECK(detector->stop());
	BOOST_CHECK(!detector->is_started());
	detector->wait_to_stop();
}

#endif

BOOST_AUTO_TEST_CASE( fiducial_detector_detection )
{
	using namespace baldzarika;

	ucv::gil::gray8_image_t gray8_frame;
	ucv::gil::png_read_and_convert_image("../lib_ucv_test/image-test.png", gray8_frame);

	ar::fiducial::gray_image_t gray_frame(gray8_frame.width(), gray8_frame.height());

	ucv::convert_scale(
		ucv::gil::const_view(gray8_frame),
		ucv::gil::view(gray_frame),
		ar::fiducial::gray_t(1.0f/255.0f)
	);

	boost::shared_ptr<ar::fiducial::detector> detector(new ar::fiducial::detector(ucv::size2ui(gray8_frame.width(), gray8_frame.height())));

	BOOST_CHECK(detector->add_marker_model( boost::shared_ptr<ar::fiducial::marker_model>(
		new ar::fiducial::bch_marker_model()
	)));



	struct _signal_listener
	{
		_signal_listener(boost::shared_ptr<ar::fiducial::detector> const &d): m_detector(d), m_detected(0){ }

		void on_marker_state_changed_signal(boost::shared_ptr<ar::fiducial::detector::marker_state> const &ms, ar::fiducial::detector::marker_state::eSC sc)
		{
			if(sc==ar::fiducial::detector::marker_state::SC_DETECTION && ms->is_detected())
			{
				
				ucv::matrix44f cam_pose=ms->get_camera_pose();
				ucv::matrix44f cam_proj=m_detector->get_camera_projection();
				ucv::matrix44f transform=cam_proj*cam_pose;
#if 0
				ucv::size2ui mark_size=ms->get_marker_size();
				float corners[4][4]=
				{
					{ 0.0f,					0.0f,				0.0f, 1.0f },
					{ mark_size.width(),	0.0f,				0.0f, 1.0f },
					{ mark_size.width(),	mark_size.height(),	0.0f, 1.0f },
					{ 0.0f,					mark_size.height(),	0.0f, 1.0f }
				};
#else
				float corners[4][4]=
				{
					{ -1.0f,  1.0f, 0.0f, 1.0f },
					{  1.0f,  1.0f,	0.0f, 1.0f },
					{  1.0f, -1.0f,	0.0f, 1.0f },
					{ -1.0f, -1.0f,	0.0f, 1.0f }
				};
#endif

				ucv::vector4f top_left=transform*ucv::vector4f(corners[0]);
				top_left*=1.0f/top_left[2];
				ucv::vector4f top_right=transform*ucv::vector4f(corners[1]);
				top_right*=1.0f/top_right[2];
				ucv::vector4f bottom_right=transform*ucv::vector4f(corners[2]);
				bottom_right*=1.0f/bottom_right[2];
				ucv::vector4f bottom_left=transform*ucv::vector4f(corners[3]);
				bottom_left*=1.0f/bottom_left[2];

				m_detected++;
			}
		}
		boost::shared_ptr<ar::fiducial::detector> m_detector;
		boost::uint32_t m_detected;

	} signal_listener(detector);



	detector->marker_state_changed().connect(boost::bind(&_signal_listener::on_marker_state_changed_signal, &signal_listener, _1, _2));
	//detector->start();
	//detector->update(ucv::gil::const_view(gray_frame));
	
	
	//detector->stop();
	//detector->wait_to_stop();

	//BOOST_CHECK_EQUAL(signal_listener.m_detected, 6);

	signal_listener.m_detected=0;
	detector->start();
	boost::uint32_t n=2;
	while(n)
	{
		if(ar::fiducial::detector::locked_frame frame_lock=detector->lock_frame())
		{
			BOOST_CHECK(static_cast<bool>(frame_lock));	
			BOOST_CHECK(ucv::copy_pixels(ucv::gil::const_view(gray_frame),frame_lock.get_view()));
			n--;
		}
	}
	detector->stop();
	detector->wait_to_stop();
	BOOST_CHECK_EQUAL(signal_listener.m_detected, 1);
}