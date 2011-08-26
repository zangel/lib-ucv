#ifndef BALDZARIKA_UCV_KLT_TRACKER_H
#define BALDZARIKA_UCV_KLT_TRACKER_H

#include <baldzarika/ucv/integral.h>

namespace baldzarika { namespace ucv {

	template < typename T >
	class klt_tracker
	{
	public:
		
		typedef T integral_t;
		typedef gil::pixel<integral_t, ucv::gil::gray_layout_t> integral_pixel_t;
		typedef gil::image< integral_pixel_t, false, std::allocator<unsigned char> > integral_image_t;
		typedef typename integral_image_t::view_t integral_view_t;
		typedef typename integral_image_t::const_view_t const_integral_view_t;
		
		klt_tracker(math::size2ui const &fs, math::size2ui const &hws=math::size2ui(5,5), boost::uint32_t nl=3, boost::uint32_t mi=5, integral_t const &e=integral_t(1.0e-4f), integral_t const &sd=integral_t(1.0e-3f))
			: m_frame_size(fs)
			, m_half_win_size(hws.width(), hws.height())
			, m_window((2*m_half_win_size.width()+1+5)*3, 2*m_half_win_size.height()+1+5)
			, m_num_levels(nl)
			, m_max_iters(mi)
			, m_epsilon(e)
			, m_small_det(sd)
		{
		}

		math::size2ui const& get_frame_size() const
		{
			return m_frame_size;
		}

		void set_frame_size(math::size2ui const &fs)
		{
			m_frame_size=fs;
		}

		math::size2ui const& get_half_win_size() const
		{
			return m_half_win_size;
		}

		void set_half_win_size(math::size2ui const& hws)
		{
			m_half_win_size=hws;
			m_window.recreate((2*m_half_win_size.width()+1+5)*3, 2*m_half_win_size.height()+1+5);
		}

		boost::uint32_t get_num_levels() const
		{
			return m_num_levels;
		}

		void  set_num_levels(boost::uint32_t nl)
		{
			m_num_levels=nl;
		}

		boost::uint32_t get_max_iterations() const
		{
			return m_max_iters;
		}

		void  set_max_iterations(boost::uint32_t mi)
		{
			m_max_iters=mi;
		}

		bool set_integral_views(const_integral_view_t prev_view, const_integral_view_t curr_view)
		{
			if(	m_frame_size.empty() ||
				prev_view.width()!=curr_view.width() || prev_view.width()!=m_frame_size.width() ||
				prev_view.height()!=curr_view.height() || prev_view.height()!=m_frame_size.height() ) return false;
			
			m_prev_view=prev_view;
			m_curr_view=curr_view;
			return true;
		}
		
		template < typename PT >
		bool operator()(std::vector< math::point2< PT > > const &prev_fps, std::vector< math::point2< PT > > &curr_fps, std::vector<bool> &status)
		{
			if(	m_frame_size.empty() ||
				m_prev_view.width()!=m_curr_view.width() || m_prev_view.width()!=m_frame_size.width() ||
				m_prev_view.height()!=m_curr_view.height() || m_prev_view.height()!=m_frame_size.height() ) return false;
						
			curr_fps.resize(prev_fps.size());
			status.resize(prev_fps.size(),false);
			for(std::size_t ipfp=0;ipfp<prev_fps.size();++ipfp)
				status[ipfp]=this->operator ()(prev_fps[ipfp], curr_fps[ipfp]);

			return true;
		}



	protected:
		template < typename PT >
		bool operator()(math::point2< PT > const &prev_fp, math::point2< PT > &curr_fp)
		{
			integral_t const range_corr=math::constant::one<integral_t>()/integral_t((2*m_half_win_size.width()+1)*(2*m_half_win_size.height()+1));
			//integral_t const step_factor=2.0f;
			curr_fp=prev_fp;
			for(boost::uint32_t l=0;l<m_num_levels;++l)
			{
				
				boost::uint32_t lshift=m_num_levels-l-1;
				integral_t level_scale=integral_t(1<<lshift);

				for(boost::uint32_t i=0;i<m_max_iters;++i)
				{
					compute_gradient_sum_and_img_diff(prev_fp, curr_fp, lshift);
					integral_t gxx, gyy, gxy, ex, ey;
					compute_gradient_matrix_and_err_vector(gxx,gyy, gxy, ex, ey);

					ex*=math::constant::two<integral_t>();
					ey*=math::constant::two<integral_t>();

					integral_t det=gxx*gyy-gxy*gxy;

					if(std::abs(det)<m_small_det)
						return false;

					integral_t dx=(gyy*ex-gxy*ey)/det;
					integral_t dy=(gxx*ey-gxy*ex)/det;

					curr_fp.x()+=dx*level_scale;
					curr_fp.y()+=dy*level_scale;

					if((dx*dx+dy*dy)<m_epsilon)
						break;
				}
			}
			return true;
		}
		
