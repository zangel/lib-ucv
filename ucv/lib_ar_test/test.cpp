#define BOOST_TEST_MODULE lib_ucv_test

#include <boost/test/unit_test.hpp>
#include <baldzarika/ar/config.h>
#include <baldzarika/ar/tracker.h>
#include <baldzarika/ar/marker.h>

BOOST_AUTO_TEST_CASE( marker_io )
{
	using namespace baldzarika;

	boost::shared_ptr<ar::marker> pmarker=ar::marker::load("../lib_ucv_test/test_img2.png");
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

	boost::shared_ptr<ar::marker> pmarker=ar::marker::load("../lib_ucv_test/test_img2.png");

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