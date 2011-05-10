#ifndef BALDZARIKA_UCV_CANNY_H
#define BALDZARIKA_UCV_CANNY_H

#include <baldzarika/ucv/size2.h>
#include <baldzarika/ucv/sobel.h>

namespace baldzarika { namespace ucv {

	template < typename PT, boost::uint32_t AS >
	class canny
	{
	public:
		static int const DELTAS[8][2];
		
		typedef std::vector<point2ui> contour_t;

		
		typedef sobel< PT, AS, 1 > sobel_t;

		typedef typename sobel_t::gray_t gray_t;
		typedef typename sobel_t::gray_pixel_t gray_pixel_t;
		typedef typename sobel_t::gray_image_t gray_image_t;
		typedef typename sobel_t::gray_view_t gray_view_t;
		typedef typename sobel_t::gray_const_view_t gray_const_view_t;

		typedef gil::gray8_image_t	map_image_t;
		typedef typename map_image_t::view_t map_view_t;
		typedef typename map_image_t::const_view_t map_const_view_t;
		typedef typename map_view_t::value_type map_pixel_t;
		typedef typename gil::channel_type<map_pixel_t>::type map_t;

		typedef std::stack< map_t*, std::vector< map_t* > >	map_stack_t;
		
		canny()
			: m_frame_size(0,0)
			, m_low_treshold(0.3)
			, m_high_treshold(0.9)
		{
		}

		canny(size2ui const &fs, gray_t const &low=gray_t(0.1), gray_t const &hi=gray_t(0.3))
			: m_frame_size(0,0)
			, m_low_treshold(low)
			, m_high_treshold(hi)
		{
			set_frame_size(fs);
			m_sobel.set_frame_size(fs);
		}

		~canny()
		{
		}

		size2ui const& get_frame_size() const
		{
			return m_frame_size;
		}

		void set_frame_size(size2ui const &fs)
		{
			if(m_frame_size==fs)
				return;
			m_frame_size=fs;
			m_sobel.set_frame_size(fs);
			m_dx_dy_img.recreate(fs.width(), fs.height()*2);
			m_mag_ring_img.recreate(fs.width()+2, 3);
			m_map_img.recreate(fs.width()+2, fs.height()+2);
		}

		template < typename BVT >
		bool operator()(gray_const_view_t img, BVT bvt)
		{
			if(img.width()!=bvt.width() || img.height()!=bvt.height())
				return false;

			if(img.width()!=m_frame_size.width() || img.height()!=m_frame_size.height())
				return false;

			if(!m_sobel(img, get_dx_view(), get_dy_view()))
				return false;

			map_stack_t map_stack;
			if(!non_maxima_suppression(map_stack))
				return false;

			if(!hysteresis_thresholding(map_stack))
				return false;
						
			typedef typename BVT::value_type	bin_pixel_t;
			typedef typename gil::channel_type<bin_pixel_t>::type bin_channel_t;

			map_const_view_t map_view=gil::const_view(m_map_img);

			for(boost::uint32_t y=0;y<m_frame_size.height();++y)
			{
				map_t const *map_row=reinterpret_cast<map_t const *>(map_view.row_begin(y+1))+1;
				bin_channel_t *bin_row=reinterpret_cast<bin_channel_t *>(bvt.row_begin(y));

				for(boost::uint32_t x=0;x<m_frame_size.width();++x)
					bin_row[x]=bin_channel_t(map_row[x]>>1);
			}
			return true;
		}

		bool operator()(gray_const_view_t img, std::list< contour_t > &contours)
		{
			if(img.width()!=bvt.width() || img.height()!=bvt.height())
				return false;

			if(img.width()!=m_frame_size.width() || img.height()!=m_frame_size.height())
				return false;

			if(!m_sobel(img, get_dx_view(), get_dy_view()))
				return false;

			map_stack_t map_stack;
			if(!non_maxima_suppression(map_stack))
				return false;

			map_stack_t contour_candidates(map_stack);

			if(!hysteresis_thresholding(map_stack))
				return false;

			contours.clear();

			boost::uint32_t const map_step=m_frame_size.width()+2;

			map_view_t map_view=gil::view(m_map_img);

			while(!contour_candidates.empty())
			{
				map_t *begin=contour_candidates.top();
				contour_candidates.pop();

				if(!(*m & 0x02))
					continue;

				map_t *curr=begin;
				boost::uint32_t cc=0;
				std::stack< int, std::vector<int> > chain_codes;

				do
				{
					
					while(cc<8)
					{
						map_t *candidate=curr+DELTAS[cc][1]*map_step+DELTAS[cc][0];
						if(*candidate & 0x02)
						{
							*candidate=0x00;
							chain_codes.push(cc);
							curr=candidate;
							cc=0;
							break;
						}
						cc++;
					}

					if(cc==8)
					{
						
					}
				}
				while(!chain_codes.empty());
			}

			return true;
		}

