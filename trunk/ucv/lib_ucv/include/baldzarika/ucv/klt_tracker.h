#ifndef BALDZARIKA_UCV_KLT_TRACKER_H
#define BALDZARIKA_UCV_KLT_TRACKER_H

namespace baldzarika { namespace ucv {

	template < boost::uint32_t I, boost::uint32_t F >
	class klt_tracker
	{
	public:
		
		typedef fixed_point<I, F> integral_t;
		typedef gil::pixel<integral_t, ucv::gil::gray_layout_t> integral_pixel_t;
		typedef gil::image< integral_pixel_t, false, std::allocator<unsigned char> > integral_image_t;
		typedef typename integral_image_t::view_t integral_view_t;
		
		klt_tracker(integral_view_t prev, integral_view_t curr, size2ui const &ws=ucv::size2ui(10,10), boost::uint32_t nl=3, boost::uint32_t mi=5, integral_t const &e=integral_t(1.0e-3f), integral_t const &sd=integral_t(1.0e-2f))
			: m_prev(prev)
			, m_curr(curr)
			, m_win_size(ws.width()/2, ws.height()/2)
			, m_window((2*m_win_size.width()+1+5)*3, 2*m_win_size.height()+1+5)
			, m_num_levels(nl)
			, m_max_iters(mi)
			, m_epsilon(e)
			, m_small_det(sd)
		{
		}

		integral_view_t img_diff()
		{
			return gil::subimage_view(gil::view(m_window), 0*(2*m_win_size.width()+1+5), 0, 2*m_win_size.width()+1+5, 2*m_win_size.height()+1+5);
		}

		integral_view_t grad_sum_x()
		{
			return gil::subimage_view(gil::view(m_window), 1*(2*m_win_size.width()+1+5), 0, 2*m_win_size.width()+1+5, 2*m_win_size.height()+1+5);
		}

		integral_view_t grad_sum_y()
		{
			return gil::subimage_view(gil::view(m_window), 2*(2*m_win_size.width()+1+5), 0, 2*m_win_size.width()+1+5, 2*m_win_size.height()+1+5);
		}

		template < boost::uint32_t I2, boost::uint32_t F2 >
		bool operator()(point2< fixed_point<I2,F2> > const &prev_fp, point2< fixed_point<I2,F2> > &curr_fp)
		{
			integral_t const range_corr=detail::constants::one<integral_t>()/integral_t((2*m_win_size.width()+1)*(2*m_win_size.height()+1));
			integral_t const step_factor=1.5f;
			curr_fp=prev_fp;
			for(boost::uint32_t l=0;l<m_num_levels;++l)
			{
				boost::uint32_t ls=1<<(m_num_levels-l-1);
				integral_t level_scale=ls;
				
				for(boost::uint32_t i=0;i<m_max_iters;++i)
				{
					compute_gradient_sum_and_img_diff(prev_fp, curr_fp, ls);
					integral_t gxx, gyy, gxy, ex, ey;
					compute_gradient_matrix_and_err_vector(gxx,gyy, gxy, ex, ey);
					gxx*=range_corr;
					gyy*=range_corr;
					gxy*=range_corr;
					ex*=range_corr*step_factor;
					ey*=range_corr*step_factor;

					

					integral_t det=gxx*gyy-gxy*gxy;
					if(fabs(det)<m_small_det)
						return false;

					integral_t dx=(gyy*ex-gxy*ey)/det;
					integral_t dy=(gxx*ey-gxy*ex)/det;

					curr_fp.x+=dx*level_scale;
					curr_fp.y+=dy*level_scale;

					if((dx*dx+dy*dy)<m_epsilon)
						break;
				}
			}
			return true;
		}

		template < boost::uint32_t I2, boost::uint32_t F2 >
		void operator()(std::vector< point2< fixed_point<I2,F2> > > const &prev_fps, std::vector< point2< fixed_point<I2,F2> > > &curr_fps, std::vector<bool> &status)
		{
			curr_fps.resize(prev_fps.size());
			status.resize(prev_fps.size(),false);
			for(std::size_t ipfp=0;ipfp<prev_fps.size();++ipfp)
				status[ipfp]=this->operator ()(prev_fps[ipfp], curr_fps[ipfp]);
		}

