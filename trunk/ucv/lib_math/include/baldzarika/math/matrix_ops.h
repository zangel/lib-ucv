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
			for(boost::uint32_t r=0;r<R;++r)
				for(boost::uint32_t c=0;c<C;++c)
					res.m_data[r][c]=constant::zero<T>();
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
				res.m_data[i][i]=constant::one<T>();
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
				row_vec.m_data[c]=mat.m_data[r][c];
			return row_vec;
		}

		static inline vector<T,R> get_col(matrix<T,R,C> const &mat, boost::int32_t c)
		{
			vector<T,R> col_vec;
			for(boost::uint32_t r=0;r<R;++r)
				col_vec.m_data[r]=mat.m_data[r][c];
			return col_vec;
		}

		template < typename RVT >
		static inline matrix<T,R,C>& set_row(matrix<T,R,C> &mat, boost::int32_t r, vector<RVT,C> const &row_vec)
		{
			for(boost::uint32_t c=0;c<C;++c)
				mat.m_data[r][c]=static_cast<T>(row_vec.m_data[c]);
			return mat;
		}

		template < typename CVT >
		static inline matrix<T,R,C>& set_col(matrix<T,R,C> &mat, boost::int32_t c, vector<CVT,R> const &col_vec)
		{
			for(boost::uint32_t r=0;r<R;++r)
				mat.m_data[r][c]=static_cast<T>(col_vec.m_data[r]);
			return mat;
		}

		template < typename RT >
		static inline matrix<T,R,C>& assign(matrix<T,R,C> &lhs, matrix<RT,R,C> const &rhs)
		{
			for(boost::uint32_t r=0;r<R;++r)
				for(boost::uint32_t c=0;c<C;++c)
					lhs.m_data[r][c]=static_cast<T>(rhs.m_data[r][c]);
			return lhs;
		}

		template < typename RT >
		static inline matrix<T,R,C>& plus_assign(matrix<T,R,C> &lhs, matrix<RT,R,C> const &rhs)
		{
			for(boost::uint32_t r=0;r<R;++r)
				for(boost::uint32_t c=0;c<C;++c)
					lhs.m_data[r][c]+=static_cast<T>(rhs.m_data[r][c]);
			return lhs;
		}

		template < typename RT >
		static inline matrix<T,R,C>& minus_assign(matrix<T,R,C> &lhs, matrix<RT,R,C> const &rhs)
		{
			for(boost::uint32_t r=0;r<R;++r)
				for(boost::uint32_t c=0;c<C;++c)
					lhs.m_data[r][c]-=static_cast<T>(rhs.m_data[r][c]);
			return lhs;
		}

		template < typename ST >
		static inline matrix<T,R,C>& scalar_multiply_assign(matrix<T,R,C> &lhs, ST const &s)
		{
			for(boost::uint32_t r=0;r<R;++r)
				for(boost::uint32_t c=0;c<C;++c)
					lhs.m_data[r][c]*=static_cast<T>(s);
			return lhs;
		}

		template < typename VT >
		static inline vector<VT,R> vector_multiply(matrix<T,R,C> const &mat, vector<VT,C> const &vec)
		{
			vector<VT,R> res=vector<VT,R>::zero();
			for(boost::uint32_t r=0;r<R;++r)
				for(boost::uint32_t c=0;c<C;++c)
					res.m_data[r]+=vec.m_data[c]*static_cast<VT>(mat.m_data[r][c]);
			return res;
		}

		template < typename RT, boost::uint32_t RR, boost::uint32_t RC >
		struct product_result
		{
			typedef matrix<T, R, RC> type;
		};

		template < typename RT, boost::uint32_t RR, boost::uint32_t RC >
		static inline typename product_result<RT,RR,RC>::type product(matrix<T,R,C> const &lhs, matrix<RT,RR,RC> const &rhs)
		{
			typedef typename product_result<RT,RR,RC>::type result_t;

			result_t res;

			for(boost::uint32_t r=0;r<result_t::ROWS;++r)
			{
				for(boost::uint32_t c=0;c<result_t::COLS;++c)
				{
					T res_value=constant::zero<T>();
					for(boost::uint32_t c2=0;c2<C;++c2)
						res_value+=lhs.m_data[r][c2]*rhs.m_data[c2][c];
					res.m_data[r][c]=res_value;
				}
			}
			return res;
		}
		
		static inline matrix<T,C,R> transpose(matrix<T,R,C> const &mat)
		{
			matrix<T,C,R> res;
			for(boost::uint32_t r=0;r<R;++r)
				for(boost::uint32_t c=0;c<C;++c)
					res.m_data[c][r]=mat.m_data[r][c];
			return res;
		}

		static inline T determinant(matrix<T,R,C> const &mat)
		{
			BOOST_ASSERT(false);
			return constant::zero<T>();
		}

		static inline matrix<T,C,R> invert(matrix<T,R,C> const &mat)
		{
			BOOST_ASSERT(false);
			return matrix<T,C,R>();
		}
	};

	template < typename T >
	struct matrix_ops<T, 1, 1>
	{
		static inline matrix<T,1,1> const& zero()
		{
			static T const _zero[1][1]=
			{
				{ constant::zero<T>() }
			};
			return *(reinterpret_cast<matrix<T,1,1> const *>(&_zero[0][0]));
		}

		static inline matrix<T,1,1> const& identity()
		{
			static T const _identity[1][1]=
			{
				{ constant::one<T>()}
			};
			return *(reinterpret_cast<matrix<T,1,1> const *>(&_identity[0][0]));
		}

		static inline vector<T,1> get_row(matrix<T,1,1> const &mat, boost::int32_t r)
		{
			BOOST_ASSERT(r==0);
			vector<T,1> row_vec;
			row_vec.m_data[0]=mat.m_data[0][0];
			return row_vec;
		}

		static inline vector<T,1> get_col(matrix<T,1,1> const &mat, boost::int32_t c)
		{
			BOOST_ASSERT(c==0);
			vector<T,1> col_vec;
			col_vec.m_data[0]=mat.m_data[0][0];
			return col_vec;
		}

		template < typename RVT >
		static inline matrix<T,1,1>& set_row(matrix<T,1,1> &mat, boost::int32_t r, vector<RVT,1> const &row_vec)
		{
			BOOST_ASSERT(r==0);
			mat.m_data[0][0]=static_cast<T>(row_vec.m_data[0]);
			return mat;
		}

		template < typename CVT >
		static inline matrix<T,1,1>& set_col(matrix<T,1,1> &mat, boost::int32_t c, vector<CVT,1> const &col_vec)
		{
			BOOST_ASSERT(c==0);
			mat.m_data[0][0]=static_cast<T>(col_vec.m_data[0]);
			return mat;
		}

		template < typename RT >
		static inline matrix<T,1,1>& assign(matrix<T,1,1> &lhs, matrix<RT,1,1> const &rhs)
		{
			lhs.m_data[0][0]=static_cast<T>(rhs.m_data[0][0]);
			return lhs;
		}

		template < typename RT >
		static inline matrix<T,1,1>& plus_assign(matrix<T,1,1> &lhs, matrix<RT,1,1> const &rhs)
		{
			lhs.m_data[0][0]+=static_cast<T>(rhs.m_data[0][0]);
			return lhs;
		}

		template < typename RT >
		static inline matrix<T,1,1>& minus_assign(matrix<T,1,1> &lhs, matrix<RT,1,1> const &rhs)
		{
			lhs.m_data[0][0]-=static_cast<T>(rhs.m_data[0][0]);
			return lhs;
		}

		template < typename VT >
		static inline matrix<T,1,1>& scalar_multiply_assign(matrix<T,1,1> &lhs, VT const &v)
		{
			lhs.m_data[0][0]*=static_cast<T>(v);
			return lhs;
		}

		template < typename VT >
		static inline vector<VT,1> vector_multiply(matrix<T,1,1> const &mat, vector<VT,1> const &vec)
		{
			vector<VT,1> res;
			res.m_data[0]=static_cast<VT>(mat.m_data[0][0]*static_cast<T>(vec.m_data[0]));
			return res;
		}

		template < typename RT, boost::uint32_t RR, boost::uint32_t RC > struct product_result;

		template < typename RT, boost::uint32_t RC >
		struct product_result<RT, 1, RC>
		{
			typedef matrix<T, 1, RC> type;
		};

		template < typename RT, boost::uint32_t RC >
		static inline typename product_result<RT,1,RC>::type product(matrix<T,1,1> const &lhs, matrix<RT,1,RC> const &rhs)
		{
			typedef typename product_result<RT,1,RC>::type result_t;
			result_t res;
			for(boost::uint32_t c=0;c<result_t::COLS;++c)
				res.m_data[0][c]=lhs.m_data[0][0]*rhs.m_data[0][c];
			return res;
		}


		template < typename RT >
		struct product_result<RT, 1, 1>
		{
			typedef matrix<T, 1, 1> type;
		};

		template < typename RT >
		static inline matrix<T,1,1> product(matrix<T,1,1> const &lhs, matrix<RT,1,1> const &rhs)
		{
			matrix<T,1,1> res;
			res.m_data[0][0]=lhs.m_data[0][0]*static_cast<T>(rhs.m_data[0][0]);
			return res;
		}

		static inline matrix<T,1,1> transpose(matrix<T,1,1> const &mat)
		{
			return mat;
		}

		static inline T determinant(matrix<T,1,1> const &mat)
		{
			return mat.m_data[0][0];
		}

		static inline matrix<T,1,1> invert(matrix<T,1,1> const &mat)
		{
			matrix<T,1,1> res;
			res.m_data[0][0]=std::abs(mat.m_data[0][0])>std::numeric_limits<T>::epsilon()?
				math::constant::one<T>()/mat.m_data[0][0]:
				math::constant::zero<T>();
			return res;
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
			row_vec.m_data[0]=mat.m_data[r][0];
			row_vec.m_data[1]=mat.m_data[r][1];
			return row_vec;
		}

		static inline vector<T,2> get_col(matrix<T,2,2> const &mat, boost::int32_t c)
		{
			vector<T,2> col_vec;
			col_vec.m_data[0]=mat.m_data[0][c];
			col_vec.m_data[1]=mat.m_data[1][c];
			return col_vec;
		}

		template < typename RVT >
		static inline matrix<T,2,2>& set_row(matrix<T,2,2> &mat, boost::int32_t r, vector<RVT,2> const &row_vec)
		{
			mat.m_data[r][0]=static_cast<T>(row_vec.m_data[0]);
			mat.m_data[r][1]=static_cast<T>(row_vec.m_data[1]);
			return mat;
		}

		template < typename CVT >
		static inline matrix<T,2,2>& set_col(matrix<T,2,2> &mat, boost::int32_t c, vector<CVT,2> const &col_vec)
		{
			mat.m_data[0][c]=static_cast<T>(col_vec.m_data[0]);
			mat.m_data[1][c]=static_cast<T>(col_vec.m_data[1]);
			return mat;
		}

		template < typename RT >
		static inline matrix<T,2,2>& assign(matrix<T,2,2> &lhs, matrix<RT,2,2> const &rhs)
		{
			lhs.m_data[0][0]=static_cast<T>(rhs.m_data[0][0]);
			lhs.m_data[0][1]=static_cast<T>(rhs.m_data[0][1]);
			lhs.m_data[1][0]=static_cast<T>(rhs.m_data[1][0]);
			lhs.m_data[1][1]=static_cast<T>(rhs.m_data[1][1]);
			return lhs;
		}

		template < typename RT >
		static inline matrix<T,2,2>& plus_assign(matrix<T,2,2> &lhs, matrix<RT,2,2> const &rhs)
		{
			lhs.m_data[0][0]+=static_cast<T>(rhs.m_data[0][0]);
			lhs.m_data[0][1]+=static_cast<T>(rhs.m_data[0][1]);
			lhs.m_data[1][0]+=static_cast<T>(rhs.m_data[1][0]);
			lhs.m_data[1][1]+=static_cast<T>(rhs.m_data[1][1]);
			return lhs;
		}

		template < typename RT >
		static inline matrix<T,2,2>& minus_assign(matrix<T,2,2> &lhs, matrix<RT,2,2> const &rhs)
		{
			lhs.m_data[0][0]-=static_cast<T>(rhs.m_data[0][0]);
			lhs.m_data[0][1]-=static_cast<T>(rhs.m_data[0][1]);
			lhs.m_data[1][0]-=static_cast<T>(rhs.m_data[1][0]);
			lhs.m_data[1][1]-=static_cast<T>(rhs.m_data[1][1]);
			return lhs;
		}

		template < typename VT >
		static inline matrix<T,2,2>& scalar_multiply_assign(matrix<T,2,2> &lhs, VT const &v)
		{
			lhs.m_data[0][0]*=static_cast<T>(v);
			lhs.m_data[0][1]*=static_cast<T>(v);
			lhs.m_data[1][0]*=static_cast<T>(v);
			lhs.m_data[1][1]*=static_cast<T>(v);
			return lhs;
		}

		template < typename VT >
		static inline vector<VT,2> vector_multiply(matrix<T,2,2> const &mat, vector<VT,2> const &vec)
		{
			vector<VT,2> res;
			res.m_data[0]=static_cast<VT>(mat.m_data[0][0]*static_cast<T>(vec.m_data[0])+mat.m_data[0][1]*static_cast<T>(vec.m_data[1]));
			res.m_data[1]=static_cast<VT>(mat.m_data[1][0]*static_cast<T>(vec.m_data[0])+mat.m_data[1][1]*static_cast<T>(vec.m_data[1]));
			return res;
		}

		template < typename RT, boost::uint32_t RR, boost::uint32_t RC > struct product_result;

		template < typename RT, boost::uint32_t RC >
		struct product_result<RT, 2, RC>
		{
			typedef matrix<T, 2, RC> type;
		};

		template < typename RT, boost::uint32_t RC >
		static inline typename product_result<RT,2,RC>::type product(matrix<T,2,2> const &lhs, matrix<RT,2,RC> const &rhs)
		{
			typedef typename product_result<RT,2,RC>::type result_t;
			result_t res;
			for(boost::uint32_t c=0;c<result_t::COLS;++c)
				res.m_data[0][c]=lhs.m_data[0][0]*rhs.m_data[0][c]+lhs.m_data[0][1]*rhs.m_data[1][c];

			for(boost::uint32_t c=0;c<result_t::COLS;++c)
				res.m_data[1][c]=lhs.m_data[1][0]*rhs.m_data[0][c]+lhs.m_data[1][1]*rhs.m_data[1][c];

			return res;
		}


		template < typename RT >
		struct product_result<RT, 2, 2>
		{
			typedef matrix<T, 2, 2> type;
		};

		template < typename RT >
		static inline matrix<T,2,2> product(matrix<T,2,2> const &lhs, matrix<RT,2,2> const &rhs)
		{
			matrix<T,2,2> res;
			res.m_data[0][0]=lhs.m_data[0][0]*static_cast<T>(rhs.m_data[0][0])+lhs.m_data[0][1]*static_cast<T>(rhs.m_data[1][0]);
			res.m_data[0][1]=lhs.m_data[0][0]*static_cast<T>(rhs.m_data[0][1])+lhs.m_data[0][1]*static_cast<T>(rhs.m_data[1][1]);
			res.m_data[1][0]=lhs.m_data[1][0]*static_cast<T>(rhs.m_data[0][0])+lhs.m_data[1][1]*static_cast<T>(rhs.m_data[1][0]);
			res.m_data[1][1]=lhs.m_data[1][0]*static_cast<T>(rhs.m_data[0][1])+lhs.m_data[1][1]*static_cast<T>(rhs.m_data[1][1]);
			return res;
		}

		static inline matrix<T,2,2> transpose(matrix<T,2,2> const &mat)
		{
			matrix<T,2,2> res(mat);
			std::swap(res.m_data[0][1], res.m_data[1][0]);
			return res;
		}

		static inline T determinant(matrix<T,2,2> const &mat)
		{
			return mat.m_data[0][0]*mat.m_data[1][1]-mat.m_data[0][1]*mat.m_data[1][1];
		}

		static inline matrix<T,2,2> invert(matrix<T,2,2> const &mat)
		{
			matrix<T,2,2> res;
			T det=determinant(mat);
			if(det!=constant::zero<T>())
			{
				det=constant::one<T>()/det;
				res.m_data[0][0]=mat.m_data[1][1]*det;
				res.m_data[1][1]=mat.m_data[0][0]*det;
				res.m_data[0][1]=-mat.m_data[0][1]*det;
				res.m_data[1][0]=-mat.m_data[1][0]*det;
			}
			return res;
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
			vector<T,3> res;
			res.m_data[0]=mat.m_data[r][0];
			res.m_data[1]=mat.m_data[r][1];
			res.m_data[2]=mat.m_data[r][2];
			return res;
		}

		static inline vector<T,3> get_col(matrix<T,3,3> const &mat, boost::int32_t c)
		{
			vector<T,3> res;
			res.m_data[0]=mat.m_data[0][c];
			res.m_data[1]=mat.m_data[1][c];
			res.m_data[2]=mat.m_data[2][c];
			return res;
		}

		template < typename RVT >
		static inline matrix<T,3,3>& set_row(matrix<T,3,3> &mat, boost::int32_t r, vector<RVT,3> const &row_vec)
		{
			mat.m_data[r][0]=static_cast<T>(row_vec.m_data[0]);
			mat.m_data[r][1]=static_cast<T>(row_vec.m_data[1]);
			mat.m_data[r][2]=static_cast<T>(row_vec.m_data[2]);
			return mat;
		}

		template < typename CVT >
		static inline matrix<T,3,3>& set_col(matrix<T,3,3> &mat, boost::int32_t c, vector<CVT,3> const &col_vec)
		{
			mat.m_data[0][c]=static_cast<T>(col_vec.m_data[0]);
			mat.m_data[1][c]=static_cast<T>(col_vec.m_data[1]);
			mat.m_data[2][c]=static_cast<T>(col_vec.m_data[2]);
			return mat;
		}

		template < typename RT >
		static inline matrix<T,3,3>& assign(matrix<T,3,3> &lhs, matrix<RT,3,3> const &rhs)
		{
			lhs.m_data[0][0]=static_cast<T>(rhs.m_data[0][0]);
			lhs.m_data[0][1]=static_cast<T>(rhs.m_data[0][1]);
			lhs.m_data[0][2]=static_cast<T>(rhs.m_data[0][2]);
			lhs.m_data[1][0]=static_cast<T>(rhs.m_data[1][0]);
			lhs.m_data[1][1]=static_cast<T>(rhs.m_data[1][1]);
			lhs.m_data[1][2]=static_cast<T>(rhs.m_data[1][2]);
			lhs.m_data[2][0]=static_cast<T>(rhs.m_data[2][0]);
			lhs.m_data[2][1]=static_cast<T>(rhs.m_data[2][1]);
			lhs.m_data[2][2]=static_cast<T>(rhs.m_data[2][2]);
			return lhs;
		}

		template < typename RT >
		static inline matrix<T,3,3>& plus_assign(matrix<T,3,3> &lhs, matrix<RT,2,2> const &rhs)
		{
			lhs.m_data[0][0]+=static_cast<T>(rhs.m_data[0][0]);
			lhs.m_data[0][1]+=static_cast<T>(rhs.m_data[0][1]);
			lhs.m_data[0][2]+=static_cast<T>(rhs.m_data[0][2]);

			lhs.m_data[1][0]+=static_cast<T>(rhs.m_data[1][0]);
			lhs.m_data[1][1]+=static_cast<T>(rhs.m_data[1][1]);
			lhs.m_data[1][2]+=static_cast<T>(rhs.m_data[1][2]);

			lhs.m_data[2][0]+=static_cast<T>(rhs.m_data[2][0]);
			lhs.m_data[2][1]+=static_cast<T>(rhs.m_data[2][1]);
			lhs.m_data[2][2]+=static_cast<T>(rhs.m_data[2][2]);
			
			return lhs;
		}

		template < typename RT >
		static inline matrix<T,3,3>& minus_assign(matrix<T,3,3> &lhs, matrix<RT,2,2> const &rhs)
		{
			lhs.m_data[0][0]-=static_cast<T>(rhs.m_data[0][0]);
			lhs.m_data[0][1]-=static_cast<T>(rhs.m_data[0][1]);
			lhs.m_data[0][2]-=static_cast<T>(rhs.m_data[0][2]);

			lhs.m_data[1][0]-=static_cast<T>(rhs.m_data[1][0]);
			lhs.m_data[1][1]-=static_cast<T>(rhs.m_data[1][1]);
			lhs.m_data[1][2]-=static_cast<T>(rhs.m_data[1][2]);

			lhs.m_data[2][0]-=static_cast<T>(rhs.m_data[2][0]);
			lhs.m_data[2][1]-=static_cast<T>(rhs.m_data[2][1]);
			lhs.m_data[2][2]-=static_cast<T>(rhs.m_data[2][2]);
			return lhs;
		}

		template < typename VT >
		static inline matrix<T,3,3>& scalar_multiply_assign(matrix<T,3,3> &lhs, VT const &v)
		{
			lhs.m_data[0][0]*=static_cast<T>(v);
			lhs.m_data[0][1]*=static_cast<T>(v);
			lhs.m_data[0][2]*=static_cast<T>(v);

			lhs.m_data[1][0]*=static_cast<T>(v);
			lhs.m_data[1][1]*=static_cast<T>(v);
			lhs.m_data[1][2]*=static_cast<T>(v);

			lhs.m_data[2][0]*=static_cast<T>(v);
			lhs.m_data[2][1]*=static_cast<T>(v);
			lhs.m_data[2][2]*=static_cast<T>(v);
			return lhs;
		}

		template < typename VT >
		static inline vector<VT,3> vector_multiply(matrix<T,3,3> const &mat, vector<VT,3> const &vec)
		{
			vector<VT,3> res;
			res.m_data[0]=static_cast<VT>(
				mat.m_data[0][0]*static_cast<T>(vec.m_data[0])+
				mat.m_data[0][1]*static_cast<T>(vec.m_data[1])+
				mat.m_data[0][2]*static_cast<T>(vec.m_data[2])
				);
			res.m_data[1]=static_cast<VT>(
				mat.m_data[1][0]*static_cast<T>(vec.m_data[0])+
				mat.m_data[1][1]*static_cast<T>(vec.m_data[1])+
				mat.m_data[1][2]*static_cast<T>(vec.m_data[2])
				);
			res.m_data[2]=static_cast<VT>(
				mat.m_data[2][0]*static_cast<T>(vec.m_data[0])+
				mat.m_data[2][1]*static_cast<T>(vec.m_data[1])+
				mat.m_data[2][2]*static_cast<T>(vec.m_data[2])
				);
			return res;
		}


		template < typename RT, boost::uint32_t RR, boost::uint32_t RC > struct product_result;

		template < typename RT >
		struct product_result<RT, 3, 3>
		{
			typedef matrix<T, 3, 3> type;
		};

		template < typename RT >
		static inline matrix<T,3,3> product(matrix<T,3,3> const &lhs, matrix<RT,3,3> const &rhs)
		{
			matrix<T,3,3> res;

			res.m_data[0][0]=
				lhs.m_data[0][0]*static_cast<T>(rhs.m_data[0][0])+
				lhs.m_data[0][1]*static_cast<T>(rhs.m_data[1][0])+
				lhs.m_data[0][2]*static_cast<T>(rhs.m_data[2][0]);
			res.m_data[0][1]=
				lhs.m_data[0][0]*static_cast<T>(rhs.m_data[0][1])+
				lhs.m_data[0][1]*static_cast<T>(rhs.m_data[1][1])+
				lhs.m_data[0][2]*static_cast<T>(rhs.m_data[2][1]);
			res.m_data[0][2]=
				lhs.m_data[0][0]*static_cast<T>(rhs.m_data[0][2])+
				lhs.m_data[0][1]*static_cast<T>(rhs.m_data[1][2])+
				lhs.m_data[0][2]*static_cast<T>(rhs.m_data[2][2]);

			res.m_data[1][0]=
				lhs.m_data[1][0]*static_cast<T>(rhs.m_data[0][0])+
				lhs.m_data[1][1]*static_cast<T>(rhs.m_data[1][0])+
				lhs.m_data[1][2]*static_cast<T>(rhs.m_data[2][0]);
			res.m_data[1][1]=
				lhs.m_data[1][0]*static_cast<T>(rhs.m_data[0][1])+
				lhs.m_data[1][1]*static_cast<T>(rhs.m_data[1][1])+
				lhs.m_data[1][2]*static_cast<T>(rhs.m_data[2][1]);
			res.m_data[1][2]=
				lhs.m_data[1][0]*static_cast<T>(rhs.m_data[0][2])+
				lhs.m_data[1][1]*static_cast<T>(rhs.m_data[1][2])+
				lhs.m_data[1][2]*static_cast<T>(rhs.m_data[2][2]);

			res.m_data[2][0]=
				lhs.m_data[2][0]*static_cast<T>(rhs.m_data[0][0])+
				lhs.m_data[2][1]*static_cast<T>(rhs.m_data[1][0])+
				lhs.m_data[2][2]*static_cast<T>(rhs.m_data[2][0]);
			res.m_data[2][1]=
				lhs.m_data[2][0]*static_cast<T>(rhs.m_data[0][1])+
				lhs.m_data[2][1]*static_cast<T>(rhs.m_data[1][1])+
				lhs.m_data[2][2]*static_cast<T>(rhs.m_data[2][1]);
			res.m_data[2][2]=
				lhs.m_data[2][0]*static_cast<T>(rhs.m_data[0][2])+
				lhs.m_data[2][1]*static_cast<T>(rhs.m_data[1][2])+
				lhs.m_data[2][2]*static_cast<T>(rhs.m_data[2][2]);

			return res;
		}

		static inline matrix<T,3,3> transpose(matrix<T,3,3> const &mat)
		{
			matrix<T,3,3> res(mat);
			std::swap(res.m_data[0][1], res.m_data[1][0]);
			std::swap(res.m_data[0][2], res.m_data[2][0]);
			std::swap(res.m_data[1][2], res.m_data[2][1]);
			return res;
		}

		static inline T determinant(matrix<T,3,3> const &mat)
		{
			return	mat.m_data[0][0]*(mat.m_data[1][1]*mat.m_data[2][2]-mat.m_data[1][2]*mat.m_data[2][1])-
					mat.m_data[0][1]*(mat.m_data[1][0]*mat.m_data[2][2]-mat.m_data[1][2]*mat.m_data[2][0])+
					mat.m_data[0][2]*(mat.m_data[1][0]*mat.m_data[2][1]-mat.m_data[1][1]*mat.m_data[2][0]);
		}

		static inline matrix<T,3,3> invert(matrix<T,3,3> const &mat)
		{
			matrix<T,3,3> res;
			T det=determinant(mat);
			if(det!=constant::zero<T>())
			{
				det=constant::one<T>()/det;

				res.m_data[0][0]=(mat.m_data[1][1]*mat.m_data[2][2]-mat.m_data[1][2]*mat.m_data[2][1])*det;
				res.m_data[0][1]=(mat.m_data[0][2]*mat.m_data[2][1]-mat.m_data[0][1]*mat.m_data[2][2])*det;
				res.m_data[0][2]=(mat.m_data[0][1]*mat.m_data[1][2]-mat.m_data[0][2]*mat.m_data[1][1])*det;

				res.m_data[1][0]=(mat.m_data[1][2]*mat.m_data[2][0]-mat.m_data[1][0]*mat.m_data[2][2])*det;
				res.m_data[1][1]=(mat.m_data[0][0]*mat.m_data[2][2]-mat.m_data[0][2]*mat.m_data[2][0])*det;
				res.m_data[1][2]=(mat.m_data[0][2]*mat.m_data[1][0]-mat.m_data[0][0]*mat.m_data[1][2])*det;

				res.m_data[2][0]=(mat.m_data[1][0]*mat.m_data[2][1]-mat.m_data[1][1]*mat.m_data[2][0])*det;
				res.m_data[2][1]=(mat.m_data[0][1]*mat.m_data[2][0]-mat.m_data[0][0]*mat.m_data[2][1])*det;
				res.m_data[2][2]=(mat.m_data[0][0]*mat.m_data[1][1]-mat.m_data[0][1]*mat.m_data[1][0])*det;
			}
			return res;
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
			vector<T,4> res;
			res.m_data[0]=mat.m_data[r][0];
			res.m_data[1]=mat.m_data[r][1];
			res.m_data[2]=mat.m_data[r][2];
			res.m_data[3]=mat.m_data[r][3];
			return res;
		}

		static inline vector<T,4> get_col(matrix<T,4,4> const &mat, boost::int32_t c)
		{
			vector<T,4> res;
			res.m_data[0]=mat.m_data[0][c];
			res.m_data[1]=mat.m_data[1][c];
			res.m_data[2]=mat.m_data[2][c];
			res.m_data[3]=mat.m_data[3][c];
			return res;
		}

		template < typename RVT >
		static inline matrix<T,4,4>& set_row(matrix<T,4,4> &mat, boost::int32_t r, vector<RVT,4> const &row_vec)
		{
			mat.m_data[r][0]=static_cast<T>(row_vec.m_data[0]);
			mat.m_data[r][1]=static_cast<T>(row_vec.m_data[1]);
			mat.m_data[r][2]=static_cast<T>(row_vec.m_data[2]);
			mat.m_data[r][3]=static_cast<T>(row_vec.m_data[3]);
			return mat;
		}

		template < typename CVT >
		static inline matrix<T,4,4>& set_col(matrix<T,4,4> &mat, boost::int32_t c, vector<CVT,4> const &col_vec)
		{
			mat.m_data[0][c]=static_cast<T>(col_vec.m_data[0]);
			mat.m_data[1][c]=static_cast<T>(col_vec.m_data[1]);
			mat.m_data[2][c]=static_cast<T>(col_vec.m_data[2]);
			mat.m_data[3][c]=static_cast<T>(col_vec.m_data[3]);
			return mat;
		}

		template < typename RT >
		static inline matrix<T,4,4>& assign(matrix<T,4,4> &lhs, matrix<RT,4,4> const &rhs)
		{
			for(boost::uint32_t r=0;r<4;++r)
				for(boost::uint32_t c=0;c<4;++c)
					lhs.m_data[r][c]=static_cast<T>(rhs.m_data[r][c]);
			return lhs;
		}

		template < typename RT >
		static inline matrix<T,4,4>& plus_assign(matrix<T,4,4> &lhs, matrix<RT,4,4> const &rhs)
		{
			for(boost::uint32_t r=0;r<4;++r)
				for(boost::uint32_t c=0;c<4;++c)
					lhs.m_data[r][c]+=static_cast<T>(rhs.m_data[r][c]);
		}

		template < typename RT >
		static inline matrix<T,4,4>& minus_assign(matrix<T,4,4> &lhs, matrix<RT,4,4> const &rhs)
		{
			for(boost::uint32_t r=0;r<4;++r)
				for(boost::uint32_t c=0;c<4;++c)
					lhs.m_data[r][c]-=static_cast<T>(rhs.m_data[r][c]);
		}

		template < typename VT >
		static inline matrix<T,4,4>& scalar_multiply_assign(matrix<T,4,4> &lhs, VT const &v)
		{
			for(boost::uint32_t r=0;r<4;++r)
				for(boost::uint32_t c=0;c<4;++c)
					lhs.m_data[r][c]*=static_cast<T>(v);
			return lhs;
		}

		template < typename VT >
		static inline vector<VT,4> vector_multiply(matrix<T,4,4> const &mat, vector<VT,4> const &vec)
		{
			vector<VT,4> res;
			res.m_data[0]=static_cast<VT>(
				mat.m_data[0][0]*static_cast<T>(vec.m_data[0])+
				mat.m_data[0][1]*static_cast<T>(vec.m_data[1])+
				mat.m_data[0][2]*static_cast<T>(vec.m_data[2])+
				mat.m_data[0][3]*static_cast<T>(vec.m_data[3])
			);
			res.m_data[1]=static_cast<VT>(
				mat.m_data[1][0]*static_cast<T>(vec.m_data[0])+
				mat.m_data[1][1]*static_cast<T>(vec.m_data[1])+
				mat.m_data[1][2]*static_cast<T>(vec.m_data[2])+
				mat.m_data[1][3]*static_cast<T>(vec.m_data[3])
			);
			res.m_data[2]=static_cast<VT>(
				mat.m_data[2][0]*static_cast<T>(vec.m_data[0])+
				mat.m_data[2][1]*static_cast<T>(vec.m_data[1])+
				mat.m_data[2][2]*static_cast<T>(vec.m_data[2])+
				mat.m_data[2][3]*static_cast<T>(vec.m_data[3])
			);
			res.m_data[3]=static_cast<VT>(
				mat.m_data[3][0]*static_cast<T>(vec.m_data[0])+
				mat.m_data[3][1]*static_cast<T>(vec.m_data[1])+
				mat.m_data[3][2]*static_cast<T>(vec.m_data[2])+
				mat.m_data[3][3]*static_cast<T>(vec.m_data[3])
			);
			return res;
		}

		template < typename RT, boost::uint32_t RR, boost::uint32_t RC > struct product_result;

		template < typename RT >
		struct product_result<RT, 4, 4>
		{
			typedef matrix<T, 4, 4> type;
		};

		template < typename RT >
		static inline matrix<T,4,4> product(matrix<T,4,4> const &lhs, matrix<RT,4,4> const &rhs)
		{
			matrix<T,4,4> res;
			for(boost::uint32_t r=0;r<4;++r)
				for(boost::uint32_t c=0;c<4;++c)
					res.m_data[r][c]=
						lhs.m_data[r][0]*static_cast<T>(rhs.m_data[0][c])+
						lhs.m_data[r][1]*static_cast<T>(rhs.m_data[1][c])+
						lhs.m_data[r][2]*static_cast<T>(rhs.m_data[2][c])+
						lhs.m_data[r][3]*static_cast<T>(rhs.m_data[3][c]);
			return res;
		}

		static inline matrix<T,4,4> transpose(matrix<T,4,4> const &mat)
		{
			matrix<T,4,4> res(mat);
			for(boost::uint32_t r=0;r<4;++r)
				for(boost::uint32_t c=r+1;r<4;++r)
					std::swap(res.m_data[r][c], res.m_data[c][r]);
			return res;
		}

		static inline T determinant(matrix<T,4,4> const &mat)
		{
			BOOST_ASSERT(false);
			return constant::zero<T>();
		}

		static inline matrix<T,4,4> invert(matrix<T,4,4> const &mat)
		{
			BOOST_ASSERT(false);
			return matrix<T,4,4>();
		}
	};

} //namespace math
} //namespace baldzarika

#endif //BALDZARIKA_MATH_MATRIX_OPS_H
