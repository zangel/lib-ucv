#include <baldzarika/ucv/config.h>
#include <baldzarika/ucv/surf.h>
#include <baldzarika/ucv/integral.h>
#include <baldzarika/ucv/haar_wavelets.h>
#include <baldzarika/ucv/estimate_rotation.h>
#include <baldzarika/ucv/extract_surf_descriptor.h>

namespace baldzarika { namespace ucv  {

	namespace {
		
		template < boost::uint32_t I, boost::uint32_t F, boost::uint32_t S, boost::uint32_t SS >
		class gauss_25_lut
		{
		public:
			typedef math::fixed_point<I,F> value_type;
		private:
			gauss_25_lut()
			{
				float sig2=6.25f;
				
				//return (1.0f/(2.0f*math::constants::pi<float>()*sig_*sig_))*std::exp(-(x_*x_+y_*y_)/(2.0f*sig_*sig_));

				for(boost::uint32_t y=0;y<S*SS;++y)
				{
					for(boost::uint32_t x=0;x<S*SS;++x)
					{
						m_lut[y][x]=value_type(
							(1.0f/(2.0f*math::constant::pi<float>()*sig2))*
							std::exp(-float(x*x+y*y)/(2.0f*sig2*float(SS*SS)))
						);
					}
				}
			}
		public:
			static gauss_25_lut const& get()
			{
				static gauss_25_lut _gauss_25_lut;
				return _gauss_25_lut;
			}

			template < boost::uint32_t I1, boost::uint32_t F1 >
			value_type operator()(math::fixed_point<I1,F1> const &x, math::fixed_point<I1,F1> const &y) const
			{
				static math::fixed_point<I1,F1> const _scale=SS;
				BOOST_ASSERT(std::round<boost::uint32_t>(std::abs(x*_scale))<S*SS);
				BOOST_ASSERT(std::round<boost::uint32_t>(std::abs(y*_scale))<S*SS);
				return m_lut[std::round<boost::uint32_t>(std::abs(y*_scale))][std::round<boost::uint32_t>(std::abs(x*_scale))];
			}
		
		private:
			value_type	m_lut[S*SS][S*SS];
		};

		static void append_feature_point(surf::feature_point_t::point2_t const &p, surf::feature_point_t::desc_value_type const &s, bool l, std::vector<surf::feature_point_t> &fps)
		{
			fps.push_back(surf::feature_point_t(p,s,l));
		}

		template <typename KDTREET>
		static void append_feature_point2(surf::feature_point_t::point2_t const &p, surf::feature_point_t::desc_value_type const &s, bool l, KDTREET &fps)
		{
			//std::pair<surf::feature_point_tree_t::const_iterator,surf::feature_point_t::accessor::result_type> nearest=
			//	fps.find_nearest(p, std::numeric_limits<surf::feature_point_t::accessor::result_type>::epsilon());
			//if(nearest.first==fps.end())
			fps.insert(surf::feature_point_t(p,s,l));
		}

		static inline bool intersects(
			boost::int32_t min_x1, boost::int32_t max_x1, boost::int32_t min_y1, boost::int32_t max_y1,
			boost::int32_t min_x2, boost::int32_t max_x2, boost::int32_t min_y2, boost::int32_t max_y2
		)
		{
			boost::int32_t min_x=std::max(min_x1, min_x2); boost::int32_t max_x=std::min(max_x1, max_x2);
			boost::int32_t min_y=std::max(min_y1, min_y2); boost::int32_t max_y=std::min(max_y1, max_y2);

			return (max_y-min_y)>0 && (max_x-min_x)>0;
		}


	} //namespace anonymous

	
	boost::uint32_t const surf::SAMPLES_PER_BLOCK=9;
	surf::response_layer::response_layer(response_layer const &that)
		: m_surf(that.m_surf)
	{
		*this=that;
	}

	surf::response_layer::response_layer(surf &sr, math::point2ui const &ro, surf::response_view_t rv, surf::laplacian_view_t lv, boost::uint32_t ss, boost::uint32_t fs)
		: m_surf(sr)
		, m_response_offset(ro)
		, m_response_view(rv)
		, m_laplacian_view(lv)
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
		m_laplacian_view=rhs.m_laplacian_view;
		m_sample_step=rhs.m_sample_step;
		m_filter_size=rhs.m_filter_size;
		return *this;
	}

	surf::response_layer::operator bool() const
	{
		return !m_response_view.size();
	}

