#define BOOST_TEST_MODULE lib_ucv_test

#include <boost/test/unit_test.hpp>
#include <baldzarika/ar/config.h>
#include <baldzarika/ar/tracker.h>

BOOST_AUTO_TEST_CASE( tracker_start_stop_test )
{
	using namespace baldzarika;

	ar::tracker tracker(ucv::size2ui(100,100));

	BOOST_CHECK(!tracker.is_started());
	BOOST_CHECK(tracker.start());
	BOOST_CHECK(tracker.is_started());
	BOOST_CHECK(tracker.stop());
	BOOST_CHECK(!tracker.is_started());
}

BOOST_AUTO_TEST_CASE( tracker_frame_size )
{
	using namespace baldzarika;

	ar::tracker tracker(ucv::size2ui(100,100));

	BOOST_CHECK(tracker.get_frame_size()==ucv::size2ui(100,100));
	BOOST_CHECK(tracker.set_frame_size(ucv::size2ui(200,200)));
	BOOST_CHECK(tracker.get_frame_size()==ucv::size2ui(200,200));
	tracker.start();
	BOOST_CHECK(!tracker.set_frame_size(ucv::size2ui(100,100)));
	tracker.stop();
	BOOST_CHECK(tracker.set_frame_size(ucv::size2ui(100,100)));
	BOOST_CHECK(tracker.get_frame_size()==ucv::size2ui(100,100));
}