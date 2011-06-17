#ifndef BALDZARIKA_UCV_RANGED_HISTOGRAM_H
#define BALDZARIKA_UCV_RANGED_HISTOGRAM_H

namespace baldzarika { namespace ucv {

	template < typename VT , typename HT, boost::uint32_t N >
	bool ranged_histogram(VT iv, boost::array<HT, N> &bins,
		typename gil::channel_type<typename VT::value_type>::type min_val,
		typename gil::channel_type<typename VT::value_type>::type max_val)
	{
		if(min_val>=max_val)
			return false;

		typedef typename gil::channel_type<typename VT::value_type>::type channel_t;

		HT const hist_inc=math::constant::one<HT>()/HT(iv.width())/HT(iv.height());
		
		channel_t const bins_range=channel_t(N)/(max_val-min_val);

		std::fill(bins.begin(), bins.end(), math::constant::zero<HT>());
		
		for(boost::int32_t y=0;y<iv.height();++y)
		{
			channel_t const *img_row=reinterpret_cast<channel_t const *>(iv.row_begin(y));
			for(boost::int32_t x=0;x<iv.width();++x)
			{
				boost::int32_t bin_id=static_cast<boost::int32_t>((img_row[x]-min_val)*bins_range);
				bins[std::max<boost::int32_t>(0,std::min<boost::int32_t>(N-1,bin_id))]+=hist_inc;
			}
		}
		return true;
	}

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_RANGED_HISTOGRAM_H