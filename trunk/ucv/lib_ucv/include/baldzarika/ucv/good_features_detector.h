#ifndef BALDZARIKA_UCV_GOOD_FEATURES_DETECTOR_H
#define BALDZARIKA_UCV_GOOD_FEATURES_DETECTOR_H

#include <kdtree++/kdtree.hpp>

namespace baldzarika { namespace ucv {

	template < typename T >
	class good_features_detector
	{
	public:
		typedef T integral_t;
		typedef gil::pixel<integral_t, ucv::gil::gray_layout_t> integral_pixel_t;
		typedef gil::image< integral_pixel_t, false, std::allocator<unsigned char> > integral_image_t;
		typedef typename integral_image_t::view_t integral_view_t;

	protected:
		template < typename T2 >
		static bool compare_points(std::pair< integral_t, math::point2< T2 > > const &a, std::pair< integral_t, math::point2< T2 > > const &b)
		{
			return a.first < b.first;
		}

		template < typename T2 >
		struct point2_accessor
		{
			typedef T2 result_type;
			typedef math::point2<result_type> point2_t;

			result_type operator()(point2_t const &t, size_t k) const { return t[k]; }
		};

	public:
		
		good_features_detector(math::size2ui const &fs, boost::uint32_t bs=3, integral_t const &q=integral_t(1.0e-2f))
			: m_frame_size(fs)
			, m_block_size(bs)
			, m_quality(q)
			, m_gradients((m_frame_size.width()+2)*2, m_frame_size.height()+2)
			, m_min_eigen_values(
				m_frame_size.width()-2*(m_block_size/2),
				m_frame_size.height()-2*(m_block_size/2)
			)
		{
		}

		template < typename T2 >
		bool operator()(integral_view_t im, std::vector< math::point2< T2 > > &dps, boost::uint32_t md=5, boost::uint32_t mp=100)
		{
			typedef T2 point_real_t;
			typedef math::point2<point_real_t> point2_t;
			
			point_real_t const min_dist=md;

			if(im.width()-1!=m_frame_size.width() || im.height()-1!=m_frame_size.height())
				return false;

			dps.clear();
			compute_gradients(im);
#if 1
			integral_t max_eigen_value=compute_min_eigen_values();

			std::vector< std::pair< integral_t, point2_t> > points;
			collect_points(max_eigen_value, points);

			std::sort(points.begin(), points.end(), compare_points<T2>);

			KDTree::KDTree<2, point2_t, point2_accessor<T2> > dist_search;

			for(std::size_t ifp=0;ifp<points.size() && dist_search.size()<mp;++ifp)
			{
				std::pair<
					KDTree::KDTree<2, point2_t, point2_accessor<T2> >::iterator,
					point2_accessor<T2>::result_type
				> nearest=dist_search.find_nearest(points[ifp].second, min_dist);
				if(nearest.first==dist_search.end())
				{
					dist_search.insert(points[ifp].second);
					dps.push_back(points[ifp].second);
				}
			}
#endif
			return true;
		}

	protected:
		integral_view_t grad_x()
		{
			return gil::subimage_view(gil::view(m_gradients), 0*(m_frame_size.width()+2), 0, m_frame_size.width()+2, m_frame_size.height()+2);
		}
		
		integral_view_t grad_y()
		{
			return gil::subimage_view(gil::view(m_gradients), 1*(m_frame_size.width()+2), 0, m_frame_size.width()+2, m_frame_size.height()+2);
		}

