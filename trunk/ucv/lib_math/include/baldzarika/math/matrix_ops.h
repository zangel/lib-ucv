#ifndef BALDZARIKA_MATH_MATRIX_OPS_H
#define BALDZARIKA_MATH_MATRIX_OPS_H

#include <baldzarika/math/constant.h>

namespace baldzarika { namespace math {

	template < typename T , boost::uint32_t R, boost::uint32_t C >
	struct matrix_ops
	{
		static inline matrix<T,R,C> make_zero()
		{
			matrix<T,R,C> res;
			for(boost::uint32_t i=0;i<matrix<T,R,C>::RXC;++i)
				res.m._1d[i]=constant::zero<T>();
			return res;
		}

		static inline matrix<T,R,C> const& zero()
		{
			static matrix<T,R,C> const _zero=make_zero();
			return _zero;
		}


		static inline matrix<T,R,C> make_identity()
		{
			matrix<T,R,C> res=make_zero();
			for(boost::uint32_t i=0;i<std::min(R,C);++i)
				res.m._2d[i][i]=constant::one<T>();
			return res;
		}

		static inline matrix<T,R,C> const& identity()
		{
			static matrix<T,R,C> const _identity=make_identity();
			return _identity;
		}

		static inline vector<T,C> get_row(matrix<T,R,C> const &mat, boost::int32_t r)
		{
			vector<T,C> row_vec;
			for(boost::uint32_t c=0;c<C;++c)
				row_vec.m_data[c]=mat.m._2d[r][c];
			return row_vec;
		}

		static inline vector<T,R> get_col(matrix<T,R,C> const &mat, boost::int32_t c)
		{
			vector<T,R> col_vec;
			for(boost::uint32_t r=0;r<R;++r)
				col_vec.m_data[r]=mat.m._2d[r][c];
			return col_vec;
		}

		template < typename RVT >
		static inline matrix<T,R,C>& set_row(matrix<T,R,C> &mat, boost::int32_t r, vector<RVT,C> const &row_vec)
		{
			for(boost::uint32_t c=0;c<C;++c)
				mat.m._2d[r][c]=static_cast<T>(row_vec.m_data[c]);
			return mat;
		}

		template < typename CVT >
		static inline matrix<T,R,C>& set_col(matrix<T,R,C> &mat, boost::int32_t c, vector<CVT,R> const &col_vec)
		{
			for(boost::uint32_t r=0;r<R;++r)
				mat.m._2d[r][c]=static_cast<T>(col_vec.m_data[r]);
			return mat;
		}

		template < typename RT >
		static inline matrix<T,R,C>& assign(matrix<T,R,C> &lhs, matrix<RT,R,C> const &rhs)
		{
			for(boost::uint32_t i=0;i<matrix<T,R,C>::RXC;++i)
				lhs.m._1d[i]=static_cast<T>(rhs.m._1d[i]);
			return lhs;
		}

		template < typename RT >
		static inline matrix<T,R,C>& plus_assign(matrix<T,R,C> &lhs, matrix<RT,R,C> const &rhs)
		{
			for(boost::uint32_t i=0;i<matrix<T,R,C>::RXC;++i)
				lhs.m._1d[i]+=static_cast<T>(rhs.m._1d[i]);
			return lhs;
		}

		template < typename RT >
		static inline matrix<T,R,C>& minus_assign(matrix<T,R,C> &lhs, matrix<RT,R,C> const &rhs)
		{
			for(boost::uint32_t i=0;i<matrix<T,R,C>::RXC;++i)
				lhs.m._1d[i]-=static_cast<T>(rhs.m._1d[i]);
			return lhs;
		}

		template < typename RT >
		static inline matrix<T,R,C>& prod(matrix<T,R,C> &lhs, matrix<RT,C,R> const &rhs)
		{
			matrix<T,R,C> tmp=lhs;
			for(boost::uint32_t r=0;r<R;++r)
			{
				for(boost::uint32_t c=0;c<C;++c)
				{
					T prod_sum=constant::zero<T>();
					for(boost::uint32_t c2=0;c2<C;++c)
						prod_sum+=

				}
			}
			return lhs;
		}
	};


