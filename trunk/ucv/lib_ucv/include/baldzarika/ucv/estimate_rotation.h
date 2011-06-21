#ifndef BALDZARIKA_UCV_ESTIMATE_ORIENTATION_H
#define BALDZARIKA_UCV_ESTIMATE_ORIENTATION_H

#include <baldzarika/math/constant.h>
#include <baldzarika/ucv/integral.h>
#include <baldzarika/ucv/haar_wavelets.h>

namespace baldzarika { namespace ucv {

	namespace detail {

		template < typename T >
		static inline T get_angle(T const &x, T const &y)
		{
			if(y<math::constant::zero<T>())
			{
				if(x<math::constant::zero<T>()) return math::constant::pi<T>()+std::atan2(y,x);
				if(x>math::constant::zero<T>()) return math::constant::pi_2<T>()-std::atan2(-y,x);
			}
			else
			{
				if(x<math::constant::zero<T>()) return math::constant::pi<T>()-std::atan2(-y,x);
				if(x>math::constant::zero<T>()) return std::atan2(y,x);
			}
			return math::constant::zero<T>();
		}

		/*
		template < typename T, boost::uint32_t S >
		struct gauss25 { };

		template < typename T >
		struct gauss25<T,6>
		{
			static T const s_values[7][7];
		};


		
		template < typename T >
		T const gauss25<T,6>::s_values[7][7]=
		{
			{	0,02350693969273,	0.01849121369071,	0.01239503121241,	0.00708015417522,	0.00344628101733,	0.00142945847484,	0.00050524879060	},
			{	0,02169964028389,	0.01706954162243,	0.01144205592615,	0.00653580605408,	0.00318131834134,	0.00131955648461,	0.00046640341759	},
			{	0.01706954162243,	0.01342737701584,	0.00900063997939,	0.00514124713667,	0.00250251364222,	0.00103799989504,	0.00036688592278	},
			{	0.01144205592615,	0.00900063997939,	0.00603330940534,	0.00344628101733,	0.00167748505986,	0.00069579213743,	0.00024593098864	},
			{	0.00653580605408,	0.00514124713667,	0.00344628101733,	0.00196854695367,	0.00095819467066,	0.00039744277546,	0.00014047800980	},
			{	0.00318131834134,	0.00250251364222,	0.00167748505986,	0.00095819467066,	0.00046640341759,	0.00019345616757,	0.00006837798818	},
			{	0.00131955648461,	0.00103799989504,	0.00069579213743,	0.00039744277546,	0.00019345616757,	0.00008024231247,	0.00002836202103	}
		};
		*/

		template < typename T, boost::uint32_t R >
		struct rotation_helper {};
		

		template <typename T>
		struct rotation_helper<T,6>
		{
		public:
			enum { N=109 };
		private:
			rotation_helper()
			{
				boost::int32_t idx=0;
				for(boost::int32_t i=-6;i<=6;++i)
				{
					for(boost::int32_t j=-6;j<=6;++j)
					{
						if(i*i+j*j<36)
						{
							m_indices[idx][0]=i;
							m_indices[idx][1]=j;
							idx++;
						}
					}
				}

				for(boost::uint32_t x=0;x<=6;++x)
				{
					float fx=float(x);
					for(boost::uint32_t y=0;y<=6;++y)
					{
						float fy=float(y);
						m_gauss25[y][x]=T(1.0f/(2.0f*math::constant::pi<float>()*6.25f)*
							std::exp(-(fx*fx+fy*fy)/(2.0f*6.25f)));
					}
				}
			}

		public:
			static inline rotation_helper const& get()
			{
				static rotation_helper _rotation_helper;
				return _rotation_helper;
			}
		public:
			boost::int32_t	m_indices[N][2];
			T				m_gauss25[7][7];
		};

		template <typename T>
		struct rotation_helper<T,3>
		{
		public:
			enum { N=25 };
		private:
			rotation_helper()
			{
				boost::int32_t idx=0;
				for(boost::int32_t i=-3;i<=3;++i)
				{
					for(boost::int32_t j=-3;j<=3;++j)
					{
						if(i*i+j*j<9)
						{
							m_indices[idx][0]=i;
							m_indices[idx][1]=j;
							idx++;
						}
					}
				}

				for(boost::uint32_t y=0;y<=3;++y)
				{
					float fy=float(y);
					for(boost::uint32_t x=0;x<=3;++x)
					{
						float fx=float(x);
						m_gauss25[y][x]=T(1.0f/(2.0f*math::constant::pi<float>()*6.25f)*
							std::exp(-(fx*fx+fy*fy)/(2.0f*6.25f)));
					}
				}
			}

		public:
			static inline rotation_helper const& get()
			{
				static rotation_helper _rotation_helper;
				return _rotation_helper;
			}
		public:
			boost::int32_t	m_indices[N][2];
			T				m_gauss25[4][4];
		};



	} //namespace detail

	