	protected:
		bool non_maxima_suppression(map_stack_t &map_stack)
		{
			static gray_t const tan_22_5=0.4142135623730950488016887242097;
			static gray_t const tan_67_5=2.4142135623730950488016887242097;

			gray_const_view_t dx_view=get_dx_const_view();
			gray_const_view_t dy_view=get_dy_const_view();

			gray_view_t mag_buf_view[3]=
			{
				get_mag_view<0>(),
				get_mag_view<1>(),
				get_mag_view<2>()
			};

			gray_t *mag_buf[3]=
			{
				reinterpret_cast<gray_t *>(mag_buf_view[0].row_begin(0)),
				reinterpret_cast<gray_t *>(mag_buf_view[1].row_begin(0)),
				reinterpret_cast<gray_t *>(mag_buf_view[2].row_begin(0))
			};

			std::fill(mag_buf[0], mag_buf[0]+m_frame_size.width()+2, detail::constant::zero<gray_t>());

			map_view_t map_view=gil::view(m_map_img);
			
			std::fill(
				reinterpret_cast<map_t *>(map_view.row_begin(0)),
				reinterpret_cast<map_t *>(map_view.row_begin(0))+m_frame_size.width()+2,
				detail::constant::one<map_t>()
			);
			std::fill(
				reinterpret_cast<map_t *>(map_view.row_begin(map_view.height()-1)),
				reinterpret_cast<map_t *>(map_view.row_begin(map_view.height()-1))+m_frame_size.width()+2,
				detail::constant::one<map_t>()
			);
			
			boost::uint32_t const map_step=m_frame_size.width()+2;

			for(boost::int32_t y=0;y<=boost::int32_t(m_frame_size.height());++y)
			{
				gray_t *mag=mag_buf[(y>0?2:1)]+1;
				gray_t const *dx_row=reinterpret_cast<gray_t const *>(dx_view.row_begin(y));
				gray_t const *dy_row=reinterpret_cast<gray_t const *>(dy_view.row_begin(y));

				if(y<boost::int32_t(m_frame_size.height()))
				{
					mag[-1]=mag[m_frame_size.width()]=detail::constant::zero<gray_t>();
					for(boost::uint32_t x=0;x<m_frame_size.width();++x)
						mag[x]=abs(dx_row[x])+abs(dy_row[x]);
				}
				else
				{
					std::fill(mag-1, mag+m_frame_size.width()+1, detail::constant::zero<gray_t>());
				}

				if(y==0) continue;

				map_t *map=reinterpret_cast<map_t *>(map_view.row_begin(y))+1;
				map[-1]=map[m_frame_size.width()]=detail::constant::one<map_t>();


				mag=mag_buf[1]+1;
				dx_row=reinterpret_cast<gray_t const *>(dx_view.row_begin(y-1));
				dy_row=reinterpret_cast<gray_t const *>(dy_view.row_begin(y-1));

				boost::int32_t mag_step1=mag_buf[2]-mag_buf[1];
				boost::int32_t mag_step2=mag_buf[0]-mag_buf[1];

				boost::uint8_t prev_flag=0;

				for(boost::int32_t x=0;x<boost::int32_t(m_frame_size.width());++x)
				{
					gray_t dx=dx_row[x];
					gray_t dy=dy_row[x];
					gray_t m=mag[x];

					boost::int32_t s=(dx.get() ^ dy.get())<0?-1:1;
					
					dx=abs(dx);
					dy=abs(dy);

					if(m>m_low_treshold)
					{
						gray_t tan_22_5_dx=dx*tan_22_5;
						gray_t tan_67_5_dx=dx*tan_67_5;
						
						if(dy<tan_22_5_dx)
						{
							if(m>mag[x-1] && m>=mag[x+1])
							{
								if(m>m_high_treshold && !prev_flag && map[x-map_step]!=2)
								{
									map[x]=2;
									map_stack.push(map+x);
									prev_flag=1;
								}
								else
									map[x]=0;
								continue;
							}
						}
						else
						if(dy>tan_67_5_dx)
						{
							if(m>mag[x+mag_step2] && m>=mag[x+mag_step1])
							{
								if(m>m_high_treshold && !prev_flag && map[x-map_step]!=2)
								{
									map[x]=2;
									map_stack.push(map+x);
									prev_flag=1;
								}
								else
									map[x]=0;
								continue;
							}
						}
						else
						{
							if(m>mag[x+mag_step2-s] && m>mag[x+mag_step1+s])
							{
								if(m>m_high_treshold && !prev_flag && map[x-map_step]!=2)
								{
									map[x]=2;
									map_stack.push(map+x);
									prev_flag=1;
								}
								else
									map[x]=0;
								continue;
							}
						}
					}
					prev_flag=0;
					map[x]=1;
				}

				mag=mag_buf[0];
				mag_buf[0]=mag_buf[1];
				mag_buf[1]=mag_buf[2];
				mag_buf[2]=mag;
			}
			return true;
		}

