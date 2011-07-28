#ifndef BALDZARIKA_UCV_SURF_HESSIAN_DETECTOR_H
#define BALDZARIKA_UCV_SURF_HESSIAN_DETECTOR_H

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
			if(!(operator bool() || math::size2ui(iv.width(),iv.height())==m_size))
				return false;
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