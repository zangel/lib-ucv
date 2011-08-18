#ifndef BALDZARIKA_UCV_CONVERT_SCALE_H
#define BALDZARIKA_UCV_CONVERT_SCALE_H

#include <baldzarika/ucv/gil_channel_traits.h>

namespace baldzarika { namespace ucv {

	namespace detail {

		struct convert
		{
			inline void begin() {}
			inline void begin_row() {}
			inline void end_row() {}
			inline void end() {}

			template < typename ST, typename DT >
			inline void operator()(ST const &src, DT &dst)
			{
				dst=static_cast<DT>(src);
			}

			template < typename PT >
			inline void operator()(PT const &src, PT &dst)
			{
				dst=src;
			}

		};

		struct grayscale_convert
		{
			inline void begin() {}
			inline void begin_row() {}
			inline void end_row() {}
			inline void end() {}

			inline void operator()(float const &src, boost::uint8_t &dst)
			{
				dst=static_cast<boost::uint8_t>(src*std::numeric_limits<boost::uint8_t>::max());
			}

			inline void operator()(boost::uint8_t const &src,float &dst)
			{
				dst=float(src)*math::constant::i_255<float>();
			}
						
			template < boost::uint32_t I, boost::uint32_t F >
			inline void operator()(math::fixed_point<I,F> const &src, boost::uint8_t &dst)
			{
				dst=boost::uint8_t(
					std::max<typename math::fixed_point< I, F >::value_type>(0,
						std::min<typename math::fixed_point< I, F >::value_type>(255,
							math::detail::adjuster< (F<8) >::
								template adjust<
								typename math::fixed_point< I, F >::value_type,
								typename math::fixed_point< I, F >::value_type,
								8,
								F
							>(src.get())
						)
					) & 0xFF
				);
			}

			template < boost::uint32_t I, boost::uint32_t F >
			inline void operator()(boost::uint8_t const &src, math::fixed_point<I,F> &dst)
			{
				dst=math::fixed_point<I,F>(
					math::detail::adjuster< (F>8) >::
						template adjust<
							typename math::fixed_point< I, F >::value_type,
							typename math::fixed_point< I, F >::value_type,
							F,
							8
						>(typename math::fixed_point< I, F >::value_type(src)),
					math::detail::fp_explicit_tag()
				);
			}
		};

		template < typename MT >
		struct grayscale_convert_and_median
			: grayscale_convert
		{
			grayscale_convert_and_median(MT &mv, boost::uint32_t width, boost::uint32_t height)
				: m_median_value(mv)
				, m_inv_width(math::constant::one<MT>()/MT(width))
				, m_inv_height(math::constant::one<MT>()/MT(height))
			{

			}

			grayscale_convert_and_median(grayscale_convert_and_median const &that)
				: m_median_value(that.m_median_value)
				, m_inv_width(that.m_inv_width)
				, m_inv_height(that.m_inv_height)
			{
			}

			inline void begin()
			{
				m_median_value=math::constant::zero<MT>();
			}

			inline void begin_row()
			{
				m_row_sum=math::constant::zero<MT>();

			}

			inline void end_row()
			{
				m_median_value+=m_row_sum*m_inv_width;
			}


			inline void end()
			{
				m_median_value*=m_inv_height;
			}

			template < typename ST, typename DT >
			inline void operator()(ST const &src, DT &dst)
			{
				DT tmp;
				grayscale_convert::operator()(src,tmp);
				m_row_sum+=MT(tmp);
				dst=tmp;
			}

			MT &m_median_value;
			MT m_row_sum;
			MT m_inv_width;
			MT m_inv_height;
		};

	} //namesapce detail

	template < typename SVT, typename DVT, typename CV >
	bool convert(SVT src, DVT dst, CV converter)
	{
		typedef typename SVT::value_type	src_pixel_t;
		typedef typename DVT::value_type	dst_pixel_t;

		typedef typename gil::channel_type<src_pixel_t>::type src_channel_t;
		typedef typename gil::channel_type<dst_pixel_t>::type dst_channel_t;

		if(src.width()!=dst.width() || src.height()!=dst.height() || !src.width()*dst.width())
			return false;

		boost::int32_t const width=src.width();
		boost::int32_t const height=src.height();

		converter.begin();
		for(boost::int32_t y=0;y<height;++y)
		{
			src_channel_t const *src_row=reinterpret_cast<src_channel_t const *>(src.row_begin(y));
			dst_channel_t *dst_row=reinterpret_cast<dst_channel_t *>(dst.row_begin(y));
			
			converter.begin_row();
			for(boost::int32_t x=0;x<width;++x)
				converter(*src_row++, *dst_row++);
			converter.end_row();
		}
		converter.end();
		return true;
	}