	template < typename T >
	struct matrix_ops<T, 2, 2>
	{
		static inline matrix<T,2,2> const& zero()
		{
			static T const _zero[2][2]=
			{
				{ constant::zero<T>(), constant::zero<T>() },
				{ constant::zero<T>(), constant::zero<T>() }
			};
			return *(reinterpret_cast<matrix<T,2,2> const *>(&_zero[0][0]));
		}

		static inline matrix<T,2,2> const& identity()
		{
			static T const _identity[2][2]=
			{
				{ constant::one<T>(),	constant::zero<T>() },
				{ constant::zero<T>(),	constant::one<T>()  }
			};
			return *(reinterpret_cast<matrix<T,2,2> const *>(&_identity[0][0]));
		}

		static inline vector<T,2> get_row(matrix<T,2,2> const &mat, boost::int32_t r)
		{
			vector<T,2> row_vec;
			row_vec.m_data[0]=mat.m._2d[r][0];
			row_vec.m_data[1]=mat.m._2d[r][1];
			return row_vec;
		}

		static inline vector<T,2> get_col(matrix<T,2,2> const &mat, boost::int32_t c)
		{
			vector<T,2> col_vec;
			col_vec.m_data[0]=mat.m._2d[0][c];
			col_vec.m_data[1]=mat_.m._2d[1][c];
			return col_vec;
		}

		template < typename RVT >
		static inline matrix<T,2,2>& set_row(matrix<T,2,2> &mat, boost::int32_t r, vector<RVT,2> const &row_vec)
		{
			mat.m._2d[r][0]=static_cast<T>(row_vec.m_data[0]);
			mat.m._2d[r][1]=static_cast<T>(row_vec.m_data[1]);
			return mat;
		}

		template < typename CVT >
		static inline matrix<T,2,2>& set_col(matrix<T,2,2> &mat, boost::int32_t c, vector<CVT,2> const &col_vec)
		{
			mat.m._2d[0][c]=static_cast<T>(col_vec.m_data[0]);
			mat.m._2d[1][c]=static_cast<T>(col_vec.m_data[1]);
			return mat;
		}

		template < typename RT >
		static inline matrix<T,2,2>& assign(matrix<T,2,2> &lhs, matrix<RT,2,2> const &rhs)
		{
			m._1d[0]=static_cast<T>(rhs.m._1d[0]);
			m._1d[1]=static_cast<T>(rhs.m._1d[1]);
			m._1d[2]=static_cast<T>(rhs.m._1d[2]);
			m._1d[3]=static_cast<T>(rhs.m._1d[3]);
			return lhs;
		}

		template < typename RT >
		static inline matrix<T,2,2>& plus_assign(matrix<T,2,2> &lhs, matrix<RT,2,2> const &rhs)
		{
			m._1d[0]+=static_cast<T>(rhs.m._1d[0]);
			m._1d[1]+=static_cast<T>(rhs.m._1d[1]);
			m._1d[2]+=static_cast<T>(rhs.m._1d[2]);
			m._1d[3]+=static_cast<T>(rhs.m._1d[3]);
			return lhs;
		}

		template < typename RT >
		static inline matrix<T,2,2>& minus_assign(matrix<T,2,2> &lhs, matrix<RT,2,2> const &rhs)
		{
			m._1d[0]-=static_cast<T>(rhs.m._1d[0]);
			m._1d[1]-=static_cast<T>(rhs.m._1d[1]);
			m._1d[2]-=static_cast<T>(rhs.m._1d[2]);
			m._1d[3]-=static_cast<T>(rhs.m._1d[3]);
			return lhs;
		}

