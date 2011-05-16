#ifndef BALDZARIKA_UCV_THRESHOLD_H
#define BALDZARIKA_UCV_THRESHOLD_H

namespace baldzarika { namespace ucv {

	namespace detail {

		struct normal_threshold_tag {};
		struct inverted_threshold_tag {};

		template < typename ST, typename DT >
		struct binary_threshold
		{
			binary_threshold(ST const &th, DT const &mv)
				: m_threshold(th)
				, m_max_value(mv)
			{
			}

			ST m_threshold;
			DT m_max_value;
		};

		template < typename ST, typename DT >
		struct normal_binary_threshold
			: binary_threshold<ST,DT>
		{
			normal_binary_threshold(ST const &th, DT const &mv)
				: binary_threshold<ST,DT>(th,mv)
			{

			}

			inline DT operator()(ST const &src) const
			{
				return src<m_threshold?constant::zero<DT>():m_max_value;
			}
		};

		template < typename ST, typename DT >
		struct inverted_binary_threshold
			: binary_threshold<ST,DT>
		{
			inverted_binary_threshold(ST const &th, DT const &mv)
				: binary_threshold<ST,DT>(th,mv)
			{

			}

			inline DT operator()(ST const &src) const
			{
				return src<m_threshold?m_max_value:constant::zero<DT>();
			}
		};

	} //namespace detail

	template < typename SVT, typename DVT, typename THT >
	bool threshold(SVT src, DVT dst, THT const &tht)
	{
		typedef typename SVT::value_type	src_pixel_t;
		typedef typename DVT::value_type	dst_pixel_t;

		typedef typename gil::channel_type<src_pixel_t>::type src_channel_t;
		typedef typename gil::channel_type<dst_pixel_t>::type dst_channel_t;

		if(src.width()!=dst.width() || src.height()!=dst.height())
			return false;

		for(boost::int32_t y=0;y<src.height();++y)
		{
			src_channel_t const *src_row=reinterpret_cast<src_channel_t const *>(src.row_begin(y));
			dst_channel_t *dst_row=reinterpret_cast<dst_channel_t *>(dst.row_begin(y));
			for(boost::int32_t x=0;x<src.width();++x)
				dst_row[x]=tht(src_row[x]);
		}
		return true;
	}

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_THRESHOLD_H