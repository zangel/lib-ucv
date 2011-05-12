#ifndef BALDZARIKA_UCV_APPROXIMATE_POLYGON_H
#define BALDZARIKA_UCV_APPROXIMATE_POLYGON_H

#include <baldzarika/ucv/point2.h>
#include <baldzarika/ucv/box2.h>

namespace baldzarika { namespace ucv {

	template < typename PT >
	void approximate_polygon(std::vector< point2<PT> > &poly, box2<PT> const &bbox, bool is_closed, PT const &epsilon)
	{
		typedef std::pair<boost::int32_t, boost::int32_t> slice_t;
		typedef std::stack<slice_t> slices_t;

		if(poly.empty() || bbox.width()==detail::constant::zero<PT>() || bbox.height()==detail::constant::zero<PT>())
			return;
		
		boost::int32_t count=poly.size();
		std::vector< point2<PT> > input(poly);
		
		PT const scale_coeff=1.0;

		PT const inv_width=scale_coeff/bbox.width();
		PT const inv_height=scale_coeff/bbox.height();


		PT eps=(epsilon*inv_width)*(epsilon*inv_height);

		float eps_f=eps;

		slices_t slices;
		std::vector< boost::int32_t > indices;
		
		if(is_closed)
		{
			bool le_eps=true;
			slice_t slice(0,0);
			
			for(boost::int32_t i=0;i<3;++i)
			{
				slice.first=slice.second;
				slice.second=(slice.first+1)%count;
				PT max_dist=detail::constant::zero<PT>();
				for(boost::int32_t j=1;j<count;++j)
				{
					boost::int32_t pt_idx=(slice.first+j)%count;
					point2<PT> diff=input[pt_idx]-input[slice.first];
					diff.x*=inv_width;
					diff.y*=inv_height;

					PT dist=diff.x*diff.x+diff.y*diff.y;

					if(dist>max_dist)
					{
						max_dist=dist;
						slice.second=pt_idx;
					}
				}
				le_eps=max_dist<=eps;
			}

			if(le_eps)
			{
				//poly.push_back(input.front());
				indices.push_back(0);
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

				diff.x*=inv_width;
				diff.y*=inv_height;
				
				PT max_dist=detail::constant::zero<PT>();
				
				for(boost::int32_t pt=1;pt<num_pts;++pt)
				{
					boost::int32_t pt_idx=(slice.first+pt)%count;
					point2<PT> diff_start=input[pt_idx]-input[slice.first];
					diff_start.x*=inv_width;
					diff_start.y*=inv_height;


					PT dist=fabs(diff_start.y*diff.x-diff_start.x*diff.y);
					
					if(dist>max_dist)
					{
						max_dist=dist;
						right_slice.first=pt_idx;
					}
				}
				le_eps=max_dist*max_dist<=eps*(diff.x*diff.x+diff.y*diff.y);
			}

			if(le_eps)
			{
				//poly.push_back(input[slice.first]);
				indices.push_back(slice.first);
			}
			else
			{
				slice.second=right_slice.first;
				slices.push(right_slice);
				slices.push(slice);
			}
		}
		if(!is_closed)
		{
			//poly.push_back(input.back());
			indices.push_back(input.size()-1);
		}

		poly.clear();

		count=boost::int32_t(indices.size());

		if(!is_closed)
			poly.push_back(input[indices.front()]);

		point2<PT> prev_pt=is_closed?input[indices.back()]:input[indices[0]];

		point2<PT> curr_pt=is_closed?input[indices[0]]:input[indices[1]];



		for(boost::int32_t i=is_closed?0:1;i<count-(is_closed?0:1);++i)
		{
			point2<PT> next_pt=input[indices[(i+1+count)%count]];
			point2<PT> diff=next_pt-prev_pt;
			
			diff.x*=inv_width;
			diff.y*=inv_height;

			point2<PT> diff_start=curr_pt-prev_pt;
			diff_start.x*=inv_width;
			diff_start.y*=inv_height;

			PT dist=fabs(diff_start.x*diff.y-diff_start.y*diff.x);
			dist*=dist;
			
			float dist_f=dist;

			PT dist_se=diff.x*diff.x+diff.y*diff.y;
			float dist_se_f=dist_se;
			

			if(	dist<=/*detail::constant::half<PT>()**/eps*dist_se &&
				diff.x!=detail::constant::zero<PT>() &&
				diff.y!=detail::constant::zero<PT>()
			)
			{
				curr_pt=next_pt;
				continue;
			}

#if 0
			PT dist_start=diff_start.x*diff_start.x+diff_start.y*diff_start.y;
			float dist_start_f=dist_start;

			point2<PT> diff_end=next_pt-curr_pt;
			diff_end.x*=inv_width;
			diff_end.y*=inv_height;


			PT dist_end=diff_end.x*diff_end.x+diff_end.y*diff_end.y;
			float dist_end_f=dist_end;

			if(dist_start<eps || dist_end<eps)
			{
				curr_pt=next_pt;
				continue;
			}
#endif
			
			poly.push_back(curr_pt);
			prev_pt=curr_pt;
			curr_pt=next_pt;
			
		}

		if(!is_closed)
			poly.push_back(input[indices.back()]);
	}

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_APPROXIMATE_POLYGON_H

