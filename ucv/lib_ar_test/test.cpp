#define BOOST_TEST_MODULE lib_ucv_test

#include <boost/test/unit_test.hpp>
#include <baldzarika/ar/config.h>
#include <baldzarika/ar/tracker.h>
#include <baldzarika/ar/marker.h>
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

#endif

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
		BOOST_CHECK(model.detect_markers(ucv::gil::const_view(gray_img),contours,d_infos));
		BOOST_CHECK_EQUAL(d_infos.size(),6);
	}
	boost::posix_time::ptime finish_time=boost::posix_time::microsec_clock::local_time();
	std::cout << "canny performance=" << 100.0f/(1.0e-3f*float((finish_time-start_time).total_milliseconds())) << "fps" << std::endl;
}