		void compute_gradients(integral_view_t im)
		{
			static integral_t const sobel_scale=0.5f/4.0f;

			boost::int32_t const width_step=im.row_begin(1)-im.row_begin(0);
			
			integral_box<integral_t> box_sample=integral_box<integral_t>(
				math::point2i(0,0),
				math::size2i(1,1),
				width_step,
				math::constant::one<integral_t>()
			);


			integral_view_t grad_x_view=grad_x();
			integral_view_t grad_y_view=grad_y();

			for(boost::uint32_t y=0;y<m_frame_size.height()+2;++y)
			{
				integral_t *grad_x_row=reinterpret_cast<integral_t *>(grad_x_view.row_begin(y));
				integral_t *grad_y_row=reinterpret_cast<integral_t *>(grad_y_view.row_begin(y));
				for(boost::uint32_t x=0;x<m_frame_size.width()+2;++x)
				{
					*grad_x_row++=math::constant::zero<integral_t>();
					*grad_y_row++=math::constant::zero<integral_t>();
				}
			}

			for(boost::uint32_t y=1;y<m_frame_size.height()+1;++y)
			{
				integral_t *grad_x_rows[3]=
				{
					reinterpret_cast<integral_t *>(grad_x_view.row_begin(y-1))+1,
					reinterpret_cast<integral_t *>(grad_x_view.row_begin(y+0))+1,
					reinterpret_cast<integral_t *>(grad_x_view.row_begin(y+1))+1,
				};

				integral_t *grad_y_rows[3]=
				{
					reinterpret_cast<integral_t *>(grad_y_view.row_begin(y-1))+1,
					reinterpret_cast<integral_t *>(grad_y_view.row_begin(y+0))+1,
					reinterpret_cast<integral_t *>(grad_y_view.row_begin(y+1))+1,
				};

				integral_t const *row=reinterpret_cast<integral_t *>(im.row_begin(y-1));
				
				for(boost::uint32_t x=1;x<m_frame_size.width()+1;++x, row++)
				{
					integral_t pixel_val=sobel_scale*integral_sample<integral_t,integral_t>(row, box_sample);
					
					integral_t pixel_val_2=pixel_val*math::constant::two<integral_t>();

					//grad_x
					grad_x_rows[0][-1]+=pixel_val;
					grad_x_rows[0][+1]-=pixel_val;
					grad_x_rows[1][-1]+=pixel_val_2;
					grad_x_rows[1][+1]-=pixel_val_2;
					grad_x_rows[2][-1]+=pixel_val;
					grad_x_rows[2][+1]-=pixel_val;

					//grad_y
					grad_y_rows[0][-1]+=pixel_val;
					grad_y_rows[0][+0]+=pixel_val_2;
					grad_y_rows[0][+1]+=pixel_val;
					grad_y_rows[2][-1]-=pixel_val;
					grad_y_rows[2][+0]-=pixel_val_2;
					grad_y_rows[2][+1]-=pixel_val;

					for(boost::uint32_t r=0;r<3;++r)
					{
						grad_x_rows[r]++;
						grad_y_rows[r]++;
					}

				}
			}
		}

		integral_t compute_min_eigen_values()
		{
			integral_t max_eigen_value=std::numeric_limits<integral_t>::lowest();
			
			boost::uint32_t half_bs=m_block_size/2;
			
			integral_view_t grad_x_view=grad_x();
			integral_view_t grad_y_view=grad_y();
			integral_view_t eigen_view=gil::view(m_min_eigen_values);
			
			for(boost::uint32_t y=half_bs;y<m_frame_size.height()-half_bs;++y)
			{
				integral_t *eigen_row=reinterpret_cast<integral_t *>(eigen_view.row_begin(y-half_bs));
				for(boost::uint32_t x=half_bs;x<m_frame_size.width()-half_bs;++x)
				{
					integral_t
						gxx=math::constant::zero<integral_t>(),
						gyy=math::constant::zero<integral_t>(),
						gxy=math::constant::zero<integral_t>();

					for(boost::uint32_t yy=y-half_bs;yy<=y+half_bs;++yy)
					{
						integral_t *grad_x_row=reinterpret_cast<integral_t *>(grad_x_view.row_begin(yy+1))+1;
						integral_t *grad_y_row=reinterpret_cast<integral_t *>(grad_y_view.row_begin(yy+1))+1;
						
						for(boost::uint32_t xx=x-half_bs;xx<=x+half_bs;++xx)
						{
							integral_t gx=grad_x_row[xx];
							integral_t gy=grad_y_row[xx];

							gxx+=gx*gx;
							gyy+=gy*gy;
							gxy+=gx*gy;
						}
					}
					
					integral_t min_eigen_value=(
							gxx+gyy-sqrt((gxx-gyy)*(gxx-gyy)+math::constant::four<integral_t>()*gxy*gxy)
						)*
						math::constant::half<integral_t>();

					max_eigen_value=std::max(max_eigen_value,min_eigen_value);
					*eigen_row++=min_eigen_value;
				}
			}
			return max_eigen_value;
		}

		template < typename T2 >
		void collect_points(integral_t const &mev, std::vector< std::pair< integral_t, math::point2< T2 > > > &pts)
		{
			typedef T2 point_real_t;
			typedef math::point2<point_real_t> point2_t;

			boost::uint32_t half_bs=m_block_size/2;
			
			integral_view_t eigen_view=gil::view(m_min_eigen_values);

			integral_t const treshold=mev*m_quality;
			
			for(boost::uint32_t y=half_bs;y<m_frame_size.height()-half_bs;++y)
			{
				integral_t *eigen_row=reinterpret_cast<integral_t *>(eigen_view.row_begin(y-half_bs));
				for(boost::uint32_t x=half_bs;x<m_frame_size.width()-half_bs;++x)
				{
					integral_t eigen_val=*eigen_row++;
					if(eigen_val<treshold)
						pts.push_back(std::make_pair(eigen_val, point2_t(x,y)));
				}
			}
		}
	
	private:
		math::size2ui				m_frame_size;
		boost::uint32_t		m_block_size;
		integral_t			m_quality;
		integral_image_t	m_gradients;
		integral_image_t	m_min_eigen_values;
	};

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_GOOD_FEATURES_DETECTOR_H
