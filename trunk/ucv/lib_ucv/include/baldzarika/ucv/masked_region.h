#ifndef BALDZARIKA_UCV_MASKED_REGION_H
#define BALDZARIKA_UCV_MASKED_REGION_H

namespace baldzarika { namespace ucv {

	namespace detail {

		struct no_mask
		{
			static inline bool is_masked(boost::int32_t x, boost::int32_t y)
			{
				return false;
			}
		};

		template < boost::int32_t L, boost::int32_t T, boost::int32_t W, boost::int32_t H, typename NEXT=no_mask >
		struct rectangle_mask
			: NEXT
		{
			static boost::int32_t const B=T+H;
			static boost::int32_t const R=L+W;

			static inline bool is_masked(boost::int32_t x, boost::int32_t y)
			{
				return (x>=L && x<R && y>=T && y<B) || NEXT::is_masked(x,y);
			}
		};

		template < boost::int32_t CX, boost::int32_t CY, boost::int32_t R, typename NEXT=no_mask >
		struct circle_mask
			: NEXT
		{
			static boost::int32_t const R2=R*R;

			static inline bool is_masked(boost::int32_t x, boost::int32_t y)
			{
				boost::int32_t dx=x-CX;
				boost::int32_t dy=y-CY;
				return (!((dx*dx+dy*dy)>R2)) || NEXT::is_masked(x,y);;
			}
		};

	} //namespace detail

	template < typename LIST >
	struct masked_region
	{
		static inline bool is_masked(boost::int32_t x, boost::int32_t y)
		{
			return LIST::is_masked(x,y);
		}
	};

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_MASKED_REGION_H