#ifndef BALDZARIKA_UCV_QR_CODE_H
#define BALDZARIKA_UCV_QR_CODE_H

#include <baldzarika/ucv/masked_region.h>

namespace baldzarika { namespace ucv {

	namespace qr {
		
		template < boost::int32_t V >
		struct get_dimension
		{
			enum { value=17+4*V };
		};

		template < boost::int32_t D >
		struct get_version
		{
			enum { value=(D-17)/4 };
		};

		typedef enum
		{
			ECL_INVALID=-1,
			ECL_L=0,
			ECL_M=1,
			ECL_Q=2,
			ECL_H=3
		} eECL;

		typedef enum
		{
			FMT_INVALID=-1,
			FMT_000=0,
			FMT_001=1,
			FMT_010=2,
			FMT_011=3,
			FMT_100=4,
			FMT_101=5,
			FMT_110=6,
			FMT_111=7
		} eFMT;

		struct flip_mask
		{
			typedef bool (*mask_fn_t)(boost::int32_t,boost::int32_t);

			flip_mask()
				: m_fmt(FMT_INVALID)
			{

			}

			flip_mask(eFMT fmt)
				: m_fmt(fmt)
			{
			}

			inline operator bool() const
			{
				return m_fmt!=FMT_INVALID;
			}

			inline bool operator !() const
			{
				return m_fmt==FMT_INVALID;
			}

			template < eFMT >
			static bool is_masked(boost::int32_t x, boost::int32_t y);

			template < > static bool is_masked<FMT_000>(boost::int32_t x, boost::int32_t y){ return (x+y)%2==0; }
			template < > static bool is_masked<FMT_001>(boost::int32_t x, boost::int32_t y){ return x%2==0; }
			template < > static bool is_masked<FMT_010>(boost::int32_t x, boost::int32_t y){ return y%3==0; }
			template < > static bool is_masked<FMT_011>(boost::int32_t x, boost::int32_t y){ return (x+y)%3==0; }
			template < > static bool is_masked<FMT_100>(boost::int32_t x, boost::int32_t y){ return (((x>>1)+(y/3))%2)==0; }
			template < > static bool is_masked<FMT_101>(boost::int32_t x, boost::int32_t y){ boost::int32_t t=x*y; return (t%2)+(t%3)==0; }
			template < > static bool is_masked<FMT_110>(boost::int32_t x, boost::int32_t y){ boost::int32_t t=x*y; return (((t%2)+(t%3))%2)==0; }
			template < > static bool is_masked<FMT_111>(boost::int32_t x, boost::int32_t y){ return ((((x+y)%2)+((x*y)%3))%2)==0; }
			
			mask_fn_t get_mask_fn() const
			{
				switch(m_fmt)
				{
				case FMT_000:
					return &flip_mask::is_masked<FMT_000>;
				case FMT_001:
					return &flip_mask::is_masked<FMT_001>;
				case FMT_010:
					return &flip_mask::is_masked<FMT_010>;
				case FMT_011:
					return &flip_mask::is_masked<FMT_011>;
				case FMT_100:
					return &flip_mask::is_masked<FMT_100>;
				case FMT_101:
					return &flip_mask::is_masked<FMT_101>;
				case FMT_110:
					return &flip_mask::is_masked<FMT_110>;
				case FMT_111:
					return &flip_mask::is_masked<FMT_111>;
				}
				return 0;
			}

			template < boost::int32_t V >
			bool flip(gil::gray8_view_t bv) const
			{
				if(bv.width()!=get_dimension<V>::value || bv.height()!=get_dimension<V>::value)
					return false;

				mask_fn_t mask_fn=get_mask_fn();
				if(!mask_fn) return false;
				for(boost::int32_t y=0;y<get_dimension<V>::value;++y)
				{
					for(boost::int32_t x=0;x<get_dimension<V>::value;++x)
					{
						if(mask_fn(y,x)) bv(x,y)=bv(x,y)?0:1;
					}
				}
				return true;
			}

			eFMT m_fmt;
		};

		struct format_info
		{
			enum { NUM_FORMAT_INFOS = 32 };