		template < typename RT >
		static inline matrix<T,2,2>& prod_assign(matrix<T,2,2> &lhs, matrix<RT,2,2> const &rhs)
		{
			matrix<T,2,2> tmp=lhs;
			lhs.m._1d[0]=tmp.m._1d[0]*static_cast<T>(rhs.m._1d[0])+tmp.m._1d[1]*static_cast<T>(rhs.m._1d[2]);
			lhs.m._1d[1]=tmp.m._1d[0]*static_cast<T>(rhs.m._1d[1])+tmp.m._1d[1]*static_cast<T>(rhs.m._1d[3]);
			lhs.m._1d[2]=tmp.m._1d[2]*static_cast<T>(rhs.m._1d[0])+tmp.m._1d[3]*static_cast<T>(rhs.m._1d[2]);
			lhs.m._1d[3]=tmp.m._1d[2]*static_cast<T>(rhs.m._1d[1])+tmp.m._1d[3]*static_cast<T>(rhs.m._1d[3]);
			return lhs;
		}

		template < typename VT >
		static inline matrix<T,2,2>& scalar_multiply_assign(matrix<T,2,2> &lhs, VT const &v)
		{
			m._1d[0]*=static_cast<T>(v);
			m._1d[1]*=static_cast<T>(v);
			m._1d[2]*=static_cast<T>(v);
			m._1d[3]*=static_cast<T>(v);
		}

		template < typename VT >
		static inline vector<VT,2> vector_multiply(matrix<T,2,2> const &mat, vector<VT,2> const &vec)
		{
			vector<VT,2> res;
			res.m_data[0]=static_cast<VT>(mat.m._2d[0][0]*static_cast<T>(vec.m_data[0])+mat.m._2d[0][1]*static_cast<T>(vec.m_data[1]));
			res.m_data[1]=static_cast<VT>(mat.m._2d[1][0]*static_cast<T>(vec.m_data[0])+mat.m._2d[1][1]*static_cast<T>(vec.m_data[1]));
			return res;
		}

		static inline matrix<T,2,2>& transpose(matrix<T,2,2> &mat)
		{
			std::swap(mat.m._1d[1], mat.m._1d[2]);
			return mat;
		}

		static inline matrix<T,2,2>& invert(matrix<T,2,2> &mat)
		{
			T det=mat.m._1d[0]*mat.m._1d[3]-mat.m._1d[1]*mat.m._1d[2];
			if(det!=constant::zero<T>())
			{
				det=constant::one<T>()/det;
				T t0=mat.m._1d[0]*d;
				T t1=mat.m._1d[3]*d;
				mat.m._1d[0]=t1;
				mat.m._1d[3]=t0;
				t0=-mat.m._1d[1]*d;
				t1=-mat.m._1d[2]*d;
				mat.m._1d[1]=t0;
				mat.m._1d[2]=t1;
			}
			return mat;
		}
	};


	template < typename T >
	struct matrix_ops<T, 3, 3>
	{
		static inline matrix<T,3,3> const& zero()
		{
			static T const _zero[3][3]=
			{
				{ constant::zero<T>(), constant::zero<T>(), constant::zero<T>() },
				{ constant::zero<T>(), constant::zero<T>(), constant::zero<T>() },
				{ constant::zero<T>(), constant::zero<T>(), constant::zero<T>() }
			};
			return *(reinterpret_cast<matrix<T,3,3> const *>(&_zero[0][0]));
		}

		static inline matrix<T,3,3> const& identity()
		{
			static T const _identity[3][3]=
			{
				{ constant::one<T>(),	constant::zero<T>(),	constant::zero<T>() },
				{ constant::zero<T>(),	constant::one<T>(),		constant::zero<T>() },
				{ constant::zero<T>(),	constant::zero<T>(),	constant::one<T>()  }
			};
			return *(reinterpret_cast<matrix<T,3,3> const *>(&_identity[0][0]));
		}

