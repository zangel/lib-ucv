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
	template <boost::uint32_t I, boost::uint32_t F > class fixed_point;
} //namespace ucv
} //namespace baldzarika

namespace std {

	template < boost::uint32_t I, boost::uint32_t F >
	baldzarika::ucv::fixed_point<I,F> abs(baldzarika::ucv::fixed_point<I,F> const &);

	template < boost::uint32_t I, boost::uint32_t F >
	baldzarika::ucv::fixed_point<I,F> fabs(baldzarika::ucv::fixed_point<I,F> const &);

	template < boost::uint32_t I, boost::uint32_t F >
	baldzarika::ucv::fixed_point<I,F> ceil(baldzarika::ucv::fixed_point<I,F> const &);

	template < boost::uint32_t I, boost::uint32_t F >
	baldzarika::ucv::fixed_point<I,F> floor(baldzarika::ucv::fixed_point<I,F> const &);

	template <boost::uint32_t I, boost::uint32_t F>
	baldzarika::ucv::fixed_point<I,F> fmod(baldzarika::ucv::fixed_point<I,F> const &, baldzarika::ucv::fixed_point<I,F> const &);

	template <boost::uint32_t I, boost::uint32_t F>
	baldzarika::ucv::fixed_point<I,F> modf(baldzarika::ucv::fixed_point<I,F> const &, baldzarika::ucv::fixed_point<I,F> *);

	template < typename T, boost::uint32_t I, boost::uint32_t F>
	T round(baldzarika::ucv::fixed_point<I,F> const &);

	template <boost::uint32_t I, boost::uint32_t F>
	baldzarika::ucv::fixed_point<I,F> sqrt(baldzarika::ucv::fixed_point<I,F> const &);

	template <boost::uint32_t I, boost::uint32_t F>
	baldzarika::ucv::fixed_point<I,F> log(baldzarika::ucv::fixed_point<I,F> const &);

	template <boost::uint32_t I, boost::uint32_t F>
	baldzarika::ucv::fixed_point<I,F> pow(baldzarika::ucv::fixed_point<I,F> const &, baldzarika::ucv::fixed_point<I,F> const &);

	template <boost::uint32_t I, boost::uint32_t F>
	baldzarika::ucv::fixed_point<I,F> exp(baldzarika::ucv::fixed_point<I,F> const &);

	template <boost::uint32_t I, boost::uint32_t F>
	baldzarika::ucv::fixed_point<I,F> atan2(baldzarika::ucv::fixed_point<I,F> const &, baldzarika::ucv::fixed_point<I,F> const &);

	template <boost::uint32_t I, boost::uint32_t F>
	baldzarika::ucv::fixed_point<I,F> sin(baldzarika::ucv::fixed_point<I,F> const &);
	