			static inline boost::int32_t const* lookup(boost::int32_t n)
			{
				static boost::int32_t const table[NUM_FORMAT_INFOS][2]=
				{
					{ 0x5412, 0x00 },
					{ 0x5125, 0x01 },
					{ 0x5E7C, 0x02 },
					{ 0x5B4B, 0x03 },
					{ 0x45F9, 0x04 },
					{ 0x40CE, 0x05 },
					{ 0x4F97, 0x06 },
					{ 0x4AA0, 0x07 },
					{ 0x77C4, 0x08 },
					{ 0x72F3, 0x09 },
					{ 0x7DAA, 0x0A },
					{ 0x789D, 0x0B },
					{ 0x662F, 0x0C },
					{ 0x6318, 0x0D },
					{ 0x6C41, 0x0E },
					{ 0x6976, 0x0F },
					{ 0x1689, 0x10 },
					{ 0x13BE, 0x11 },
					{ 0x1CE7, 0x12 },
					{ 0x19D0, 0x13 },
					{ 0x0762, 0x14 },
					{ 0x0255, 0x15 },
					{ 0x0D0C, 0x16 },
					{ 0x083B, 0x17 },
					{ 0x355F, 0x18 },
					{ 0x3068, 0x19 },
					{ 0x3F31, 0x1A },
					{ 0x3A06, 0x1B },
					{ 0x24B4, 0x1C },
					{ 0x2183, 0x1D },
					{ 0x2EDA, 0x1E },
					{ 0x2BED, 0x1F }
				};
				return table[n];
			}

			format_info()
				: m_ecl(ECL_INVALID)
				, m_flip_mask(FMT_INVALID)
			{

			}

			format_info(eECL ecl, eFMT fmt)
				: m_ecl(ecl)
				, m_flip_mask(fmt)
			{

			}

			inline operator bool() const
			{
				return m_ecl!=ECL_INVALID && m_flip_mask;
			}

			inline bool operator !() const
			{
				return m_ecl==ECL_INVALID || !m_flip_mask;
			}

			static inline format_info decode(boost::int32_t fi1, boost::int32_t fi2)
			{
				for(boost::int32_t filup=0;filup<NUM_FORMAT_INFOS;++filup)
				{
					boost::int32_t const *fi=lookup(filup);
					if(fi1==fi[0] || fi2==fi[0])
						return format_info(eECL((fi[1]>>3) & 0x03), eFMT(fi[1] & 0x07));
				}
				return format_info();
			}

			template < boost::int32_t V >
			static inline format_info parse(gil::gray8c_view_t bv)
			{
				if(bv.width()!=get_dimension<V>::value || bv.height()!=get_dimension<V>::value)
					return format_info();
				
				boost::int32_t fi_bits1=0;

				for(boost::int32_t x=0;x<6;++x)
					fi_bits1=(fi_bits1<<1) | (bv(x,8)?1:0);

				fi_bits1=(fi_bits1<<1) | (bv(7,8)?1:0);
				fi_bits1=(fi_bits1<<1) | (bv(8,8)?1:0);
				fi_bits1=(fi_bits1<<1) | (bv(8,7)?1:0);

				for(boost::int32_t y=5;y>=0;--y)
					fi_bits1=(fi_bits1<<1) | (bv(8,y)?1:0);

				boost::int32_t fi_bits2=0;
				for(boost::int32_t y=get_dimension<V>::value-1;y>=get_dimension<V>::value-7;--y)
					fi_bits2=(fi_bits2<<1) | (bv(8,y)?1:0);
				
				for(boost::int32_t x=get_dimension<V>::value-8;x<get_dimension<V>::value;++x)
					fi_bits2=(fi_bits2<<1) | (bv(x,8)?1:0);

				if(format_info fi=decode(fi_bits1,fi_bits2))
					return fi;
				return decode(fi_bits1 ^ 0x5412, fi_bits2 ^ 0x5412);
			}

			eECL		m_ecl;
			flip_mask	m_flip_mask;
		};

		template < boost::int32_t V >
		struct version;