		static inline vector<T,3> get_row(matrix<T,3,3> const &mat, boost::int32_t r)
		{
			vector<T,3> row_vec;
			row_vec.m_data[0]=mat.m._2d[r][0];
			row_vec.m_data[1]=mat.m._2d[r][1];
			row_vec.m_data[2]=mat.m._2d[r][2];
			return row_vec;
		}

		static inline vector<T,3> get_col(matrix<T,3,3> const &mat, boost::int32_t c)
		{
			vector<T,3> col_vec;
			col_vec.m_data[0]=mat.m._2d[0][c];
			col_vec.m_data[1]=mat_.m._2d[1][c];
			col_vec.m_data[2]=mat.m._2d[2][c];
			return col_vec;
		}

		template < typename RVT >
		static inline matrix<T,3,3>& set_row(matrix<T,3,3> &mat, boost::int32_t r, vector<RVT,3> const &row_vec)
		{
			mat.m._2d[r][0]=static_cast<T>(row_vec.m_data[0]);
			mat.m._2d[r][1]=static_cast<T>(row_vec.m_data[1]);
			mat.m._2d[r][2]=static_cast<T>(row_vec.m_data[2]);
			return mat;
		}

		template < typename CVT >
		static inline matrix<T,3,3>& set_col(matrix<T,3,3> &mat, boost::int32_t c, vector<CVT,3> const &col_vec)
		{
			mat.m._2d[0][c]=static_cast<T>(col_vec.m_data[0]);
			mat.m._2d[1][c]=static_cast<T>(col_vec.m_data[1]);
			mat.m._2d[2][c]=static_cast<T>(col_vec.m_data[2]);
			return mat;
		}

		template < typename RT >
		static inline matrix<T,3,3>& assign(matrix<T,3,3> &lhs, matrix<RT,3,3> const &rhs)
		{
			lhs.m._1d[0]=static_cast<T>(rhs.m._1d[0]);
			lhs.m._1d[1]=static_cast<T>(rhs.m._1d[1]);
			lhs.m._1d[2]=static_cast<T>(rhs.m._1d[2]);
			lhs.m._1d[3]=static_cast<T>(rhs.m._1d[3]);
			lhs.m._1d[4]=static_cast<T>(rhs.m._1d[4]);
			lhs.m._1d[5]=static_cast<T>(rhs.m._1d[5]);
			lhs.m._1d[6]=static_cast<T>(rhs.m._1d[6]);
			lhs.m._1d[7]=static_cast<T>(rhs.m._1d[7]);
			lhs.m._1d[8]=static_cast<T>(rhs.m._1d[8]);
			return lhs;
		}

		template < typename RT >
		static inline matrix<T,3,3>& plus_assign(matrix<T,3,3> &lhs, matrix<RT,2,2> const &rhs)
		{
			lhs.m._1d[0]+=static_cast<T>(rhs.m._1d[0]);
			lhs.m._1d[1]+=static_cast<T>(rhs.m._1d[1]);
			lhs.m._1d[2]+=static_cast<T>(rhs.m._1d[2]);
			lhs.m._1d[3]+=static_cast<T>(rhs.m._1d[3]);
			lhs.m._1d[4]+=static_cast<T>(rhs.m._1d[4]);
			lhs.m._1d[5]+=static_cast<T>(rhs.m._1d[5]);
			lhs.m._1d[6]+=static_cast<T>(rhs.m._1d[6]);
			lhs.m._1d[7]+=static_cast<T>(rhs.m._1d[7]);
			lhs.m._1d[8]+=static_cast<T>(rhs.m._1d[8]);
			return lhs;
		}

