#ifndef BALDZARIKA_UCV_HOMOGRAPHY_H
#define BALDZARIKA_UCV_HOMOGRAPHY_H

#include <baldzarika/ucv/lapack/jacobi.h>

namespace baldzarika { namespace ucv {

	namespace detail {

		template < typename T, typename DT>
		class homography_estimator
		{
		public:
			static T const CONFIDENCE;
			static T const OUTLIER_RATIO;
			static std::size_t const MAX_ITERS;

			typedef feature_point<T,DT> feature_point_t;
			typedef typename feature_point_t::point2_t point2_t;

			typedef typename feature_point_t::value_type value_t;
			typedef std::vector<feature_point_t> feature_points_t;
			typedef std::pair<std::size_t, std::size_t> match_t;
			typedef std::vector<match_t> matches_t;
			typedef nublas::matrix<value_t, nublas::row_major> matrix_t;

			typedef boost::uniform_int<boost::uint32_t> uint32_dist_t;
			typedef boost::variate_generator<boost::mt19937&, uint32_dist_t> uint32_rng_t;
						
			homography_estimator(feature_points_t const &ops, feature_points_t const &ips, matches_t const &oim)
				: m_ops(ops)
				, m_ips(ips)
				, m_oim(oim)
				, m_rng()
				, m_rng_gen(m_rng, uint32_dist_t(0,m_oim.size()-1))
			{
			}

			template < typename AS >
			bool operator()(nublas::matrix< value_t, nublas::row_major, AS> &hm, value_t const &ic)
			{
				if(m_oim.size()<4)
					return false;

				value_t min_median=std::numeric_limits<value_t>::max();
				matrix_t models(3,3);
				
				std::vector<value_t> err(m_oim.size());
				std::vector<point2_t> ms1(4), ms2(4);


				std::size_t n_iters=m_oim.size()==4?
					1:
					round<std::size_t>(
						log(detail::constants::one<T>()-CONFIDENCE)/
						log(detail::constants::one<T>()-pow(detail::constants::one<T>()-OUTLIER_RATIO, value_t(4)))
					);

				if(m_oim.size()==4)
				{
					
					for(std::size_t m=0;m<m_oim.size();++m)
					{
						ms1[m]=m_ops[m_oim[m].first];
						ms2[m]=m_ips[m_oim[m].second];
					}
				}
				
				n_iters=std::min<std::size_t>( n_iters, MAX_ITERS);

				for(std::size_t it=0;it<n_iters;++it)
				{
					if(m_oim.size()>4)
					{
						if(!get_sub_set(ms1,ms2, 300))
						{
							if(!it) return false;
							break;
						}
					}
					if(!run_kernel(ms1, ms2, models))
						continue;
					compute_reproj_error(models, err);
					std::sort(err.begin(), err.end(), std::less<value_t>());
					value_t median=m_oim.size()%2!=0?
						err[m_oim.size()/2]:
					(err[m_oim.size()/2-1]+err[m_oim.size()/2])*detail::constants::half<value_t>();
					if(median<min_median)
					{
						min_median=median;
						hm=models;
					}
				}

				if(min_median<std::numeric_limits<value_t>::max())
				{
					static value_t const _sigma_c=2.5*1.4826;
					static value_t const _sigma_max=0.001;
					
					value_t sigma=_sigma_c*
						(detail::constants::one<value_t>()+
							value_t(5)/
							std::max(detail::constants::one<value_t>(), value_t(m_oim.size()-4))
						)*
						sqrt(min_median);
					sigma=std::max(sigma, _sigma_max);

					return inliers_count(hm, sigma)>=ucv::round<boost::uint32_t>(value_t(m_oim.size())*ic);
				}
				return false;
			}

		protected:
			bool get_sub_set(std::vector<point2_t> &ms1, std::vector<point2_t> &ms2, std::size_t ma)
			{
				boost::uint32_t idx[4];
				std::size_t it=0, i=0;
				for(; it<ma;++it)
				{
					for(i=0;i<4 && it<ma;)
					{
						boost::uint32_t idx_i=idx[i]=m_rng_gen();
						std::size_t j=0;
						for(;j<i;++j)
							if(idx_i==idx[j])
								break;
						if(j<i)
							continue;
						
						ms1[i]=m_ops[m_oim[idx_i].first];
						ms2[i]=m_ips[m_oim[idx_i].second];
						i++;
					}
					if(i==4 && (!check_sub_set(ms1,i) || !check_sub_set(ms2,i)))
						continue;
					break;
				}
				return i==4 && it<ma;
			}

