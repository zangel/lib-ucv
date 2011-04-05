#ifndef BALDZARIKA_UCV_FIXED_POINT_H
#define BALDZARIKA_UCV_FIXED_POINT_H

#if defined(BALDZARIKA_UCV_FIXED_POINT_TRIGONO_USE_FPU)

#define BALDZARIKA_UCV_FIXED_POINT_TRIGONO_USE_FPU_ATAN2
#define BALDZARIKA_UCV_FIXED_POINT_TRIGONO_USE_FPU_EXP
#define BALDZARIKA_UCV_FIXED_POINT_TRIGONO_USE_FPU_COS
#define BALDZARIKA_UCV_FIXED_POINT_TRIGONO_USE_FPU_SIN
#define BALDZARIKA_UCV_FIXED_POINT_TRIGONO_USE_FPU_SQRT

#endif

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

		template < bool, typename T1, typename T2 >
		struct select {};

		template < typename T1, typename T2 >
		struct select<true, T1, T2>
		{
			typedef T1 type;
		};

		template < typename T1, typename T2 >
		struct select<false, T1, T2>
		{
			typedef T2 type;
		};



		template < bool >
		struct adjuster { };

		template < >
		struct adjuster<true>
		{
			template < typename T1, typename T2, boost::uint32_t F1, boost::uint32_t F2 >
			static T1 adjust(T2 v)
			{
				typedef typename select<
					(std::numeric_limits<T1>::digits > std::numeric_limits<T2>::digits),
					T1,
					T2
				>::type greater_type;
				return static_cast<T1>(greater_type(v)<<(F1-F2));
			}
		};

		template < >
		struct adjuster<false>
		{
			template < typename T1, typename T2, boost::uint32_t F1, boost::uint32_t F2 >
			static T1 adjust(T2 v)
			{
				typedef typename select<
					bool(std::numeric_limits<T1>::digits > std::numeric_limits<T2>::digits),
					T1,
					T2
				>::type greater_type;
				return static_cast<T1>(greater_type(v)>>(F2-F1));
			}
		};

		struct fp_explicit_tag {};

		namespace constants {
			#define BALDZARIKA_UCV_FIXED_POINT_DEFINE_CONSTANT(name, v)\
			template <typename T> inline T const& name()\
			{\
				static T const _value=static_cast<T>(v);\
				return _value;\
			}\

			BALDZARIKA_UCV_FIXED_POINT_DEFINE_CONSTANT(zero, 0.0)
			BALDZARIKA_UCV_FIXED_POINT_DEFINE_CONSTANT(one, 1.0)
			BALDZARIKA_UCV_FIXED_POINT_DEFINE_CONSTANT(two, 2.0)
			BALDZARIKA_UCV_FIXED_POINT_DEFINE_CONSTANT(four, 4.0)
			BALDZARIKA_UCV_FIXED_POINT_DEFINE_CONSTANT(sq_two, 1.4142135623730950488016887242097)
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
			BALDZARIKA_UCV_FIXED_POINT_DEFINE_CONSTANT(e, 2.7182818284590452353602874713527)
			BALDZARIKA_UCV_FIXED_POINT_DEFINE_CONSTANT(inv_e, 0.36787944117144232159552377016146)

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
		static boost::uint32_t const IS=I;
		static boost::uint32_t const FS=F;

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
			: m_value( detail::adjuster< (F>F2) >::
				template adjust< value_type, typename fixed_point< I2, F2 >::value_type, F, F2>(that.m_value)
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

		template< typename T >
		friend T round(fixed_point const &x)
		{
			return static_cast<T>(x>=detail::constants::zero<fixed_point>()?
				x+detail::constants::half<fixed_point>():
			x-detail::constants::half<fixed_point>()
				);
		}

		friend fixed_point log(fixed_point const &x)
		{
			return fixed_point(::logf(static_cast<float>(x)));
		}

		friend fixed_point pow(fixed_point const &x, fixed_point const &y)
		{
			return fixed_point(::pow(static_cast<float>(x), static_cast<float>(y)));
		}

#if defined(BALDZARIKA_UCV_FIXED_POINT_TRIGONO_USE_FPU_ATAN2)
		
		friend fixed_point atan2(fixed_point const &y, fixed_point const &x)
		{
			return fixed_point(::atan2(static_cast<float>(y), static_cast<float>(x)));
		}

#else

#if defined(BALDZARIKA_UCV_FIXED_POINT_TRIGONO_USE_FIXED_POINT_ATAN2_HIGH_PRECISION)
		friend fixed_point atan2(fixed_point const &y, fixed_point const &x)
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
#else
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

#endif
#endif

#if defined(BALDZARIKA_UCV_FIXED_POINT_TRIGONO_USE_FPU_EXP)

		friend fixed_point exp(fixed_point const &x)
		{
			return fixed_point(::exp(static_cast<float>(x)));
		}

#else
		friend fixed_point exp(fixed_point const &x)
		{
			static fixed_point const a[]=
			{
				1.64872127070012814684865078781,
				1.28402541668774148407342056806,
				1.13314845306682631682900722781,
				1.06449445891785942956339059464,
				1.03174340749910267093874781528,
				1.01574770858668574745853507208,
				1.00784309720644797769345355976,
				1.00391388933834757344360960390,
				1.00195503359100281204651889805,
				1.00097703949241653524284529261,
				1.00048840047869447312617362381,
				1.00024417042974785493700523392,
				1.00012207776338377107650351967,
				1.00006103701893304542177912060,
				1.00003051804379102429545128481,
				1.00001525890547841394814004262,
				1.00000762942363515447174318433,
				1.00000381470454159186605078771,
				1.00000190735045180306002872525,
				1.00000095367477115374544678825,
				1.00000047683727188998079165439,
				1.00000023841860752327418915867,
				1.00000011920929665620888994533,
				1.00000005960464655174749969329,
				1.00000002980232283178452676169,
				1.00000001490116130486995926397,
				1.00000000745058062467940380956,
				1.00000000372529030540080797502,
				1.00000000186264515096568050830,
				1.00000000093132257504915938475,
				1.00000000046566128741615947508
			};

			fixed_point y(1);
			for(boost::int32_t i=F-1; i>=0; --i)
			{
				if(!(x.m_value & 1<<i))
					y*=a[F-i-1];
			}

			boost::int32_t x_int=floor(x);
			if(x_int<0)
			{
				for(boost::int32_t i=1;i<=-x_int;++i)
					y*=detail::constants::inv_e<fixed_point>();
			}
			else
			{
				for(boost::int32_t i=1;i<=x_int;++i)
					y*=detail::constants::e<fixed_point>();
			}
			return y;
		}
#endif

#if defined(BALDZARIKA_UCV_FIXED_POINT_TRIGONO_USE_FPU_COS)

		friend fixed_point cos(fixed_point const &x)
		{
			return fixed_point(::cos(static_cast<float>(x)));
		}
#else

		friend fixed_point cos(fixed_point const &x)
		{
			static fixed_point const i_2t3t4=0.04166666666666666666666666666667;
			static fixed_point const i_2t3t4t5t6=0.00138888888888888888888888888889;

			fixed_point x_=fmod(x, detail::constants::pi_2<fixed_point>());
			if(x_>detail::constants::pi<fixed_point>())
				x_-=detail::constants::pi_2<fixed_point>();
			fixed_point xx=x_*x_;
			fixed_point y=-xx*i_2t3t4t5t6;
			y+=i_2t3t4;
			y*=xx;
			y-=detail::constants::half<fixed_point>();
			y*=xx;
			return y+detail::constants::one<fixed_point>();
		}

#endif

#if defined(BALDZARIKA_UCV_FIXED_POINT_TRIGONO_USE_FPU_SIN)

		friend fixed_point sin(fixed_point const &x)
		{
			return fixed_point(::sin(static_cast<float>(x)));
		}

#else

		friend fixed_point sin(fixed_point const &x)
		{
			static fixed_point const i_2t3=0.16666666666666666666666666666667;
			static fixed_point const i_2t3t4t5=0.00833333333333333333333333333333;
			static fixed_point const i_2t3t4t5t6t7=1.984126984126984126984126984127e-4;

			fixed_point x_=fmod(x, detail::constants::pi_2<fixed_point>());
			if(x_>detail::constants::pi<fixed_point>())
				x_-=detail::constants::pi_2<fixed_point>();

			fixed_point xx=x_*x_;

			fixed_point y=-xx*i_2t3t4t5t6t7;
			y+=i_2t3t4t5;
			y*=xx;
			y-=i_2t3;
			y*=xx;
			y+=detail::constants::one<fixed_point>();
			return y*x_;
		}

#endif

#if defined(BALDZARIKA_UCV_FIXED_POINT_TRIGONO_USE_FPU_SQRT)

		friend fixed_point sqrt(fixed_point const &x)
		{
			return fixed_point(::sqrt(static_cast<float>(x)));
		}

#else
		
		friend fixed_point sqrt(fixed_point const &x)
		{
			//return fixed_point(::sqrt(static_cast<float>(x)));

			if(x<detail::constants::zero<fixed_point>())
				return 0;
			
			typename fixed_point<2*I, 2*F>::value_type op=
				typename fixed_point<2*I, 2*F>::value_type(x.m_value)<<(I+1);
			
			typename fixed_point<2*I, 2*F>::value_type res=0;
			typename fixed_point<2*I, 2*F>::value_type one=
				typename fixed_point<2*I, 2*F>::value_type(1)<<
				(std::numeric_limits<typename fixed_point<2*I, 2*F>::value_type>::digits-1);

			while(one>op)
				one>>=2;
			
			while(one!=0)
			{
				if(op>=res+one)
				{
					op=op-(res+one);
					res=res+(one<<1);
				}
				res >>= 1;
				one >>= 2;
			}
			return fixed_point(static_cast<value_type>(res), detail::fp_explicit_tag());
		}
#endif
	
	private:
		value_type		m_value;
	};

} //namespace ucv
} //namespace baldzarika

namespace std
{
	template <boost::uint32_t I, boost::uint32_t F>
	baldzarika::ucv::fixed_point<I,F> sqrt(baldzarika::ucv::fixed_point<I,F> const &x)
	{
		return baldzarika::ucv::fixed_point<I,F>(std::sqrt(static_cast<float>(x)));
	}


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
