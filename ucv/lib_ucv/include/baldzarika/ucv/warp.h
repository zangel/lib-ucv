#ifndef BALDZARIKA_UCV_WARP_H
#define BALDZARIKA_UCV_WARP_H

#include <baldzarika/ucv/matrix.h>


namespace baldzarika { namespace ucv {
	
	template < typename SVT, typename DVT, typename MT >
	bool warp(SVT src, DVT dst, matrix<MT, 3, 3> const &m, bool inv=false)
	{
		typedef typename SVT::value_type	src_pixel_t;
		typedef typename DVT::value_type	dst_pixel_t;

		typedef typename gil::channel_type<src_pixel_t>::type src_channel_t;
		typedef typename gil::channel_type<dst_pixel_t>::type dst_channel_t;

		typedef matrix<MT, 3, 3> matrix_t;

		matrix_t mat=inv?m:m.inverse();

		for(boost::int32_t y=0;y<dst.height();++y)
		{
			dst_channel_t *dst_row=reinterpret_cast<dst_channel_t *>(dst.row_begin(y));

			MT y_dst=y;

			MT x_src_0=y_dst*mat(0,1)+mat(0,2);
			MT y_src_0=y_dst*mat(1,1)+mat(1,2);
			MT w_src_0=y_dst*mat(2,1)+mat(2,2);
						
			for(boost::int32_t x=0;x<dst.width();++x)
			{
				MT x_dst=x;

				MT inv_w_src=x_dst*mat(2,0)+w_src_0;
				inv_w_src=std::abs(inv_w_src)<std::numeric_limits<MT>::epsilon()?
					detail::constant::zero<MT>():
					detail::constant::one<MT>()/inv_w_src;

				MT x_src=(x_dst*mat(0,0)+x_src_0)*inv_w_src;
				MT y_src=(x_dst*mat(1,0)+y_src_0)*inv_w_src;

				boost::int32_t ix_src=static_cast<boost::int32_t>(x_src);
				boost::int32_t iy_src=static_cast<boost::int32_t>(y_src);

				src_channel_t frac_x=static_cast<src_channel_t>(x_src-MT(ix_src));
				src_channel_t frac_y=static_cast<src_channel_t>(y_src-MT(iy_src));

				src_channel_t const *src_row=reinterpret_cast<src_channel_t const *>(src.row_begin(iy_src));
				if(iy_src<0 || ix_src<0 || iy_src>=src.height() || ix_src>=src.width())
					dst_row[x]=detail::constant::zero<dst_channel_t>();
				else
				{
					//dst_row[x]=static_cast<dst_channel_t>(src_row[x]);
					//continue;
					if(iy_src+1<src.height())
					{
						src_channel_t const *next_src_row=reinterpret_cast<src_channel_t const *>(src.row_begin(iy_src+1));
						if(ix_src+1<src.width())
						{
							dst_row[x]=static_cast<dst_channel_t>(
								(
									src_row[ix_src]*
									(detail::constant::one<src_channel_t>()-frac_x)*
									(detail::constant::one<src_channel_t>()-frac_y)
								)+
								(
									src_row[ix_src+1]*
									frac_x*
									(detail::constant::one<src_channel_t>()-frac_y)
								)+
								(
									next_src_row[ix_src]*
									(detail::constant::one<src_channel_t>()-frac_x)*
									frac_y
								)+
								(
									next_src_row[ix_src+1]*
									frac_x*
									frac_y
								)
							);
						}
						else
						{
							dst_row[x]=static_cast<dst_channel_t>(
								(
									src_row[ix_src]*
									(detail::constant::one<src_channel_t>()-frac_y)
								)+
								(
									next_src_row[ix_src]*
									frac_y
								)
							);
						}
					}
					else
					{
						if(ix_src+1<src.width())
						{
							dst_row[x]=static_cast<dst_channel_t>(
								(
									src_row[ix_src]*
									(detail::constant::one<src_channel_t>()-frac_x)
								)+
								(
									src_row[ix_src+1]*
									frac_x
								)
							);
						
						}
						else
						{
							dst_row[x]=static_cast<dst_channel_t>(src_row[ix_src]);
						}
					}
				}
			}
		}
		return true;
	}

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_WARP_H
