#define BOOST_TEST_MODULE lib_math_test

#include <boost/test/unit_test.hpp>
#include <baldzarika/math/config.h>
#include <baldzarika/math/fixed_point.h>
#include <baldzarika/math/constant.h>
#include <baldzarika/math/matrix.h>
#include <boost/date_time.hpp>

BOOST_AUTO_TEST_CASE( test_fixed_point )
{
	namespace bmath=baldzarika::math;

	BOOST_CHECK_EQUAL(static_cast<float>(bmath::real_t(bmath::fixed_point<0,15>(0.5f))), 0.5f);
	BOOST_CHECK_EQUAL(static_cast<float>(bmath::fixed_point<0,15>(bmath::real_t(0.5f))), 0.5f);

	BOOST_CHECK_EQUAL(1, static_cast<int>(bmath::real_t(3)/bmath::real_t(2)) );
	BOOST_CHECK_EQUAL(1.5f, static_cast<float>(bmath::real_t(3)/bmath::real_t(2)));

	//sqrt
	BOOST_CHECK_EQUAL(static_cast<int>(std::sqrt(bmath::real_t(144))), 12);
	BOOST_CHECK_LT(std::abs(static_cast<float>(std::sqrt(bmath::real_t(2)))-std::sqrt(2.0f)), 1.0e-3f);

	BOOST_CHECK_LT(std::abs(static_cast<float>(std::sqrt(bmath::real_t(0.00001f)))-std::sqrt(0.00001f)), 1.0e-3f);


	//sin
	BOOST_CHECK_LT( std::abs(static_cast<float>(std::sin(bmath::constant::zero<bmath::real_t>()))-std::sin(0.0f)), 1.0e-3f);
	BOOST_CHECK_LT( std::abs(static_cast<float>(std::sin(bmath::constant::pi_i2<bmath::real_t>()))-std::sin(3.1415926535897932384626433832795f/2.0f)), 1.0e-3f);

	//cos
	BOOST_CHECK_LT( std::abs(static_cast<float>(std::cos(bmath::constant::zero<bmath::real_t>()))-std::cos(0.0f)), 1.0e-3f);
	BOOST_CHECK_LT( std::abs(static_cast<float>(std::cos(bmath::constant::pi_i2<bmath::real_t>()))-std::cos(3.1415926535897932384626433832795f/2.0f)), 1.0e-3f);


	//fabs
	BOOST_CHECK( (std::abs( bmath::fixed_point<15,16>(1) ) == bmath::fixed_point<15,16>(1)) );
	BOOST_CHECK( (std::abs( bmath::fixed_point<15,16>(0) ) == bmath::fixed_point<15,16>(0)) );
	BOOST_CHECK( (std::abs( bmath::fixed_point<15,16>(-1) ) == bmath::fixed_point<15,16>(1)) );

	//floor
	BOOST_CHECK( (std::floor( bmath::fixed_point<15,16>(1.5) ) == bmath::fixed_point<15,16>(1.0)) );
	BOOST_CHECK( (std::floor( bmath::fixed_point<15,16>(-1.5) ) == bmath::fixed_point<15,16>(-2.0)) );

	//ceil
	BOOST_CHECK( (std::ceil( bmath::fixed_point<15,16>(1.5) ) == bmath::fixed_point<15,16>(2.0)) );
	BOOST_CHECK( (std::ceil( bmath::fixed_point<15,16>(-1.5) ) == bmath::fixed_point<15,16>(-1.0)) );

	//fmod
	BOOST_CHECK( (std::fmod( bmath::fixed_point<15,16>(2.5), bmath::fixed_point<15,16>(2.0) ) == bmath::fixed_point<15,16>(0.5)) );
	BOOST_CHECK( (std::fmod( bmath::fixed_point<15,16>(1.5), bmath::fixed_point<15,16>(2.0) ) == bmath::fixed_point<15,16>(1.5)) );


	//modf
	bmath::fixed_point<15,16> ip;
	BOOST_CHECK( (std::modf( bmath::fixed_point<15,16>(10.5), &ip ) == bmath::fixed_point<15,16>(0.5)) );
	BOOST_CHECK( (ip == bmath::fixed_point<15,16>(10.0)) );

	//atan2
	float y=std::sin(3.14f/2.0f);
	float x=std::cos(3.14f/2.0f);

	BOOST_CHECK_LT( std::abs(static_cast<float>(std::atan2(bmath::fixed_point<15,16>(y),bmath::fixed_point<15,16>(x)))-std::atan2(y, x)), 7.2e-2f);
	BOOST_CHECK_LT( std::abs(static_cast<float>(std::atan2(bmath::fixed_point<15,16>(y),bmath::fixed_point<15,16>(-x)))-std::atan2(y, -x)), 7.2e-2f);
	BOOST_CHECK_LT( std::abs(static_cast<float>(std::atan2(bmath::fixed_point<15,16>(-y),bmath::fixed_point<15,16>(x)))-std::atan2(-y, x)), 7.2e-2f);
	BOOST_CHECK_LT( std::abs(static_cast<float>(std::atan2(bmath::fixed_point<15,16>(-y),bmath::fixed_point<15,16>(-x)))-std::atan2(-y, -x)), 7.2e-2f);
}

BOOST_AUTO_TEST_CASE( test_fixed_point_atan2_speed )
{
	namespace bmath=baldzarika::math;

	float f_res;
	bmath::fixed_point<15,16> fp_res;
	float y=std::sin(2.5f);
	float x=std::cos(2.5f);
	bmath::fixed_point<15,16> fp_x(x), fp_y(y);

	boost::posix_time::ptime cp_1=boost::posix_time::microsec_clock::local_time();

	for(uint32_t i=0;i<10000000;++i)
	{
		f_res=std::atan2(y, x);
	}

	boost::posix_time::ptime cp_2=boost::posix_time::microsec_clock::local_time();

	for(uint32_t i=0;i<10000000;++i)
	{
		fp_res=std::atan2(fp_y, fp_x);
	}
	boost::posix_time::ptime cp_3=boost::posix_time::microsec_clock::local_time();

	std::cout << "fixed_point speed=" << float((cp_3-cp_2).total_microseconds())/float((cp_2-cp_1).total_microseconds()) << " " << f_res << " " << static_cast<float>(fp_res) << std::endl;
}

BOOST_AUTO_TEST_CASE( test_matrix )
{
	namespace bmath=baldzarika::math;

	bmath::matrix<float,3,3> m1=bmath::matrix<double,3,3>();


	bmath::matrix<float,3,3> zero=bmath::matrix<float,3,3>::zero();
}