	template < typename IVT , typename AT >
	static inline AT estimate_rotation_fast(IVT const &iv, boost::int32_t x, boost::int32_t y, boost::uint32_t s)
	{
		typedef typename IVT::value_type iv_pixel_t;
		typedef typename gil::channel_type<iv_pixel_t>::type iv_channel_t;

		static iv_channel_t const fast_ring_x[16]=
		{
			0.0,	0.31622776601683793319988935444327,		0.70710678118654752440084436210485,		0.94868329805051379959966806332982,
			1.0,	0.94868329805051379959966806332982,		0.70710678118654752440084436210485,		0.31622776601683793319988935444327,
			0.0,	-0.31622776601683793319988935444327,	-0.70710678118654752440084436210485,	-0.94868329805051379959966806332982,
			-1.0,	-0.94868329805051379959966806332982,	-0.70710678118654752440084436210485,	-0.31622776601683793319988935444327
		};

		static iv_channel_t const fast_ring_y[16]=
		{
			-1.0,	-0.94868329805051379959966806332982,	-0.70710678118654752440084436210485,	-0.31622776601683793319988935444327,
			0.0,	0.31622776601683793319988935444327,		0.70710678118654752440084436210485,		0.94868329805051379959966806332982,
			1.0,	0.94868329805051379959966806332982,		0.70710678118654752440084436210485,		0.31622776601683793319988935444327,
			0.0,	-0.31622776601683793319988935444327,	-0.70710678118654752440084436210485,	-0.94868329805051379959966806332982
		};

		static boost::int32_t const indX[16]=
		{
			3,	3,	2,	1,
			0,	-1,	-2,	-3,
			-3,	-3,	-2,	-1,
			0,	1,	2,	3
		};
		static boost::int32_t const indY[16]=
		{
			0,	1,	2,	3,
			3,	3,	2,	1,
			0,	-1,	-2,	-3,
			-3,	-3,	-2,	-1
		};

		iv_channel_t dx=math::constant::zero<AT>();
		iv_channel_t dy=math::constant::zero<AT>();

		iv_channel_t center_val=box_integral<IVT,iv_channel_t>(iv, x-s, y-s, s<<1, s<<1);
		
		for(boost::uint32_t px=0;px<16;++px)
		{
			iv_channel_t diff=box_integral<IVT,iv_channel_t>(iv, x+indX[px]*(s<<1)-s, y+indY[px]*(s<<1)-s, s<<1, s<<1)-center_val;
			dx+=diff*fast_ring_x[px];
			dy+=diff*fast_ring_y[px];
		}

		return AT(detail::get_angle(dx,dy));
	}

	template < typename IVT , typename AT, boost::uint32_t R >
	static inline AT estimate_rotation(IVT const &iv, boost::int32_t x, boost::int32_t y, AT const &s)
	{
		typedef detail::rotation_helper<AT,R> rotation_helper_t;

		static rotation_helper_t const &rot_h=rotation_helper_t::get();
		static AT const sss=AT(6)/AT(R);
		
		static AT const r_3i20=0.15f;
		static AT const r_3i2_sq=2.25f;

		boost::uint32_t ss=static_cast<boost::uint32_t>(std::floor(s*sss+math::constant::half<AT>()));

		boost::uint32_t ss4=ss*4;//static_cast<boost::uint32_t>(std::floor(s*AT(4)+math::constant::half<AT>()));

		AT res_x[rotation_helper_t::N], res_y[rotation_helper_t::N], angle[rotation_helper_t::N];
		
		for(boost::int32_t k=0;k<rotation_helper_t::N;++k)
		{
			AT gauss=rot_h.m_gauss25[std::abs(rot_h.m_indices[k][0])][std::abs(rot_h.m_indices[k][1])];
	
			res_x[k]=gauss*haar2d_x<IVT,AT>(
				iv,
				x+rot_h.m_indices[k][0]*ss,
				y+rot_h.m_indices[k][1]*ss,
				ss4
			);
			res_y[k]=gauss*haar2d_y<IVT,AT>(
				iv,
				x+rot_h.m_indices[k][0]*ss,
				y+rot_h.m_indices[k][1]*ss,
				ss4
			);
			angle[k]=detail::get_angle(res_x[k],res_y[k]);
		}

		AT max_sum=math::constant::zero<AT>();
		AT orientation=math::constant::zero<AT>();


		for(boost::uint32_t a=0;a<42;++a)
		{
			AT ang1=AT(a)*r_3i20;
			AT ang2=((ang1+math::constant::pi_i3<AT>())>math::constant::pi_2<AT>()?
				ang1-math::constant::pi_5i3<AT>():
				ang1+math::constant::pi_i3<AT>());

			AT sum_x=math::constant::zero<AT>();
			AT sum_y=math::constant::zero<AT>();

			for(boost::int32_t k=0;k<rotation_helper_t::N;++k)
			{
				AT const &ang=angle[k];
				if(ang1<ang2 && ang1<ang && ang<ang2)
				{
					sum_x+=res_x[k];  
					sum_y+=res_y[k];
				} 
				else
				if(ang2<ang1 && ((ang>math::constant::zero<AT>() && ang<ang2) || (ang>ang1 && ang<math::constant::pi_2<AT>())))
				{
					sum_x+=res_x[k];
					sum_y+=res_y[k];
				}
			}
			
			AT this_sum=sum_x*sum_x+sum_y*sum_y;
			if(this_sum>max_sum) 
			{
				max_sum=this_sum;
				orientation=detail::get_angle(sum_x, sum_y);
			}
		}
		return orientation;
	}


} //namesapce ucv
} //namespace bnaldzarika


#endif //BALDZARIKA_UCV_ESTIMATE_ORIENTATION_H