	void surf::response_layer::build()
	{
		static response_t const three(3.0f);
		static response_t const coeff(0.81f);
		
		boost::int32_t const b=(m_filter_size-1)/2+1;
		boost::int32_t const l=m_filter_size/3;
		boost::int32_t const w=m_filter_size;
		response_t const s_coeff=(response_t(1)/response_t(w))/response_t(w);

		boost::int32_t const l1=2*l-1;
		
		for(boost::int32_t ry=0;ry<m_response_view.height();++ry) 
		{
			response_pixel_t *r_row=m_response_view.row_begin(ry);
			laplacian_view_t::x_iterator l_row=m_laplacian_view.row_begin(ry);
			
			boost::int32_t iy=ry*m_sample_step;
			boost::int32_t iy1=iy-l;
			boost::int32_t iy2=iy1+1;
			boost::int32_t iy3=iy-l/2;
			boost::int32_t iy4=iy-b;
			boost::int32_t iy5=iy+1;


			for(boost::int32_t rx=0;rx<m_response_view.width();++rx) 
			{
				boost::int32_t ix=rx*m_sample_step;
				boost::int32_t ix1=ix-l;
				boost::int32_t ix2=ix1+1;
				boost::int32_t ix3=ix-l/2;
				boost::int32_t ix4=ix-b;
				boost::int32_t ix5=ix+1;

				response_t d_xx3=box_integral<const_integral_view_t,response_t>(m_surf.m_integral_view, ix3, iy2, l, l1);
				response_t d_xx=box_integral<const_integral_view_t,response_t>(m_surf.m_integral_view, ix4, iy2, w, l1)-
					(d_xx3.operator<<(1)+d_xx3);
				
				response_t d_yy3=box_integral<const_integral_view_t,response_t>(m_surf.m_integral_view, ix2, iy3, l1, l);
				response_t d_yy=box_integral<const_integral_view_t,response_t>(m_surf.m_integral_view, ix2, iy4, l1, w)-
					(d_yy3.operator<<(1)+d_yy3);
				
				response_t d_xy=
					box_integral<const_integral_view_t,response_t>(m_surf.m_integral_view, ix5, iy1, l, l)+
					box_integral<const_integral_view_t,response_t>(m_surf.m_integral_view, ix1, iy5, l, l)-
					box_integral<const_integral_view_t,response_t>(m_surf.m_integral_view, ix1, iy1, l, l)-
					box_integral<const_integral_view_t,response_t>(m_surf.m_integral_view, ix5, iy5, l, l);
	
				d_xx*=s_coeff;
				d_yy*=s_coeff;
				d_xy*=s_coeff;

				*r_row++=(d_xx*d_yy-coeff*d_xy*d_xy);
				*l_row++=((d_xx+d_yy)>=math::constant::zero<response_t>()?1:0);
			}
		}
	}

