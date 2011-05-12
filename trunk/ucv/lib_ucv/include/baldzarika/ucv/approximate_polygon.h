#ifndef BALDZARIKA_UCV_APPROXIMATE_POLYGON_H
#define BALDZARIKA_UCV_APPROXIMATE_POLYGON_H

#include <baldzarika/ucv/point2.h>

namespace baldzarika { namespace ucv {

	template < typename PT >
	void approximate_polygon(std::vector< point2<PT> > &poly, bool is_closed, PT const &epsilon)
	{
		typedef std::pair<boost::int32_t, boost::int32_t> slice_t;
		typedef std::stack<slice_t> slices_t;

		boost::int32_t count=poly.size();
		std::vector< point2<PT> > input(poly);
		poly.clear();
		
		PT eps=epsilon*epsilon;

		slices_t slices;

		if(is_closed)
		{
			bool le_eps=true;
			slice_t slice(0,0);
			for(boost::int32_t i=0;i<3;++i)
			{
				slice.first=slice.second;
				slice.second=(slice.first+1)%count;

				PT max_dist=detail::constant::zero<PT>();
				float max_dist_f=0.0f;

				for(boost::int32_t j=1;j<count;++j)
				{
					PT dist=vector<PT,2>(input[(slice.first+j)%count]-input[slice.first]).norm_2();
					point2<PT> diff=input[(slice.first+j)%count]-input[slice.first];
					float diff_x=diff.x;
					float diff_y=diff.y;
					
					float dist_f=std::sqrt(diff_x*diff_x+diff_y*diff_y);
					if(dist_f>max_dist_f)
					{
						max_dist_f=dist_f;

						slice.second=j;
					}
				}
				le_eps=max_dist_f<=float(eps);
			}

			if(le_eps)
			{
				poly.push_back(input.front());
			}
			else
			{
				slices.push(slice);
				slices.push(slice_t(slice.second, slice.first));
			}
		}
		else
		{
			slices.push(slice_t(0,input.size()-1));
		}

		
		while(!slices.empty())
		{
			bool le_eps=true;

			slice_t slice=slices.top(); slices.pop();

			slice_t right_slice(slice);
			

			boost::int32_t num_pts=(slice.second-slice.first+count)%count;
			if(num_pts>1)
			{
				point2<PT> diff=input[slice.second]-input[slice.first];
				float diff_x=diff.x;
				float diff_y=diff.y;
				
				PT max_dist=detail::constant::zero<PT>();
				float max_dist_f=0.0f;

				for(boost::int32_t pt=1;pt<num_pts;++pt)
				{
					boost::int32_t pt_idx=(slice.first+pt)%count;
					point2<PT> diff_start=input[pt_idx]-input[slice.first];
					float diff_start_x=diff_start.x;
					float diff_start_y=diff_start.y;

					PT dist=fabs(diff_start.y*diff.x-diff_start.x*diff.y);
					float dist_f=std::abs(diff_start_y*diff_x-diff_start_x*diff_y);

					if(dist_f>max_dist_f)
					{
						max_dist_f=dist_f;
						right_slice.first=pt_idx;
					}
				}
				float diff_sq=diff.x*diff.x+diff.y*diff.y;
				le_eps=max_dist*max_dist<=eps*(diff.x*diff.x+diff.y*diff.y);
				le_eps=max_dist_f*max_dist_f<=float(eps)*(diff_x*diff_x+diff_y*diff_y);

			}

			if(le_eps)
			{
				poly.push_back(input[slice.first]);
			}
			else
			{
				slice.second=right_slice.first;
				slices.push(right_slice);
				slices.push(slice);
			}
		}
		if(!is_closed)
			poly.push_back(input.back());

	}

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_APPROXIMATE_POLYGON_H

