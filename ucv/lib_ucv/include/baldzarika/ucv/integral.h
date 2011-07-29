#ifndef BALDZARIKA_UCV_INTEGRAL_H
#define BALDZARIKA_UCV_INTEGRAL_H

#include <baldzarika/ucv/gil_channel_traits.h>

namespace baldzarika { namespace ucv {
		
	template <typename RT, typename IVT>
	static inline RT integral_sample(IVT const &iv, boost::int32_t x, boost::int32_t y, boost::uint32_t w, boost::uint32_t h)
	{
		typedef typename IVT::value_type iv_pixel_t;
		typedef typename gil::channel_type<iv_pixel_t>::type iv_channel_t;

		return RT(
			(iv(x,y).operator iv_channel_t()-iv(x+w,y).operator iv_channel_t())-
			(iv(x,y+h).operator iv_channel_t()-iv(x+w,y+h).operator iv_channel_t())
		);
	}

	template <typename RT, typename IVT>
	RT integral_sample(IVT const &iv, math::point2i const &p, math::size2ui const &s)
	{
		return integral_sample<RT,IVT>(iv,p.x(), p.y(), s.width(), s.height());
	}


	template < typename T >
	struct integral_box
	{
		integral_box()
		{
			m_p[0]=m_p[1]=m_p[2]=m_p[3]=0;
			m_step=0;
			m_coeff=math::constant::zero<T>();
		}

		integral_box(math::point2i const &of, math::size2i const &si, boost::int32_t st, T co=math::constant::one<T>())
			: m_offset(of)
			, m_size(si)
			, m_coeff(co)
		{
			m_p[0]=m_offset.y()*st+m_offset.x();
			m_p[1]=(m_offset.y()+m_size.height())*st+m_offset.x();
			m_p[2]=m_offset.y()*st+m_offset.x()+m_size.width();
			m_p[3]=(m_offset.y()+m_size.height())*st+m_offset.x()+m_size.width();
		}

		inline integral_box scaled(float s, boost::int32_t ns) const
		{
			math::point2i p1=math::point2i(
				boost::math::round(s*m_offset.x()),
				boost::math::round(s*m_offset.y())
			);

			math::point2i p2=math::point2i(
				boost::math::round(s*(m_offset.x()+m_size.width())),
				boost::math::round(s*(m_offset.y()+m_size.height()))
			);

			math::size2i nbs=p2-p1;
			return integral_box(p1, nbs, ns, static_cast<float>(m_coeff)/float(nbs.area()));
		}

		math::point2i	m_offset;
		math::size2i	m_size;
		T				m_coeff;
		boost::int32_t	m_p[4];
	};

	template <typename RT, typename IT, boost::uint32_t N>
	RT integral_sample(IT const *op, integral_box<IT>(&pat)[N])
	{
		IT ret=math::constant::zero<IT>();
		for(boost::uint32_t n=0;n<N;++n)
			ret+=(op[pat[n].m_p[0]]+op[pat[n].m_p[3]]-op[pat[n].m_p[1]]-op[pat[n].m_p[2]])*pat[n].m_coeff;
		return RT(ret);
	}

	template <typename RT, typename IVT, boost::uint32_t N>
	RT integral_sample(IVT const &iv, math::point2i const &of, integral_box<typename gil::channel_type<typename IVT::value_type>::type>(&pat)[N])
	{
		typedef typename gil::channel_type<typename IVT::value_type>::type iv_channel_t;
		return integral_sample<RT, iv_channel_t>(
			reinterpret_cast<iv_channel_t const *>(iv.row_begin(of.y()))+of.x(),
			pat
		);
	}



	template <typename SVT, typename DVT>
	bool integral(SVT const &src, DVT const &dst, typename gil::channel_type<typename SVT::value_type>::type const &sm)
	{
		typedef typename SVT::value_type	src_pixel_t;
		typedef typename DVT::value_type	dst_pixel_t;

		typedef typename gil::channel_type<src_pixel_t>::type src_channel_t;
		typedef typename gil::channel_type<dst_pixel_t>::type dst_channel_t;

		if(src.width()!=(dst.width()-1) || src.height()!=(dst.height()-1) || !src.width()*dst.width())
			return false;
		
		dst_channel_t *dst_row=reinterpret_cast<dst_channel_t *>(dst.row_begin(0));
		for(boost::int32_t x=0;x<dst.width();++x)
			dst_row[x]=math::constant::zero<dst_channel_t>();

		for(boost::int32_t y=0;y<src.height();++y)
		{
			src_channel_t const *src_row=reinterpret_cast<src_channel_t const *>(src.row_begin(y));
			dst_row=reinterpret_cast<dst_channel_t *>(dst.row_begin(y+1))+1;
			dst_channel_t const *prev_dst_row=reinterpret_cast<dst_channel_t const *>(dst.row_begin(y))+1;

			dst_channel_t sum=dst_row[-1]=math::constant::zero<dst_channel_t>();
			
			for(boost::int32_t x=0;x<src.width();++x)
				dst_row[x]=prev_dst_row[x]+(sum+=dst_channel_t(src_row[x]-sm));
		}
		return true;
	}

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_INTEGRAL_H
