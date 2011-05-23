#ifndef BALDZARIKA_UCV_CAMERA_POSE_H
#define BALDZARIKA_UCV_CAMERA_POSE_H

namespace baldzarika { namespace ucv {

	template < typename T >
	void camera_pose(T const &fx, T const &fy, point2<T> const &cc, matrix<T, 3, 3> const &hm, matrix<T, 4, 4> &pose)
	{
		matrix<T, 3, 3> inv_intrinsics=matrix<T, 3, 3>::identity();
		inv_intrinsics(0,0)=detail::constant::one<T>()/fx;
		inv_intrinsics(0,2)=-cc.x/fx;

		inv_intrinsics(1,1)=detail::constant::one<T>()/fy;
		inv_intrinsics(1,2)=-cc.y/fy;

		vector<T, 3> h0; h0[0]=hm(0,0); h0[1]=hm(1,0); h0[2]=hm(2,0);
		vector<T, 3> h1; h1[0]=hm(0,1); h1[1]=hm(1,1); h1[2]=hm(2,1);
		vector<T, 3> h2; h2[0]=hm(0,2); h2[1]=hm(1,2); h2[2]=hm(2,2);

		//vector<T, 3> h0; h0[0]=hm(0,0); h0[1]=hm(0,1); h0[2]=hm(0,2);
		//vector<T, 3> h1; h1[0]=hm(1,0); h1[1]=hm(1,1); h1[2]=hm(1,2);
		//vector<T, 3> h2; h2[0]=hm(2,0); h2[1]=hm(2,1); h2[2]=hm(2,2);

		vector<T, 3> r0=inv_intrinsics*h0;
		T l0=r0.length(); r0*=(detail::constant::one<T>()/l0);

		vector<T, 3> r1=inv_intrinsics*h1;
		T l1=r1.length(); r1*=(detail::constant::one<T>()/l1);

		vector<T, 3> r2=cross_product(r0,r1);
		T l2=(l0+l1)*detail::constant::half<T>();

		vector<T, 3> t=(inv_intrinsics*h2)*(detail::constant::one<T>()/l2);

		for(boost::uint32_t i=0;i<3;++i)
		{
			pose(i,0)=r0[i];
			pose(i,1)=r1[i];
			pose(i,2)=r2[i];
			pose(i,3)=t[i];
			pose(3,i)=detail::constant::zero<T>();
		}
		pose(3,3)=detail::constant::one<T>();
	}

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_CAMERA_POSE_H