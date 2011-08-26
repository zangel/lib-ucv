#ifndef BALDZARIKA_UCV_PIXEL_COUNT_H
#define BALDZARIKA_UCV_PIXEL_COUNT_H

namespace baldzarika { namespace ucv {

	namespace detail {

		struct is_non_zero
		{
			template < typename T >
			inline bool operator()(T const &v) const
			{
				return v;
			}

			inline bool operator()(float const &v) const
			{
				return std::abs(v)>=std::numeric_limits<float>::epsilon();
			}
		};

		struct is_zero
		{
			template < typename T >
			inline bool operator()(T const &v) const
			{
				return !v;
			}

			inline bool operator()(float const &v) const
			{
				return std::abs(v)<std::numeric_limits<float>::epsilon();
			}
		};
		
	} //namespace detail

	template < typename VT, typename P >
	boost::uint32_t pixel_count(VT iv, P const &pred)
	{
		typedef typename VT::value_type	pixel_t;
		typedef typename gil::channel_type<pixel_t>::type channel_t;

		boost::uint32_t count(0);
		
		for(boost::int32_t y=0;y<iv.height();++y)
		{
			channel_t const *row=reinterpret_cast<channel_t const *>(iv.row_begin(y));
			for(boost::int32_t x=0;x<iv.width();++x)
				count+=pred(row[x])?1:0;
		}
		return count;
	}

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_PIXEL_COUNT_H