	void surf::response_layer::detect(response_layer &bl, response_layer &ml, ranged_detect_params_t const &rdp)
	{
		//static const response_t inv_2=math::constant::one<response_t>()/response_t(2);
		//static const response_t inv_4=math::constant::one<response_t>()/response_t(4);
		//static const response_t r_2=2;
		static const response_t coeff_1=0.1333f;
		static response_t const det_eps=1.0e-3f;
		//static response_t const prec_s=512;

		response_t const response_treshold(m_surf.m_treshold);
		boost::int32_t border=(m_filter_size+1)/(2*m_sample_step);


		boost::int32_t min_x=std::max<boost::int32_t>(border+1, rdp.first.first.x()/m_sample_step);
		boost::int32_t max_x=std::min<boost::int32_t>(m_response_view.width()-border, (rdp.first.first.x()+rdp.first.second.width())/m_sample_step);
		boost::int32_t min_y=std::max<boost::int32_t>(border+1, rdp.first.first.y()/m_sample_step);
		boost::int32_t max_y=std::min<boost::int32_t>(m_response_view.height()-border, (rdp.first.first.y()+rdp.first.second.height())/m_sample_step);
		
		for(boost::int32_t y=min_y;y<max_y;++y)
		{
			for(boost::int32_t x=min_x;x<max_x;++x)
			{
				response_t candidate=ml.get_response(x,y, *this);
				if(candidate<response_treshold) continue;
				bool fp_detected(true);
				for(boost::int32_t ry=-1;ry<=1 && fp_detected;++ry)
				{
					for(boost::int32_t rx=-1;rx<=1 && fp_detected;++rx)
					{
						fp_detected=!
						(	m_response_view(x+rx,y+ry).operator response_t()>=candidate ||
							( (ry!=0||rx!=0) && ml.get_response(x+rx, y+ry, *this)>=candidate ) ||
							bl.get_response(x+rx, y+ry, *this)>=candidate
						);
					}
				}

				if(fp_detected)
				{
					response_t t=m_response_view(x, y).operator response_t();
					response_t b=bl.get_response(x, y, *this);

					response_t v=ml.get_response(x, y, *this);
					response_t v_r_2=v.operator<<(1);
					response_t d_x=(ml.get_response(x+1, y, *this)-ml.get_response(x-1, y, *this)).operator<<(8);
					response_t d_y=(ml.get_response(x, y+1, *this)-ml.get_response(x, y-1, *this)).operator<<(8);
					response_t d_s=(t-b).operator<<(8);
					
					//H[0][0]
					response_t d_xx=(ml.get_response(x+1, y, *this)+ml.get_response(x-1, y, *this)-v_r_2).operator<<(9);

					//H[1][1]
					response_t d_yy=(ml.get_response(x, y+1, *this)+ml.get_response(x, y-1, *this)-v_r_2).operator<<(9);

					//H[2][2]
					response_t d_ss=(t+b-v_r_2).operator<<(9);
					
					//H[0][1] H[1][0]
					response_t d_xy=
					(
						ml.get_response(x+1, y+1, *this)-ml.get_response(x-1, y+1, *this)-
						ml.get_response(x+1, y-1, *this)+ml.get_response(x-1, y-1, *this)
					).operator<<(7);
					
					//H[0][2] H[2][0]
					response_t d_xs=
					(
						m_response_view(x+1, y).operator response_t()-m_response_view(x-1, y).operator response_t()-
						bl.get_response(x+1, y, *this)+bl.get_response(x-1, y, *this)
					).operator<<(7);

					
					//H[1][2] H[2][1]
					response_t d_ys=
					(
						m_response_view(x, y+1).operator response_t()-m_response_view(x, y-1).operator response_t()-
						bl.get_response(x, y+1, *this)+bl.get_response(x, y-1, *this)
					).operator<<(7);
					

					response_t det=
					(
						d_xx*d_yy*d_ss+
						d_xy*d_ys*d_xs+
						d_xs*d_xy*d_ys-
						d_xx*d_ys*d_ys-
						d_xy*d_xy*d_ss-
						d_xs*d_yy*d_xs
					);

					
					if(std::abs(det)<det_eps) continue;
					response_t inv_det=math::constant::one<response_t>()/det;
					response_t H_inv[3][3]=
					{
						{  (d_yy*d_ss-d_ys*d_ys), -(d_xy*d_ss-d_xs*d_ys),  (d_xy*d_ys-d_xs*d_yy)  },
						{ -(d_xy*d_ss-d_ys*d_xs),  (d_xx*d_ss-d_xs*d_xs), -(d_xx*d_ys-d_xs*d_xy)  },
						{  (d_xy*d_ys-d_yy*d_xs), -(d_xx*d_ys-d_xy*d_xs),  (d_xx*d_yy-d_xy*d_xy)  }
					};

					response_t xx=-inv_det*(H_inv[0][0]*d_x+H_inv[0][1]*d_y+H_inv[0][2]*d_s);
					response_t xy=-inv_det*(H_inv[1][0]*d_x+H_inv[1][1]*d_y+H_inv[1][2]*d_s);
					response_t xi=-inv_det*(H_inv[2][0]*d_x+H_inv[2][1]*d_y+H_inv[2][2]*d_s);

					if(	std::abs(xx)<math::constant::half<response_t>() && std::abs(xy)<math::constant::half<response_t>() && std::abs(xi)<math::constant::half<response_t>())
					{
						rdp.second(
							feature_point_t::point2_t(
								(xx+response_t(x))*response_t(m_sample_step),
								(xy+response_t(y))*response_t(m_sample_step)
							),
							coeff_1*(response_t(ml.m_filter_size)+xi*response_t(ml.m_filter_size-bl.m_filter_size)),
							ml.get_laplacian(x,y, *this)
						);
					}
				}
			}
		}
	}

	surf::response_t surf::response_layer::get_response(boost::int32_t x, boost::int32_t y, response_layer const &src) const
	{
		boost::int32_t scale=m_response_view.width()/src.m_response_view.width();
		return m_response_view(scale*x, scale*y).operator response_t();
	}

	bool surf::response_layer::get_laplacian(boost::int32_t x, boost::int32_t y, response_layer const &src) const
	{
		boost::int32_t scale=m_response_view.width()/src.m_response_view.width();
		return gil::get_color(m_laplacian_view(scale*x, scale*y), gil::gray_color_t())?true:false;
	}

	math::point2ui surf::response_layer::get_offset() const
	{
		return m_response_offset;
	}

