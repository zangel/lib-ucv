#include <boost/test/unit_test.hpp>
#include <baldzarika/ucv/config.h>
#include <baldzarika/ucv/perspective_transform.h>

#include <boost/date_time.hpp>

BOOST_AUTO_TEST_CASE( perspective_transform_test )
{
	namespace ucv=baldzarika::ucv;
	namespace math=baldzarika::math;
	typedef math::point2<float> point_t;
	typedef math::vector<float,3> vector_t;
	typedef math::matrix<float,3,3> matrix_t;

	std::vector<point_t> src(4),dst(4);

	dst[0]=point_t( 219.0f, 316.0f ); src[0]=point_t(   0.0f,   0.0f );
	dst[1]=point_t( 317.0f, 313.0f ); src[1]=point_t(  50.0f,   0.0f );
	dst[2]=point_t( 325.0f, 396.0f ); src[2]=point_t(  50.0f,  50.0f );
	dst[3]=point_t( 213.0f, 400.0f ); src[3]=point_t(   0.0f,  50.0f );


	matrix_t pm;
	BOOST_CHECK( (ucv::perspective_transform<point_t::value_type, matrix_t::value_type>(src,dst,pm)) );

	matrix_t identity=pm*pm.inverted();

	BOOST_CHECK_LT( std::abs((pm*vector_t(src[0]))[0]-dst[0][0]), 5.0e-2f);
	BOOST_CHECK_LT( std::abs((pm*vector_t(src[0]))[1]-dst[0][1]), 5.0e-2f);

}