		template <>
		struct version<1>
		{
			typedef masked_region
			<
				ucv::detail::rectangle_mask< 0, 0, 9, 9,
				ucv::detail::rectangle_mask< get_dimension<1>::value-8, 0, 8, 9,
				ucv::detail::rectangle_mask< 0, get_dimension<1>::value-8, 9, 8,
				ucv::detail::rectangle_mask< 6, 9, 1, get_dimension<1>::value-17,
				ucv::detail::rectangle_mask< 9, 6, get_dimension<1>::value-17, 1 > > > > >
			> data_mask_t;
		};

		template <>
		struct version<2>
		{
			typedef masked_region
			<
				ucv::detail::rectangle_mask< 0, 0, 9, 9,
				ucv::detail::rectangle_mask< get_dimension<2>::value-8, 0, 8, 9,
				ucv::detail::rectangle_mask< 0, get_dimension<2>::value-8, 9, 8,
				ucv::detail::rectangle_mask< get_dimension<2>::value-9, get_dimension<2>::value-9, 5, 5,
				ucv::detail::rectangle_mask< 6, 9, 1, get_dimension<2>::value-17,
				ucv::detail::rectangle_mask< 9, 6, get_dimension<2>::value-17, 1 > > > > > >
			> data_mask_t;
		};
		
		template <>
		struct version<3>
		{
			typedef masked_region
			<
				ucv::detail::rectangle_mask< 0, 0, 9, 9,
				ucv::detail::rectangle_mask< get_dimension<3>::value-8, 0, 8, 9,
				ucv::detail::rectangle_mask< 0, get_dimension<3>::value-8, 9, 8,
				ucv::detail::rectangle_mask< 20, 20, 5, 5,
				ucv::detail::rectangle_mask< 6, 9, 1, get_dimension<3>::value-17,
				ucv::detail::rectangle_mask< 9, 6, get_dimension<3>::value-17, 1 > > > > > >
			> data_mask_t;
		};
		
		template <>
		struct version<4>
		{
			typedef masked_region
			<
				ucv::detail::rectangle_mask< 0, 0, 9, 9,
				ucv::detail::rectangle_mask< get_dimension<4>::value-8, 0, 8, 9,
				ucv::detail::rectangle_mask< 0, get_dimension<4>::value-8, 9, 8,
				ucv::detail::rectangle_mask< get_dimension<4>::value-9, get_dimension<4>::value-9, 5, 5,
				ucv::detail::rectangle_mask< 6, 9, 1, get_dimension<4>::value-17,
				ucv::detail::rectangle_mask< 9, 6, get_dimension<4>::value-17, 1 > > > > > >
			> data_mask_t;
		};
		
		template < boost::int32_t V >
		struct data
		{
			format_info m_format_info;
		};

	} //namespace qr


	template < boost::int32_t V >
	boost::shared_ptr< qr::data<V> > qr_decode(gil::gray8_view_t bv)
	{
		boost::shared_ptr< qr::data<V> > ret_val(new qr::data<V>() );
		if(!(ret_val->m_format_info=qr::format_info::parse<V>(bv))) return boost::shared_ptr< qr::data<V> >();
		if(!(ret_val->m_format_info.m_flip_mask.flip<V>(bv))) return boost::shared_ptr< qr::data<V> >();

		bool read_up=true;
		boost::uint8_t current_byte=0;
		boost::int32_t bits_read=0;
		boost::int32_t total_bits_read=0;
		boost::int32_t result_offset=0;

		for(boost::int32_t x=qr::get_dimension<V>::value-1;x>0;x-=2)
		{
			if(x==6) x--;
			
			for(boost::int32_t c=0;c<qr::get_dimension<V>::value;c++)
			{
				boost::int32_t y=read_up?qr::get_dimension<V>::value-1-c:c;
				for(boost::int32_t dx=0;dx<2;dx++)
				{
					if(!qr::version<V>::data_mask_t::is_masked(x-dx,y))
					{
						bits_read++;
						total_bits_read++;
						current_byte<<=1;
						current_byte|=bv(x-dx,y)?1:0;
						
						if(bits_read==8)
						{
							bits_read=0;
							current_byte=0;
							result_offset++;
						}
					}
				}
			}
			read_up=!read_up;
		}
		
		return ret_val;
	}
	
} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_QR_CODE_H