#ifndef BALDZARIKA_MATH_JACOBI_H
#define BALDZARIKA_MATH_JACOBI_H

#include <baldzarika/math/matrix.h>
#include <baldzarika/math/vector.h>
#include <baldzarika/math/utils.h>

namespace baldzarika { namespace math {

	template < typename T, boost::uint32_t N >
	bool jacobi(matrix<T, N, N> const &S0, vector<T, N> &e, matrix<T, N, N> &E, bool ce=true, T eps=std::numeric_limits<T>::epsilon())
	{
		static boost::int32_t const n=boost::int32_t(N);
		boost::int32_t max_iters=N*N*30;

		boost::int32_t i, j, k, m, it;

		vector<T,N> maxSR, maxSC;
		boost::int32_t indR[N], indC[N];

		matrix<T, N, N> S(S0);

		if(ce) E=matrix<T, N, N>::identity();

		T mv;

		for(k=0;k<n;++k)
		{
			e[k]=S(k,k);
			if(k<n-1)
			{
				for(m=k+1,mv=std::abs(S(k,m)),i=k+2;i<n;++i)
				{
					T v=std::abs(S(k,i));
					if(mv<v)
						mv=v,m=i;
				}
				maxSR(k)=mv;
				indR[k]=m;
			}
			if(k>0)
			{
				for(m=0,mv=std::abs(S(0,k)),i=1;i<k;++i)
				{
					T v=std::abs(S(i,k));
					if(mv<v)
						mv=v,m=i;
				}
				maxSC(k)=mv;
				indC[k]=m;
			}
		}

		for(it=0;it<max_iters;++it)
		{
			for(k=0,mv=maxSR(0),i=1;i<n-1;++i)
			{
				T v=maxSR(i);
				if(mv<v)
					mv=v,k=i;
			}

			boost::int32_t l=indR[k];
			for(i=1;i<n;++i)
			{
				T v=maxSC(i);
				if(mv<v)
					mv=v,k=indC[i],l=i;
			}

			T p=S(k,l);
			if(std::abs(p)<=eps)
				break;
			T y=(e(l)-e(k))*constant::half<T>();
			T t=std::abs(y)+utils::hypot(p,y);
			T s=utils::hypot(p,t);
			T c=t/s;
			s=p/s; t=(p/t)*p;
			if(y<constant::zero<T>())
				s=-s,t=-t;

			S(k,l)=constant::zero<T>();
			e(k)-=t;
			e(l)+=t;

			T a0, b0;

#undef rotate
#define rotate(v0, v1) a0 = v0, b0 = v1, v0 = a0*c - b0*s, v1 = a0*s + b0*c
			for(i=0;i<k;++i)
				rotate(S(i,k), S(i,l));
			for(i=k+1;i<l;++i)
				rotate(S(k,i), S(i,l));
			for(i=l+1;i<n;++i)
				rotate(S(k,i), S(l,i));
			if(ce)
			{
				for(i=0;i<n;++i)
				{
					rotate(E(k,i), E(l,i));
				}
			}
#undef rotate
			for(j=0;j<2;++j)
			{
				boost::int32_t idx=j==0?k:l;
				if(idx<n-1)
				{
					for(m=idx+1,mv=std::abs(S(idx,m)),i=idx+2;i<n;++i)
					{
						T v=std::abs(S(idx,i));
						if(mv<v)
							mv=v,m=i;
					}
					maxSR(idx)=mv;
					indR[idx]=m;
				}
				if(idx>0)
				{
					for(m=0,mv=std::abs(S(0,idx)),i=1;i<idx;++i)
					{
						T v=std::abs(S(i,idx));
						if(mv<v)
							mv=v,m=i;
					}
					maxSC(idx)=mv;
					indC[idx]=m;
				}
			}
		}
		for(k=0;k<n-1;++k)
		{
			m=k;
			for(i=k+1;i<n;++i)
			{
				if(e(m)<e(i))
					m=i;
			}
			if(k!=m)
			{
				std::swap(e(m), e(k));
				if(ce)
				{
					for(i=0;i<n;++i)
						std::swap(E(m,i),E(k,i));
				}
			}
		}
		return true;
	}

} //namespace math
} //namespace baldzarika


#endif //BALDZARIKA_MATH_JACOBI_H