		template < typename RT >
		static inline matrix<T,3,3>& minus_assign(matrix<T,3,3> &lhs, matrix<RT,2,2> const &rhs)
		{
			lhs.m._1d[0]-=static_cast<T>(rhs.m._1d[0]);
			lhs.m._1d[1]-=static_cast<T>(rhs.m._1d[1]);
			lhs.m._1d[2]-=static_cast<T>(rhs.m._1d[2]);
			lhs.m._1d[3]-=static_cast<T>(rhs.m._1d[3]);
			lhs.m._1d[4]-=static_cast<T>(rhs.m._1d[4]);
			lhs.m._1d[5]-=static_cast<T>(rhs.m._1d[5]);
			lhs.m._1d[6]-=static_cast<T>(rhs.m._1d[6]);
			lhs.m._1d[7]-=static_cast<T>(rhs.m._1d[7]);
			lhs.m._1d[8]-=static_cast<T>(rhs.m._1d[8]);
			return lhs;
		}

		template < typename RT >
		static inline matrix<T,3,3>& prod_assign(matrix<T,3,3> &lhs, matrix<RT,3,3> const &rhs)
		{
			matrix<T,3,3> tmp=lhs;
			lhs.m._1d[0]=tmp.m._1d[0]*static_cast<T>(rhs.m._1d[0])+tmp.m._1d[1]*static_cast<T>(rhs.m._1d[3])+tmp.m._1d[2]*static_cast<T>(rhs.m._1d[6]);
			lhs.m._1d[1]=tmp.m._1d[0]*static_cast<T>(rhs.m._1d[1])+tmp.m._1d[1]*static_cast<T>(rhs.m._1d[4])+tmp.m._1d[2]*static_cast<T>(rhs.m._1d[7]);
			lhs.m._1d[2]=tmp.m._1d[0]*static_cast<T>(rhs.m._1d[2])+tmp.m._1d[1]*static_cast<T>(rhs.m._1d[5])+tmp.m._1d[2]*static_cast<T>(rhs.m._1d[8]);
			lhs.m._1d[3]=tmp.m._1d[3]*static_cast<T>(rhs.m._1d[0])+tmp.m._1d[4]*static_cast<T>(rhs.m._1d[3])+tmp.m._1d[5]*static_cast<T>(rhs.m._1d[6]);
			lhs.m._1d[4]=tmp.m._1d[3]*static_cast<T>(rhs.m._1d[1])+tmp.m._1d[4]*static_cast<T>(rhs.m._1d[4])+tmp.m._1d[5]*static_cast<T>(rhs.m._1d[7]);
			lhs.m._1d[5]=tmp.m._1d[3]*static_cast<T>(rhs.m._1d[2])+tmp.m._1d[4]*static_cast<T>(rhs.m._1d[5])+tmp.m._1d[5]*static_cast<T>(rhs.m._1d[8]);
			lhs.m._1d[6]=tmp.m._1d[6]*static_cast<T>(rhs.m._1d[0])+tmp.m._1d[7]*static_cast<T>(rhs.m._1d[3])+tmp.m._1d[8]*static_cast<T>(rhs.m._1d[6]);
			lhs.m._1d[7]=tmp.m._1d[6]*static_cast<T>(rhs.m._1d[1])+tmp.m._1d[7]*static_cast<T>(rhs.m._1d[4])+tmp.m._1d[8]*static_cast<T>(rhs.m._1d[7]);
			lhs.m._1d[8]=tmp.m._1d[6]*static_cast<T>(rhs.m._1d[2])+tmp.m._1d[7]*static_cast<T>(rhs.m._1d[5])+tmp.m._1d[8]*static_cast<T>(rhs.m._1d[8]);
			return lhs;
		}

		template < typename VT >
		static inline matrix<T,3,3>& scalar_multiply_assign(matrix<T,3,3> &lhs, VT const &v)
		{
			lhs.m._1d[0]*=static_cast<T>(v);
			lhs.m._1d[1]*=static_cast<T>(v);
			lhs.m._1d[2]*=static_cast<T>(v);
			lhs.m._1d[3]*=static_cast<T>(v);
			lhs.m._1d[4]*=static_cast<T>(v);
			lhs.m._1d[5]*=static_cast<T>(v);
			lhs.m._1d[6]*=static_cast<T>(v);
			lhs.m._1d[7]*=static_cast<T>(v);
			lhs.m._1d[8]*=static_cast<T>(v);
			return lhs;
		}