		integral_view_t img_diff()
		{
			return gil::subimage_view(gil::view(m_window), 0*(2*m_half_win_size.width()+1+5), 0, 2*m_half_win_size.width()+1+5, 2*m_half_win_size.height()+1+5);
		}

		integral_view_t grad_sum_x()
		{
			return gil::subimage_view(gil::view(m_window), 1*(2*m_half_win_size.width()+1+5), 0, 2*m_half_win_size.width()+1+5, 2*m_half_win_size.height()+1+5);
		}

		integral_view_t grad_sum_y()
		{
			return gil::subimage_view(gil::view(m_window), 2*(2*m_half_win_size.width()+1+5), 0, 2*m_half_win_size.width()+1+5, 2*m_half_win_size.height()+1+5);
		}
		
		void compute_gradient_matrix_and_err_vector(integral_t &gxx, integral_t &gyy, integral_t &gxy, integral_t &ex, integral_t &ey)
		{
			gxx=math::constant::zero<integral_t>();
			gyy=math::constant::zero<integral_t>();
			gxy=math::constant::zero<integral_t>();
			ex=math::constant::zero<integral_t>();
			ey=math::constant::zero<integral_t>();
						
			integral_view_t img_diff_view=img_diff();
			integral_view_t grad_sum_x_view=grad_sum_x();
			integral_view_t grad_sum_y_view=grad_sum_y();

			for(boost::uint32_t y=2;y<2*m_half_win_size.height()+1+2;++y)
			{
				integral_t *img_diff_row=reinterpret_cast<integral_t *>(img_diff_view.row_begin(y))+2;
				integral_t *grad_sum_x_row=reinterpret_cast<integral_t *>(grad_sum_x_view.row_begin(y))+2;
				integral_t *grad_sum_y_row=reinterpret_cast<integral_t *>(grad_sum_y_view.row_begin(y))+2;

				for(boost::uint32_t x=2;x<2*m_half_win_size.width()+1+2;++x)
				{
					integral_t diff=*img_diff_row++;
					integral_t gx=*grad_sum_x_row++;
					integral_t gy=*grad_sum_y_row++;

					gxx+=gx*gx;
					gyy+=gy*gy;
					gxy+=gx*gy;
					ex+=diff*gx;
					ey+=diff*gy;
				}
			}
		}

