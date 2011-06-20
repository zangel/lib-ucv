#ifndef BALDZARIKA_UCV_EXTRACT_SURF_DESCRIPTOR_H
#define BALDZARIKA_UCV_EXTRACT_SURF_DESCRIPTOR_H

#include <baldzarika/ucv/feature_point.h>
#include <baldzarika/ucv/haar_wavelets.h>

namespace baldzarika { namespace ucv {

	namespace detail {

		template < typename CT, typename ST >
		static inline ST gaussian(CT const &x, CT const &y, ST const &sigma)
		{
			float sig_=static_cast<float>(sigma);
			float x_=static_cast<float>(x);
			float y_=static_cast<float>(y);

			return ST(1.0f/(2.0f*math::constant::pi<float>()*sig_*sig_)*std::exp(-float(x_*x_+y_*y_)/(2.0f*sig_*sig_)));
		}

	} //namespace detail
	
	template < typename IVT, typename FPT,  typename FPDT, boost::int32_t FPBS, boost::int32_t SPB >
	static inline void extract_surf_descriptor(IVT const &iv, feature_point<FPT,FPDT,FPBS> &fp)
	{
		typedef feature_point<FPT,FPDT,FPBS> feature_point_t;
		typedef typename feature_point_t::value_type value_t;
		typedef typename feature_point_t::desc_value_type desc_value_t;

		static desc_value_t const c_5i2=2.5f;
		static desc_value_t const c_3i2=1.5f;
		static desc_value_t const c_3i2_sq=2.25f;

		static desc_value_t const inv_len=math::constant::one<desc_value_t>()/desc_value_t(feature_point_t::DESCRIPTOR_SIZE);

		static boost::int32_t half_region=FPBS*(SPB-(FPBS-1))/2;
		static boost::int32_t samples_center=SPB/2+1;

		boost::uint32_t ui_scale=static_cast<boost::uint32_t>(std::floor(fp.m_scale+math::constant::half<desc_value_t>()));

		desc_value_t co=std::cos(fp.m_orientation);
		desc_value_t si=std::sin(fp.m_orientation);

		desc_value_t inv_scale=math::constant::one<desc_value_t>()/desc_value_t(fp.m_scale);
		desc_value_t inv_scale2=math::constant::one<desc_value_t>()/desc_value_t(fp.m_scale*fp.m_scale);
		value_t sco=fp.m_scale*co;
		value_t ssi=fp.m_scale*si;

		desc_value_t len=math::constant::zero<desc_value_t>();
		desc_value_t mean=math::constant::zero<desc_value_t>();
		
		boost::uint32_t count=0;

		desc_value_t cy=math::constant::half<desc_value_t>();
		for(boost::int32_t regy=-half_region;regy<half_region-FPBS;regy+=SPB-FPBS)//i
		{
			desc_value_t cx=math::constant::half<desc_value_t>();
			boost::int32_t scy=regy+samples_center;

			for(boost::int32_t regx=-half_region;regx<half_region-FPBS;regx+=SPB-FPBS)//j
			{
				boost::int32_t scx=regx+samples_center;

				desc_value_t	dx=math::constant::zero<desc_value_t>(),
								dy=math::constant::zero<desc_value_t>(),
								mdx=math::constant::zero<desc_value_t>(),
								mdy=math::constant::zero<desc_value_t>();

				boost::int32_t xs=static_cast<boost::int32_t>(
						std::floor(fp.x()+value_t(-scx)*ssi+value_t(scy)*sco+
						math::constant::half<value_t>()
					)
				);
				boost::int32_t ys=static_cast<boost::int32_t>(
						std::floor(fp.y()+value_t(scx)*sco+value_t(scy)*ssi+
						math::constant::half<value_t>()
					)
				);

				for(boost::int32_t sy=regy;sy<regy+SPB;++sy)
				{
					for(boost::int32_t sx=regx;sx<regx+SPB;++sx)
					{
						boost::int32_t sample_x=static_cast<boost::int32_t>(
								std::floor(fp.x()+value_t(-sx)*ssi+value_t(sy)*sco+
								math::constant::half<value_t>()
							)
						);
						boost::int32_t sample_y=static_cast<boost::int32_t>(
								std::floor(fp.y()+value_t(sx)*sco+value_t(sy)*ssi+
								math::constant::half<value_t>()
							)
						);

						desc_value_t gauss_s1=detail::gaussian(xs-sample_x, ys-sample_y, c_5i2*fp.m_scale);
						
						desc_value_t rx=haar2d_x<IVT,desc_value_t>(
							iv,
							sample_x,
							sample_y,
							ui_scale<<1
						);

						desc_value_t ry=haar2d_y<IVT,desc_value_t>(
							iv,
							sample_x,
							sample_y,
							ui_scale<<1
						);

						desc_value_t rrx=gauss_s1*(ry*co-rx*si);
						desc_value_t rry=gauss_s1*(rx*co+ry*si);

						dx+=rrx;
						dy+=rry;
						mdx+=std::abs(rrx);
						mdy+=std::abs(rry);
					}
				}

				desc_value_t gauss_s2=detail::gaussian(
					cx-math::constant::two<desc_value_t>(),
					cy-math::constant::two<desc_value_t>(),
					c_3i2
				);
				
				desc_value_t dxg=dx*gauss_s2;
				desc_value_t dyg=dy*gauss_s2;
				desc_value_t mdxg=mdx*gauss_s2;
				desc_value_t mdyg=mdy*gauss_s2;

				
				
				fp.m_desc[count++]=dxg;
				fp.m_desc[count++]=dyg;
				fp.m_desc[count++]=mdxg;
				fp.m_desc[count++]=mdyg;

				len+=(dxg*dxg+dyg*dyg+mdxg*mdxg+mdyg*mdyg);
				//mean+=dxg+dyg+mdxg+mdyg;

				cx+=math::constant::one<desc_value_t>();
			}
			cy+=math::constant::one<desc_value_t>();
		}
#if 0
		mean*=inv_len;
		len=math::constant::one<desc_value_t>()/
			(std::sqrt(len*inv_len-(mean*mean))+std::numeric_limits<desc_value_t>::epsilon());

		for(boost::uint32_t d=0;d<feature_point_t::DESCRIPTOR_SIZE;++d)
			fp.m_desc[d]=(fp.m_desc[d]-mean)*len;
#else
		len=math::constant::one<desc_value_t>()/std::max(std::sqrt(len), std::numeric_limits<desc_value_t>::epsilon());

		for(boost::uint32_t i=0;i<feature_point_t::DESCRIPTOR_SIZE;++i)
			fp.m_desc[i]*=len;
#endif
	}

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_EXTRACT_SURF_DESCRIPTOR_H