			bool check_sub_set(std::vector<point2_t> const &ms, std::size_t n)
			{
				std::size_t i=0;
				for(;i<=(n-1);++i)
				{
					std::size_t j=0;
					for(;j<i;j++)
					{
						value_t dx1=ms[j].x-ms[i].x;
						value_t dy1=ms[j].y-ms[i].y;
						std::size_t k=0;
						for(;k<j;k++)
						{
							value_t dx2=ms[k].x-ms[i].x;
							value_t dy2=ms[k].y-ms[i].y;
							if(fabs(dx2*dy1-dy2*dx1)<=std::numeric_limits<value_t>::epsilon()*(fabs(dx1)+fabs(dy1)+fabs(dx2)+fabs(dy2)))
								break;
						}
						if(k<j)
							break;
					}
					if(j<i)
						break;
				}
				return i>=n-1;
			}

			bool run_kernel(std::vector<point2_t> const &ms1, std::vector<point2_t> const &ms2, matrix_t &models)
			{
				BOOST_ASSERT(ms1.size()==ms2.size());

				value_t count_(ms1.size());
				value_t LtL[9][9], W[9][9], V[9][9];
				typename feature_point_t::point2_t cM(0,0), cm(0,0), sM(0,0), sm(0,0);
				
				for(std::size_t i=0;i<ms1.size();++i)
				{
					cm.x+=ms2[i].x; cm.y+=ms2[i].y;
					cM.x+=ms1[i].x; cM.y+=ms1[i].y;
				}

				cm.x/=count_; cm.y/=count_;
				cM.x/=count_; cM.y/=count_;
				
				for(std::size_t i=0;i<ms1.size();++i)
				{
					sm.x+=fabs(ms2[i].x-cm.x);
					sm.y+=fabs(ms2[i].y-cm.y);
					sM.x+=fabs(ms1[i].x-cM.x);
					sM.y+=fabs(ms1[i].y-cM.y);
				}

				if(	fabs(sm.x)<std::numeric_limits<value_t>::epsilon() || fabs(sm.y)<std::numeric_limits<value_t>::epsilon() ||
					fabs(sM.x)<std::numeric_limits<value_t>::epsilon() || fabs(sM.y)<std::numeric_limits<value_t>::epsilon() )
					return false;
				sm.x=count_/sm.x; sm.y=count_/sm.y;
				sM.x=count_/sM.x; sM.y=count_/sM.y;
				
				value_t invHnorm[9]=
				{
					detail::constants::one<value_t>()/sm.x,	detail::constants::zero<value_t>(),		cm.x,
					detail::constants::zero<value_t>(),		detail::constants::one<value_t>()/sm.y,	cm.y,
					detail::constants::zero<value_t>(),		detail::constants::zero<value_t>(),		detail::constants::one<value_t>()
				};
				
				value_t Hnorm2[9]=
				{
					sM.x,								detail::constants::zero<value_t>(),		-cM.x*sM.x,
					detail::constants::zero<value_t>(),	sM.y,									-cM.y*sM.y,
					detail::constants::zero<value_t>(), detail::constants::zero<value_t>(),		detail::constants::one<value_t>()
				};

				for(std::size_t i=0;i<9;++i)
					for(std::size_t j=0;j<9;++j)
						LtL[i][j]=detail::constants::zero<value_t>();
				
				for(std::size_t i=0;i<ms1.size();++i)
				{
					value_t x=(ms2[i].x-cm.x)*sm.x, y=(ms2[i].y-cm.y)*sm.y;
					value_t X=(ms1[i].x-cM.x)*sM.x, Y=(ms1[i].y-cM.y)*sM.y;
					value_t Lx[9]=
					{
						X,									Y,									detail::constants::one<value_t>(),
						detail::constants::zero<value_t>(), detail::constants::zero<value_t>(),	detail::constants::zero<value_t>(),
						-x*X,								-x*Y,								-x
					};

					value_t Ly[9]=
					{
						detail::constants::zero<value_t>(),	detail::constants::zero<value_t>(), detail::constants::zero<value_t>(),
						X,									Y,									detail::constants::one<value_t>(),
						-y*X,								-y*Y,								-y
					};

					for(std::size_t j=0;j<9;++j)
					{
						for(std::size_t k=j;k<9;++k)
						{
							LtL[j][k]+=Lx[j]*Lx[k]+Ly[j]*Ly[k];
						}
					}
				}

				for(std::size_t i=0;i<9;++i)
				{
					for(std::size_t j=0;j<i;++j)
					{
						LtL[i][j]=LtL[j][i];
					}
				}

				{
					typedef nublas::matrix<value_t, nublas::row_major, nublas::shallow_array_adaptor<value_t> > shalow_adapted_mat_t;

					shalow_adapted_mat_t LtL_(9, 9, nublas::shallow_array_adaptor<value_t>(9*9, &LtL[0][0]) );
					shalow_adapted_mat_t W_(9, 9, nublas::shallow_array_adaptor<value_t>(9*9, &W[0][0]) );
					shalow_adapted_mat_t V_(9, 9, nublas::shallow_array_adaptor<value_t>(9*9, &V[0][0]) );


					lapack::jacobi(LtL_, W_, V_);

					shalow_adapted_mat_t _H0(3,3, nublas::shallow_array_adaptor<value_t>(3*3, V[8]));
					shalow_adapted_mat_t _Htemp(3,3, nublas::shallow_array_adaptor<value_t>(3*3, V[7]));
					shalow_adapted_mat_t _invHnorm(3,3, nublas::shallow_array_adaptor<value_t>(3*3, invHnorm));
					shalow_adapted_mat_t _Hnorm2(3,3, nublas::shallow_array_adaptor<value_t>(3*3, Hnorm2));
				
					_Htemp=nublas::prod(_invHnorm,_H0);
					_H0=nublas::prod(_Htemp, _Hnorm2);
					//float d_H0=static_cast<float>(_H0(2,2));
					models=_H0*(detail::constants::one<value_t>()/_H0(2,2));
				}
				return true;
			}

