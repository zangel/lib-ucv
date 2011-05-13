#ifndef BALDZARIKA_UCV_SVD_H
#define BALDZARIKA_UCV_SVD_H

#include <baldzarika/ucv/matrix.h>
#include <baldzarika/ucv/vector.h>

namespace baldzarika { namespace ucv {

	namespace detail {

		template < typename T >
		static inline T sign(T const &a, T const &b)
		{
			return std::abs(a)*(b<detail::constant::zero<T>()?-detail::constant::one<T>():detail::constant::one<T>());
		}

		template < typename T >
		static inline T sqr(T const &x)
		{
			return x*x;
		}
		


		template < typename T >
		static inline T pythag(T const &a, T const &b)
		{
			T const absa=std::abs(a);
			T const absb=std::abs(b);
			
			return (absa>absb?
				absa*std::sqrt(constant::one<T>()+sqr(absb/absa)):
				(absb==constant::zero<T>()?
					constant::zero<T>():
					absb*std::sqrt(constant::one<T>()+sqr(absa/absb))
				)
			);
		}

	} //namespace detail

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
		g=scale=anorm=detail::constant::zero<T>();
		for(i=0;i<n;i++)
		{
			l=i+2;
			rv1[i]=scale*g;
			g=s=scale=detail::constant::zero<T>();
			if(i<m)
			{
				for(k=i;k<m;k++)
					scale+=std::abs(u(k,i));
				if(scale!=detail::constant::zero<T>())
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
					for(j=l-1;j<n;j++)
					{
						for(s=detail::constant::zero<T>(),k=i;k<m;k++)
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
			g=s=scale=detail::constant::zero<T>();
			if(i+1<=m && i+1!=n)
			{
				for(k=l-1;k<n;k++)
					scale+=std::abs(u(i,k));
				if(scale!=detail::constant::zero<T>())
				{
					for(k=l-1;k<n;k++)
					{
						u(i,k)/=scale;
						s+=u(i,k)*u(i,k);
					}
					f=u(i,l-1);
					g=-detail::sign(std::sqrt(s),f);
					h=f*g-s;
					u(i,l-1)=f-g;
					for(k=l-1;k<n;k++)
						rv1[k]=u(i,k)/h;
					for(j=l-1;j<m;j++)
					{
						for(s=detail::constant::zero<T>(),k=l-1;k<n;k++)
							s+=u(j,k)*u(i,k);
						for(k=l-1;k<n;k++)
							u(j,k)+=s*rv1[k];
					}
					for(k=l-1;k<n;k++)
						u(i,k)*=scale;
				}
			}
			anorm=std::max(anorm,(std::abs(w[i])+std::abs(rv1[i])));
		}
		
		for(i=n-1;i>=0;i--)
		{
			if(i<n-1)
			{
				if(g!=detail::constant::zero<T>())
				{
					for(j=l;j<n;j++)
						v(j,i)=(u(i,j)/u(i,l))/g;
					for(j=l;j<n;j++)
					{
						for(s=detail::constant::zero<T>(),k=l;k<n;k++)
							s+=u(i,k)*v(k,j);
						for(k=l;k<n;k++)
							v(k,j)+=s*v(k,i);
					}
				}
				for(j=l;j<n;j++)
					v(i,j)=v(j,i)=detail::constant::zero<T>();
			}
			v(i,i)=detail::constant::zero<T>();
			g=rv1[i];
			l=i;
		}
		
		for(i=std::min(m,n)-1;i>=0;i--)
		{
			l=i+1;
			g=w[i];
			for(j=l;j<n;j++)
				u(i,j)=detail::constant::zero<T>();
			if(g!=detail::constant::zero<T>())
			{
				g=detail::constant::one<T>()/g;
				for(j=l;j<n;j++)
				{
					for(s=detail::constant::zero<T>(),k=l;k<m;k++)
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
					u(j,i)=detail::constant::zero<T>();
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
					if(l==0 || std::abs(rv1[l])<=eps*anorm)
					{
						flag=false;
						break;
					}
					if(std::abs(w[nm])<=eps*anorm)
						break;
				}
				if(flag)
				{
					c=detail::constant::zero<T>();
					s=detail::constant::one<T>();
					for(i=l;i<k+1;i++)
					{
						f=s*rv1[i];
						rv1[i]=c*rv1[i];
						if(std::abs(f)<=eps*anorm)
							break;
						g=w[i];
						h=detail::pythag(f,g);
						w[i]=h;
						h=detail::constant::one<T>()/h;
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
					if(z<detail::constant::zero<T>())
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
				f=((y-z)*(y+z)+(g-h)*(g+h))/(detail::constant::two<T>()*h*y);
				g=detail::pythag(f,detail::constant::one<T>());
				f=((x-z)*(x+z)+h*((y/(f+detail::sign(g,f)))-h))/x;
				c=s=detail::constant::one<T>();
				for(j=l;j<=nm;j++)
				{
					i=j+1;
					g=rv1[i];
					y=w[i];
					h=s*g;
					g=c*g;
					z=detail::pythag(f,h);
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
					z=detail::pythag(f,h);
					w[j]=z;
					if(z)
					{
						z=detail::constant::one<T>()/z;
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
				rv1[l]=detail::constant::zero<T>();
				rv1[k]=f;
				w[k]=x;
			}
		}
		return true;
	}


} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_SVD_H