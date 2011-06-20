#ifndef BALDZARIKA_UCV_EXTRACT_SURF_DESCRIPTOR_H
#define BALDZARIKA_UCV_EXTRACT_SURF_DESCRIPTOR_H

#include <baldzarika/ucv/feature_point.h>
#include 

namespace baldzarika { namespace ucv {

	template < typename IVT, typename FPT,  typename FPDT, boost::uint32_t FPBS, boost::uint32_t SPB >
	void extract_surf_descriptor(IVT const &iv, feature_point<FPT,FPDT,FPBS> &fp)
	{
		typedef feature_point<FPT,FPDT,FPBS> feature_point_t;
		typedef typename feature_point_t::value_type value_t;
		typedef typename feature_point_t::desc_value_type desc_t;

		enum { HALF_REGION = FPBS*(SPB-(FPBS-1))/2 };

		value_t x=fp.x();
		value_t y=fp.y();
		desc_t scale=fp.m_scale;

		boost::int32_t const int_scale=static_cast<boost::int32_t>(std::floor(scale+math::constant::half<desc_t>()));
		
		desc_t gaussian[2*HALF_REGION];
		
		float const sigma=2.0f*static_cast<float>(scale);
		for(boost::int32_t i=-HALF_REGION;i<HALF_REGION;++i)
			gaussian[i+HALF_REGION]=1.0f/std::sqrt(2.0f*math::constant::pi<float>()*sigma*sigma)*std::exp(-(x*x/2.0f/sigma/sigma));
		
		desc_t co=std::cos(fp.m_orientation);
		desc_t si=std::sin(fp.m_orientation);


		boost::int32_t done_dims=0;
		for(boost::int32_t row=-HALF_REGION;row<HALF_REGION-FPBS;row+=(SPB-FPBS))
		{
			for(int col = -half_region; col < half_region - blocks;
        col += (samples_per_block - blocks)) {
      Vec4f components(0.0f,0.0f,0.0f,0.0f);
      for (int r = row; r < row + samples_per_block; ++r) {
        for (int c = col; c < col + samples_per_block; ++c) {

          //Get coords of sample point on the rotated axis
          int sample_col = lround(x + (-r*scale*si + c*scale*co));
          int sample_row = lround(y + ( r*scale*co + c*scale*si));

          float weight = gaussian(r + half_region) * gaussian(c + half_region);
          //Compute Harr response on rotated axis
          float rrx = HarrX(integral_image, sample_row, sample_col, int_scale);
          float rry = HarrY(integral_image, sample_row, sample_col, int_scale);

          Vec2f dxy;
          dxy << (co*rry - si*rrx),
                 (si*rry + co*rrx);
          dxy *= weight;
          components.head<2>() += dxy;
          components.tail<2>().array() += dxy.array().abs();
        }
      }
      float gauss_BigBox = Gaussian2D(row/half_region,col/half_region,1.0f);
      (*descriptor).segment<4>(done_dims) = components*gauss_BigBox;
      done_dims += 4;
    }
  }
	}

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_EXTRACT_SURF_DESCRIPTOR_H