		template < typename VT >
		static inline vector<VT,3> vector_multiply(matrix<T,3,3> const &mat, vector<VT,3> const &vec)
		{
			vector<VT,3> res;
			res.m_data[0]=static_cast<VT>(
				mat.m._2d[0][0]*static_cast<T>(vec.m_data[0])+
				mat.m._2d[0][1]*static_cast<T>(vec.m_data[1])+
				mat.m._2d[0][2]*static_cast<T>(vec.m_data[2])
			);
			res.m_data[1]=static_cast<VT>(
				mat.m._2d[1][0]*static_cast<T>(vec.m_data[0])+
				mat.m._2d[1][1]*static_cast<T>(vec.m_data[1])+
				mat.m._2d[1][2]*static_cast<T>(vec.m_data[2])
			);
			res.m_data[2]=static_cast<VT>(
				mat.m._2d[2][0]*static_cast<T>(vec.m_data[0])+
				mat.m._2d[2][1]*static_cast<T>(vec.m_data[1])+
				mat.m._2d[2][2]*static_cast<T>(vec.m_data[2])
			);
			return res;
		}

		static inline matrix<T,3,3>& transpose(matrix<T,3,3> &mat)
		{
			std::swap(mat.m._1d[1], mat.m._1d[3]);
			std::swap(mat.m._1d[2], mat.m._1d[6]);
			std::swap(mat.m._1d[5], mat.m._1d[7]);
			return mat;
		}

		static inline matrix<T,3,3>& invert(matrix<T,3,3> &mat)
		{
			T det=
				mat.m._2d[0][0]*(mat.m._2d[1][1]*mat.m._2d[2][2]-mat.m._2d[1][2]*mat.m._2d[2][1])-
				mat.m._2d[0][1]*(mat.m._2d[1][0]*mat.m._2d[2][2]-mat.m._2d[1][2]*mat.m._2d[2][0])+
				mat.m._2d[0][2]*(mat.m._2d[1][0]*mat.m._2d[2][1]-mat.m._2d[1][1]*mat.m._2d[2][0]);

			if(det!=constant::zero<T>())
			{
				det=constant::one<T>()/det;

				T const tmp[3][3]=
				{
					{
						(mat.m._2d[1][1]*mat.m._2d[2][2]-mat.m._2d[1][2]*mat.m._2d[2][1])*det,
						(mat.m._2d[0][2]*mat.m._2d[2][1]-mat.m._2d[0][1]*mat.m._2d[2][2])*det,
						(mat.m._2d[0][1]*mat.m._2d[1][2]-mat.m._2d[0][2]*mat.m._2d[1][1])*det
					},
					{
						(mat.m._2d[1][2]*mat.m._2d[2][0]-mat.m._2d[1][0]*mat.m._2d[2][2])*det,
						(mat.m._2d[0][0]*mat.m._2d[2][2]-mat.m._2d[0][2]*mat.m._2d[2][0])*det,
						(mat.m._2d[0][2]*mat.m._2d[1][0]-mat.m._2d[0][0]*mat.m._2d[1][2])*det
					},
					{
						(mat.m._2d[1][0]*mat.m._2d[2][1]-mat.m._2d[1][1]*mat.m._2d[2][0])*det,
						(mat.m._2d[0][1]*mat.m._2d[2][0]-mat.m._2d[0][0]*mat.m._2d[2][1])*det,
						(mat.m._2d[0][0]*mat.m._2d[1][1]-mat.m._2d[0][1]*mat.m._2d[1][0])*det
					}
				};

				mat=*reinterpret_cast<matrix<T,3,3> const *>(&tmp[0][0]);
			}
			return mat;
		}
	};

