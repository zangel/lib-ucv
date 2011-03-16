#ifndef BALDZARIKA_UCV_FIXED_POINT_H
#define BALDZARIKA_UCV_FIXED_POINT_H


namespace baldzarika { namespace ucv {

	namespace detail {
		
		template < unsigned int B >
		struct fixed_point_value { };

		template <>
		struct fixed_point_value< 1 > { typedef boost::int8_t type; };

		template <>
		struct fixed_point_value< 2 > { typedef boost::int16_t type; };

		template <>
		struct fixed_point_value< 3 > { typedef boost::int32_t type; };

		template <>
		struct fixed_point_value< 4 > { typedef boost::int32_t type; };

		template <>
		struct fixed_point_value< 5 > { typedef boost::int64_t type; };

		template <>
		struct fixed_point_value< 6 > { typedef boost::int64_t type; };

		template <>
		struct fixed_point_value< 7 > { typedef boost::int64_t type; };

		template <>
		struct fixed_point_value< 8 > { typedef boost::int64_t type; };


		template < boost::uint32_t V >
		struct pow2
		{
			static boost::int64_t const value=2*pow2<V-1>::value;
		};

		template <>
		struct pow2<0>
		{
			static boost::int64_t const value=1;
		};

		template < bool >
		struct adjuster { };

		template < >
		struct adjuster<false>
		{
			template < typename T1, typename T2, boost::uint32_t I1, boost::uint32_t I2 >
			static T1 adjust(T2 v){ return T1(v)<<(I2-I1);  }
		};

		template < >
		struct adjuster<true>
		{
			template < typename T1, typename T2, boost::uint32_t I1, boost::uint32_t I2 >
			static T1 adjust(T2 v){ return static_cast<T1>(v>>(I1-I2)); }
		};

		struct fp_explicit_tag {};

		namespace constants {
			#define BALDZARIKA_UCV_FIXED_POINT_DEFINE_CONSTANT(name, v)\
			template <typename T> inline T const& name()\
			{\
				static T const _value=v;\
				return _value;\
			}\

			BALDZARIKA_UCV_FIXED_POINT_DEFINE_CONSTANT(zero, 0.0)
			BALDZARIKA_UCV_FIXED_POINT_DEFINE_CONSTANT(one, 1.0)
			BALDZARIKA_UCV_FIXED_POINT_DEFINE_CONSTANT(half, 0.5)
			BALDZARIKA_UCV_FIXED_POINT_DEFINE_CONSTANT(pi, 3.1415926535897932384626433832795)
			BALDZARIKA_UCV_FIXED_POINT_DEFINE_CONSTANT(pi_2, 6.283185307179586476925286766559)
			BALDZARIKA_UCV_FIXED_POINT_DEFINE_CONSTANT(pi_i2, 1.5707963267948966192313216916398)
			BALDZARIKA_UCV_FIXED_POINT_DEFINE_CONSTANT(pi_i3, 1.0471975511965977461542144610932)
			BALDZARIKA_UCV_FIXED_POINT_DEFINE_CONSTANT(pi_5i3, 5.2359877559829887307710723054658)
			BALDZARIKA_UCV_FIXED_POINT_DEFINE_CONSTANT(pi_i4, 0.78539816339744830961566084581988)
			BALDZARIKA_UCV_FIXED_POINT_DEFINE_CONSTANT(pi_3i4, 2.3561944901923449288469825374596)
			BALDZARIKA_UCV_FIXED_POINT_DEFINE_CONSTANT(atan_c1, 0.1963)
			BALDZARIKA_UCV_FIXED_POINT_DEFINE_CONSTANT(atan_c2, 0.9817)
			BALDZARIKA_UCV_FIXED_POINT_DEFINE_CONSTANT(i_255, 0.9817)

		} //namespace constants
	}