	protected:

		template < boost::uint32_t I, boost::uint32_t F >
		void compute_gradient_matrix_and_err_vector(fixed_point<I,F> &gxx, fixed_point<I,F> &gyy, fixed_point<I,F> &gxy, fixed_point<I,F> &ex, fixed_point<I,F> &ey)
		{
			typedef fixed_point<I,F> real_t;
			gxx=detail::constants::zero<real_t>();
			gyy=detail::constants::zero<real_t>();
			gxy=detail::constants::zero<real_t>();
			ex=detail::constants::zero<real_t>();
			ey=detail::constants::zero<real_t>();
						
			integral_view_t img_diff_view=img_diff();
			integral_view_t grad_sum_x_view=grad_sum_x();
			integral_view_t grad_sum_y_view=grad_sum_y();

			for(boost::uint32_t y=2;y<2*m_win_size.height()+1+2;++y)
			{
				integral_t *img_diff_row=reinterpret_cast<integral_t *>(img_diff_view.row_begin(y))+2;
				integral_t *grad_sum_x_row=reinterpret_cast<integral_t *>(grad_sum_x_view.row_begin(y))+2;
				integral_t *grad_sum_y_row=reinterpret_cast<integral_t *>(grad_sum_y_view.row_begin(y))+2;

				for(boost::uint32_t x=2;x<2*m_win_size.width()+1+2;++x)
				{
					real_t diff=*img_diff_row++;
					real_t gx=*grad_sum_x_row++;
					real_t gy=*grad_sum_y_row++;

					gxx+=gx*gx;
					gyy+=gy*gy;
					gxy+=gx*gy;
					ex+=diff*gx;
					ey+=diff*gy;
				}
			}
		}

