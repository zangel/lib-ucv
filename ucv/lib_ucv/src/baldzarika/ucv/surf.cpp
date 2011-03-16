#include <baldzarika/ucv/config.h>
#include <baldzarika/ucv/surf.h>
#include <baldzarika/ucv/integral.h>

namespace baldzarika { namespace ucv  {

	namespace {
		
		static surf::response_t const gauss_25[7][7]=
		{
			{	0.02350693969273,	0.01849121369071,	0.01239503121241,	0.00708015417522,	0.00344628101733,	0.00142945847484,	0.00050524879060	},
			{	0.02169964028389,	0.01706954162243,	0.01144205592615,	0.00653580605408,	0.00318131834134,	0.00131955648461,	0.00046640341759	},
			{	0.01706954162243,	0.01342737701584,	0.00900063997939,	0.00514124713667,	0.00250251364222,	0.00103799989504,	0.00036688592278	},
			{	0.01144205592615,	0.00900063997939,	0.00603330940534,	0.00344628101733,	0.00167748505986,	0.00069579213743,	0.00024593098864	},
			{	0.00653580605408,	0.00514124713667,	0.00344628101733,	0.00196854695367,	0.00095819467066,	0.00039744277546,	0.00014047800980	},
			{	0.00318131834134,	0.00250251364222,	0.00167748505986,	0.00095819467066,	0.00046640341759,	0.00019345616757,	0.00006837798818	},
			{	0.00131955648461,	0.00103799989504,	0.00069579213743,	0.00039744277546,	0.00019345616757,	0.00008024231247,	0.00002836202103	}
		};

		class orientation_indices
		{
		private:
			orientation_indices()
			{
				boost::int32_t idx=0;
				for(boost::int32_t i=-6;i<=6;++i)
				{
					for(boost::int32_t j=-6;j<=6;++j)
					{
						if(i*i+j*j<36)
						{
							m_values[idx][0]=i;
							m_values[idx][1]=j;
							idx++;
						}
					}
				}
			}

		public:
			static orientation_indices const& get()
			{
				static orientation_indices _orientation_indices;
				return _orientation_indices;
			}
		public:
			boost::int32_t	m_values[109][2];
		};

	} //namespace anonymous

	
	surf::response_layer::response_layer(response_layer const &that)
		: m_surf(that.m_surf)
	{
		*this=that;
	}

	surf::response_layer::response_layer(surf &sr, point2ui const &ro, surf::response_view_t rv, boost::uint32_t ss, boost::uint32_t fs)
		: m_surf(sr)
		, m_response_offset(ro)
		, m_response_view(rv)
		, m_sample_step(ss)
		, m_filter_size(fs)
	{
	}

	surf::response_layer::~response_layer()
	{
	}

	surf::response_layer& surf::response_layer::operator =(response_layer const &rhs)
	{
		m_response_offset=rhs.m_response_offset;
		m_response_view=rhs.m_response_view;
		m_sample_step=rhs.m_sample_step;
		m_filter_size=rhs.m_filter_size;
		return *this;
	}

	surf::response_layer::operator bool() const
	{
		return !m_response_view.size();
	}

