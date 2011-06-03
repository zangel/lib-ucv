#ifndef BALDZARIKA_UCV_CORNERS_SUBPIXEL_H
#define BALDZARIKA_UCV_CORNERS_SUBPIXEL_H

#include <baldzarika/ucv/point2.h>

namespace baldzarika { namespace ucv {

	template < typename SVT, typename PT >
	void corners_subpixel(SVT src, std::vector< math::point2<PT> > &corners, boost::uint32_t max_iters=2, PT eps=1.0e-3)
	{
		typedef typename SVT::value_type	src_pixel_t;
		typedef typename gil::channel_type<src_pixel_t>::type src_channel_t;

		static PT const mask[5][5]=
		{
			{ PT(0.13533528323661269189399949497248), PT(0.28650479686019010032488542664784),	PT(0.36787944117144232159552377016146), PT(0.28650479686019010032488542664784), PT(0.13533528323661269189399949497248) },
			{ PT(0.28650479686019010032488542664784), PT(0.60653065971263342360379953499118),	PT(0.77880078307140486824517026697832), PT(0.60653065971263342360379953499118), PT(0.28650479686019010032488542664784) },
			{ PT(0.36787944117144232159552377016146), PT(0.77880078307140486824517026697832),	PT(1.0),								PT(0.77880078307140486824517026697832),	PT(0.36787944117144232159552377016146) },
			{ PT(0.28650479686019010032488542664784), PT(0.60653065971263342360379953499118),	PT(0.77880078307140486824517026697832), PT(0.60653065971263342360379953499118), PT(0.28650479686019010032488542664784) },
			{ PT(0.13533528323661269189399949497248), PT(0.28650479686019010032488542664784),	PT(0.36787944117144232159552377016146), PT(0.28650479686019010032488542664784), PT(0.13533528323661269189399949497248) },
		};

		src_channel_t win[7][7];


		for(boost::uint32_t p=0;p<corners.size();++p)
		{
			math::point2<PT> cT=corners[p];
			math::point2<PT> cI=cT;
			boost::uint32_t iter=0;
			PT err=detail::constant::zero<PT>();

			do
			{
				if( !(cI.x>detail::constant::two<PT>() && cI.x<PT(src.width()-2) && cI.y>detail::constant::two<PT>() && cI.x<PT(src.height()-2)))
					break;
				
				boost::uint32_t ix=static_cast<boost::uint32_t>(cI.x);
				boost::uint32_t iy=static_cast<boost::uint32_t>(cI.y);

				src_channel_t frac_x=static_cast<src_channel_t>(cI.x-PT(ix));
				src_channel_t frac_y=static_cast<src_channel_t>(cI.y-PT(iy));

				//subpixel window sampling
				for(boost::uint32_t y=0;y<7;++y)
				{
					src_channel_t const *src_row=reinterpret_cast<src_channel_t const *>(src.row_begin(iy+y-3))+ix-3;
					src_channel_t const *nxt_src_row=reinterpret_cast<src_channel_t const *>(src.row_begin(iy+y+1-3))+ix-3;
					
					for(boost::uint32_t x=0;x<7;++x)
					{
						win[y][x]=static_cast<src_channel_t>(
							(
								src_row[x]*
								(detail::constant::one<src_channel_t>()-frac_x)*
								(detail::constant::one<src_channel_t>()-frac_y)
							)+
							(
								src_row[x+1]*
								frac_x*
								(detail::constant::one<src_channel_t>()-frac_y)
							)+
							(
								nxt_src_row[x]*
								(detail::constant::one<src_channel_t>()-frac_x)*
								frac_y
							)+
							(
								nxt_src_row[x+1]*
								frac_x*
								frac_y
							)
						);
					}
				}

				//gradients and solving
				PT a=detail::constant::zero<PT>();
				PT b=detail::constant::zero<PT>();
				PT c=detail::constant::zero<PT>();
				PT bb1=detail::constant::zero<PT>();
				PT bb2=detail::constant::zero<PT>();
				
				for(boost::int32_t y=0;y<5;++y)
				{
					PT py=PT(y-2);
					for(boost::int32_t x=0;x<5;++x)
					{
						PT grad_x=PT((win[y+1][x+2]-win[y+1][x]).operator>>(1));
						PT grad_y=PT((win[y+2][x+1]-win[y][x+1]).operator>>(1));

						PT gxx=grad_x*grad_x*mask[y][x];
						PT gxy=grad_x*grad_y*mask[y][x];
						PT gyy=grad_y*grad_y*mask[y][x];
						PT px=PT(x-2);

						a+=gxx;
						b+=gxy;
						c+=gyy;

						bb1+=gxx*px+gxy*py;
						bb2+=gxy*px+gyy*py;
					}
				}

				PT det=a*c-b*b;
				if(det!=detail::constant::zero<PT>())
				{
					det=detail::constant::one<PT>()/det;
					PT t00=c*det;
					PT t01=-b*det;
					PT t10=-b*det;
					PT t11=a*det;

					math::point2<PT> cI2=math::point2<PT>(
						cI.x+t00*bb1+t01*bb2,
						cI.y+t10*bb1+t11*bb2
					);
					
					err=(cI2.x-cI.x)*(cI2.x-cI.x)+(cI2.y-cI.y)*(cI2.y-cI.y);
					cI=cI2;
				}
				else
					break;
			}
			while(iter++<max_iters && err>eps);

			if(!(std::abs(cI.x-cT.x)>detail::constant::three<PT>() || std::abs(cI.y-cT.y)>detail::constant::three<PT>()))
				corners[p]=cI;
		}
	}

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_CORNERS_SUBPIXEL_H