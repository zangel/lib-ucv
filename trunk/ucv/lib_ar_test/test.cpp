#define BOOST_TEST_MODULE lib_ar_test

#include <boost/test/unit_test.hpp>
#include <baldzarika/ar/config.h>
#include <baldzarika/ar/tracker.h>
#include <baldzarika/ar/fiducial/tracker.h>
#include <baldzarika/ar/fiducial/bch_marker_model.h>
#include <baldzarika/ar/fiducial/qr_marker_model.h>
#include <baldzarika/ar/markerless/tracker.h>
#include <baldzarika/ar/markerless/marker.h>

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

	boost::shared_ptr<ar::markerless::marker> pmarker=ar::markerless::marker::create_from_file("../lib_ucv_test/test_img2.png");
	ucv::gil::gray8_image_t frame;
	ucv::gil::png_read_and_convert_image("../lib_ucv_test/test_img2.png", frame);

	boost::shared_ptr<ar::markerless::tracker> ar_tracker( new ar::markerless::tracker(
		math::size2ui(
			pmarker->get_size().width(),
			pmarker->get_size().height()
		)
	));


	BOOST_CHECK(ar_tracker->add_marker(pmarker));

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
	while(!ar_tracker->is_started()) boost::this_thread::yield();
	ar_tracker->process_frame(ucv::gil::const_view(frame));
	ar_tracker->process_frame(ucv::gil::const_view(frame));
	ar_tracker->stop();
	ar_tracker->wait_to_stop();
	boost::shared_ptr<ar::markerless::tracker::marker_state> pdmms(
		anonymous.pdms,
		boost::detail::dynamic_cast_tag()
	);
	BOOST_CHECK(pdmms);
	BOOST_CHECK_EQUAL(pdmms->get_marker(), pmarker);
}




BOOST_AUTO_TEST_CASE( fiducial_bch_marker_model )
{
	using namespace baldzarika;
	typedef ucv::gaussian_blur<ar::gray_t, 5> gaussian_blur_t;
	typedef ucv::canny<ar::gray_t, 3> canny_t;

	ucv::gil::gray8_image_t gray8_img;
	ucv::gil::png_read_and_convert_image("../lib_ucv_test/fiducial_test.png", gray8_img);

	ar::gray_image_t gray_img(gray8_img.width(), gray8_img.height());
	
	ucv::convert(
		ucv::gil::const_view(gray8_img),
		ucv::gil::view(gray_img),
		ucv::detail::grayscale_convert()
	);

	ar::gray_image_t gray_blurred_img(gray8_img.width(), gray8_img.height());
	gaussian_blur_t gaussian_blur(math::size2ui(gray8_img.width(), gray8_img.height()));
	canny_t canny(math::size2ui(gray8_img.width(), gray8_img.height()), 0.4, 1.2);

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

	boost::shared_ptr<ar::fiducial::tracker> tracker(new ar::fiducial::tracker(math::size2ui(100,100)));

	BOOST_CHECK(!tracker->is_started());
	BOOST_CHECK(tracker->start());
	BOOST_CHECK(tracker->is_starting() || tracker->is_started());
	BOOST_CHECK(tracker->stop());
	BOOST_CHECK(!tracker->is_started());
	tracker->wait_to_stop();
	BOOST_CHECK(!tracker->is_active());
}

#endif

template < typename MarkerModel >
void test_fiducial_marker_model_detection(char const *test_image, boost::uint32_t expected_markers_count, boost::uint32_t n_frames=1)
{
	using namespace baldzarika;

	ucv::gil::gray8_image_t gray8_frame;
	ucv::gil::png_read_and_convert_image(test_image, gray8_frame);

	ar::gray_image_t gray_frame(gray8_frame.width(), gray8_frame.height());

	ucv::convert(
		ucv::gil::const_view(gray8_frame),
		ucv::gil::view(gray_frame),
		ucv::detail::grayscale_convert()
	);

	boost::shared_ptr<ar::fiducial::tracker> tracker(new ar::fiducial::tracker(math::size2ui(gray8_frame.width(), gray8_frame.height())));

	BOOST_CHECK(tracker->add_marker_model( boost::shared_ptr<ar::fiducial::marker_model>(
		new MarkerModel()
	)));



	struct _signal_listener
	{
		_signal_listener(boost::shared_ptr<ar::fiducial::tracker> const &t): m_tracker(t), m_detected(0){ }

		void on_marker_state_changed_signal(boost::shared_ptr<ar::tracker::marker_state> const &ms, ar::tracker::marker_state::eSC sc)
		{
			if(sc==ar::tracker::marker_state::SC_DETECTION && ms->is_detected())
			{
				
				math::matrix44f cam_pose=ms->get_camera_pose();
				math::matrix44f cam_proj=m_tracker->get_camera_projection();
				math::matrix44f transform=cam_proj*cam_pose;
				m_detected++;
			}
		}
		boost::shared_ptr<ar::fiducial::tracker> m_tracker;
		boost::uint32_t m_detected;

	} signal_listener(tracker);



	tracker->marker_state_changed().connect(boost::bind(&_signal_listener::on_marker_state_changed_signal, &signal_listener, _1, _2));
	
	signal_listener.m_detected=0;
	tracker->start();
	while(!tracker->is_started()) boost::this_thread::yield();
	
	while(n_frames)
	{
		if(tracker->process_frame(ucv::gil::const_view(gray8_frame)))
			n_frames--;
		else
			boost::this_thread::yield();
	}
	tracker->stop();
	tracker->wait_to_stop();
	BOOST_CHECK_EQUAL(signal_listener.m_detected, expected_markers_count);

}


BOOST_AUTO_TEST_CASE( fiducial_tracker_bch_detection )
{
	test_fiducial_marker_model_detection<baldzarika::ar::fiducial::bch_marker_model>("../lib_ucv_test/fiducial_test.png",6);
}

BOOST_AUTO_TEST_CASE( fiducial_tracker_qr_detection )
{
	test_fiducial_marker_model_detection<baldzarika::ar::fiducial::qr_marker_model>("../lib_ucv_test/qr_code_test.png", 1, 2);
}