	bool surf::response_layer::build()
	{
		static response_t const three(3.0f);
		static response_t const coeff(0.81f);
		
		integral_view_t iv=gil::view(m_surf.m_integral_img);
		boost::int32_t const b=(m_filter_size-1)/2+1;
		boost::int32_t const l=m_filter_size/3;
		boost::int32_t const w=m_filter_size;
		response_t const s_coeff=(response_t(1)/response_t(w))/response_t(w);
		
		for(boost::int32_t ry=0;ry<m_response_view.height();++ry) 
		{
			response_pixel_t *r_row=m_response_view.row_begin(ry);
			
			boost::int32_t iy=ry*m_sample_step;
			for(boost::int32_t rx=0;rx<m_response_view.width();++rx) 
			{
				boost::int32_t ix=rx*m_sample_step;
				response_t d_xx=
					box_integral<integral_view_t,response_t>(iv, point2i(ix-b, iy-l+1), size2ui(w, 2*l-1))-
					box_integral<integral_view_t,response_t>(iv, point2i(ix-l/2, iy-l+1), size2ui(l, 2*l-1))*three;
				//float d_xx_=d_xx;
				
				response_t d_yy=
					box_integral<integral_view_t,response_t>(iv, point2i(ix-l+1, iy-b), size2ui(2*l-1, w))-
					box_integral<integral_view_t,response_t>(iv, point2i(ix-l+1, iy-l/2), size2ui(2*l-1, l))*three;
				//float d_yy_=d_yy;
				
				response_t d_xy=
					box_integral<integral_view_t,response_t>(iv, point2i(ix+1, iy-l), size2ui(l, l))+
					box_integral<integral_view_t,response_t>(iv, point2i(ix-l, iy+1), size2ui(l, l))-
					box_integral<integral_view_t,response_t>(iv, point2i(ix-l, iy-l), size2ui(l, l))-
					box_integral<integral_view_t,response_t>(iv, point2i(ix+1, iy+1), size2ui(l, l));

				//float d_xy_=d_xy;
				
				d_xx*=s_coeff;
				d_yy*=s_coeff;
				d_xy*=s_coeff;

				//float s_coeff_=s_coeff;

				//float f_d_xx=d_xx;
				//float f_d_yy=d_yy;
				//float f_d_xy=d_xy;

				*r_row++=(d_xx*d_yy-coeff*d_xy*d_xy);
			}
		}
		return true;
	}