	template < typename T >
	struct matrix_ops<T, 4, 4>
	{
		static inline matrix<T,4,4> const& zero()
		{
			static T const _zero[4][4]=
			{
				{ constant::zero<T>(), constant::zero<T>(), constant::zero<T>(), constant::zero<T>() },
				{ constant::zero<T>(), constant::zero<T>(), constant::zero<T>(), constant::zero<T>() },
				{ constant::zero<T>(), constant::zero<T>(), constant::zero<T>(), constant::zero<T>() },
				{ constant::zero<T>(), constant::zero<T>(), constant::zero<T>(), constant::zero<T>() }
			};
			return *(reinterpret_cast<matrix<T,4,4> const *>(&_zero[0][0]));
		}

		static inline matrix<T,4,4> const& identity()
		{
			static T const _identity[4][4]=
			{
				{ constant::one<T>(),	constant::zero<T>(),	constant::zero<T>(),	constant::zero<T>() },
				{ constant::zero<T>(),	constant::one<T>(),		constant::zero<T>(),	constant::zero<T>() },
				{ constant::zero<T>(),	constant::zero<T>(),	constant::one<T>(),		constant::zero<T>() },
				{ constant::zero<T>(),	constant::zero<T>(),	constant::zero<T>(),	constant::one<T>() }
			};
			return *(reinterpret_cast<matrix<T,4,4> const *>(&_identity[0][0]));
		}

		static inline vector<T,4> get_row(matrix<T,4,4> const &mat, boost::int32_t r)
		{
			vector<T,4> row_vec;
			row_vec.m_data[0]=mat.m._2d[r][0];
			row_vec.m_data[1]=mat.m._2d[r][1];
			row_vec.m_data[2]=mat.m._2d[r][2];
			row_vec.m_data[3]=mat.m._2d[r][3];
			return row_vec;
		}

		static inline vector<T,4> get_col(matrix<T,4,4> const &mat, boost::int32_t c)
		{
			vector<T,4> col_vec;
			col_vec.m_data[0]=mat.m._2d[0][c];
			col_vec.m_data[1]=mat.m._2d[1][c];
			col_vec.m_data[2]=mat.m._2d[2][c];
			col_vec.m_data[3]=mat.m._2d[3][c];
			return col_vec;
		}

		template < typename RVT >
		static inline matrix<T,4,4>& set_row(matrix<T,4,4> &mat, boost::int32_t r, vector<RVT,4> const &row_vec)
		{
			mat.m._2d[r][0]=static_cast<T>(row_vec.m_data[0]);
			mat.m._2d[r][1]=static_cast<T>(row_vec.m_data[1]);
			mat.m._2d[r][2]=static_cast<T>(row_vec.m_data[2]);
			mat.m._2d[r][3]=static_cast<T>(row_vec.m_data[3]);
			return mat;
		}

		template < typename CVT >
		static inline matrix<T,4,4>& set_col(matrix<T,4,4> &mat, boost::int32_t c, vector<CVT,4> const &col_vec)
		{
			mat.m._2d[0][c]=static_cast<T>(col_vec.m_data[0]);
			mat.m._2d[1][c]=static_cast<T>(col_vec.m_data[1]);
			mat.m._2d[2][c]=static_cast<T>(col_vec.m_data[2]);
			mat.m._2d[3][c]=static_cast<T>(col_vec.m_data[3]);
			return mat;
		}

		template < typename RT >
		static inline matrix<T,4,4>& assign(matrix<T,4,4> &lhs, matrix<RT,4,4> const &rhs)
		{
			for(boost::uint32_t i=0;i<matrix<T,4,4>::RXC;++i)
				lhs.m._1d[i]=static_cast<T>(rhs.m._1d[i]);
			return lhs;
		}