	math::size2ui surf::response_layer::get_size() const
	{
		return math::size2ui(m_response_view.width(), m_response_view.height());
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
	
	surf::surf(math::size2ui const &fs, boost::uint32_t o, boost::uint32_t i, boost::uint32_t s, float t, boost::uint32_t mf/* =64 */)
		: m_frame_size()
		, m_octaves(o)
		, m_intervals(i)
		, m_sample_step(s)
		, m_treshold(t)
		, m_max_features(mf)
	{
		resize(fs);
	}

	surf::~surf()
	{
	}

	boost::uint32_t surf::octaves() const
	{
		return m_octaves;
	}

	boost::uint32_t surf::intervals() const
	{
		return m_intervals;
	}

	boost::uint32_t surf::sample_step() const
	{
		return m_sample_step;
	}

	float surf::treshold() const
	{
		return m_treshold;
	}

	void surf::set_treshold(float t)
	{
		m_treshold=t;
	}
	
	bool surf::resize(math::size2ui const &fs)
	{
		if(fs.empty())
			return false;
		m_frame_size=fs;
		
		m_response_layers.clear();

		boost::uint32_t	width=fs.width()/m_sample_step;
		boost::uint32_t	height=fs.height()/m_sample_step;

		m_response_img.recreate(2*width, 2*height+height/2);
		m_laplacian_img.recreate(2*width, 2*height+height/2);
		
		if(m_octaves>=1)
		{
			m_response_layers.push_back(
				response_layer(
					*this,
					math::point2ui(0, 0),
					gil::subimage_view(gil::view(m_response_img), 0, 0, width, height),
					gil::subimage_view(gil::view(m_laplacian_img), 0, 0, width, height),
					m_sample_step, 9
				)
			);
			m_response_layers.push_back(
				response_layer(
					*this,
					math::point2ui(width, 0),
					gil::subimage_view(gil::view(m_response_img), width, 0, width, height),
					gil::subimage_view(gil::view(m_laplacian_img), width, 0, width, height),
					m_sample_step, 15
				)
			);
			m_response_layers.push_back(
				response_layer(
					*this,
					math::point2ui(0, height),
					gil::subimage_view(gil::view(m_response_img), 0, height, width, height),
					gil::subimage_view(gil::view(m_laplacian_img), 0, height, width, height),
					m_sample_step, 21
				)
			);
			m_response_layers.push_back(
				response_layer(
					*this,
					math::point2ui(width, height),
					gil::subimage_view(gil::view(m_response_img), width, height, width, height),
					gil::subimage_view(gil::view(m_laplacian_img), width, height, width, height),
					m_sample_step, 27
				)
			);
		}

		if(m_octaves>=2)
		{
			m_response_layers.push_back(
				response_layer(
					*this,
					math::point2ui(0, 2*height),
					gil::subimage_view(gil::view(m_response_img), 0, 2*height, width/2, height/2),
					gil::subimage_view(gil::view(m_laplacian_img), 0, 2*height, width/2, height/2),
					m_sample_step*2, 39
				)
			);
			m_response_layers.push_back(
				response_layer(
					*this,
					math::point2ui(width/2, 2*height),
					gil::subimage_view(gil::view(m_response_img), width/2, 2*height, width/2, height/2),
					gil::subimage_view(gil::view(m_laplacian_img), width/2, 2*height, width/2, height/2),

					m_sample_step*2, 51
				)
			);
		}

		if(m_octaves>=3)
		{
			m_response_layers.push_back(
				response_layer(
					*this,
					math::point2ui(2*(width/2), 2*height),
					gil::subimage_view(gil::view(m_response_img), 2*(width/2), 2*height, width/4, height/4),
					gil::subimage_view(gil::view(m_laplacian_img), 2*(width/2), 2*height, width/4, height/4),
					m_sample_step*4, 75
				)
			);
			m_response_layers.push_back(
				response_layer(
					*this,
					math::point2ui(2*(width/2)+width/4, 2*height),
					gil::subimage_view(gil::view(m_response_img), 2*(width/2)+width/4, 2*height, width/4, height/4),
					gil::subimage_view(gil::view(m_laplacian_img), 2*(width/2)+width/4, 2*height, width/4, height/4),
					m_sample_step*4, 99
				)
			);
		}

		if(m_octaves>=4)
		{
			m_response_layers.push_back(
				response_layer(
					*this,
					math::point2ui(2*(width/2)+2*(width/4), 2*height),
					gil::subimage_view(gil::view(m_response_img), 2*(width/2)+2*(width/4), 2*height, width/8, height/8),
					gil::subimage_view(gil::view(m_laplacian_img), 2*(width/2)+2*(width/4), 2*height, width/8, height/8),
					m_sample_step*8, 147
				)
			);
			m_response_layers.push_back(
				response_layer(
					*this,
					math::point2ui(2*(width/2)+2*(width/4)+width/8, 2*height),
					gil::subimage_view(gil::view(m_response_img), 2*(width/2)+2*(width/4)+width/8, 2*height, width/8, height/8),
					gil::subimage_view(gil::view(m_laplacian_img), 2*(width/2)+2*(width/4)+width/8, 2*height, width/8, height/8),
					m_sample_step*8, 195
				)
			);
		}

		if(m_octaves>=5)
		{
			m_response_layers.push_back(
				response_layer(
					*this,
					math::point2ui(2*(width/2)+2*(width/4)+width/8+2*(width/8), 2*height),
					gil::subimage_view(gil::view(m_response_img), 2*(width/2)+2*(width/4)+width/8+2*(width/8), 2*height, width/16, height/16),
					gil::subimage_view(gil::view(m_laplacian_img), 2*(width/2)+2*(width/4)+width/8+2*(width/8), 2*height, width/16, height/16),
					m_sample_step*16, 291
				)
			);
			m_response_layers.push_back(
				response_layer(
					*this,
					math::point2ui(2*(width/2)+2*(width/4)+width/8+2*(width/8)+width/16, 2*height),
					gil::subimage_view(gil::view(m_response_img), 2*(width/2)+2*(width/4)+width/8+2*(width/8)+width/16, 2*height, width/16, height/16),
					gil::subimage_view(gil::view(m_laplacian_img), 2*(width/2)+2*(width/4)+width/8+2*(width/8)+width/16, 2*height, width/16, height/16),
					m_sample_step*16, 387
				)
			);
		}
		return true;
	}

	math::size2ui const& surf::size() const
	{
		return m_frame_size;
	}

	bool surf::set_integral_view(const_integral_view_t iv)
	{
		if(iv.width()!=m_frame_size.width() || iv.height()!=m_frame_size.height())
			return false;
		m_integral_view=iv;
		return true;
	}

	void surf::build_response_layers()
	{
		BOOST_ASSERT(!m_response_layers.empty());
		for(std::size_t rl=0;rl<m_response_layers.size();++rl)
			m_response_layers[rl].build();
	}

	void surf::detect(std::vector<feature_point_t> &fps)
	{
		fps.clear();
		std::vector<ranged_detect_params_t> full_range_detect;
		full_range_detect.push_back(
			std::make_pair(
				std::make_pair(
					math::point2i(0,0),
					size()
				),
				boost::bind<void>(&append_feature_point, _1, _2, _3, boost::ref(fps) )
			)
		);
		ranged_detect(full_range_detect);
	}

	void surf::detect(surf::fps_by_pos_tree_t &fps)
	{
		fps.clear();
		std::vector<ranged_detect_params_t> full_range_detect;
		full_range_detect.push_back(
			std::make_pair(
				std::make_pair(
					math::point2i(0,0),
					size()
				),
				boost::bind<void>(&append_feature_point2<fps_by_pos_tree_t>, _1, _2, _3, boost::ref(fps) )
			)
		);
		ranged_detect(full_range_detect);
	}

	void surf::detect(surf::fps_by_desc_tree_t &fps)
	{
		fps.clear();
		std::vector<ranged_detect_params_t> full_range_detect;
		full_range_detect.push_back(
			std::make_pair(
				std::make_pair(
					math::point2i(0,0),
					size()
				),
				boost::bind<void>(&append_feature_point2<fps_by_desc_tree_t>, _1, _2, _3, boost::ref(fps) )
			)
		);
		ranged_detect(full_range_detect);
	}
/*
	bool surf::find(std::vector<feature_point_t::point2_t> const &gp, boost::uint32_t ws, feature_point_tree_t &fps)
	{
		fps.clear();
		std::vector<ranged_detect_params_t> find_ranged_detect;
		find_ranged_detect.push_back(
			std::make_pair(
				std::make_pair(
					math::point2i(0,0),
					size()
				),
				boost::bind<void>(&append_feature_point2, _1, _2, boost::ref(fps))
			)
		);
		ranged_detect(find_ranged_detect);
		return true;

		for(std::size_t gpid=0;gpid<gp.size();++gpid)
		{
			boost::int32_t x=round<boost::int32_t>(gp[gpid].x());
			boost::int32_t y=round<boost::int32_t>(gp[gpid].y());
			find_ranged_detect.push_back(
				std::make_pair(
					std::make_pair(
						math::point2i(x-ws, y-ws),
						math::size2ui(2*ws, 2*ws)
					),
					boost::bind<void>(&append_feature_point2, _1, _2, boost::ref(fps))
				)
			);
		}
#if 0
		//split intersecting regions
		std::list<ranged_detect_params_t>::iterator begin_r=find_ranged_detect.begin();
		std::list<ranged_detect_params_t>::iterator end_r=find_ranged_detect.end();
		while(begin_r!=end_r)
		{
			boost::int32_t min_x1=begin_r->first.first.x();
			boost::int32_t max_x1=begin_r->first.first.x()+begin_r->first.second.width();

			boost::int32_t min_y1=begin_r->first.first.y();
			boost::int32_t max_y1=begin_r->first.first.y()+begin_r->first.second.height();
			
			std::list<ranged_detect_params_t>::iterator cmp_r=begin_r; ++cmp_r;
			bool restarted=false;
			while(cmp_r!=end_r && !restarted)
			{
				boost::int32_t min_x2=cmp_r->first.first.x(); boost::int32_t max_x2=cmp_r->first.first.x()+cmp_r->first.second.width();
				boost::int32_t min_y2=cmp_r->first.first.y(); boost::int32_t max_y2=cmp_r->first.first.y()+cmp_r->first.second.height();

				if(intersects(min_x1,max_x1,min_y1,max_y1,min_x2,max_x2,min_y2,max_y2))
				{
					boost::int32_t xvals[4]={ min_x1, max_x1, min_x2, max_x2 }; std::sort(&xvals[0], &xvals[0]+4);
					boost::int32_t yvals[4]={ min_y1, max_y1, min_y2, max_y2 }; std::sort(&yvals[0], &yvals[0]+4);
					for(boost::uint32_t y=0;y<3;++y)
					{
						for(boost::uint32_t x=0;x<3;++x)
						{
							bool wf_intersects=intersects(xvals[x],xvals[x+1],yvals[y],yvals[y+1],min_x1,max_x1,min_y1,max_y1);
							bool ws_intersects=intersects(xvals[x],xvals[x+1],yvals[y],yvals[y+1],min_x2,max_x2,min_y2,max_y2);
							if(wf_intersects || ws_intersects)
							{
								std::list<ranged_detect_params_t>::iterator rng=find_ranged_detect.insert(
									find_ranged_detect.end(),
									std::make_pair(
										std::make_pair(
											math::point2i(xvals[x], yvals[y]),
											math::size2ui(xvals[x+1]-xvals[x],yvals[y+1]-yvals[y])
										),
										std::vector<point_detected_t>()
									)
								);
								if(wf_intersects) rng->second.insert(rng->second.end(), begin_r->second.begin(), begin_r->second.end());
								if(ws_intersects) rng->second.insert(rng->second.end(), cmp_r->second.begin(), cmp_r->second.end());
							}
						}
					}
					find_ranged_detect.erase(cmp_r);
					begin_r=find_ranged_detect.erase(begin_r);
					end_r=find_ranged_detect.end();
					restarted=true;
					break;
				}
				else
					++cmp_r;
			}
			if(!restarted) ++begin_r;
		}
#endif
		ranged_detect(find_ranged_detect);
		return true;
	}
*/
	void surf::describe(std::vector<feature_point_t> &fps)
	{
		for(std::size_t ifp=0;ifp<fps.size();++ifp)
		{
			compute_orientation(fps[ifp]);
			compute_descriptor(fps[ifp]);
		}
	}

	void surf::describe(surf::fps_by_pos_tree_t &fps)
	{
		for(fps_by_pos_tree_t::iterator itfp=fps.begin();itfp!=fps.end();++itfp)
		{
			compute_orientation(const_cast<feature_point_t&>(*itfp));
			compute_descriptor(const_cast<feature_point_t&>(*itfp));
		}
	}

	void surf::describe(fps_by_desc_tree_t &fps)
	{
		for(fps_by_desc_tree_t::iterator itfp=fps.begin();itfp!=fps.end();++itfp)
			compute_orientation(const_cast<feature_point_t&>(*itfp));

		for(fps_by_desc_tree_t::iterator itfp=fps.begin();itfp!=fps.end();++itfp)
			compute_descriptor(const_cast<feature_point_t&>(*itfp));
	}

	void surf::ranged_detect(std::vector<ranged_detect_params_t> const &rdp)
	{
		for(boost::uint32_t o=0;o<m_octaves;++o)
		{
			for(boost::uint32_t i=0;i<2;++i)
			{
				response_layer &bl=m_response_layers[FILTER_MAP[o][i+0]];
				response_layer &ml=m_response_layers[FILTER_MAP[o][i+1]];
				response_layer &tl=m_response_layers[FILTER_MAP[o][i+2]];
				for(std::size_t rdpi=0;rdpi<rdp.size();++rdpi)
					tl.detect(bl, ml, rdp[rdpi]);
			}
		}
	}

	void surf::compute_orientation(feature_point_t &fp)
	{
		typedef feature_point_t::desc_value_type dec_t;
#if 1
		fp.m_orientation=estimate_rotation<const_integral_view_t,dec_t,3>(
			m_integral_view,
			static_cast<boost::int32_t>(std::floor(fp.x()+math::constant::half<feature_point_t::value_type>())),
			static_cast<boost::int32_t>(std::floor(fp.y()+math::constant::half<feature_point_t::value_type>())),
			fp.m_scale
		);
#else
		fp.m_orientation=estimate_rotation_fast<const_integral_view_t,dec_t>(
			m_integral_view,
			static_cast<boost::int32_t>(std::floor(fp.x()+math::constant::half<feature_point_t::value_type>())),
			static_cast<boost::int32_t>(std::floor(fp.y()+math::constant::half<feature_point_t::value_type>())),
			static_cast<boost::uint32_t>(std::floor(fp.m_scale+math::constant::half<dec_t>()))
		);
#endif
	}
	
	void surf::compute_descriptor(feature_point_t &fp)
	{
#if 1
		extract_surf_descriptor<
			const_integral_view_t,
			feature_point_t::value_type,
			feature_point_t::desc_value_type,
			feature_point_t::BLOCK_SIZE,
			7
		>(m_integral_view,fp);
#else
		typedef feature_point_t::desc_value_type dec_t;

		static dec_t const c_5i2=2.5f;
		static dec_t const c_3i2=1.5f;
		static dec_t const c_3i2_sq=2.25f;
		
		math::point2ui pt(
			static_cast<boost::uint32_t>(std::floor(fp.x()+math::constant::half<feature_point_t::value_type>())),
			static_cast<boost::uint32_t>(std::floor(fp.y()+math::constant::half<feature_point_t::value_type>()))
		);

		boost::uint32_t ui_scale=static_cast<boost::uint32_t>(std::floor(fp.m_scale+math::constant::half<dec_t>()));

		dec_t co=std::cos(fp.m_orientation);
		dec_t si=std::sin(fp.m_orientation);

		dec_t inv_scale=math::constant::one<dec_t>()/dec_t(fp.m_scale);
		dec_t inv_scale2=math::constant::one<dec_t>()/dec_t(fp.m_scale*fp.m_scale);
		feature_point_t::value_type sco=fp.m_scale*co;
		feature_point_t::value_type ssi=fp.m_scale*si;
			
		boost::int32_t i=-8;

		dec_t len=math::constant::zero<dec_t>();
		//float flen=0.0f;
			
		boost::uint32_t count=0;

		dec_t cx=-math::constant::half<dec_t>();
		while(i<12)
		{
			boost ::int32_t j=-8;
			i-=4;
			cx+=math::constant::one<dec_t>();
			dec_t cy=-math::constant::half<dec_t>();
				
			while(j<12) 
			{
				dec_t	dx=math::constant::zero<dec_t>(),
						dy=math::constant::zero<dec_t>(),
						mdx=math::constant::zero<dec_t>(),
						mdy=math::constant::zero<dec_t>();
				
				cy+=math::constant::one<dec_t>();
				j-=4;
					
				boost::int32_t ix=i+5;
				boost::int32_t jx=j+5;
					
				boost::int32_t xs=static_cast<boost::int32_t>(
					std::floor(fp.x()+feature_point_t::value_type(-jx)*ssi+feature_point_t::value_type(ix)*sco+
						math::constant::half<feature_point_t::value_type>()
					)
				);
				boost::int32_t ys=static_cast<boost::int32_t>(
					std::floor(fp.y()+feature_point_t::value_type(jx)*sco+feature_point_t::value_type(ix)*ssi+
						math::constant::half<feature_point_t::value_type>()
					)
				);
					
				for(boost::int32_t k=i;k<i+9;++k) 
				{
					for(boost::int32_t l=j;l<j+9;++l) 
					{
						boost::int32_t sample_x=static_cast<boost::int32_t>(
							std::floor(fp.x()+feature_point_t::value_type(-l)*ssi+feature_point_t::value_type(k)*sco+
								math::constant::half<feature_point_t::value_type>()
							)
						);
						boost::int32_t sample_y=static_cast<boost::int32_t>(
							std::floor(fp.y()+feature_point_t::value_type(l)*sco+feature_point_t::value_type(k)*ssi+
								math::constant::half<feature_point_t::value_type>()
							)
						);
						
						//dec_t gauss_s0=gaussian(0, 0, c_5i2*fp.m_scale);
						//dec_t gauss_s0_=gauss_25_lut<dec_t::IS,dec_t::FS,10,2>::get()(math::constants::zero<dec_t>(),math::constants::zero<dec_t>())*inv_scale2;


						dec_t gauss_s1=gaussian(xs-sample_x, ys-sample_y, c_5i2*fp.m_scale);
						//dec_t gauss_s1=gauss_25_lut<dec_t::IS,dec_t::FS, 16, 2>::get()(dec_t(xs-sample_x)*inv_scale,dec_t(ys-sample_y)*inv_scale);

						dec_t rx=haar2d_x<const_integral_view_t,dec_t>(
							m_integral_view,
							sample_x,
							sample_y,
							ui_scale<<1
						);
							
						dec_t ry=haar2d_y<const_integral_view_t,dec_t>(
							m_integral_view,
							sample_x,
							sample_y,
							ui_scale<<1
						);

						dec_t rrx=gauss_s1*(ry*co-rx*si);
						dec_t rry=gauss_s1*(rx*co+ry*si);
							
						dx+=rrx;
						dy+=rry;
						mdx+=std::abs(rrx);
						mdy+=std::abs(rry);
					}
				}

				dec_t gauss_s2=gaussian(
					cx-math::constant::two<dec_t>(),
					cy-math::constant::two<dec_t>(),
					c_3i2
				);
				//dec_t gauss_s2=gauss_25_lut<dec_t::IS,dec_t::FS,16,2>::get()(cx-math::constants::two<dec_t>(),cy-math::constants::two<dec_t>());

				dec_t dxg=dx*gauss_s2;
				dec_t dyg=dy*gauss_s2;
				dec_t mdxg=mdx*gauss_s2;
				dec_t mdyg=mdy*gauss_s2;
	
				fp.m_desc[count++]=dxg;
				fp.m_desc[count++]=dyg;
				fp.m_desc[count++]=mdxg;
				fp.m_desc[count++]=mdyg;

				//fp.m_desc[count++]=dx;
				//fp.m_desc[count++]=dy;
				//fp.m_desc[count++]=mdx;
				//fp.m_desc[count++]=mdy;

				len+=(dxg*dxg+dyg*dyg+mdxg*mdxg+mdyg*mdyg);
				//len+=(dx*dx+dy*dy+mdx*mdx+mdy*mdy);
				j+=9;
			}
			i+=9;
		}

		len=math::constant::one<dec_t>()/std::max(std::sqrt(len), std::numeric_limits<dec_t>::epsilon());
		
		for(boost::uint32_t i=0;i<64;++i)
			fp.m_desc[i]*=len;
#endif
	}

	template < boost::uint32_t I, boost::uint32_t F >
	math::fixed_point<I,F> surf::gaussian(boost::int32_t x, boost::int32_t y, math::fixed_point<I,F> const &sig)
	{
#if 0
		math::fixed_point<I,F> x_=math::fixed_point<I,F>(x)/(sig*math::constants::sq_two< math::fixed_point<I,F> >());
		math::fixed_point<I,F> y_=math::fixed_point<I,F>(y)/(sig*math::constants::sq_two< math::fixed_point<I,F> >());
						
		return math::constants::half< math::fixed_point<I,F> >()/(math::constants::pi< math::fixed_point<I,F> >()*sig*sig)*
			exp(-x_*x_)* exp(-y_*y_);
#else
		float sig_=static_cast<float>(sig);
		float x_=static_cast<float>(x);
		float y_=static_cast<float>(y);
		
		return (1.0f/(2.0f*math::constant::pi<float>()*sig_*sig_))*std::exp(-(x_*x_+y_*y_)/(2.0f*sig_*sig_));
#endif
	}
	
	template < boost::uint32_t I, boost::uint32_t F >
	math::fixed_point<I,F> surf::gaussian(math::fixed_point<I,F> const &x, math::fixed_point<I,F> const &y, math::fixed_point<I,F> const &sig)
	{
#if 0
		math::fixed_point<I,F> x_=x/(sig*math::constants::sq_two< math::fixed_point<I,F> >());
		math::fixed_point<I,F> y_=y/(sig*math::constants::sq_two< math::fixed_point<I,F> >());

		return math::constants::half< math::fixed_point<I,F> >()/(math::constants::pi< math::fixed_point<I,F> >()*sig*sig)*
			exp( -x_*x_)*
			exp( -y_*y_);
#else
		float sig_=static_cast<float>(sig);
		float x_=static_cast<float>(x);
		float y_=static_cast<float>(y);

		return (1.0f/(2.0f*math::constant::pi<float>()*sig_*sig_))*std::exp(-(x_*x_+y_*y_)/(2.0f*sig_*sig_));

#endif
	}

	void surf::match_feature_points(std::vector<feature_point_t> const &fps1, std::vector<feature_point_t> const &fps2, std::vector< std::pair<std::size_t, std::size_t> > &m, surf::feature_point_t::value_type const &d)
	{
		m.clear();
		for(std::size_t i=0;i<fps1.size();++i) 
		{
			feature_point_t::value_type d1=std::numeric_limits<feature_point_t::value_type>::max();
			feature_point_t::value_type d2=std::numeric_limits<feature_point_t::value_type>::max();
			
			std::vector<feature_point_t>::const_iterator match=fps2.end();
			for(std::size_t j=0;j<fps2.size();++j) 
			{
				feature_point_t::value_type dist=fps1[i]-fps2[j];
				
				if(dist<d1)
				{
					d2=d1;
					d1=dist;
					match=fps2.begin()+j;
				}
				else
				if(dist<d2)
					d2=dist;
			}

			//float d1_=d1;
			//float d2_=d2;

			if(match!=fps2.end() && (d1==math::constant::zero<feature_point_t::value_type>() || (d2!=math::constant::zero<feature_point_t::value_type>() && d1/d2<d)))
				m.push_back(std::make_pair(i,match-fps2.begin()));
		}
	}

} //namespace ucv
} //namespace baldzarika
