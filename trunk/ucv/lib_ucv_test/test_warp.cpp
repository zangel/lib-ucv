#include <boost/test/unit_test.hpp>
#include <baldzarika/ucv/config.h>
#include <baldzarika/ucv/convert.h>
#include <baldzarika/ucv/warp.h>
#include <baldzarika/ucv/perspective_transform.h>

BOOST_AUTO_TEST_CASE( warp_test )
{
	namespace ucv=baldzarika::ucv;
	namespace math=baldzarika::math;
	

	typedef math::fixed_point<10,21> gray_t;
	typedef ucv::gil::pixel< gray_t, ucv::gil::gray_layout_t > gray_pixel_t;
	typedef ucv::gil::image< gray_pixel_t, false, std::allocator< unsigned char > > gray_image_t;
	typedef gray_image_t::view_t gray_view_t;


	ucv::gil::gray8_image_t src8_img;
	ucv::gil::png_read_and_convert_image("fiducial_test.png", src8_img);

	gray_image_t src_img(src8_img.width(), src8_img.height());

	ucv::convert(
		ucv::gil::const_view(src8_img),
		ucv::gil::view(src_img),
		ucv::detail::grayscale_convert()
	);

	typedef math::point2<float> point_t;
	typedef math::vector<float,3> vector_t;
	typedef math::matrix<float,3,3> matrix_t;

	std::vector<point_t> src(4),dst(4);

	dst[0]=point_t( 219.0f, 316.0f ); src[0]=point_t(   0.0f,   0.0f );
	dst[1]=point_t( 317.0f, 313.0f ); src[1]=point_t(  50.0f,   0.0f );
	dst[2]=point_t( 325.0f, 396.0f ); src[2]=point_t(  50.0f,  50.0f );
	dst[3]=point_t( 213.0f, 400.0f ); src[3]=point_t(   0.0f,  50.0f );


	matrix_t pm;
	BOOST_CHECK( (ucv::perspective_transform(src,dst,pm)) );

	for(boost::uint32_t p=0;p<4;p++)
	{
		point_t delta=src[p].transformed(pm)-dst[p];
		BOOST_CHECK_LT(delta.length(), 1.0e-3f);
	}

	gray_image_t dst_img(50, 50);

	BOOST_CHECK(ucv::warp(ucv::gil::const_view(src_img), ucv::gil::view(dst_img), pm, true));

	ucv::gil::gray8_image_t dst8_img(50, 50);

	BOOST_CHECK(ucv::convert(
		ucv::gil::const_view(dst_img),
		ucv::gil::view(dst8_img),
		ucv::detail::grayscale_convert()
	));

	ucv::gil::png_write_view("warped-fiducial_test.png", ucv::gil::const_view(dst8_img));
}