		template < typename PT >
		void compute_gradient_sum_and_img_diff(math::point2< PT > const &prev_pt, math::point2< PT > const &curr_pt, boost::uint32_t lshift)
		{
			typedef math::point2<PT> point2_t;

			boost::uint32_t s=1<<lshift;
			//integral_t 
			integral_t const sca=math::constant::one<integral_t>()/integral_t(1<<lshift);
			integral_t const i16=1.0f/float(1<<4);
			integral_t const i8=1.0f/float(1<<3);

			boost::int32_t const prev_w_step=m_prev_view.row_begin(1)-=m_prev_view.row_begin(0);
			boost::int32_t const curr_w_step=m_curr_view.row_begin(1)-=m_curr_view.row_begin(0);



			integral_box<integral_t> prev_box_sample=integral_box<integral_t>(
				math::point2i(0,0),
				math::size2i(1,1),
				prev_w_step,
				math::constant::one<integral_t>()
			).scaled(1<<lshift, prev_w_step);

			integral_box<integral_t> curr_box_sample=integral_box<integral_t>(
				math::point2i(0,0),
				math::size2i(1,1),
				curr_w_step,
				-math::constant::one<integral_t>()
			).scaled(1<<lshift, curr_w_step);

			integral_t prev_ax=integral_t(prev_pt.x()-static_cast<boost::int32_t>(prev_pt.x()))*sca;
			integral_t prev_ay=integral_t(prev_pt.y()-static_cast<boost::int32_t>(prev_pt.y()))*sca;
			integral_t prev_c00=(math::constant::one<integral_t>()-prev_ax)*(math::constant::one<integral_t>()-prev_ay);
			integral_t prev_c01=prev_ax*(math::constant::one<integral_t>()-prev_ay);
			integral_t prev_c10=(math::constant::one<integral_t>()-prev_ax)*prev_ay;
			integral_t prev_c11=prev_ax*prev_ay;
			

			integral_t curr_ax=integral_t(curr_pt.x()-static_cast<boost::int32_t>(curr_pt.x()))*sca;
			integral_t curr_ay=integral_t(curr_pt.y()-static_cast<boost::int32_t>(curr_pt.y()))*sca;
			integral_t curr_c00=(math::constant::one<integral_t>()-curr_ax)*(math::constant::one<integral_t>()-curr_ay);
			integral_t curr_c01=curr_ax*(math::constant::one<integral_t>()-curr_ay);
			integral_t curr_c10=(math::constant::one<integral_t>()-curr_ax)*curr_ay;
			integral_t curr_c11=curr_ax*curr_ay;
			
			integral_view_t img_diff_view=img_diff();
			integral_view_t grad_sum_x_view=grad_sum_x();
			integral_view_t grad_sum_y_view=grad_sum_y();

			for(boost::uint32_t y=0;y<2*m_half_win_size.height()+5+1;++y)
			{
				integral_t *img_diff_row=reinterpret_cast<integral_t *>(img_diff_view.row_begin(y));
				integral_t *grad_sum_x_row=reinterpret_cast<integral_t *>(grad_sum_x_view.row_begin(y));
				integral_t *grad_sum_y_row=reinterpret_cast<integral_t *>(grad_sum_y_view.row_begin(y));
				for(boost::uint32_t x=0;x<2*m_half_win_size.width()+5+1;++x)
				{
					*img_diff_row++=math::constant::zero<integral_t>();
					*grad_sum_x_row++=math::constant::zero<integral_t>();
					*grad_sum_y_row++=math::constant::zero<integral_t>();
				}
			}

			for(boost::uint32_t y=1;y<(2*m_half_win_size.height()+1)+2+1;++y)
			{
				integral_t *img_diff_rows[4]=
				{
					reinterpret_cast<integral_t *>(img_diff_view.row_begin(y-1))+1,
					reinterpret_cast<integral_t *>(img_diff_view.row_begin(y+0))+1,
					reinterpret_cast<integral_t *>(img_diff_view.row_begin(y+1))+1,
					reinterpret_cast<integral_t *>(img_diff_view.row_begin(y+2))+1
				};

				integral_t *grad_sum_x_rows[4]=
				{
					reinterpret_cast<integral_t *>(grad_sum_x_view.row_begin(y-1))+1,
					reinterpret_cast<integral_t *>(grad_sum_x_view.row_begin(y+0))+1,
					reinterpret_cast<integral_t *>(grad_sum_x_view.row_begin(y+1))+1,
					reinterpret_cast<integral_t *>(grad_sum_x_view.row_begin(y+2))+1
				};

				integral_t *grad_sum_y_rows[4]=
				{
					reinterpret_cast<integral_t *>(grad_sum_y_view.row_begin(y-1))+1,
					reinterpret_cast<integral_t *>(grad_sum_y_view.row_begin(y+0))+1,
					reinterpret_cast<integral_t *>(grad_sum_y_view.row_begin(y+1))+1,
					reinterpret_cast<integral_t *>(grad_sum_y_view.row_begin(y+2))+1
				};

				for(boost::uint32_t x=1;x<(2*m_half_win_size.width()+1)+2+1;++x)
				{
					math::point2i prev_pos(
						static_cast<boost::int32_t>(prev_pt.x())+((x-m_half_win_size.width()-2)<<lshift),
						static_cast<boost::int32_t>(prev_pt.y())+((y-m_half_win_size.height()-2)<<lshift)
					);

					math::point2i curr_pos(
						static_cast<math::point2i::value_type>(curr_pt.x())+((x-m_half_win_size.width()-2)<<lshift),
						static_cast<math::point2i::value_type>(curr_pt.y())+((y-m_half_win_size.height()-2)<<lshift)
					);

					//integral_t prev_val=box_integral<const_integral_view_t,integral_t>(m_prev_view, prev_pos, math::size2ui(s,s)).operator>>(lshift<<1);
					integral_t prev_val=integral_sample<integral_t,integral_t>(
						reinterpret_cast<integral_t const *>(m_prev_view.row_begin(prev_pos.y()))+prev_pos.x(),
						prev_box_sample
					);

					//integral_t curr_val=box_integral<const_integral_view_t,integral_t>(m_curr_view, curr_pos, math::size2ui(s,s)).operator>>(lshift<<1);
					integral_t curr_val=integral_sample<integral_t,integral_t>(
						reinterpret_cast<integral_t const *>(m_curr_view.row_begin(curr_pos.y()))+curr_pos.x(),
						curr_box_sample
					);

					//intensity difference
					integral_t diff_00=(prev_val*prev_c11-curr_val*curr_c11)*i16;
					integral_t diff_01=(prev_val*prev_c10-curr_val*curr_c10)*i16;
					integral_t diff_10=(prev_val*prev_c01-curr_val*curr_c01)*i16;
					integral_t diff_11=(prev_val*prev_c00-curr_val*curr_c00)*i16;

					integral_t diff_00_2=math::constant::two<integral_t>()*diff_00;
					integral_t diff_01_2=math::constant::two<integral_t>()*diff_01;
					integral_t diff_10_2=math::constant::two<integral_t>()*diff_10;
					integral_t diff_11_2=math::constant::two<integral_t>()*diff_11;

					//[-1][-1]
					img_diff_rows[0][-1]+=diff_00;
					img_diff_rows[0][+0]+=diff_01;
					img_diff_rows[1][-1]+=diff_10;
					img_diff_rows[1][+0]+=diff_11;

					//[-1][+0]
					img_diff_rows[0][+0]+=diff_00_2;
					img_diff_rows[0][+1]+=diff_01_2;
					img_diff_rows[1][+0]+=diff_10_2;
					img_diff_rows[1][+1]+=diff_11_2;

					//[-1][+1]
					img_diff_rows[0][+1]+=diff_00;
					img_diff_rows[0][+2]+=diff_01;
					img_diff_rows[1][+1]+=diff_10;
					img_diff_rows[1][+2]+=diff_11;

					//[+0][-1]
					img_diff_rows[1][-1]+=diff_00_2;
					img_diff_rows[1][+0]+=diff_01_2;
					img_diff_rows[2][-1]+=diff_10_2;
					img_diff_rows[2][+0]+=diff_11_2;

					//[+0][+0]
					img_diff_rows[1][+0]+=math::constant::four<integral_t>()*diff_00;
					img_diff_rows[1][+1]+=math::constant::four<integral_t>()*diff_01;
					img_diff_rows[2][+0]+=math::constant::four<integral_t>()*diff_10;
					img_diff_rows[2][+1]+=math::constant::four<integral_t>()*diff_11;

					//[+0][+1]
					img_diff_rows[1][+1]+=diff_00_2;
					img_diff_rows[1][+2]+=diff_01_2;
					img_diff_rows[2][+1]+=diff_10_2;
					img_diff_rows[2][+2]+=diff_11_2;


					//[+1][-1]
					img_diff_rows[2][-1]+=diff_00;
					img_diff_rows[2][+0]+=diff_01;
					img_diff_rows[3][-1]+=diff_10;
					img_diff_rows[3][+0]+=diff_11;

					//[+1][+0]
					img_diff_rows[2][+0]+=diff_00_2;
					img_diff_rows[2][+1]+=diff_01_2;
					img_diff_rows[3][+0]+=diff_10_2;
					img_diff_rows[3][+1]+=diff_11_2;

					//[+1][+1]
					img_diff_rows[2][+1]+=diff_00;
					img_diff_rows[2][+2]+=diff_01;
					img_diff_rows[3][+1]+=diff_10;
					img_diff_rows[3][+2]+=diff_11;
					
					//gradient x sum
					integral_t sum_00=(prev_val*prev_c11+curr_val*curr_c11)*i8;
					integral_t sum_01=(prev_val*prev_c10+curr_val*curr_c10)*i8;
					integral_t sum_10=(prev_val*prev_c01+curr_val*curr_c01)*i8;
					integral_t sum_11=(prev_val*prev_c00+curr_val*curr_c00)*i8;

					integral_t sum_00_2=math::constant::two<integral_t>()*sum_00;
					integral_t sum_01_2=math::constant::two<integral_t>()*sum_01;
					integral_t sum_10_2=math::constant::two<integral_t>()*sum_10;
					integral_t sum_11_2=math::constant::two<integral_t>()*sum_11;

					//[-1][-1]
					grad_sum_x_rows[0][-1]+=sum_00;
					grad_sum_x_rows[0][+0]+=sum_01;
					grad_sum_x_rows[1][-1]+=sum_10;
					grad_sum_x_rows[1][+0]+=sum_11;
					//[-1][+1]
					grad_sum_x_rows[0][+1]-=sum_00;
					grad_sum_x_rows[0][+2]-=sum_01;
					grad_sum_x_rows[1][+1]-=sum_10;
					grad_sum_x_rows[1][+2]-=sum_11;
					//[+0][-1]
					grad_sum_x_rows[1][-1]+=sum_00_2;
					grad_sum_x_rows[1][+0]+=sum_01_2;
					grad_sum_x_rows[2][-1]+=sum_10_2;
					grad_sum_x_rows[2][+0]+=sum_11_2;
					//[+0][+1]
					grad_sum_x_rows[1][+1]-=sum_00_2;
					grad_sum_x_rows[1][+2]-=sum_01_2;
					grad_sum_x_rows[2][+1]-=sum_10_2;
					grad_sum_x_rows[2][+2]-=sum_11_2;
					//[+1][-1]
					grad_sum_x_rows[2][-1]+=sum_00;
					grad_sum_x_rows[2][+0]+=sum_01;
					grad_sum_x_rows[3][-1]+=sum_10;
					grad_sum_x_rows[3][+0]+=sum_11;
					//[+1][+1]
					grad_sum_x_rows[2][+1]-=sum_00;
					grad_sum_x_rows[2][+2]-=sum_01;
					grad_sum_x_rows[3][+1]-=sum_10;
					grad_sum_x_rows[3][+2]-=sum_11;

					//gradient y sum
					//[-1][-1]
					grad_sum_y_rows[0][-1]+=sum_00;
					grad_sum_y_rows[0][+0]+=sum_01;
					grad_sum_y_rows[1][-1]+=sum_10;
					grad_sum_y_rows[1][+0]+=sum_11;
					//[-1][+0]
					grad_sum_y_rows[0][+0]+=sum_00_2;
					grad_sum_y_rows[0][+1]+=sum_01_2;
					grad_sum_y_rows[1][+0]+=sum_10_2;
					grad_sum_y_rows[1][+1]+=sum_11_2;
					//[-1][+1]
					grad_sum_y_rows[0][+1]+=sum_00;
					grad_sum_y_rows[0][+2]+=sum_01;
					grad_sum_y_rows[1][+1]+=sum_10;
					grad_sum_y_rows[1][+2]+=sum_11;
					//[+1][-1]
					grad_sum_y_rows[2][-1]-=sum_00;
					grad_sum_y_rows[2][+0]-=sum_01;
					grad_sum_y_rows[3][-1]-=sum_10;
					grad_sum_y_rows[3][+0]-=sum_11;
					//[+1][+0]
					grad_sum_y_rows[2][+0]-=sum_00_2;
					grad_sum_y_rows[2][+1]-=sum_01_2;
					grad_sum_y_rows[3][+0]-=sum_10_2;
					grad_sum_y_rows[3][+1]-=sum_11_2;
					//[+1][+1]
					grad_sum_y_rows[2][+1]-=sum_00;
					grad_sum_y_rows[2][+2]-=sum_01;
					grad_sum_y_rows[3][+1]-=sum_10;
					grad_sum_y_rows[3][+2]-=sum_11;

					for(boost::uint32_t r=0;r<4;++r)
					{
						img_diff_rows[r]++;
						grad_sum_x_rows[r]++;
						grad_sum_y_rows[r]++;
					}
				}
			}
		}

	private:
		math::size2ui			m_frame_size;
		math::size2ui			m_half_win_size;
		integral_image_t		m_window;
		boost::uint32_t			m_num_levels;
		boost::uint32_t			m_max_iters;
		integral_t				m_epsilon;
		integral_t				m_small_det;

		const_integral_view_t	m_prev_view;
		const_integral_view_t	m_curr_view;
	};

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_KLT_TRACKER_H
