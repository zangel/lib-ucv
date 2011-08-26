#ifndef BALDZARIKA_UCV_SURF_HESSIAN_DETECTOR_H
#define BALDZARIKA_UCV_SURF_HESSIAN_DETECTOR_H

#include <baldzarika/ucv/integral.h>

namespace baldzarika { namespace ucv { namespace surf {

	template < typename T >
	class hessian_detector
	{
		static boost::uint32_t const HAAR_SIZE0=9;
		static boost::uint32_t const HAAR_SIZE_INC=6;

		typedef typename ucv::gil::pixel<T, ucv::gil::gray_layout_t> gray_pixel_t;
		typedef typename ucv::gil::image< gray_pixel_t, false, std::allocator<unsigned char> > gray_image_t;
		typedef typename gray_image_t::view_t gray_view_t;
		typedef typename gray_image_t::const_view_t gray_const_view_t;
		
		struct layer
		{
			layer()
				: m_response()
				, m_laplacian()
				, m_sample_step(0)
				, m_haar_size(0)
			{
			}

			layer(gray_view_t res, gray_view_t lap, boost::uint32_t ss, boost::uint32_t hs)
				: m_response(res)
				, m_laplacian(lap)
				, m_sample_step(ss)
				, m_haar_size(hs)
			{

			}

			inline operator bool() const
			{
				return !(
					math::size2ui(
						m_response.width(),
						m_response.height()
					).empty() ||
					(
						math::size2ui(
							m_response.width(),
							m_response.height()
						)!=
						math::size2ui(
							m_laplacian.width(),
							m_laplacian.height()
						)
					) ||
					m_sample_step==0 ||
					m_haar_size ==0
				);
			}

			template < typename IVT >
			inline bool update(IVT iv)
			{
				typedef typename IVT::value_type	iv_pixel_t;
				typedef typename gil::channel_type<iv_pixel_t>::type iv_channel_t;

				static T const c_081=0.81f;

				boost::int32_t const width_step=iv.row_begin(1)-iv.row_begin(0);
				float const layer_scale=float(m_haar_size)/float(HAAR_SIZE0);

				integral_box<iv_channel_t> dx_pattern[]=
				{
					integral_box<iv_channel_t>(math::point2i(0,2), math::size2i(3,5), width_step,  math::constant::one<iv_channel_t>()).scaled(layer_scale,width_step),
					integral_box<iv_channel_t>(math::point2i(3,2), math::size2i(3,5), width_step, -math::constant::two<iv_channel_t>()).scaled(layer_scale,width_step),
					integral_box<iv_channel_t>(math::point2i(6,2), math::size2i(3,5), width_step,  math::constant::one<iv_channel_t>()).scaled(layer_scale,width_step)
				};

				integral_box<iv_channel_t> dy_pattern[]=
				{
					integral_box<iv_channel_t>(math::point2i(2,0), math::size2i(5,3), width_step,  math::constant::one<iv_channel_t>()).scaled(layer_scale,width_step),
					integral_box<iv_channel_t>(math::point2i(2,3), math::size2i(5,3), width_step, -math::constant::two<iv_channel_t>()).scaled(layer_scale,width_step),
					integral_box<iv_channel_t>(math::point2i(2,6), math::size2i(5,3), width_step,  math::constant::one<iv_channel_t>()).scaled(layer_scale,width_step)
				};

				integral_box<iv_channel_t> dxy_pattern[]=
				{
					integral_box<iv_channel_t>(math::point2i(1,1), math::size2i(3,3), width_step,  math::constant::one<iv_channel_t>()).scaled(layer_scale,width_step),
					integral_box<iv_channel_t>(math::point2i(5,1), math::size2i(3,3), width_step, -math::constant::one<iv_channel_t>()).scaled(layer_scale,width_step),
					integral_box<iv_channel_t>(math::point2i(1,5), math::size2i(3,3), width_step, -math::constant::one<iv_channel_t>()).scaled(layer_scale,width_step),
					integral_box<iv_channel_t>(math::point2i(5,5), math::size2i(3,3), width_step,  math::constant::one<iv_channel_t>()).scaled(layer_scale,width_step)
				};

				boost::int32_t samples_y=1+(iv.height()-1-m_haar_size)/m_sample_step;
				boost::int32_t samples_x=1+(iv.width()-1-m_haar_size)/m_sample_step;
				boost::int32_t margin=(m_haar_size/2)/m_sample_step;

				for(boost::int32_t y=0;y<samples_y;++y)
				{
					iv_channel_t const *piv=reinterpret_cast<iv_channel_t const *>(iv.row_begin(y*m_sample_step));
					T *pres=reinterpret_cast<T *>(m_response.row_begin(y+margin))+margin;
					T *plap=reinterpret_cast<T *>(m_laplacian.row_begin(y+margin))+margin;
					for(boost::int32_t x=0;x<samples_x;++x)
					{
						T dx=integral_sample<T,iv_channel_t>(piv,dx_pattern);
						T dy=integral_sample<T,iv_channel_t>(piv,dy_pattern);
						T dxy=integral_sample<T,iv_channel_t>(piv,dxy_pattern);
						piv+=m_sample_step;
						*pres++=(dx*dy-c_081*dxy*dxy);
						*plap++=(dx+dy);
					}
				}
				return true;
			}