	template < typename DVT >
	bool convert_nv16_to_gray(gil::gray8_view_t y, gil::gray16_view_t uv, DVT dst, typename gil::channel_type<typename DVT::value_type>::type &mv, float scale=1.0f)
	{
		typedef gil::channel_type<gil::gray8_pixel_t>::type y_channel_t;
		typedef gil::channel_type<gil::gray16_pixel_t>::type uv_channel_t;


		typedef typename DVT::value_type	dst_pixel_t;
		typedef typename gil::channel_type<dst_pixel_t>::type dst_channel_t;

		dst_channel_t const r_s=scale*0.30f;
		dst_channel_t const g_s=scale*0.59f;
		dst_channel_t const b_s=scale*0.11f;

		if(y.width()/2!=uv.width() || y.height()/2!=uv.height() || !uv.width()*uv.height() || y.width()!=dst.width() || y.height()!=dst.height())
			return false;
		
		dst_channel_t const inv_width=math::constant::one<dst_channel_t>()/dst_channel_t(dst.width());
		dst_channel_t const inv_height=math::constant::one<dst_channel_t>()/dst_channel_t(dst.height());

		dst_channel_t median_sum=math::constant::zero<dst_channel_t>();

		for(std::size_t r=0;r<static_cast<std::size_t>(y.height());++r)
		{
			y_channel_t *y_row=reinterpret_cast<y_channel_t *>(y.row_begin(r));
			uv_channel_t *uv_row=reinterpret_cast<uv_channel_t *>(uv.row_begin(r>>1));
			dst_channel_t *dst_row=reinterpret_cast<dst_channel_t *>(dst.row_begin(r));
			dst_channel_t median_row_sum=math::constant::zero<dst_channel_t>();
			for(std::size_t c=0;c<y.width();++c)
			{
				boost::int32_t Y=std::max<boost::int32_t>(0,static_cast<boost::int32_t>(*y_row)-16);
				boost::int32_t V=static_cast<boost::int32_t>((*uv_row) & 0x00ff)-128;
				boost::int32_t U=static_cast<boost::int32_t>(((*uv_row) & 0xff00) >> 8)-128;
								
				dst_channel_t B=dst_channel_t(
						std::min<boost::int32_t>(262143, std::max<boost::int32_t>(1192*Y+2066*U, 0))>>10
					)*math::constant::i_255<dst_channel_t>();

				dst_channel_t G=dst_channel_t(
						std::min<boost::int32_t>(262143, std::max<boost::int32_t>(1192*Y-833*V-400*U, 0))>>10
					)*math::constant::i_255<dst_channel_t>();

				dst_channel_t R=dst_channel_t(
						std::min<boost::int32_t>(262143, std::max<boost::int32_t>(1192*Y+1634*V, 0))>>10
					)*math::constant::i_255<dst_channel_t>();

				dst_channel_t v=(B*b_s+G*g_s+R*r_s);
				*dst_row=v;
				median_row_sum+=v;
				y_row++;
				uv_row+=(c%2?1:0);
				dst_row++;
			}
			median_sum+=median_row_sum*inv_width;
		}
		mv=median_sum*inv_height;
		return true;
	}


#if defined(ANDROID)
#if defined(FPU_ARM_VFP2)

namespace detail {

	template <>
	struct grayscale_convert_and_median<float>
	{
		grayscale_convert_and_median(float &mv, boost::uint32_t width, boost::uint32_t height)
		: m_median_value(mv)
		{
		}

		float		&m_median_value;
	};
} //namespace detail

	bool convert
	(	boost::gil::gray8c_view_t src,
		gil::image< gil::pixel<float, gil::gray_layout_t> , false, std::allocator<unsigned char> >::view_t dst,
		detail::grayscale_convert_and_median<float> converter
	)
	{
		if(src.width()!=dst.width() || src.height()!=dst.height() || !src.width()*dst.width())
			return false;

		boost::int32_t const width=src.width();
		boost::int32_t const height=src.height();


		return false;
	}

#endif //FPU_ARM_VFP2
#endif //ANDROID


} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_CONVERT_SCALE_H
