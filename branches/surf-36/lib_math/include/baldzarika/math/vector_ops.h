#ifndef BALDZARIKA_MATH_VECTOR_OPS_H
#define BALDZARIKA_MATH_VECTOR_OPS_H

namespace baldzarika { namespace math {

	template < typename T , boost::uint32_t D >
	struct vector_ops
	{
		static inline vector<T,D> make_zero()
		{
			vector<T,D> res;
			for(boost::uint32_t i=0;i<D;++i)
				res.m_data[i]=constant::zero<T>();
			return res;
		}

		static inline vector<T,D> const& zero()
		{
			static vector<T,D> const _zero=make_zero();
			return _zero;
		}

		template < boost::uint32_t A >
		static inline vector<T,D> make_unit()
		{
			vector<T,D> res;
			for(boost::uint32_t i=0;i<D;++i)
				res.m_data[i]=i==A?constant::one<T>:constant::zero<T>();
			return res;
		}

		template < boost::uint32_t A >
		static inline vector<T,D> const& unit()
		{
			static vector<T,D> const _unit=make_unit<A>();
			return _unit;
		}

		template < typename RT >
		static inline vector<T,D>& assign(vector<T,D> &lhs, vector<RT,D> const &rhs)
		{
			for(boost::uint32_t d=0;d<D;++d)
				lhs.m_data[d]=static_cast<T>(rhs.m_data[d]);
			return lhs;
		}

		template < typename RT, boost::uint32_t RD >
		static inline vector<T,D>& assign(vector<T,D> &lhs, vector<RT,RD> const &rhs)
		{
			for(boost::uint32_t d=0;d<D;++d)
				lhs.m_data[d]=d<RD?static_cast<T>(rhs.m_data[d]):(d<D-1?constant::zero<T>():constant::one<T>());
			return lhs;
		}

		template < typename RT >
		static inline vector<T,D>& plus_assign(vector<T,D> &lhs, vector<RT,D> const &rhs)
		{
			for(boost::uint32_t d=0;d<D;++d)
				lhs.m_data[d]+=static_cast<T>(rhs.m_data[d]);
			return lhs;
		}

		template < typename RT >
		static inline vector<T,D>& minus_assign(vector<T,D> &lhs, vector<RT,D> const &rhs)
		{
			for(boost::uint32_t d=0;d<D;++d)
				lhs.m_data[d]-=static_cast<T>(rhs.m_data[d]);
			return lhs;
		}

		template < typename RT >
		static inline vector<T,D>& vector_multiply_assign(vector<T,D> &lhs, vector<RT,D> const &rhs)
		{
			for(boost::uint32_t d=0;d<D;++d)
				lhs.m_data[d]*=static_cast<T>(rhs.m_data[d]);
			return lhs;
		}

		template < typename RT >
		static inline vector<T,D>& vector_divide_assign(vector<T,D> &lhs, vector<RT,D> const &rhs)
		{
			for(boost::uint32_t d=0;d<D;++d)
				lhs.m_data[d]/=static_cast<T>(rhs.m_data[d]);
			return lhs;
		}

		template < typename ST >
		static inline vector<T,D>& scalar_multiply_assign(vector<T,D> &vec, ST const &s)
		{
			for(boost::uint32_t d=0;d<D;++d)
				vec.m_data[d]*=static_cast<T>(s);
			return vec;
		}


		template < typename ST >
		static inline vector<T,D>& scalar_divide_assign(vector<T,D> &vec, ST const &s)
		{
			for(boost::uint32_t d=0;d<D;++d)
				vec.m_data[d]/=static_cast<T>(s);
			return vec;
		}

		template < typename RT >
		static inline T dot_product(vector<T,D> const &lhs, vector<RT,D> const &rhs)
		{
			T res=constant::zero<T>();
			for(boost::uint32_t d=0;d<D;++d)
				res+=lhs.m_data[d]*static_cast<T>(rhs.m_data[d]);
			return res;
		}

		typedef void cross_product_result_t;

		template < typename RT >
		static inline cross_product_result_t cross_product(vector<T,D> const &lhs, vector<RT,D> const &rhs)
		{
			BOOST_ASSERT(false);
		}

		static inline T norm_1(vector<T,D> const &vec)
		{
			T res=constant::zero<T>();
			for(boost::uint32_t d=0;d<D;++d)
				res+=std::abs(vec.m_data[d]);
			return res;
		}

		static inline T norm_2(vector<T,D> const &vec)
		{
			return std::sqrt(dot_product(vec,vec));
		}
	};

	template < typename T >
	struct vector_ops<T,2>
	{

		static inline vector<T,2> const& zero()
		{
			static T const _zero[2]={ constant::zero<T>(), constant::zero<T>() };
			return *(reinterpret_cast<vector<T,2> const *>(&_zero[0]));
		}