			template < typename PT >
			inline bool detect(T tsh, boost::function<void (math::point2<PT> const&, boost::int32_t, bool)> const &dcb, layer const &bl, layer const &tl) const
			{
				static T const i4=math::constant::one<T>()/math::constant::four<T>();

				boost::int32_t margin=(tl.m_haar_size/2)/m_sample_step+1;

				boost::int32_t const ws1=bl.m_response.row_begin(1)-bl.m_response.row_begin(0);
				boost::int32_t const ws2=m_response.row_begin(1)-m_response.row_begin(0);
				boost::int32_t const ws3=tl.m_response.row_begin(1)-tl.m_response.row_begin(0);
				

				for(boost::int32_t y=margin;y<m_response.height()-margin;++y)
				{
					T const *pres=reinterpret_cast<T *>(m_response.row_begin(y));
					T const *plap=reinterpret_cast<T *>(m_laplacian.row_begin(y));

					for(boost::int32_t x=margin;x<m_response.width()-margin;++x)
					{
						T val111=pres[x];
						if(val111>tsh)
						{

							T const *pres1=reinterpret_cast<T *>(bl.m_response.row_begin(y))+x;
							T const *pres2=pres+x;
							T const *pres3=reinterpret_cast<T *>(tl.m_response.row_begin(y))+x;

							T N333[3][9]=
							{
								{
									pres1[-ws1-1],	pres1[-ws1],	pres1[-ws1+1],
									pres1[-1],		pres1[0],		pres1[1],
									pres1[ws1-1],	pres1[ws1],		pres1[ws1+1]
								},
								{
									pres2[-ws2-1],	pres2[-ws2],	pres2[-ws2+1],
									pres2[-1],		pres2[0],		pres2[1],
									pres2[ws2-1],	pres2[ws2],		pres2[ws2+1]
								},
								{
									pres3[-ws3-1],	pres3[-ws3],	pres3[-ws3+1],
									pres3[-1],		pres3[0],		pres3[1],
									pres3[ws3-1],	pres3[ws3],		pres3[ws3+1]
								}
							};
							
							if(
								val111>N333[0][0] && val111>N333[0][1] && val111>N333[0][2] &&
								val111>N333[0][3] && val111>N333[0][4] && val111>N333[0][5] &&
								val111>N333[0][6] && val111>N333[0][7] && val111>N333[0][8] &&
								val111>N333[1][0] && val111>N333[1][1] && val111>N333[1][2] &&
								val111>N333[1][3]					   && val111>N333[1][5] &&
								val111>N333[1][6] && val111>N333[1][7] && val111>N333[1][8] &&
								val111>N333[2][0] && val111>N333[2][1] && val111>N333[2][2] &&
								val111>N333[2][3] && val111>N333[2][4] && val111>N333[2][5] &&
								val111>N333[2][6] && val111>N333[2][7] && val111>N333[2][8]
							)
							{
								boost::int32_t int_y=m_sample_step*(y-(m_haar_size/2)/m_sample_step);
								boost::int32_t int_x=m_sample_step*(x-(m_haar_size/2)/m_sample_step);

								PT center_y=PT(int_y)+PT(m_haar_size-1)*math::constant::half<PT>();
								PT center_x=PT(int_x)+PT(m_haar_size-1)*math::constant::half<PT>();
								
								
								
								T A[9];

								A[0]=N333[1][3]-math::constant::two<T>()*N333[1][4]+N333[1][5];
								A[1]=(N333[1][8]-N333[1][6]-N333[1][2]+N333[1][0])*i4;
								A[2]=(N333[2][5]-N333[2][3]-N333[0][5]+N333[0][3])*i4;
								A[3]=A[1];
								A[4]=N333[1][1]-math::constant::two<T>()*N333[1][4]+N333[1][7];
								A[5]=(N333[2][7]-N333[2][1]-N333[0][7]+N333[0][1])*i4;
								A[6]=A[2];
								A[7]=A[5];
								A[8]=N333[0][4]-math::constant::two<T>()*N333[1][4]+N333[2][4];

								math::matrix<T, 3, 3> invA=reinterpret_cast< math::matrix<T, 3, 3> const *>(A)->inverted();

								T b[3]=
								{
									-(N333[1][5]-N333[1][3])*math::constant::half<T>(),
									-(N333[1][7]-N333[1][1])*math::constant::half<T>(),
									-(N333[2][4]-N333[0][4])*math::constant::half<T>()
								};

								math::vector<T, 3> xi=invA*(*reinterpret_cast< math::vector<T, 3> const *>(b));
								if(	std::abs(xi(0))<math::constant::half<T>() &&
									std::abs(xi(1))<math::constant::half<T>() && 
									std::abs(xi(2))<math::constant::half<T>()
								)
								{
									dcb(
										math::point2<T>(center_x+PT(xi(0))*PT(m_sample_step),center_y+PT(xi(1))*PT(m_sample_step)),
										math::iround(T(m_haar_size)+xi(2)*T(m_haar_size-bl.m_haar_size)),
										plap[x]>math::constant::zero<T>()
									);
								}
							}
                		}
					}
				}
				return true;
			}