			void compute_reproj_error(matrix_t &models, std::vector<value_t> &err)
			{
				typename matrix_t::array_type const &H=models.data();
				for(std::size_t i=0;i<m_oim.size();++i)
				{
					value_t ww=detail::constants::one<value_t>()/(H[6]*m_ops[m_oim[i].first].x+H[7]*m_ops[m_oim[i].first].y+detail::constants::one<value_t>());
					value_t dx=(H[0]*m_ops[m_oim[i].first].x+H[1]*m_ops[m_oim[i].first].y+H[2])*ww-m_ips[m_oim[i].second].x;
					value_t dy=(H[3]*m_ops[m_oim[i].first].x+H[4]*m_ops[m_oim[i].first].y+H[5])*ww-m_ips[m_oim[i].second].y;
					err[i]=(dx*dx + dy*dy);
				}
			}

			boost::uint32_t inliers_count(matrix_t &models, value_t const &t)
			{
				value_t treshold=t*t;

				std::vector<value_t> err(m_oim.size());
				compute_reproj_error(models, err);

				boost::uint32_t n_inliers=0;
				for(std::size_t i=0;i<m_oim.size();++i)
					n_inliers+=err[i]<treshold?1:0;
				return n_inliers;
			}

		private:
			feature_points_t const	&m_ops;
			feature_points_t const	&m_ips;
			matches_t const			&m_oim;
			boost::mt19937			m_rng;
			uint32_rng_t			m_rng_gen;
			
		};

		template < typename T, typename DT >
		T const homography_estimator<T,DT>::CONFIDENCE=0.995;
		template < typename T, typename DT >
		T const homography_estimator<T,DT>::OUTLIER_RATIO=0.45;
		template < typename T, typename DT >
		std::size_t const homography_estimator<T,DT>::MAX_ITERS=2000;

	}

	template < typename T, typename DT, typename AS>
	bool find_homography(
		std::vector< feature_point<T,DT> > const &ops,
		std::vector< feature_point<T,DT> > const &ips,
		std::vector< std::pair<std::size_t, std::size_t> > const &oim,
		nublas::matrix< typename feature_point<T,DT>::value_type, nublas::row_major, AS > &hm,
		T const &ic=0.5
	)
	{
		detail::homography_estimator<T,DT> hest(ops, ips, oim);
		return hest(hm, ic);
	}

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_HOMOGRAPHY_H
