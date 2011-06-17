#ifndef BALDZARIKA_MATH_SVD_H
#define BALDZARIKA_MATH_SVD_H

#include <baldzarika/math/matrix.h>
#include <baldzarika/math/vector.h>
#include <baldzarika/math/constant.h>
#include <baldzarika/math/utils.h>

namespace baldzarika { namespace math {

	template < typename T, boost::uint32_t M, boost::uint32_t N >
	static inline bool svd(matrix<T, M, N> const &a, matrix<T, M, N > &u, vector<T, N> &w, matrix<T, N, N> &v, T eps=1.0e-3)
	{
		boost::int32_t const n=boost::int32_t(N);
		boost::int32_t const m=boost::int32_t(M);
		
		u=a;
		bool flag;
		boost::int32_t i,its,j,jj,k,l,nm;
		T anorm,c,f,g,h,s,scale,x,y,z;

		vector<T,N> rv1;
		g=scale=anorm=constant::zero<T>();
		for(i=0;i<n;i++)
		{
			l=i+1;
			rv1[i]=scale*g;
			g=s=scale=constant::zero<T>();
			if(i<m)
			{
				for(k=i;k<m;k++)
					scale+=std::abs(u(k,i));
				if(scale!=constant::zero<T>())
				{
					for(k=i;k<m;k++)
					{
						u(k,i)/=scale;
						s+=u(k,i)*u(k,i);
					}
					f=u(i,i);
					g=-detail::sign(std::sqrt(s),f);
					h=f*g-s;
					u(i,i)=f-g;
					for(j=l;j<n;j++)
					{
						for(s=constant::zero<T>(),k=i;k<m;k++)
							s+=u(k,i)*u(k,j);
						f=s/h;
						for(k=i;k<m;k++)
							u(k,j)+=f*u(k,i);
					}
					for(k=i;k<m;k++)
						u(k,i)*=scale;
				}
			}
			w[i]=scale*g;
			g=s=scale=constant::zero<T>();
			if(i<m && i+1!=n)
			{
				for(k=l-1;k<n;k++)
					scale+=std::abs(u(i,k));
				if(scale!=constant::zero<T>())
				{
					for(k=l;k<n;k++)
					{
						u(i,k)/=scale;
						s+=u(i,k)*u(i,k);
					}
					f=u(i,l);
					g=-detail::sign(std::sqrt(s),f);
					h=f*g-s;
					u(i,l)=f-g;
					for(k=l;k<n;k++)
						rv1[k]=u(i,k)/h;
					for(j=l;j<m;j++)
					{
						for(s=constant::zero<T>(),k=l;k<n;k++)
							s+=u(j,k)*u(i,k);
						for(k=l;k<n;k++)
							u(j,k)+=s*rv1[k];
					}
					for(k=l;k<n;k++)
						u(i,k)*=scale;
				}
			}
			anorm=std::max(anorm,(std::abs(w[i])+std::abs(rv1[i])));
		}
		
		for(i=n-1;i>=0;i--)
		{
			if(i<n-1)
			{
				if(g!=constant::zero<T>())
				{
					for(j=l;j<n;j++)
						v(j,i)=(u(i,j)/u(i,l))/g;
					for(j=l;j<n;j++)
					{
						for(s=constant::zero<T>(),k=l;k<n;k++)
							s+=u(i,k)*v(k,j);
						for(k=l;k<n;k++)
							v(k,j)+=s*v(k,i);
					}
				}
				for(j=l;j<n;j++)
					v(i,j)=v(j,i)=constant::zero<T>();
			}
			v(i,i)=constant::one<T>();
			g=rv1[i];
			l=i;
		}
		
		for(i=std::min(m,n)-1;i>=0;i--)
		{
			l=i+1;
			g=w[i];
			for(j=l;j<n;j++)
				u(i,j)=constant::zero<T>();
			if(g!=constant::zero<T>())
			{
				g=constant::one<T>()/g;
				for(j=l;j<n;j++)
				{
					for(s=constant::zero<T>(),k=l;k<m;k++)
						s+=u(k,i)*u(k,j);
					f=(s/u(i,i))*g;
					for(k=i;k<m;k++)
						u(k,j)+=f*u(k,i);
				}
				for(j=i;j<m;j++)
					u(j,i)*=g;
			}
			else
			{
				for(j=i;j<m;j++)
					u(j,i)=constant::zero<T>();
			}
			++u(i,i);
		}
		
		for(k=n-1;k>=0;k--)
		{
			for(its=0;its<30;its++)
			{
				flag=true;
				for(l=k;l>=0;l--)
				{
					nm=l-1;
					if((std::abs(rv1[l])+anorm)==anorm)
					{
						flag=false;
						break;
					}
					if((std::abs(rv1[nm])+anorm)==anorm)
						break;
				}
				if(flag)
				{
					c=constant::zero<T>();
					s=constant::one<T>();
					for(i=l;i<=k;i++)
					{
						f=s*rv1[i];
						rv1[i]=c*rv1[i];
						if((std::abs(f)+anorm)==anorm)
							break;
						g=w[i];
						h=detail::hypot(f,g);
						w[i]=h;
						h=constant::one<T>()/h;
						c=g*h;
						s=-f*h;
						for(j=0;j<m;j++)
						{
							y=u(j,nm);
							z=u(j,i);
							u(j,nm)=y*c+z*s;
							u(j,i)=z*c-y*s;
						}
					}
				}
				z=w[k];
				if(l==k)
				{
					if(z<constant::zero<T>())
					{
						w[k]=-z;
						for(j=0;j<n;j++)
							v(j,k)=-v(j,k);
					}
					break;
				}
				if(its==29)
					return false;
				x=w[l];
				nm=k-1;
				y=w[nm];
				g=rv1[nm];
				h=rv1[k];
				f=((y-z)*(y+z)+(g-h)*(g+h))/(constant::two<T>()*h*y);
				g=detail::hypot(f,constant::one<T>());
				f=((x-z)*(x+z)+h*((y/(f+detail::sign(g,f)))-h))/x;
				c=s=constant::one<T>();
				for(j=l;j<=nm;j++)
				{
					i=j+1;
					g=rv1[i];
					y=w[i];
					h=s*g;
					g=c*g;
					z=detail::hypot(f,h);
					rv1[j]=z;
					c=f/z;
					s=h/z;
					f=x*c+g*s;
					g=g*c-x*s;
					h=y*s;
					y*=c;
					for(jj=0;jj<n;jj++)
					{
						x=v(jj,j);
						z=v(jj,i);
						v(jj,j)=x*c+z*s;
						v(jj,i)=z*c-x*s;
					}
					z=detail::hypot(f,h);
					w[j]=z;
					if(z)
					{
						z=constant::one<T>()/z;
						c=f*z;
						s=h*z;
					}
					f=c*g+s*y;
					x=c*y-s*g;
					for(jj=0;jj<m;jj++)
					{
						y=u(jj,j);
						z=u(jj,i);
						u(jj,j)=y*c+z*s;
						u(jj,i)=z*c-y*s;
					}
				}
				rv1[l]=constant::zero<T>();
				rv1[k]=f;
				w[k]=x;
			}
		}
#if 0
		for(k=0;k<n;k++)
		{
			s=0;
			for(i=0;i<m;i++) if(u(i,k)<constant::zero<T>()) s++;
			for(j=0;j<n;j++) if(v(j,k)<constant::zero<T>()) s++;
			if(s>(m+n)/2)
			{
				for(i=0;i<m;i++) u(i,k) = -u(i,k);
				for(j=0;j<n;j++) v(j,k) = -v(j,k);
			}
		}
#endif
		return true;
	}


} //namespace math
} //namespace baldzarika

#endif //BALDZARIKA_MATH_SVD_H