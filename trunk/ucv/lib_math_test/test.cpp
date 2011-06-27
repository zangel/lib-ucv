#define BOOST_TEST_MODULE lib_math_test

#include <boost/test/unit_test.hpp>
#include <baldzarika/math/config.h>
#include <baldzarika/math/fixed_point.h>
#include <baldzarika/math/constant.h>
#include <baldzarika/math/matrix.h>
#include <baldzarika/math/vector.h>
#include <baldzarika/math/point2.h>
#include <baldzarika/math/ec/galois/field.h>
#include <boost/date_time.hpp>

#if 0

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

BOOST_AUTO_TEST_CASE( test_vector )
{
	namespace bmath=baldzarika::math;
	typedef bmath::vector<float,3> vec3f;
	typedef bmath::vector<float,2> vec2f;

	vec3f z=vec3f::unit<0>().cross(vec3f::unit<1>());
	float one=vec2f::unit<0>().cross(vec2f::unit<1>());
	float mins_one=vec2f::unit<1>().cross(vec2f::unit<0>());
}

BOOST_AUTO_TEST_CASE( test_point2 )
{
	namespace bmath=baldzarika::math;
	typedef bmath::point2<float> point2f;
	typedef bmath::vector<float,3> vector3f;
	typedef bmath::matrix<float,3,3> matrix33f;
}

template < typename T, boost::uint32_t MS >
void test_square_matrix()
{
	namespace bmath=baldzarika::math;

	typedef bmath::matrix<T, MS, MS> matrix_t;

	BOOST_CHECK_EQUAL((matrix_t::identity()*matrix_t::identity().inverted()).determinant(), bmath::constant::one<T>());
}


BOOST_AUTO_TEST_CASE( test_matrix )
{
	namespace bmath=baldzarika::math;

	//test_square_matrix<bmath::real_t,2>();
	test_square_matrix<float,2>();
	test_square_matrix<float,3>();
	test_square_matrix<float,4>();
	//test_square_matrix<double,2>();

	//test_square_matrix<bmath::real_t,3>();
	//test_square_matrix<float,3>();
	//test_square_matrix<double,3>();

	//test_square_matrix<bmath::real_t,4>();
	//test_square_matrix<float,4>();
	//test_square_matrix<double,4>();

	//test_square_matrix<bmath::real_t,5>();
	//test_square_matrix<float,5>();
	//test_square_matrix<double,5>();
}

#endif

BOOST_AUTO_TEST_CASE( test_ec_galois_field )
{
	namespace bmath=baldzarika::math;
	bmath::ec::galois::field<8,6> const &f=bmath::ec::galois::field<8,6>::get();
}