	template <boost::uint32_t I, boost::uint32_t F>
	baldzarika::ucv::fixed_point<I,F> cos(baldzarika::ucv::fixed_point<I,F> const &);

} //namespace std

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

		namespace constant {
			#define BALDZARIKA_UCV_FIXED_POINT_DEFINE_CONSTANT(name, v)\
			template <typename T> inline T const& name()\
			{\
				static T const _value=static_cast<T>(v);\
				return _value;\
			}\

			BALDZARIKA_UCV_FIXED_POINT_DEFINE_CONSTANT(zero, 0.0)
			BALDZARIKA_UCV_FIXED_POINT_DEFINE_CONSTANT(one, 1.0)
			BALDZARIKA_UCV_FIXED_POINT_DEFINE_CONSTANT(two, 2.0)
			BALDZARIKA_UCV_FIXED_POINT_DEFINE_CONSTANT(three, 3.0)
			BALDZARIKA_UCV_FIXED_POINT_DEFINE_CONSTANT(four, 4.0)
			BALDZARIKA_UCV_FIXED_POINT_DEFINE_CONSTANT(five, 5.0)
			BALDZARIKA_UCV_FIXED_POINT_DEFINE_CONSTANT(six, 6.0)
			BALDZARIKA_UCV_FIXED_POINT_DEFINE_CONSTANT(eleven, 11.0)
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
			BALDZARIKA_UCV_FIXED_POINT_DEFINE_CONSTANT(i_255, 0.0039215686274509803921568627451)
			BALDZARIKA_UCV_FIXED_POINT_DEFINE_CONSTANT(e, 2.7182818284590452353602874713527)
			BALDZARIKA_UCV_FIXED_POINT_DEFINE_CONSTANT(inv_e, 0.36787944117144232159552377016146)

		} //namespace constant
	}

	template < boost::uint32_t I, boost::uint32_t F >
	class fixed_point
		//: boost::ordered_field_operators< fixed_point<I, F>
		//, boost::unit_steppable< fixed_point<I, F>
		//, boost::shiftable< fixed_point<I, F>, boost::uint32_t > 
		//> >
	{
		template <boost::uint32_t, boost::uint32_t> friend class fixed_point;
	
	public:
		static boost::uint32_t const IS=I;
		static boost::uint32_t const FS=F;

		static boost::uint32_t const int_bits_count=I+F+1;
		static boost::uint32_t const int_bytes_count=(int_bits_count+7)/8;
		typedef typename detail::fixed_point_value<int_bytes_count>::type value_type;

#if defined (NDEBUG)
		fixed_point()
			: m_value(0)
		{
		}
#else
		fixed_point()
			: m_value(0)
		{
		}
#endif

		inline explicit fixed_point(value_type v, detail::fp_explicit_tag)
			: m_value(v)
		{
		}

#define BALDZARIKA_UCV_FIXED_POINT_INTEGRAL_CONSTRUCTOR(T)\
		inline fixed_point(T v)\
			: m_value(static_cast<value_type>(v)<<F)\
		{\
		}
		
		BALDZARIKA_UCV_FIXED_POINT_INTEGRAL_CONSTRUCTOR(boost::uint8_t)
		BALDZARIKA_UCV_FIXED_POINT_INTEGRAL_CONSTRUCTOR(boost::uint16_t)
		BALDZARIKA_UCV_FIXED_POINT_INTEGRAL_CONSTRUCTOR(boost::uint32_t)
		BALDZARIKA_UCV_FIXED_POINT_INTEGRAL_CONSTRUCTOR(boost::uint64_t)

		BALDZARIKA_UCV_FIXED_POINT_INTEGRAL_CONSTRUCTOR(boost::int8_t)
		BALDZARIKA_UCV_FIXED_POINT_INTEGRAL_CONSTRUCTOR(boost::int16_t)
		BALDZARIKA_UCV_FIXED_POINT_INTEGRAL_CONSTRUCTOR(boost::int32_t)
		BALDZARIKA_UCV_FIXED_POINT_INTEGRAL_CONSTRUCTOR(boost::int64_t)
	
		

		inline fixed_point(float v)
			: m_value(static_cast<value_type>(v*detail::pow2<F>::value+(v>=0.0f?0.5f:-0.5f)))
		{
		}

		inline fixed_point(double v)
			: m_value(static_cast<value_type>(v*detail::pow2<F>::value+(v>=0.0?0.5:-0.5)))
		{
		}

		inline fixed_point(long double v)
			: m_value(static_cast<value_type>(v*detail::pow2<F>::value+(v>=0.0?0.5:-0.5)))
		{
		}

		inline fixed_point(fixed_point const& that)
			: m_value(that.m_value)
		{
		}

		template< boost::uint32_t I2, boost::uint32_t F2 >
		inline fixed_point(fixed_point< I2, F2 > const& that)
			: m_value( detail::adjuster< (F>F2) >::
				template adjust< value_type, typename fixed_point< I2, F2 >::value_type, F, F2>(that.m_value)
			)
		{ 
		}

		inline fixed_point& operator =(fixed_point const& rhs)
		{
			m_value=rhs.m_value;
			return *this;
		}

		template< boost::uint32_t I2, boost::uint32_t F2 >
		inline fixed_point& operator =(fixed_point< I2, F2 > const& rhs)
		{
			m_value=fixed_point(rhs).m_value;
			return *this;
		}

		inline bool operator <(fixed_point const& rhs) const
		{
			return m_value<rhs.m_value; 
		}

		inline bool operator <=(fixed_point const& rhs) const
		{
			return m_value<=rhs.m_value; 
		}

		inline bool operator >(fixed_point const& rhs) const
		{
			return m_value>rhs.m_value; 
		}

		inline bool operator >=(fixed_point const& rhs) const
		{
			return m_value>=rhs.m_value; 
		}

		inline bool operator ==(fixed_point const& rhs) const
		{
			return m_value==rhs.m_value; 
		}

		inline bool operator !=(fixed_point const& rhs) const
		{
			return m_value!=rhs.m_value; 
		}

		inline bool operator !() const
		{
			return m_value==0;
		}

		inline fixed_point operator -() const
		{
			return fixed_point(-m_value, detail::fp_explicit_tag());
		}

		inline fixed_point& operator ++()
		{
			m_value+=detail::pow2<F>::value;
			return *this;
		}

		inline fixed_point& operator --()
		{
			m_value-=detail::pow2<F>::value;
			return *this;
		}
		
		inline fixed_point& operator +=(fixed_point const& rhs)
		{
			m_value+=rhs.m_value;
			return *this;
		}

		inline fixed_point operator +(fixed_point const& rhs) const
		{
			return fixed_point(m_value+rhs.m_value, detail::fp_explicit_tag());
		}

		inline fixed_point& operator -=(fixed_point const& rhs)
		{
			m_value-=rhs.m_value;
			return *this;
		}

		inline fixed_point operator -(fixed_point const& rhs) const
		{
			return fixed_point(m_value-rhs.m_value, detail::fp_explicit_tag());
		}

		inline fixed_point& operator *=(fixed_point const& rhs)
		{
			m_value=(typename fixed_point<2*I, 2*F>::value_type(m_value)*rhs.m_value)>>F;
			return *this;
		}

		inline fixed_point operator *(fixed_point const& rhs) const
		{
			return fixed_point(
				(typename fixed_point<2*I, 2*F>::value_type(m_value)*rhs.m_value)>>F,
				detail::fp_explicit_tag()
			);
		}

		inline fixed_point& operator /=(fixed_point const& rhs)
		{
			m_value=(typename fixed_point<2*I, 2*F>::value_type(m_value)<<F)/rhs.m_value;
			return *this;
		}

		inline fixed_point operator /(fixed_point const& rhs) const
		{
			return fixed_point(
				(typename fixed_point<2*I, 2*F>::value_type(m_value)<<F)/rhs.m_value,
				detail::fp_explicit_tag()
			);
		}

		inline fixed_point& operator >>=(boost::uint32_t b)
		{
			m_value>>=b;
			return *this;
		}

		inline fixed_point& operator <<=(boost::uint32_t b)
		{
			m_value<<=b;
			return *this;
		}

		inline fixed_point operator >>(boost::uint32_t b) const
		{
			return fixed_point(m_value>>b, detail::fp_explicit_tag());
		}

		inline fixed_point operator <<(boost::uint32_t b) const
		{
			return fixed_point(m_value<<b, detail::fp_explicit_tag());
		}

		inline operator char() const { return static_cast<char>(m_value>>F); }

		inline operator signed char() const { return static_cast<signed char>(m_value>>F); }

		inline operator unsigned char() const { return static_cast<unsigned char>(m_value>>F); }

		inline operator short() const { return static_cast<short>(m_value>>F); }

		inline operator unsigned short() const { return static_cast<unsigned short>(m_value>>F); }

		inline operator int() const { return static_cast<int>(m_value>>F); }

		inline operator unsigned int() const { return static_cast<unsigned int>(m_value>>F); }

		inline operator long() const { return static_cast<long>(m_value>>F); }

		inline operator unsigned long() const { return static_cast<unsigned long>(m_value>>F); }

		inline operator long long() const { return static_cast<long long>(m_value>>F); }

		inline operator unsigned long long() const { return static_cast<unsigned long long>(m_value>>F); }

		inline operator bool() const { return m_value?true:false; }

		inline operator float() const { return static_cast<float>(m_value)/detail::pow2<F>::value; }
		
		inline operator double() const { return static_cast<double>(m_value)/detail::pow2<F>::value; }

		inline operator long double() const { return static_cast<long double>(m_value)/detail::pow2<F>::value; }


		inline value_type get() const { return m_value; }

		template < boost::uint32_t I2, boost::uint32_t F2 >
		friend fixed_point<I2,F2> std::abs(fixed_point<I2,F2> const &);

		template < boost::uint32_t I2, boost::uint32_t F2 >
		friend fixed_point<I2,F2> std::fabs(fixed_point<I2,F2> const &);

		template < boost::uint32_t I2, boost::uint32_t F2 >
		friend fixed_point<I2,F2> std::ceil(fixed_point<I2,F2> const &);
		
		template < boost::uint32_t I2, boost::uint32_t F2 >
		friend fixed_point<I2,F2> std::floor(fixed_point<I2,F2> const &);

		template < boost::uint32_t I2, boost::uint32_t F2 >
		friend fixed_point<I2,F2> std::fmod(fixed_point<I2,F2> const &, fixed_point<I2,F2> const &);

		template < boost::uint32_t I2, boost::uint32_t F2 >
		friend fixed_point<I2,F2> std::modf(fixed_point<I2,F2> const &, fixed_point<I2,F2> *);

		template< typename T, boost::uint32_t I2, boost::uint32_t F2 >
		friend T std::round(fixed_point<I2,F2> const &);

		template < boost::uint32_t I2, boost::uint32_t F2 >
		friend fixed_point<I2,F2> std::sqrt(fixed_point<I2,F2> const &);

		template < boost::uint32_t I2, boost::uint32_t F2 >
		friend fixed_point<I2,F2> std::log(fixed_point<I2,F2> const &);
		
		template < boost::uint32_t I2, boost::uint32_t F2 >
		friend fixed_point<I2,F2> std::pow(fixed_point<I2,F2> const &, fixed_point<I2,F2> const &);

		template < boost::uint32_t I2, boost::uint32_t F2 >
		friend fixed_point<I2,F2> std::exp(fixed_point<I2,F2> const &);

		template < boost::uint32_t I2, boost::uint32_t F2 >
		friend fixed_point<I2,F2> std::atan2(fixed_point<I2,F2> const &, fixed_point<I2,F2> const &);

		template < boost::uint32_t I2, boost::uint32_t F2 >
		friend fixed_point<I2,F2> std::sin(fixed_point<I2,F2> const &);

		template < boost::uint32_t I2, boost::uint32_t F2 >
		friend fixed_point<I2,F2> std::cos(fixed_point<I2,F2> const &);

		
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
		static inline fp_type (min)()
		{
			//return fp_type((std::numeric_limits<typename fp_type::value_type>::min)(), baldzarika::ucv::detail::fp_explicit_tag());
			return fp_type(1, baldzarika::ucv::detail::fp_explicit_tag());
		}

		static inline fp_type (max)()
		{
			return fp_type((std::numeric_limits<typename fp_type::value_type>::max)(), baldzarika::ucv::detail::fp_explicit_tag());
		}

		static inline fp_type epsilon()
		{
			return fp_type(1, baldzarika::ucv::detail::fp_explicit_tag());
		}

		static inline fp_type (lowest)()
		{
			return fp_type((std::numeric_limits<typename fp_type::value_type>::min)(), baldzarika::ucv::detail::fp_explicit_tag());
		}

		static inline fp_type round_error()
		{
			return fp_type(0.5);
		}

		static inline fp_type denorm_min()
		{
			return fp_type(0);
		}

		static inline fp_type infinity()
		{
			return fp_type(0);
		}

		static inline fp_type quiet_NaN()
		{
			return fp_type(0);
		}

		static inline fp_type signaling_NaN()
		{
			return fp_type(0);
		}
	};

	template < boost::uint32_t I, boost::uint32_t F >
	inline baldzarika::ucv::fixed_point<I,F> abs(baldzarika::ucv::fixed_point<I,F> const &x)
	{
		namespace bucv=baldzarika::ucv;
		return x<bucv::detail::constant::zero< bucv::fixed_point<I,F> >()?-x:x;
	}

	template < boost::uint32_t I, boost::uint32_t F >
	inline baldzarika::ucv::fixed_point<I,F> fabs(baldzarika::ucv::fixed_point<I,F> const &x)
	{
		namespace bucv=baldzarika::ucv;
		return x<bucv::detail::constant::zero< bucv::fixed_point<I,F> >()?-x:x;
	}

	template < boost::uint32_t I, boost::uint32_t F >
	inline baldzarika::ucv::fixed_point<I,F> ceil(baldzarika::ucv::fixed_point<I,F> const &x)
	{
		namespace bucv=baldzarika::ucv;
		return bucv::fixed_point<I,F>(x.m_value & ~(bucv::detail::pow2<F>::value-1), bucv::detail::fp_explicit_tag())+
			((x.m_value & (bucv::detail::pow2<F>::value-1))?
				bucv::detail::constant::one< bucv::fixed_point<I,F> >():
				bucv::detail::constant::zero< bucv::fixed_point<I,F> >()
			);
	}

	template < boost::uint32_t I, boost::uint32_t F >
	inline baldzarika::ucv::fixed_point<I,F> floor(baldzarika::ucv::fixed_point<I,F> const &x)
	{
		namespace bucv=baldzarika::ucv;
		return bucv::fixed_point<I,F>(x.m_value & ~(bucv::detail::pow2<F>::value-1), bucv::detail::fp_explicit_tag());
	}

	template < boost::uint32_t I, boost::uint32_t F >
	inline baldzarika::ucv::fixed_point<I,F> fmod(baldzarika::ucv::fixed_point<I,F> const &x, baldzarika::ucv::fixed_point<I,F> const &y)
	{
		namespace bucv=baldzarika::ucv;
		return bucv::fixed_point<I,F>(x.m_value % y.m_value, bucv::detail::fp_explicit_tag());
	}

	template < boost::uint32_t I, boost::uint32_t F >
	inline baldzarika::ucv::fixed_point<I,F> modf(baldzarika::ucv::fixed_point<I,F> const &x, baldzarika::ucv::fixed_point<I,F> *ptr)
	{
		namespace bucv=baldzarika::ucv;

		*ptr=bucv::fixed_point<I,F>(x.m_value & ~(bucv::detail::pow2<F>::value-1), bucv::detail::fp_explicit_tag());
		if(x<bucv::detail::constant::zero< bucv::fixed_point<I,F> >()) *ptr+=bucv::detail::constant::one< bucv::fixed_point<I,F> >();

		return bucv::fixed_point<I,F>(
			(x<bucv::detail::constant::zero< bucv::fixed_point<I,F> >()?
				-(x.m_value & (bucv::detail::pow2<F>::value-1)):
				(x.m_value & (bucv::detail::pow2<F>::value-1))
			),
			bucv::detail::fp_explicit_tag()
		);
	}

	template< typename T, boost::uint32_t I, boost::uint32_t F >
	inline T round(baldzarika::ucv::fixed_point<I,F> const &x)
	{
		namespace bucv=baldzarika::ucv;

		return static_cast<T>(x>=bucv::detail::constant::zero< bucv::fixed_point<I,F> >()?
			x+bucv::detail::constant::half< bucv::fixed_point<I,F> >():
			x-bucv::detail::constant::half< bucv::fixed_point<I,F> >()
		);
	}



	template < boost::uint32_t I, boost::uint32_t F >
	inline baldzarika::ucv::fixed_point<I,F> sqrt(baldzarika::ucv::fixed_point<I,F> const &x)
	{
		namespace bucv=baldzarika::ucv;

#if defined(BALDZARIKA_UCV_FIXED_POINT_TRIGONO_USE_FPU_SQRT)
		return bucv::fixed_point<I,F>(std::sqrt(static_cast<float>(x)));
#else

#if 0
		using baldzarika::ucv::detail::constant::zero;
		using baldzarika::ucv::detail::fp_explicit_tag;

		if(x<zero< fixed_point<I,F> >())
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
		return fixed_point<I,F>(static_cast< fixed_point<I, F>::value_type >(res), fp_explicit_tag());
#endif
#endif
	}



	template < boost::uint32_t I, boost::uint32_t F >
	inline baldzarika::ucv::fixed_point<I,F> log(baldzarika::ucv::fixed_point<I,F> const &x)
	{
		namespace bucv=baldzarika::ucv;
		return bucv::fixed_point<I,F>(::logf(static_cast<float>(x)));
	}

	template < boost::uint32_t I, boost::uint32_t F >
	inline baldzarika::ucv::fixed_point<I,F> pow(baldzarika::ucv::fixed_point<I,F> const &x, baldzarika::ucv::fixed_point<I,F> const &y)
	{
		namespace bucv=baldzarika::ucv;
		return bucv::fixed_point<I,F>(::pow(static_cast<float>(x), static_cast<float>(y)));
	}


	template < boost::uint32_t I, boost::uint32_t F >
	inline baldzarika::ucv::fixed_point<I,F> exp(baldzarika::ucv::fixed_point<I,F> const &x)
	{
		namespace bucv=baldzarika::ucv;

#if defined(BALDZARIKA_UCV_FIXED_POINT_TRIGONO_USE_FPU_EXP)

		return bucv::fixed_point<I,F>(::exp(static_cast<float>(x)));

#else
		static bucv::fixed_point<I,F> const a[]=
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

		bucv::fixed_point<I,F> y(1);
		for(boost::int32_t i=F-1; i>=0; --i)
		{
			if(!(x.m_value & 1<<i))
				y*=a[F-i-1];
		}

		boost::int32_t x_int=std::floor(x);
		if(x_int<0)
		{
			for(boost::int32_t i=1;i<=-x_int;++i)
				y*=bucv::detail::constant::inv_e< bucv::fixed_point<I,F> >();
		}
		else
		{
			for(boost::int32_t i=1;i<=x_int;++i)
				y*=bucv::detail::constant::e< bucv::fixed_point<I,F> >();
		}
		return y;
#endif

	}



	template < boost::uint32_t I, boost::uint32_t F >
	inline baldzarika::ucv::fixed_point<I,F> atan2(baldzarika::ucv::fixed_point<I,F> const &y, baldzarika::ucv::fixed_point<I,F> const &x)
	{
		namespace bucv=baldzarika::ucv;

#if defined(BALDZARIKA_UCV_FIXED_POINT_TRIGONO_USE_FPU_ATAN2)
		return bucv::fixed_point<I,F>(::atan2(static_cast<float>(y), static_cast<float>(x)));
#else
#if defined(BALDZARIKA_UCV_FIXED_POINT_TRIGONO_USE_FIXED_POINT_ATAN2_HIGH_PRECISION)
		
	fixed_point abs_y=fabs(y)+fixed_point(1, detail::fp_explicit_tag());
	fixed_point angle;

	if(x>=detail::constant::zero<fixed_point>())
	{
		fixed_point r=fixed_point(x.m_value-abs_y.m_value, detail::fp_explicit_tag())/fixed_point(x.m_value+abs_y.m_value, detail::fp_explicit_tag());
		angle=detail::constant::atan_c1<fixed_point>()*r*r*r-detail::constant::atan_c2<fixed_point>()*r+detail::constant::pi_i4<fixed_point>();
	}
	else
	{
		fixed_point r=fixed_point(x.m_value+abs_y.m_value, detail::fp_explicit_tag())/fixed_point(abs_y.m_value-x.m_value, detail::fp_explicit_tag());
		angle=detail::constant::atan_c1<fixed_point>()*r*r*r-detail::constant::atan_c2<fixed_point>()*r+detail::constant::pi_3i4<fixed_point>();
	}
	return (y<detail::constant::zero<fixed_point>()?-angle:angle);

#else

		bucv::fixed_point<I,F> abs_y=std::abs(y)+bucv::fixed_point<I,F>(1, bucv::detail::fp_explicit_tag());

		bucv::fixed_point<I,F> angle(
			x>=bucv::detail::constant::zero< bucv::fixed_point<I,F> >()?
				bucv::detail::constant::pi_i4< bucv::fixed_point<I,F> >()-bucv::detail::constant::pi_i4< bucv::fixed_point<I,F> >()*
					(bucv::fixed_point<I,F>(x.m_value-abs_y.m_value, bucv::detail::fp_explicit_tag())/bucv::fixed_point<I,F>(x.m_value+abs_y.m_value, bucv::detail::fp_explicit_tag())):
				bucv::detail::constant::pi_3i4< bucv::fixed_point<I,F> >()-bucv::detail::constant::pi_i4< bucv::fixed_point<I,F> >()*
					(bucv::fixed_point<I,F>(x.m_value+abs_y.m_value, bucv::detail::fp_explicit_tag())/bucv::fixed_point<I,F>(abs_y.m_value-x.m_value, bucv::detail::fp_explicit_tag()))
		);
		return (y<bucv::detail::constant::zero< bucv::fixed_point<I,F> >()?-angle:angle);

#endif
#endif
	}

	template < boost::uint32_t I, boost::uint32_t F >
	inline baldzarika::ucv::fixed_point<I,F> sin(baldzarika::ucv::fixed_point<I,F> const &x)
	{
		namespace bucv=baldzarika::ucv;

#if defined(BALDZARIKA_UCV_FIXED_POINT_TRIGONO_USE_FPU_SIN)

		return bucv::fixed_point<I,F>(::sin(static_cast<float>(x)));
#else

		static bucv::fixed_point<I,F> const i_2t3=0.16666666666666666666666666666667;
		static bucv::fixed_point<I,F> const i_2t3t4t5=0.00833333333333333333333333333333;
		static bucv::fixed_point<I,F> const i_2t3t4t5t6t7=1.984126984126984126984126984127e-4;

		bucv::fixed_point<I,F> x_=std::fmod(x, bucv::detail::constant::pi_2< bucv::fixed_point<I,F> >());
		if(x_>bucv::detail::constant::pi< bucv::fixed_point<I,F> >())
			x_-=bucv::detail::constant::pi_2< bucv::fixed_point<I,F> >();

		bucv::fixed_point<I,F> xx=x_*x_;

		bucv::fixed_point<I,F> y=-xx*i_2t3t4t5t6t7;
		y+=i_2t3t4t5;
		y*=xx;
		y-=i_2t3;
		y*=xx;
		y+=bucv::detail::constant::one< bucv::fixed_point<I,F> >();
		return y*x_;
#endif
	}


	template < boost::uint32_t I, boost::uint32_t F >
	inline baldzarika::ucv::fixed_point<I,F> cos(baldzarika::ucv::fixed_point<I,F> const &x)
	{
		namespace bucv=baldzarika::ucv;

#if defined(BALDZARIKA_UCV_FIXED_POINT_TRIGONO_USE_FPU_COS)

		return bucv::fixed_point<I,F>(::cos(static_cast<float>(x)));
#else

		static bucv::fixed_point<I,F> const i_2t3t4=0.04166666666666666666666666666667;
		static bucv::fixed_point<I,F> const i_2t3t4t5t6=0.00138888888888888888888888888889;

		bucv::fixed_point<I,F> x_=std::fmod(x, bucv::detail::constant::pi_2< bucv::fixed_point<I,F> >());
		if(x_>bucv::detail::constant::pi< bucv::fixed_point<I,F> >())
			x_-=bucv::detail::constant::pi_2< bucv::fixed_point<I,F> >();
		bucv::fixed_point<I,F> xx=x_*x_;
		bucv::fixed_point<I,F> y=-xx*i_2t3t4t5t6;
		y+=i_2t3t4;
		y*=xx;
		y-=bucv::detail::constant::half< bucv::fixed_point<I,F> >();
		y*=xx;
		return y+bucv::detail::constant::one< bucv::fixed_point<I,F> >();
#endif
	}

} //namespace std

namespace baldzarika { namespace ucv {

	typedef fixed_point<15,16> decimal_t;

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_FIXED_POINT_H