		template < typename RT >
		static inline matrix<T,4,4>& plus_assign(matrix<T,4,4> &lhs, matrix<RT,4,4> const &rhs)
		{
			for(boost::uint32_t i=0;i<matrix<T,4,4>::RXC;++i)
				lhs.m._1d[i]+=static_cast<T>(rhs.m._1d[i]);
		}

		template < typename RT >
		static inline matrix<T,4,4>& minus_assign(matrix<T,4,4> &lhs, matrix<RT,4,4> const &rhs)
		{
			for(boost::uint32_t i=0;i<matrix<T,4,4>::RXC;++i)
				lhs.m._1d[i]-=static_cast<T>(rhs.m._1d[i]);
		}

		template < typename RT >
		static inline matrix<T,4,4>& prod_assign(matrix<T,4,4> &lhs, matrix<RT,4,4> const &rhs)
		{
			matrix<T,4,4> tmp;
			for(boost::uint32_t r=0;r<4;++r)
			{
				for(boost::uint32_t c=0;c<4;++c)
				{
					tmp.m._2d[r][c]=
						lhs.m._2d[r][0]*static_cast<T>(rhs.m._2d[0][c])+
						lhs.m._2d[r][1]*static_cast<T>(rhs.m._2d[1][c])+
						lhs.m._2d[r][2]*static_cast<T>(rhs.m._2d[2][c])+
						lhs.m._2d[r][3]*static_cast<T>(rhs.m._2d[3][c]);
				}
			}
			lhs=tmp;
			return lhs;
		}

		template < typename VT >
		static inline matrix<T,4,4>& scalar_multiply_assign(matrix<T,4,4> &lhs, VT const &v)
		{
			for(boost::uint32_t i=0;i<matrix<T,4,4>::RXC;++i)
				lhs.m._1d[i]*=static_cast<T>(v);
			return lhs;
		}

		template < typename VT >
		static inline vector<VT,4> vector_multiply(matrix<T,4,4> const &mat, vector<VT,4> const &vec)
		{
			vector<VT,3> res;
			res.m_data[0]=static_cast<VT>(
				mat.m._2d[0][0]*static_cast<T>(vec.m_data[0])+
				mat.m._2d[0][1]*static_cast<T>(vec.m_data[1])+
				mat.m._2d[0][2]*static_cast<T>(vec.m_data[2])+
				mat.m._2d[0][3]*static_cast<T>(vec.m_data[3])
			);
			res.m_data[1]=static_cast<VT>(
				mat.m._2d[1][0]*static_cast<T>(vec.m_data[0])+
				mat.m._2d[1][1]*static_cast<T>(vec.m_data[1])+
				mat.m._2d[1][2]*static_cast<T>(vec.m_data[2])+
				mat.m._2d[1][3]*static_cast<T>(vec.m_data[3])
			);
			res.m_data[2]=static_cast<VT>(
				mat.m._2d[2][0]*static_cast<T>(vec.m_data[0])+
				mat.m._2d[2][1]*static_cast<T>(vec.m_data[1])+
				mat.m._2d[2][2]*static_cast<T>(vec.m_data[2])+
				mat.m._2d[2][3]*static_cast<T>(vec.m_data[3])
			);
			res.m_data[3]=static_cast<VT>(
				mat.m._2d[3][0]*static_cast<T>(vec.m_data[0])+
				mat.m._2d[3][1]*static_cast<T>(vec.m_data[1])+
				mat.m._2d[3][2]*static_cast<T>(vec.m_data[2])+
				mat.m._2d[3][3]*static_cast<T>(vec.m_data[3])
			);
			return res;
		}

		static inline matrix<T,4,4>& transpose(matrix<T,4,4> &mat)
		{
			for(boost::uint32_t r=0;r<4;++r)
				for(boost::uint32_t c=r+1;r<4;++r)
					std::swap(mat.m._2d[r][c],mat.m._2d[c][r]);
			return mat;
		}
	};

} //namespace math
} //namespace baldzarika

#endif //BALDZARIKA_MATH_MATRIX_OPS_H