		template < boost::uint32_t A >
		static inline vector<T,2> const& unit()
		{
			BOOST_STATIC_ASSERT(A<2);
			static T const _unit[2]=
			{
				A==0?constant::one<T>():constant::zero<T>(),
				A==1?constant::one<T>():constant::zero<T>()
			};
			return *(reinterpret_cast<vector<T,2> const *>(&_unit[0]));
		}

		template < typename RT >
		static inline vector<T,2>& assign(vector<T,2> &lhs, vector<RT,2> const &rhs)
		{
			lhs.m_data[0]=static_cast<T>(rhs.m_data[0]);
			lhs.m_data[1]=static_cast<T>(rhs.m_data[1]);
			return lhs;
		}

		template < typename RT, boost::uint32_t RD >
		static inline vector<T,2>& assign(vector<T,2> &lhs, vector<RT,RD> const &rhs)
		{
			lhs.m_data[0]=static_cast<T>(rhs.m_data[0]);
			lhs.m_data[1]=RD>1?static_cast<T>(rhs.m_data[1]):constant::one<T>();
			return lhs;
		}

		template < typename RT >
		static inline vector<T,2>& plus_assign(vector<T,2> &lhs, vector<RT,2> const &rhs)
		{
			lhs.m_data[0]+=static_cast<T>(rhs.m_data[0]);
			lhs.m_data[1]+=static_cast<T>(rhs.m_data[1]);
			return lhs;
		}

		template < typename RT >
		static inline vector<T,2>& minus_assign(vector<T,2> &lhs, vector<RT,2> const &rhs)
		{
			lhs.m_data[0]-=static_cast<T>(rhs.m_data[0]);
			lhs.m_data[1]-=static_cast<T>(rhs.m_data[1]);
			return lhs;
		}

		template < typename RT >
		static inline vector<T,2>& vector_multiply_assign(vector<T,2> &lhs, vector<RT,2> const &rhs)
		{
			lhs.m_data[0]*=static_cast<T>(rhs.m_data[0]);
			lhs.m_data[1]*=static_cast<T>(rhs.m_data[1]);
			return lhs;
		}

		template < typename RT >
		static inline vector<T,2>& vector_divide_assign(vector<T,2> &lhs, vector<RT,2> const &rhs)
		{
			lhs.m_data[0]/=static_cast<T>(rhs.m_data[0]);
			lhs.m_data[1]/=static_cast<T>(rhs.m_data[1]);
			return lhs;
		}

		template < typename RT >
		static inline vector<T,2>& scalar_multiply_assign(vector<T,2> &lhs, RT const &rhs)
		{
			lhs.m_data[0]*=static_cast<T>(rhs);
			lhs.m_data[1]*=static_cast<T>(rhs);
			return lhs;
		}
		
		template < typename RT >
		static inline vector<T,2>& scalar_divide_assign(vector<T,2> &lhs, RT const &rhs)
		{
			lhs.m_data[0]/=static_cast<T>(rhs);
			lhs.m_data[1]/=static_cast<T>(rhs);
			return lhs;
		}

		template < typename RT >
		static inline T dot_product(vector<T,2> const &lhs, vector<RT,2> const &rhs)
		{
			return lhs.m_data[0]*static_cast<T>(rhs.m_data[0])+
				lhs.m_data[1]*static_cast<T>(rhs.m_data[1]);
		}

		typedef T cross_product_result_t;

		template < typename RT >
		static inline T cross_product(vector<T,2> const &lhs, vector<RT,2> const &rhs)
		{
			return lhs.m_data[0]*static_cast<T>(rhs.m_data[1])-lhs.m_data[1]*static_cast<T>(rhs.m_data[0]);
		}

		static inline T norm_1(vector<T,2> const &vec)
		{
			return std::abs(vec.m_data[0])+std::abs(vec.m_data[1]);
		}

		static inline T norm_2(vector<T,2> const &vec)
		{
			return std::sqrt(dot_product(vec,vec));
		}
	};


	template < typename T >
	struct vector_ops<T,3>
	{
		static inline vector<T,3> const& zero()
		{
			static T const _zero[3]={ constant::zero<T>(), constant::zero<T>(), constant::zero<T>() };
			return *(reinterpret_cast<vector<T,3> const *>(&_zero[0]));
		}

		template < boost::uint32_t A >
		static inline vector<T,3> const& unit()
		{
			BOOST_STATIC_ASSERT(A<3);
			static T const _unit[3]=
			{
				A==0?constant::one<T>():constant::zero<T>(),
				A==1?constant::one<T>():constant::zero<T>(),
				A==2?constant::one<T>():constant::zero<T>()
			};
			return *(reinterpret_cast<vector<T,3> const *>(&_unit[0]));
		}