	bool surf::response_layer::detect(response_layer &bl, response_layer &ml, std::vector<feature_point_t> &fps)
	{
		static const response_t inv_2=detail::constants::one<response_t>()/response_t(2);
		static const response_t inv_4=detail::constants::one<response_t>()/response_t(4);
		static const response_t r_2=2;
		static const response_t coeff_1=0.1333f;
		static response_t const det_eps=1.0e-3f;
		static response_t const prec_s=500;

		response_t const response_treshold(m_surf.m_treshold);
		boost::int32_t border=(m_filter_size+1)/(2*m_sample_step);

		for(boost::int32_t y=border+1;y<m_response_view.height()-border;++y)
		{
			for(boost::int32_t x=border+1;x<m_response_view.width()-border;++x)
			{
				response_t cur_response=ml.get_response(x,y, *this);
				//float f_cur_response=cur_response;
				//float f_treshold=m_surf.m_treshold;
				if(cur_response<response_treshold) continue;
				bool fp_detected(true);
				for(boost::int32_t ry=-1;ry<=1 && fp_detected;++ry)
				{
					for(boost::int32_t rx=-1;rx<=1 && fp_detected;++rx)
					{
						fp_detected=!
						(	m_response_view(x+rx,y+ry).operator response_t()>=cur_response ||
							( (ry!=0||rx!=0) && ml.get_response(x+rx, y+ry, *this)>=cur_response ) ||
							bl.get_response(x+rx, y+ry, *this)>=cur_response
						);
					}
				}

				if(fp_detected)
				{
					
					response_t v=ml.get_response(x, y, *this);
					response_t v_r_2=v*r_2;
					response_t d_x=prec_s*(ml.get_response(x+1, y, *this)-ml.get_response(x-1, y, *this))*inv_2;
					response_t d_y=prec_s*(ml.get_response(x, y+1, *this)-ml.get_response(x, y-1, *this))*inv_2;
					response_t d_s=prec_s*(cur_response-bl.get_response(x, y, *this))*inv_2;
					
					//H[0][0]
					response_t d_xx=prec_s*(ml.get_response(x+1, y, *this)+ml.get_response(x-1, y, *this)-v_r_2);
					//float f_d_xx=d_xx;

					//H[1][1]
					response_t d_yy=prec_s*(ml.get_response(x, y+1, *this)+ml.get_response(x, y-1, *this)-v_r_2);
					//float f_d_yy=d_yy;

					//H[2][2]			
					response_t d_ss=prec_s*(cur_response+bl.get_response(x, y, *this)-v_r_2);
					//float f_d_ss=d_ss;
					
					//H[0][1] H[1][0]
					response_t d_xy=prec_s*
						(
							ml.get_response(x+1, y+1, *this)-ml.get_response(x-1, y+1, *this)-
							ml.get_response(x+1, y-1, *this)+ml.get_response(x-1, y-1, *this)
						)*inv_4;
					//float f_d_xy=d_xy;
					
					//H[0][2] H[2][0]
					response_t d_xs=prec_s*
						(
							m_response_view(x+1, y).operator response_t()-m_response_view(x-1, y).operator response_t()-
							bl.get_response(x+1, y, *this)+bl.get_response(x-1, y, *this)
						)*inv_4;
					//float f_d_xs=d_xs;
					
					//H[1][2] H[2][1]
					response_t d_ys=prec_s*
						(
							m_response_view(x, y+1).operator response_t()-m_response_view(x, y-1).operator response_t()-
							bl.get_response(x, y+1, *this)+bl.get_response(x, y-1, *this)
						)*inv_4;
					//float f_d_ys=d_ys;

					response_t det=
						(
							d_xx*d_yy*d_ss+
							d_xy*d_ys*d_xs+
							d_xs*d_xy*d_ys-
							d_xx*d_ys*d_ys-
							d_xy*d_xy*d_ss-
							d_xs*d_yy*d_xs
						);
					//float f_det=det;

					
					if(fabs(det)<det_eps) continue;
					response_t inv_det=detail::constants::one<response_t>()/det;

					//float f_inv_det=inv_det;
					
					response_t H_inv[3][3]=
						{
							{  (d_yy*d_ss-d_ys*d_ys), -(d_xy*d_ss-d_xs*d_ys),  (d_xy*d_ys-d_xs*d_yy)  },
							{ -(d_xy*d_ss-d_ys*d_xs),  (d_xx*d_ss-d_xs*d_xs), -(d_xx*d_ys-d_xs*d_xy)  },
							{  (d_xy*d_ys-d_yy*d_xs), -(d_xx*d_ys-d_xy*d_xs),  (d_xx*d_yy-d_xy*d_xy)  }
						};

					response_t xx=-inv_det*(H_inv[0][0]*d_x+H_inv[0][1]*d_y+H_inv[0][2]*d_s);
					response_t xy=-inv_det*(H_inv[1][0]*d_x+H_inv[1][1]*d_y+H_inv[1][2]*d_s);
					response_t xi=-inv_det*(H_inv[2][0]*d_x+H_inv[2][1]*d_y+H_inv[2][2]*d_s);


					//float f_xx=xx;
					//float f_xy=xy;
					//float f_xi=xi;
					
					if(	fabs(xx)<detail::constants::one<response_t>() && fabs(xy)<detail::constants::one<response_t>() && fabs(xi)<detail::constants::one<response_t>())
					{
						fps.push_back(
							feature_point_t(
								feature_point_t::point2_t(
									(xx+response_t(x))*response_t(m_sample_step),
									(xy+response_t(y))*response_t(m_sample_step)
								),
								coeff_1*(response_t(ml.m_filter_size)+xi*response_t(ml.m_filter_size-bl.m_filter_size))
							)
						);
					}
				}
			}
		}
		return true;
	}

	surf::response_t surf::response_layer::get_response(boost::int32_t x, boost::int32_t y, response_layer const &src) const
	{
		boost::int32_t scale=m_response_view.width()/src.m_response_view.width();
		return m_response_view(scale*x, scale*y).operator response_t();
	}

	point2ui surf::response_layer::get_offset() const
	{
		return m_response_offset;
	}

	size2ui surf::response_layer::get_size() const
	{
		return size2ui(m_response_view.width(), m_response_view.height());
	}

