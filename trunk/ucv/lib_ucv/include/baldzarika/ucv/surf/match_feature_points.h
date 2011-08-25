#ifndef BALDZARIKA_UCV_SURF_MATCH_FEATURE_POINTS_H
#define BALDZARIKA_UCV_SURF_MATCH_FEATURE_POINTS_H

#include <baldzarika/ucv/surf/feature_point.h>


namespace baldzarika { namespace ucv { namespace surf {
	
	template < typename FP, typename FPS1, typename FPS2 >
	void match_feature_points(FPS1 const &fps1, FPS2 const &fps2, std::vector< std::pair<typename FPS1::const_iterator, typename FPS2::const_iterator> > &m, typename FP::value_type const &d=typename FP::value_type(0.65f))
	{
		m.clear();
		for(typename FPS1::const_iterator ifp1=fps1.begin(); ifp1!=fps1.end();++ifp1)
		{
			typename FP::value_type d1=std::numeric_limits<typename FP::value_type>::max();
			typename FP::value_type d2=std::numeric_limits<typename FP::value_type>::max();

			typename FPS2::const_iterator match=fps2.end();
			for(typename FPS2::const_iterator ifp2=fps2.begin(); ifp2!=fps2.end();++ifp2) 
			{
				if(!(*ifp1 && *ifp2))
					continue;
				
				typename FP::value_type dist=*ifp1-*ifp2;

				if(dist<d1)
				{
					d2=d1;
					d1=dist;
					match=ifp2;
				}
				else
				if(dist<d2)
					d2=dist;
			}

			if(match!=fps2.end() && (d1==math::constant::zero<typename FP::value_type>() || (d2!=math::constant::zero<typename FP::value_type>() && d1/d2<d)))
				m.push_back(std::make_pair(ifp1,match));
		}
	}

	template < typename FP, typename FPS1 >
	void match_feature_points(FPS1 const &fps1, KDTree::KDTree<FP::DESCRIPTOR_SIZE, FP, typename FP::description_accessor> const &fps2,
		std::vector< std::pair<typename FPS1::const_iterator, typename KDTree::KDTree<FP::DESCRIPTOR_SIZE, FP, typename FP::description_accessor>::const_iterator > > &m,
		typename FP::value_type const &d=typename FP::value_type(0.65f))
	{
		typedef KDTree::KDTree<FP::DESCRIPTOR_SIZE, FP, typename FP::description_accessor > FPS2;
		m.clear();
		for(typename FPS1::const_iterator ifp1=fps1.begin(); ifp1!=fps1.end();++ifp1)
		{
			std::list< std::pair<typename FPS2::const_iterator, typename FPS2::distance_type> > res;
			fps2.find_k_nearest(*ifp1, res, 2);
			if(res.empty() || res.size()!=2) continue;
			if(res.begin()->second/res.rbegin()->second<d)
				m.push_back(std::make_pair(ifp1,res.begin()->first));
		}
	}

} //namespace surf
} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_SURF_MATCH_FEATURE_POINTS_H