		template < boost::uint32_t I, boost::uint32_t F >
		void compute_gradient_sum_and_img_diff(point2< fixed_point<I,F> > const &prev_pt, point2< fixed_point<I,F> > const &curr_pt, boost::uint32_t s)
		{
			typedef fixed_point<I,F> real_t;
			typedef point2<real_t> point2_t;

			integral_t inv_s=detail::constants::one<integral_t>()/integral_t(s);
			integral_t inv_area=inv_s*inv_s;


			integral_t prev_ax=integral_t(prev_pt.x-real_t(static_cast<boost::int32_t>(prev_pt.x)))*inv_s;
			integral_t prev_ay=integral_t(prev_pt.y-real_t(static_cast<boost::int32_t>(prev_pt.y)))*inv_s;

			integral_t prev_c00=(detail::constants::one<integral_t>()-prev_ax)*(detail::constants::one<integral_t>()-prev_ay);
			integral_t prev_c01=prev_ax*(detail::constants::one<integral_t>()-prev_ay);
			integral_t prev_c10=(detail::constants::one<integral_t>()-prev_ax)*prev_ay;
			integral_t prev_c11=prev_ax*prev_ay;
			

			integral_t curr_ax=integral_t(curr_pt.x-real_t(static_cast<boost::int32_t>(curr_pt.x)))*inv_s;
			integral_t curr_ay=integral_t(curr_pt.y-real_t(static_cast<boost::int32_t>(curr_pt.y)))*inv_s;
			integral_t curr_c00=(detail::constants::one<integral_t>()-curr_ax)*(detail::constants::one<integral_t>()-curr_ay);
			integral_t curr_c01=curr_ax*(detail::constants::one<integral_t>()-curr_ay);
			integral_t curr_c10=(detail::constants::one<integral_t>()-curr_ax)*curr_ay;
			integral_t curr_c11=curr_ax*curr_ay;
			
			integral_view_t img_diff_view=img_diff();
			integral_view_t grad_sum_x_view=grad_sum_x();
			integral_view_t grad_sum_y_view=grad_sum_y();

			for(boost::uint32_t y=0;y<2*m_win_size.height()+5+1;++y)
			{
				integral_t *img_diff_row=reinterpret_cast<integral_t *>(img_diff_view.row_begin(y));
				integral_t *grad_sum_x_row=reinterpret_cast<integral_t *>(grad_sum_x_view.row_begin(y));
				integral_t *grad_sum_y_row=reinterpret_cast<integral_t *>(grad_sum_y_view.row_begin(y));
				for(boost::uint32_t x=0;x<2*m_win_size.width()+5+1;++x)
				{
					*img_diff_row++=detail::constants::zero<integral_t>();
					*grad_sum_x_row++=detail::constants::zero<integral_t>();
					*grad_sum_y_row++=detail::constants::zero<integral_t>();
				}
			}

			for(boost::uint32_t y=1;y<(2*m_win_size.height()+1)+2+1;++y)
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

				for(boost::uint32_t x=1;x<(2*m_win_size.width()+1)+2+1;++x)
				{
					point2i prev_pos(
						static_cast<boost::int32_t>(prev_pt.x)+(x-m_win_size.width()-3)*s,
						static_cast<boost::int32_t>(prev_pt.y)+(y-m_win_size.height()-3)*s
					);

					point2i curr_pos(
						static_cast<point2i::value_type>(curr_pt.x)+(x-m_win_size.width()-3)*s,
						static_cast<point2i::value_type>(curr_pt.y)+(y-m_win_size.height()-3)*s
					);

					integral_t prev_val=box_integral<integral_view_t,integral_t>(m_prev, prev_pos, size2ui(s,s))*inv_area;
					integral_t curr_val=box_integral<integral_view_t,integral_t>(m_curr, curr_pos, size2ui(s,s))*inv_area;

					//intensity difference
					img_diff_rows[1][0]+=prev_val*prev_c00-curr_val*curr_c00;
					img_diff_rows[1][1]+=prev_val*prev_c01-curr_val*curr_c01;
					img_diff_rows[2][0]+=prev_val*prev_c10-curr_val*curr_c10;
					img_diff_rows[2][1]+=prev_val*prev_c11-curr_val*curr_c11;


					integral_t sum_00=prev_val*prev_c00+curr_val*curr_c00;
					integral_t sum_01=prev_val*prev_c01+curr_val*curr_c01;
					integral_t sum_10=prev_val*prev_c10+curr_val*curr_c10;
					integral_t sum_11=prev_val*prev_c11+curr_val*curr_c11;

					//gradient x sum
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
					grad_sum_x_rows[1][-1]+=detail::constants::two<integral_t>()*sum_00;
					grad_sum_x_rows[1][+0]+=detail::constants::two<integral_t>()*sum_01;
					grad_sum_x_rows[2][-1]+=detail::constants::two<integral_t>()*sum_10;
					grad_sum_x_rows[2][+0]+=detail::constants::two<integral_t>()*sum_11;
					//[+0][+1]
					grad_sum_x_rows[1][+1]-=detail::constants::two<integral_t>()*sum_00;
					grad_sum_x_rows[1][+2]-=detail::constants::two<integral_t>()*sum_01;
					grad_sum_x_rows[2][+1]-=detail::constants::two<integral_t>()*sum_10;
					grad_sum_x_rows[2][+2]-=detail::constants::two<integral_t>()*sum_11;
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
					grad_sum_y_rows[0][+0]+=detail::constants::two<integral_t>()*sum_00;
					grad_sum_y_rows[0][+1]+=detail::constants::two<integral_t>()*sum_01;
					grad_sum_y_rows[1][+0]+=detail::constants::two<integral_t>()*sum_10;
					grad_sum_y_rows[1][+1]+=detail::constants::two<integral_t>()*sum_11;
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
					grad_sum_y_rows[2][+0]-=detail::constants::two<integral_t>()*sum_00;
					grad_sum_y_rows[2][+1]-=detail::constants::two<integral_t>()*sum_01;
					grad_sum_y_rows[3][+0]-=detail::constants::two<integral_t>()*sum_10;
					grad_sum_y_rows[3][+1]-=detail::constants::two<integral_t>()*sum_11;
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
		integral_view_t		m_prev;
		integral_view_t		m_curr;
		size2ui				m_win_size;
		integral_image_t	m_window;
		boost::uint32_t		m_num_levels;
		boost::uint32_t		m_max_iters;
		integral_t			m_epsilon;
		integral_t			m_small_det;
	};

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_KLT_TRACKER_H