	boost::uint32_t surf::response_layer::get_sample_step() const
	{
		return m_sample_step;
	}

	boost::uint32_t	surf::response_layer::get_filter_size() const
	{
		return m_filter_size;
	}

	boost::uint32_t const surf::FILTER_MAP[MAX_OCTAVES][MAX_INTERVALS]=
	{
		{	0,	1,	2,	3	},
		{	1,	3,	4,	5	},
		{	3,	5,	6,	7	},
		{	5,	7,	8,	9	},
		{	7,	9,	10,	11	}
	};
	
	surf::surf(size2ui const &is, boost::uint32_t o, boost::uint32_t i, boost::uint32_t s, float t, boost::uint32_t mf/* =64 */)
		: m_integral_img()
		, m_octaves(o)
		, m_intervals(i)
		, m_sample_step(s)
		, m_treshold(t)
		, m_max_features(mf)
	{
		resize(is);
	}

	surf::~surf()
	{
	
	}

	bool surf::resize(size2ui const &is)
	{
		if(is.empty())
			return false;
		m_integral_img.recreate(is.width(),is.height(),4);

		m_response_layers.clear();

		boost::uint32_t	width=is.width()/m_sample_step;
		boost::uint32_t	height=is.height()/m_sample_step;

		m_response_img.recreate(2*width, 2*height+height/2);

		if(m_octaves>=1)
		{
			m_response_layers.push_back(
				response_layer(
					*this,
					point2ui(0, 0),
					gil::subimage_view(gil::view(m_response_img), 0, 0, width, height),
					m_sample_step, 9
				)
			);
			m_response_layers.push_back(
				response_layer(
					*this,
					point2ui(width, 0),
					gil::subimage_view(gil::view(m_response_img), width, 0, width, height),
					m_sample_step, 15
				)
			);
			m_response_layers.push_back(
				response_layer(
					*this,
					point2ui(0, height),
					gil::subimage_view(gil::view(m_response_img), 0, height, width, height),
					m_sample_step, 21
				)
			);
			m_response_layers.push_back(
				response_layer(
					*this,
					point2ui(width, height),
					gil::subimage_view(gil::view(m_response_img), width, height, width, height),
					m_sample_step, 27
				)
			);
		}

		if(m_octaves>=2)
		{
			m_response_layers.push_back(
				response_layer(
					*this,
					point2ui(0, 2*height),
					gil::subimage_view(gil::view(m_response_img), 0, 2*height, width/2, height/2),
					m_sample_step*2, 39
				)
			);
			m_response_layers.push_back(
				response_layer(
					*this,
					point2ui(width/2, 2*height),
					gil::subimage_view(gil::view(m_response_img), width/2, 2*height, width/2, height/2),
					m_sample_step*2, 51
				)
			);
		}

		if(m_octaves>=3)
		{
			m_response_layers.push_back(
				response_layer(
					*this,
					point2ui(2*(width/2), 2*height),
					gil::subimage_view(gil::view(m_response_img), 2*(width/2), 2*height, width/4, height/4),
					m_sample_step*4, 75
				)
			);
			m_response_layers.push_back(
				response_layer(
					*this,
					point2ui(2*(width/2)+width/4, 2*height),
					gil::subimage_view(gil::view(m_response_img), 2*(width/2)+width/4, 2*height, width/4, height/4),
					m_sample_step*4, 99
				)
			);
		}

		if(m_octaves>=4)
		{
			m_response_layers.push_back(
				response_layer(
					*this,
					point2ui(2*(width/2)+2*(width/4), 2*height),
					gil::subimage_view(gil::view(m_response_img), 2*(width/2)+2*(width/4), 2*height, width/8, height/8),
					m_sample_step*8, 147
				)
			);
			m_response_layers.push_back(
				response_layer(
					*this,
					point2ui(2*(width/2)+2*(width/4)+width/8, 2*height),
					gil::subimage_view(gil::view(m_response_img), 2*(width/2)+2*(width/4)+width/8, 2*height, width/8, height/8),
					m_sample_step*8, 195
				)
			);
		}

		if(m_octaves>=5)
		{
			m_response_layers.push_back(
				response_layer(
					*this,
					point2ui(2*(width/2)+2*(width/4)+width/8+2*(width/8), 2*height),
					gil::subimage_view(gil::view(m_response_img), 2*(width/2)+2*(width/4)+width/8+2*(width/8), 2*height, width/16, height/16),
					m_sample_step*16, 291
				)
			);
			m_response_layers.push_back(
				response_layer(
					*this,
					point2ui(2*(width/2)+2*(width/4)+width/8+2*(width/8)+width/16, 2*height),
					gil::subimage_view(gil::view(m_response_img), 2*(width/2)+2*(width/4)+width/8+2*(width/8)+width/16, 2*height, width/16, height/16),
					m_sample_step*16, 387
				)
			);
		}
		return true;
	}