		bool hysteresis_thresholding(map_stack_t &map_stack)
		{
			boost::uint32_t const map_step=m_frame_size.width()+2;
			while(!map_stack.empty())
			{
				map_t *m=map_stack.top(), *mt;
				map_stack.pop();

				mt=m-1;
				if(!*mt) { *mt=2; map_stack.push(mt); }

				mt=m+1;
				if(!*mt) { *mt=2; map_stack.push(mt); }

				mt=m-map_step-1;
				if(!*mt) { *mt=2; map_stack.push(mt); }

				mt=m-map_step;
				if(!*mt) { *mt=2; map_stack.push(mt); }

				mt=m-map_step+1;
				if(!*mt) { *mt=2; map_stack.push(mt); }

				mt=m+map_step-1;
				if(!*mt) { *mt=2; map_stack.push(mt); }

				mt=m+map_step;
				if(!*mt) { *mt=2; map_stack.push(mt); }

				mt=m+map_step+1;
				if(!*mt) { *mt=2; map_stack.push(mt); }
			}
			return true;
		}

		gray_const_view_t get_dx_const_view() const
		{
			return gil::subimage_view(
				gil::const_view(m_dx_dy_img),
				0, 0,
				m_frame_size.width(), m_frame_size.height()
			);
		}

		gray_view_t get_dx_view()
		{
			return gil::subimage_view(
				gil::view(m_dx_dy_img),
				0, 0,
				m_frame_size.width(), m_frame_size.height()
			);
		}

		gray_const_view_t get_dy_const_view() const
		{
			return gil::subimage_view(
				gil::const_view(m_dx_dy_img),
				0, m_frame_size.height(),
				m_frame_size.width(), m_frame_size.height()
			);
		}

		gray_view_t get_dy_view()
		{
			return gil::subimage_view(
				gil::view(m_dx_dy_img),
				0, m_frame_size.height(),
				m_frame_size.width(), m_frame_size.height()
			);
		}

		template < boost::uint32_t I >
		gray_const_view_t get_mag_const_view() const
		{
			BOOST_STATIC_ASSERT(I>=0 && I<3);
			return gil::subimage_view(
				gil::const_view(m_mag_ring_img),
				0, I,
				m_frame_size.width()+2, 1
			);
		}

		template < boost::uint32_t I >
		gray_view_t get_mag_view()
		{
			BOOST_STATIC_ASSERT(I>=0 && I<3);
			return gil::subimage_view(
				gil::view(m_mag_ring_img),
				0, I,
				m_frame_size.width()+2, 1
			);
		}

	private:
		size2ui				m_frame_size;
		gray_t				m_low_treshold;
		gray_t				m_high_treshold;
		sobel_t				m_sobel;
		gray_image_t		m_dx_dy_img;
		gray_image_t		m_mag_ring_img;
		map_image_t			m_map_img;
	};

	template < typename PT, boost::uint32_t AS >
	int const canny<PT,AS>::DELTAS[8][2]=
	{
		{ 1, 0 },
		{ 1,-1 },
		{ 0,-1 },
		{-1,-1 },
		{-1, 0 },
		{-1, 1 },
		{ 0, 1 },
		{ 1, 1 }
	};


} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_CANNY_H