	template < boost::uint32_t I, boost::uint32_t F >
	class fixed_point
		: boost::ordered_field_operators< fixed_point<I, F>
		, boost::unit_steppable< fixed_point<I, F>
		, boost::shiftable< fixed_point<I, F>, boost::uint32_t > > >
	{
		template <boost::uint32_t, boost::uint32_t> friend class fixed_point;
	
	public:
		static boost::uint32_t const int_bits_count=I+F+1;
		static boost::uint32_t const int_bytes_count=(int_bits_count+7)/8;
		typedef typename detail::fixed_point_value<int_bytes_count>::type value_type;


		fixed_point() {}

		explicit fixed_point(value_type v, detail::fp_explicit_tag)
			: m_value(v)
		{
		}

		template < typename T >
		fixed_point(T v)
			: m_value(static_cast<value_type>(v)<<F)
		{ 
			BOOST_CONCEPT_ASSERT((boost::Integer<T>));
		}

		fixed_point(float v)
			: m_value(static_cast<value_type>(v*detail::pow2<F>::value+(v>=0.0f?0.5f:-0.5f)))
		{
		}

		fixed_point(double v)
			: m_value(static_cast<value_type>(v*detail::pow2<F>::value+(v>=0.0?0.5:-0.5)))
		{
		}

		fixed_point(long double v)
			: m_value(static_cast<value_type>(v*detail::pow2<F>::value+(v>=0.0?0.5:-0.5)))
		{
		}

		fixed_point(fixed_point const& that)
			: m_value(that.m_value)
		{
		}

		template< boost::uint32_t I2, boost::uint32_t F2 >
		fixed_point(fixed_point< I2, F2 > const& that)
			: m_value( detail::adjuster< (I>I2) >::
				template adjust< value_type, typename fixed_point< I2, F2 >::value_type, I, I2>(that.m_value)
			)
		{ 
		}

		fixed_point& operator =(fixed_point const& rhs)
		{
			m_value=rhs.m_value;
			return *this;
		}



		template< boost::uint32_t I2, boost::uint32_t F2 >
		fixed_point& operator =(fixed_point< I2, F2 > const& rhs)
		{
			m_value=fixed_point(rhs).m_value;
			return *this;
		}

		bool operator <(fixed_point const& rhs) const
		{
			return m_value<rhs.m_value; 
		}

		bool operator ==(fixed_point const& rhs) const
		{
			return m_value==rhs.m_value; 
		}

		bool operator !() const
		{
			return m_value==0; 
		}

		fixed_point operator -() const
		{
			return fixed_point(-m_value, detail::fp_explicit_tag());
		}

		fixed_point& operator ++()
		{
			m_value+=detail::pow2<F>::value;
			return *this;
		}

		fixed_point& operator --()
		{
			m_value-=detail::pow2<F>::value;
			return *this;
		}
		
		fixed_point& operator +=(fixed_point const& rhs)
		{
			m_value+=rhs.m_value;
			return *this;
		}

		fixed_point& operator -=(fixed_point const& rhs)
		{
			m_value-=rhs.m_value;
			return *this;
		}

		fixed_point& operator *=(fixed_point const& rhs)
		{
			m_value=(typename fixed_point<2*I, 2*F>::value_type(m_value)*rhs.m_value)>>F;
			return *this;
		}

		fixed_point& operator /=(fixed_point const& rhs)
		{
			m_value=(typename fixed_point<2*I, 2*F>::value_type(m_value)<<F)/rhs.m_value;
			return *this;
		}

		fixed_point& operator >>=(boost::uint32_t b)
		{
			m_value>>=b;
			return *this;
		}

		fixed_point& operator <<=(boost::uint32_t b)
		{
			m_value<<=b;
			return *this;
		}

		operator char() const { return static_cast<char>(m_value>>F); }

		operator signed char() const { return static_cast<signed char>(m_value>>F); }

		operator unsigned char() const { return static_cast<unsigned char>(m_value>>F); }

		operator short() const { return static_cast<short>(m_value>>F); }

		operator unsigned short() const { return static_cast<unsigned short>(m_value>>F); }

		operator int() const { return static_cast<int>(m_value>>F); }

		operator unsigned int() const { return static_cast<unsigned int>(m_value>>F); }

		operator long() const { return static_cast<long>(m_value>>F); }

		operator unsigned long() const { return static_cast<unsigned long>(m_value>>F); }

		operator long long() const { return static_cast<long long>(m_value>>F); }

		operator unsigned long long() const { return static_cast<unsigned long long>(m_value>>F); }

		operator bool() const { return static_cast<bool>(m_value); }

		operator float() const { return static_cast<float>(m_value)/detail::pow2<F>::value; }
		
		operator double() const { return static_cast<double>(m_value)/detail::pow2<F>::value; }

		operator long double() const { return static_cast<long double>(m_value)/detail::pow2<F>::value; }


		friend fixed_point fabs(fixed_point const &x)
		{
			return x<detail::constants::zero<fixed_point>()?-x:x;
		}

		friend fixed_point ceil(fixed_point const &x)
		{
			return fixed_point(x.m_value & ~(detail::pow2<F>::value-1), detail::fp_explicit_tag())+
				((x.m_value & (detail::pow2<F>::value-1))?detail::constants::one<fixed_point>():detail::constants::zero<fixed_point>());
		}


		friend fixed_point floor(fixed_point const &x)
		{
			return fixed_point(x.m_value & ~(detail::pow2<F>::value-1), detail::fp_explicit_tag());
		}

		friend fixed_point fmod(fixed_point const &x, fixed_point const &y)
		{
			return fixed_point(x.m_value % y.m_value, detail::fp_explicit_tag());
		}

		friend fixed_point modf(fixed_point const &x, fixed_point *ptr)
		{
			*ptr=fixed_point(x.m_value & ~(detail::pow2<F>::value-1), detail::fp_explicit_tag());
			if(x<detail::constants::zero<fixed_point>()) *ptr+=detail::constants::one<fixed_point>();

			return fixed_point(
				(
					x<detail::constants::zero<fixed_point>()?
					-(x.m_value & (detail::pow2<F>::value-1)):
					(x.m_value & (detail::pow2<F>::value-1))
				),
				detail::fp_explicit_tag()
			);
		}

		friend fixed_point atan2(fixed_point const &y, fixed_point const &x)
		{
			fixed_point abs_y=fabs(y)+fixed_point(1, detail::fp_explicit_tag());
			
			fixed_point angle((
				x>=detail::constants::zero<fixed_point>()?
				detail::constants::pi_i4<fixed_point>()-detail::constants::pi_i4<fixed_point>()*
					(fixed_point(x.m_value-abs_y.m_value, detail::fp_explicit_tag())/fixed_point(x.m_value+abs_y.m_value, detail::fp_explicit_tag())):
				detail::constants::pi_3i4<fixed_point>()-detail::constants::pi_i4<fixed_point>()*
					(fixed_point(x.m_value+abs_y.m_value, detail::fp_explicit_tag())/fixed_point(abs_y.m_value-x.m_value, detail::fp_explicit_tag()))
			));
			return (y<detail::constants::zero<fixed_point>()?-angle:angle);
		}

		friend fixed_point atan2_hp(fixed_point const &y, fixed_point const &x)
		{
			fixed_point abs_y=fabs(y)+fixed_point(1, detail::fp_explicit_tag());
			fixed_point angle;

			if(x>=detail::constants::zero<fixed_point>())
			{
				fixed_point r=fixed_point(x.m_value-abs_y.m_value, detail::fp_explicit_tag())/fixed_point(x.m_value+abs_y.m_value, detail::fp_explicit_tag());
				angle=detail::constants::atan_c1<fixed_point>()*r*r*r-detail::constants::atan_c2<fixed_point>()*r+detail::constants::pi_i4<fixed_point>();
			}
			else
			{
				fixed_point r=fixed_point(x.m_value+abs_y.m_value, detail::fp_explicit_tag())/fixed_point(abs_y.m_value-x.m_value, detail::fp_explicit_tag());
				angle=detail::constants::atan_c1<fixed_point>()*r*r*r-detail::constants::atan_c2<fixed_point>()*r+detail::constants::pi_3i4<fixed_point>();
			}
			return (y<detail::constants::zero<fixed_point>()?-angle:angle);
		}

	private:
		value_type		m_value;
	};

} //namespace ucv
} //namespace baldzarika