	bool surf::update(gray_view_t gi)
	{
		if(!ucv::integral(gi, gil::view(m_integral_img)))
			return false;

		if(m_response_layers.empty())
			return false;

		for(std::size_t rl=0;rl<m_response_layers.size();++rl)
			if(!m_response_layers[rl].build())
				return false;
		
		return true;
	}

	bool surf::detect(std::vector<feature_point_t> &fps)
	{
		fps.clear();
		for(boost::uint32_t o=0;o<m_octaves;++o)
		{
			for(boost::uint32_t i=0;i<2;++i)
			{
				response_layer &bl=m_response_layers[FILTER_MAP[o][i+0]];
				response_layer &ml=m_response_layers[FILTER_MAP[o][i+1]];
				response_layer &tl=m_response_layers[FILTER_MAP[o][i+2]];
				tl.detect(bl, ml, fps);
			}
		}
		return true;
	}

	bool surf::describe(std::vector<feature_point_t> &fps)
	{
		if(!compute_orientations(fps))
			return false;
		if(!compute_descriptors(fps))
			return false;
		return true;
	}

	bool surf::compute_orientations(std::vector<feature_point_t> &fps)
	{
		static response_t const r_3i20=0.15f;
		static response_t const r_2=2.0f;

		for(std::size_t ifp=0;ifp<fps.size();++ifp)
		{
			point2ui pt(
				static_cast<boost::uint32_t>(floor(fps[ifp].x+detail::constants::half<feature_point_t::value_type>())),
				static_cast<boost::uint32_t>(floor(fps[ifp].y+detail::constants::half<feature_point_t::value_type>()))
			);
			response_t scale=fps[ifp].m_scale;
			boost::uint32_t s=static_cast<boost::uint32_t>(floor(fps[ifp].m_scale+detail::constants::half<feature_point_t::value_type>()));

			response_t res_x[109], res_y[109], angle[109];
			for(boost::uint32_t k=0;k<109;++k)
			{
				response_t gauss=gauss_25[std::abs(orientation_indices::get().m_values[k][0])][std::abs(orientation_indices::get().m_values[k][1])];
				res_x[k]=gauss*haar_x(
					point2i(
						pt.x+orientation_indices::get().m_values[k][0]*s,
						pt.y+orientation_indices::get().m_values[k][1]*s
					),
					4*s
				);
				//float f_res_x=res_x[k];

				res_y[k]=gauss*haar_y(
					point2i(
						pt.x+orientation_indices::get().m_values[k][0]*s,
					pt.y+orientation_indices::get().m_values[k][1]*s
					),
					4*s
				);
				//float f_res_y=res_y[k];
				
				angle[k]=get_angle(res_x[k],res_y[k]);
				//float f_angle=angle[k];
				//int c=0;
			}

			response_t max_sum=detail::constants::zero<response_t>();
			response_t orientation=detail::constants::zero<response_t>();
			for(boost::uint32_t a=0;a<42;++a)
			{
				

				response_t ang1=response_t(a)*r_3i20;
				response_t ang2=((ang1+detail::constants::pi_i3<response_t>())>detail::constants::pi_2<response_t>()?
					ang1-detail::constants::pi_5i3<response_t>():
					ang1+detail::constants::pi_i3<response_t>());

				response_t sum_x=detail::constants::zero<response_t>();
				response_t sum_y=detail::constants::zero<response_t>();

				for(boost::uint32_t k=0;k<109;++k)
				{
					response_t const &ang=angle[k];
					if(ang1<ang2 && ang1<ang && ang<ang2)
					{
						sum_x+=res_x[k];  
						sum_y+=res_y[k];
					} 
					else
					if(ang2<ang1 && ((ang>detail::constants::zero<response_t>() && ang<ang2) || (ang>ang1 && ang<detail::constants::pi_2<response_t>())))
					{
						sum_x+=res_x[k];  
						sum_y+=res_y[k];
					}
				}
				response_t this_sum=sum_x*sum_x+sum_y*sum_y;
				if(this_sum>max_sum) 
				{
					max_sum=this_sum;
					orientation=get_angle(sum_x, sum_y);
				}
			}
			//float f_orientation=orientation;
			fps[ifp].m_orientation=orientation;
		}
		return true;
	}

