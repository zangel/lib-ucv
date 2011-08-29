#include <boost/test/unit_test.hpp>
#include <baldzarika/ucv/config.h>
#include <baldzarika/ucv/perspective_transform.h>
#include <baldzarika/ucv/camera_pose.h>


BOOST_AUTO_TEST_CASE( camera_pose_test )
{
	namespace ucv=baldzarika::ucv;
	namespace math=baldzarika::math;

	math::size2ui const fs=math::size2ui(100,100);
	math::size2ui const ms=math::size2ui(50,50);
	
	std::vector<math::point2f> src(4),dst(4);

	src[0]=math::point2f( 0.0f,					0.0f );					dst[0]=math::point2f( 0.0f,					0.0f );			
	src[1]=math::point2f( float(ms.width()),	0.0f );					dst[1]=math::point2f( float(fs.width()),	0.0f );			
	src[2]=math::point2f( float(ms.width()),	float(ms.height()) );	dst[2]=math::point2f( float(fs.width()),	float(fs.height()) );
	src[3]=math::point2f( 0.0f,					float(ms.height()) );	dst[3]=math::point2f( 0.0f,					float(fs.height()) );

	math::matrix33f hm;

	ucv::perspective_transform(src,dst,hm);

	float const fovy=45.0f;
	float ty=std::tan(fovy*0.5f/360.0f*math::constant::pi<float>());
	float fl=fs.height()/(2.0f*ty);
	
	math::matrix44f cam_matrix;
	ucv::camera_pose(fl, fl, math::point2f(float(fs.width()/2),float(fs.height()/2)), hm, cam_matrix);

	math::matrix44f cam_adjust=math::matrix44f::identity();

	cam_adjust(0,0)=ms.width()/2.0f;
	cam_adjust(0,3)=ms.width()/2.0f;

	cam_adjust(1,1)=-(ms.height()/2.0f);
	cam_adjust(1,3)=ms.height()/2.0f;

	cam_matrix*=cam_adjust;
	
	float corners[4][4]=
	{
		{ -1.0f,  1.0f, 0.0f, 1.0f },
		{  1.0f,  1.0f,	0.0f, 1.0f },
		{  1.0f, -1.0f,	0.0f, 1.0f },
		{ -1.0f, -1.0f,	0.0f, 1.0f }
	};
	
	math::vector4f top_left=cam_matrix*math::vector4f(corners[0]);
	math::vector4f top_right=cam_matrix*math::vector4f(corners[1]);
	math::vector4f bottom_right=cam_matrix*math::vector4f(corners[2]);
	math::vector4f bottom_left=cam_matrix*math::vector4f(corners[3]);

	math::matrix44f cam_intr=math::matrix44f::identity();
	cam_intr(0,0)=fl;
	cam_intr(0,2)=float(fs.width()/2);
	cam_intr(1,1)=fl;
	cam_intr(1,2)=float(fs.height()/2);

	top_left=cam_intr*top_left; top_left/=top_left[2];
	top_right=cam_intr*top_right; top_right/=top_right[2];
	bottom_right=cam_intr*bottom_right; bottom_right/=bottom_right[2];
	bottom_left=cam_intr*bottom_left; bottom_left/=bottom_left[2];
}