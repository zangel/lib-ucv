#ifndef BALDZARIKA_UCV_LAPACK_JACOBI_H
#define BALDZARIKA_UCV_LAPACK_JACOBI_H

namespace baldzarika { namespace ucv  { namespace lapack {

	namespace detail {
		template< boost::uint32_t I, boost::uint32_t F >
		static inline fixed_point<I,F> hypot(fixed_point<I,F> const &a, fixed_point<I,F> const &b)
		{
			typedef fixed_point<I,F> value_t;

			value_t a_=fabs(a);
			value_t b_=fabs(b);
			
			if(a_>b_)
			{
				value_t f=b_/a_;
				return a_*sqrt(ucv::detail::constants::one<value_t>()+f*f);
			}
			if(b_==ucv::detail::constants::zero<value_t>())
				return ucv::detail::constants::zero<value_t>();
			value_t f=a_/b_;
			return b_*sqrt(ucv::detail::constants::one<value_t>()+f*f);
		}

	} //namespace detail

	template < boost::uint32_t I, boost::uint32_t F, typename SAS, typename SAe, typename SAE >
	bool jacobi(
		nublas::matrix< fixed_point<I,F>, nublas::row_major, SAS > const &_S0,
		nublas::matrix< fixed_point<I,F>, nublas::row_major, SAe > &_e,
		nublas::matrix< fixed_point<I,F>, nublas::row_major, SAE > &matE,
		bool ce=true,
		fixed_point<I,F> const &eps=std::numeric_limits< fixed_point<I,F> >::epsilon()
	)
	{
		typedef fixed_point<I,F> value_t;

		BOOST_ASSERT(_S0.size1()==_S0.size2());
		BOOST_ASSERT(_e.size1()==_e.size2());
		BOOST_ASSERT(matE.size1()==matE.size2());
		BOOST_ASSERT(_S0.size1()==_e.size1() && _e.size1()==matE.size1());

		boost::int32_t const n=_S0.size1(), max_iters=n*n*30;;
		boost::int32_t i, j, k, m, it;

		if(ce) matE=nublas::identity_matrix<value_t>(n,n);
		
		

		boost::scoped_array<value_t> S(new value_t[n*n]);

		boost::scoped_array<value_t> maxSR(new value_t[n]);
		boost::scoped_array<value_t> maxSC(new value_t[n]);
		boost::scoped_array<boost::int32_t> indR(new boost::int32_t[n]);
		boost::scoped_array<boost::int32_t> indC(new boost::int32_t[n]);
		
		SAS S0=_S0.data();
		SAe e=_e.data();
		SAE E=matE.data();
		
		

		for(i=0;i<n*n;++i)
			S[i]=S0[i];

		value_t mv;

		for(k=0;k<n;++k)
		{
			e[k*n] = S[(n+1)*k];
			if(k<n-1)
			{
				for(m=k+1,mv=fabs(S[n*k+m]),i=k+2;i<n;++i)
				{
					value_t v=fabs(S[n*k+i]);
					if(mv<v)
						mv=v,m=i;
				}
				maxSR[k] = mv;
				indR[k] = m;
			}
			if(k>0)
			{
				for(m=0,mv=fabs(S[k]),i=1;i<k;++i)
				{
					value_t v=fabs(S[n*i+k]);
					if(mv<v)
						mv=v,m=i;
				}
				maxSC[k]=mv;
				indC[k]=m;
			}
		}

		for(it=0;it<max_iters;++it)
		{
			for(k=0,mv=maxSR[0],i=1;i<n-1;++i)
			{
				value_t v=maxSR[i];
				if(mv<v)
					mv=v,k=i;
			}
			
			boost::int32_t l=indR[k];
			for(i=1;i<n;++i)
			{
				value_t v=maxSC[i];
				if(mv<v)
					mv=v,k=indC[i],l=i;
			}

			value_t p=S[n*k+l];
			if(fabs(p)<=eps)
				break;
			value_t y=(e[n*l]-e[n*k])*ucv::detail::constants::half<value_t>();
			value_t t=fabs(y)+detail::hypot(p,y);
			value_t s=detail::hypot(p,t);
			value_t c=t/s;
			s=p/s; t=(p/t)*p;
			if(y<ucv::detail::constants::zero<value_t>())
				s=-s,t=-t;
			
			S[n*k+l]=ucv::detail::constants::zero<value_t>();
			e[n*k]-=t;
			e[n*l]+=t;

			value_t a0, b0;

#undef rotate
#define rotate(v0, v1) a0 = v0, b0 = v1, v0 = a0*c - b0*s, v1 = a0*s + b0*c
			for(i=0;i<k;++i)
				rotate(S[n*i+k], S[n*i+l]);
			for(i=k+1;i<l;++i)
				rotate(S[n*k+i], S[n*i+l]);
			for(i=l+1;i<n;++i)
				rotate(S[n*k+i], S[n*l+i]);
			if(ce)
			{
				for(i=0;i<n;++i)
				{
					rotate(E[n*k+i], E[n*l+i]);
				}
			}
#undef rotate
			for(j=0;j<2;++j)
			{
				boost::int32_t idx=j==0?k:l;
				if(idx<n-1)
				{
					for(m=idx+1,mv=fabs(S[n*idx+m]),i=idx+2;i<n;++i)
					{
						value_t v=fabs(S[n*idx+i]);
						if(mv<v)
							mv=v,m=i;
					}
					maxSR[idx]=mv;
					indR[idx]=m;
				}
				if(idx>0)
				{
					for(m=0,mv=fabs(S[idx]),i=1;i<idx;++i)
					{
						value_t v=fabs(S[n*i+idx]);
						if(mv<v)
							mv=v,m=i;
					}
					maxSC[idx]=mv;
					indC[idx]=m;
				}
			}
		}
		for(k=0;k<n-1;++k)
		{
			m=k;
			for(i=k+1;i<n;++i)
			{
				if(e[n*m]<e[n*i])
					m=i;
			}
			if(k!=m)
			{
				std::swap(e[n*m], e[n*k]);
				if(ce)
				{
					for(i=0;i<n;++i)
						std::swap(E[n*m+i],E[n*k+i]);
				}
			}
		}
		return true;
	}

} //namespace lapack
} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_LAPACK_JACOBI_H