	bool surf::compute_descriptors(std::vector<feature_point_t> &fps)
	{
		for(std::size_t ifp=0;ifp<fps.size();++ifp)
		{
			point2ui pt(
				static_cast<boost::uint32_t>(floor(fps[ifp].x+detail::constants::half<feature_point_t::value_type>())),
				static_cast<boost::uint32_t>(floor(fps[ifp].y+detail::constants::half<feature_point_t::value_type>()))
			);
			response_t scale=fps[ifp].m_scale;
		}
		return true;
	}

	surf::response_t surf::haar_x(point2i const &p, boost::uint32_t s)
	{
		return	box_integral<integral_view_t, response_t>(gil::view(m_integral_img), point2i(p.x,p.y-s/2), size2ui(s/2, s))-
				box_integral<integral_view_t, response_t>(gil::view(m_integral_img), point2i(p.x-s/2,p.y-s/2), size2ui(s/2, s));
	}
	
	surf::response_t surf::haar_y(point2i const &p, boost::uint32_t s)
	{
		return	box_integral<integral_view_t, response_t>(gil::view(m_integral_img), point2i(p.x-s/2,p.y), size2ui(s, s/2))-
				box_integral<integral_view_t, response_t>(gil::view(m_integral_img), point2i(p.x-s/2,p.y-s/2), size2ui(s, s/2));
	}

	surf::response_t surf::get_angle(surf::response_t const &x, surf::response_t const &y)
	{
		if(y<detail::constants::zero<response_t>())
		{
			if(x<detail::constants::zero<response_t>()) return detail::constants::pi<response_t>()+atan2(y,x);
			if(x>detail::constants::zero<response_t>()) return detail::constants::pi_2<response_t>()-atan2(-y,x);
		}
		else
		{
			if(x<detail::constants::zero<response_t>()) return detail::constants::pi<response_t>()-atan2(-y,x);
			if(x>detail::constants::zero<response_t>()) return atan2(y,x);
		}
		return detail::constants::zero<response_t>();
	}

	surf::response_t surf::gaussian(boost::int32_t x, boost::int32_t y, response_t const &sig)
	{
		return detail::constants::half<response_t>()/(detail::constants::pi<response_t>()*sig*sig)*
			exp(response_t(-(x*x+y*y))*detail::constants::half<response_t>()/(sig*sig));
	}
	
	surf::response_t surf::gaussian(response_t const &x, response_t const &y, response_t const &sig)
	{
		return detail::constants::half<response_t>()/(detail::constants::pi<response_t>()*sig*sig)*
			exp(-(x*x+y*y)*detail::constants::half<response_t>()/(sig*sig));
	}

} //namespace ucv
} //namespace baldzarika