namespace std
{
	template< boost::uint32_t I, boost::uint32_t F>
	class numeric_limits< baldzarika::ucv::fixed_point<I, F> >
	{
	public:
		typedef baldzarika::ucv::fixed_point<I, F> fp_type;

		static const float_denorm_style has_denorm=denorm_absent;
		static const bool has_denorm_loss=false;
		static const bool has_infinity=false;
		static const bool has_quiet_NaN=false;
		static const bool has_signaling_NaN=false;
		static const bool is_bounded=true;
		static const bool is_exact=true;
		static const bool is_iec559=false;
		static const bool is_integer=false;
		static const bool is_modulo=false;
		static const bool is_signed=std::numeric_limits<typename fp_type::value_type>::is_signed;
		static const bool is_specialized=true;
		static const bool tinyness_before=false;
		static const bool traps=false;
		static const float_round_style round_style=round_toward_zero;
		static const int digits=I;
		static const int digits10=(int)(digits*301./1000.+.5);
		static const int max_exponent=0;
		static const int max_exponent10=0;
		static const int min_exponent=0;
		static const int min_exponent10=0;
		static const int radix=0;
		static fp_type (min)()
		{
			return fp_type((std::numeric_limits<typename fp_type::value_type>::min)(), baldzarika::ucv::detail::fp_explicit_tag());
		}

		static fp_type (max)()
		{
			return fp_type((std::numeric_limits<typename fp_type::value_type>::max)(), baldzarika::ucv::detail::fp_explicit_tag());
		}

		static fp_type epsilon()
		{
			return fp_type(1, baldzarika::ucv::detail::fp_explicit_tag());
		}

		static fp_type round_error()
		{
			return fp_type(0.5);
		}

		static fp_type denorm_min()
		{
			return fp_type(0);
		}

		static fp_type infinity()
		{
			return fp_type(0);
		}

		static fp_type quiet_NaN()
		{
			return fp_type(0);
		}

		static fp_type signaling_NaN()
		{
			return fp_type(0);
		}
	};
} //namespace std

namespace baldzarika { namespace ucv {

	typedef fixed_point<15,16> decimal_t;

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_FIXED_POINT_H