		template < typename RT >
		static inline vector<T,3>& assign(vector<T,3> &lhs, vector<RT,3> const &rhs)
		{
			lhs.m_data[0]=static_cast<T>(rhs.m_data[0]);
			lhs.m_data[1]=static_cast<T>(rhs.m_data[1]);
			lhs.m_data[2]=static_cast<T>(rhs.m_data[2]);
			return lhs;
		}

		template < typename RT, boost::uint32_t RD >
		static inline vector<T,3>& assign(vector<T,3> &lhs, vector<RT,RD> const &rhs)
		{
			lhs.m_data[0]=static_cast<T>(rhs.m_data[0]);
			lhs.m_data[1]=RD>1?static_cast<T>(rhs.m_data[1]):constant::zero<T>();
			lhs.m_data[2]=RD>2?static_cast<T>(rhs.m_data[2]):constant::one<T>();
			return lhs;
		}

		template < typename RT >
		static inline vector<T,3>& plus_assign(vector<T,3> &lhs, vector<RT,3> const &rhs)
		{
			lhs.m_data[0]+=static_cast<T>(rhs.m_data[0]);
			lhs.m_data[1]+=static_cast<T>(rhs.m_data[1]);
			lhs.m_data[2]+=static_cast<T>(rhs.m_data[2]);
			return lhs;
		}

		template < typename RT >
		static inline vector<T,3>& minus_assign(vector<T,3> &lhs, vector<RT,3> const &rhs)
		{
			lhs.m_data[0]-=static_cast<T>(rhs.m_data[0]);
			lhs.m_data[1]-=static_cast<T>(rhs.m_data[1]);
			lhs.m_data[2]-=static_cast<T>(rhs.m_data[2]);
			return lhs;
		}

		template < typename RT >
		static inline vector<T,3>& vector_multiply_assign(vector<T,3> &lhs, vector<RT,3> const &rhs)
		{
			lhs.m_data[0]*=static_cast<T>(rhs.m_data[0]);
			lhs.m_data[1]*=static_cast<T>(rhs.m_data[1]);
			lhs.m_data[2]*=static_cast<T>(rhs.m_data[2]);
			return lhs;
		}

		template < typename RT >
		static inline vector<T,3>& vector_divide_assign(vector<T,3> &lhs, vector<RT,3> const &rhs)
		{
			lhs.m_data[0]/=static_cast<T>(rhs.m_data[0]);
			lhs.m_data[1]/=static_cast<T>(rhs.m_data[1]);
			lhs.m_data[2]/=static_cast<T>(rhs.m_data[2]);
			return lhs;
		}

		template < typename RT >
		static inline vector<T,3>& scalar_multiply_assign(vector<T,3> &lhs, RT const &rhs)
		{
			lhs.m_data[0]*=static_cast<T>(rhs);
			lhs.m_data[1]*=static_cast<T>(rhs);
			lhs.m_data[2]*=static_cast<T>(rhs);
			return lhs;
		}

		template < typename RT >
		static inline vector<T,3>& scalar_divide_assign(vector<T,3> &lhs, RT const &rhs)
		{
			lhs.m_data[0]/=static_cast<T>(rhs);
			lhs.m_data[1]/=static_cast<T>(rhs);
			lhs.m_data[2]/=static_cast<T>(rhs);
			return lhs;
		}

		template < typename RT >
		static inline T dot_product(vector<T,3> const &lhs, vector<RT,3> const &rhs)
		{
			return lhs.m_data[0]*static_cast<T>(rhs.m_data[0])+
				lhs.m_data[1]*static_cast<T>(rhs.m_data[1])+
				lhs.m_data[2]*static_cast<T>(rhs.m_data[2]);
		}

		typedef vector<T,3> cross_product_result_t;

		template < typename RT >
		static inline cross_product_result_t cross_product(vector<T,3> const &lhs, vector<RT,3> const & rhs)
		{
			T res[3]=
			{
				lhs.m_data[1]*static_cast<T>(rhs.m_data[2])-static_cast<T>(rhs.m_data[1])*lhs.m_data[2],
				lhs.m_data[2]*static_cast<T>(rhs.m_data[0])-static_cast<T>(rhs.m_data[2])*lhs.m_data[0],
				lhs.m_data[0]*static_cast<T>(rhs.m_data[1])-static_cast<T>(rhs.m_data[0])*lhs.m_data[1]
			};
			return vector<T,3>(res);
		}

		static inline T norm_1(vector<T,3> const &vec)
		{
			return std::abs(vec.m_data[0])+std::abs(vec.m_data[1]+std::abs(vec.m_data[2]));
		}

		static inline T norm_2(vector<T,3> const &vec)
		{
			return std::sqrt(dot_product(vec,vec));
		}
	};

} //namespace math
} //namesapce baldzarika

#endif //BALDZARIKA_MATH_VECTOR_OPS_H