			gray_view_t			m_response;
			gray_view_t			m_laplacian;
			boost::uint32_t		m_sample_step;
			boost::uint32_t		m_haar_size;
		};

	public:
		hessian_detector()
			: m_size(0,0)
			, m_octaves(0)
			, m_octave_layers(0)
			, m_sample_step0(0)
		{
		}

		hessian_detector(math::size2ui const &sz, boost::uint32_t oc=4, boost::uint32_t ol=2, boost::uint32_t ss0=1)
		{
			recreate(sz,oc,ol,ss0);
		}

		operator bool() const
		{
			return !(
				m_size.empty() ||
				m_octaves==0 ||
				m_octave_layers==0 ||
				m_sample_step0==0 ||
				m_layers.empty() ||
				m_layers_buffer.get()==0
			);
		}

		math::size2ui const& size() const
		{
			return m_size;
		}

		boost::uint32_t octaves() const
		{
			return m_octaves;
		}

		boost::uint32_t octave_layers() const
		{
			return m_octave_layers;
		}

		boost::uint32_t sample_step0() const
		{
			return m_sample_step0;
		}

		bool resize(math::size2ui const &sz)
		{
			return recreate(sz,m_octaves,m_octave_layers,m_sample_step0);
		}

		bool recreate(math::size2ui const &sz, boost::uint32_t oc=4, boost::uint32_t ol=2, boost::uint32_t ss0=1)
		{
			if(sz.empty() || oc==0 || ol==0 || ss0==0) return false;
			
			m_size=sz;
			m_octaves=oc;
			m_octave_layers=ol;
			m_sample_step0=ss0;

			boost::uint32_t layers_size=m_octaves*(m_octave_layers+2);
			
			m_layers.resize(layers_size);
			
			boost::uint32_t buffer_size(0);
			for(boost::uint32_t o=0;o<m_octaves;++o)
				buffer_size+=math::size2ui(
					m_size.width()/(m_sample_step0<<o),
					m_size.height()/(m_sample_step0<<o)
				).area();

			buffer_size=buffer_size*(m_octave_layers+2)*2;
			m_layers_buffer.reset(new T[buffer_size]);
			
			T *layers_buffer=m_layers_buffer.get();
			
			for(boost::uint32_t o=0;o<m_octaves;++o)
			{
				math::size2ui octave_size(
					m_size.width()/(m_sample_step0<<o),
					m_size.height()/(m_sample_step0<<o)
				);

				for(boost::uint32_t ol=0;ol<m_octave_layers+2;++ol)
				{
					m_layers[o*(m_octave_layers+2)+ol]=layer(
						ucv::gil::interleaved_view(
							octave_size.width(), octave_size.height(),
							reinterpret_cast<gray_pixel_t*>(layers_buffer),
							octave_size.width()*sizeof(T)
						),
						ucv::gil::interleaved_view(
							octave_size.width(), octave_size.height(),
							reinterpret_cast<gray_pixel_t*>(layers_buffer+octave_size.area()),
							octave_size.width()*sizeof(T)
						),
						m_sample_step0<<o,
						(HAAR_SIZE0+HAAR_SIZE_INC*ol)<<o
					);
					layers_buffer+=2*octave_size.area();
				}
			}
			return true;
		}

		template < typename IVT >
		bool update(IVT iv)
		{
			if(!(operator bool() && math::size2ui(iv.width()-1,iv.height()-1)==m_size))
				return false;
			for(boost::uint32_t l=0;l<m_layers.size();++l)
				m_layers[l].update(iv);
			return true;
		}


		template < typename PT >
		bool detect(T tsh, boost::function<void (math::point2<PT> const&, boost::int32_t, bool)> const &dcb) const
		{
			for(boost::uint32_t ml=0;ml<m_octave_layers*m_octaves;++ml)
			{
				boost::int32_t mlid=(ml/m_octave_layers)*(m_octave_layers+2)+(ml%m_octave_layers)+1;
				m_layers[mlid].detect(tsh, dcb, m_layers[mlid-1], m_layers[mlid+1]);
			}
			return true;
		}
	
	private:
		math::size2ui					m_size;
		boost::uint32_t					m_octaves;
		boost::uint32_t					m_octave_layers;
		boost::uint32_t					m_sample_step0;
		boost::scoped_array<T>			m_layers_buffer;
		std::vector<layer>				m_layers;
	};

} //namespace surf
} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_SURF_HESSIAN_DETECTOR_H
