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
			typedef nublas::matrix< T > matrix_t;
			
			homography_estimator(feature_points_t const &ops, feature_points_t const &ips, matches_t const &oim, matrix_t &hm)
				: m_ops(ops)
				, m_ips(ips)
				, m_oim(oim)
				, m_hm(hm)
			{
			}

			bool operator()()
			{
				if(m_oim.size()<=4)
					return false;
				matrix_t models(3,3);
				
				std::vector<value_t> err(m_oim.size());
				std::vector<point2_t> ms1(m_oim.size()), ms2(m_oim.size());
				for(std::size_t m=0;m<m_oim.size();++m)
				{
					ms1[m]=m_ops[m_oim[m].first];
					ms2[m]=m_ips[m_oim[m].second];
				}

				std::size_t n_iters=round<std::size_t>(
					log(detail::constants::one<T>()-CONFIDENCE)/
					log(detail::constants::one<T>()-pow(detail::constants::one<T>()-OUTLIER_RATIO, T(m_oim.size())))
				);
				n_iters=std::min<std::size_t>( std::max<std::size_t>(n_iters, 3), MAX_ITERS);

				for(std::size_t it=0;it<n_iters;++it)
				{
					if(!run_kernel(ms1, ms2, models))
						continue;
					/*
					for( i = 0; i < nmodels; i++ )
					{
						CvMat model_i;
						cvGetRows( models, &model_i, i*modelSize.height, (i+1)*modelSize.height );
						computeReprojError( m1, m2, &model_i, err );
						icvSortDistances( err->data.i, count, 0 );

						double median = count % 2 != 0 ?
							err->data.fl[count/2] : (err->data.fl[count/2-1] + err->data.fl[count/2])*0.5;

						if( median < minMedian )
						{
							minMedian = median;
							cvCopy( &model_i, model );
						}
					}
					*/
				}

				/*
				if( minMedian < DBL_MAX )
				{
					sigma = 2.5*1.4826*(1 + 5./(count - modelPoints))*sqrt(minMedian);
					sigma = MAX( sigma, 0.001 );

					count = findInliers( m1, m2, model, err, mask, sigma );
					result = count >= modelPoints;
				}
				*/

				return true;
			}

		protected:
			bool run_kernel(std::vector<point2_t> const &ms1, std::vector<point2_t> const &ms2, matrix_t &models)
			{
				BOOST_ASSERT(ms1.size()==ms2.size());

				value_t count_(ms1.size());
				value_t LtL[9][9], W[9][9], V[9][9];
				typename feature_point_t::point2_t cM(0,0), cm(0,0), sM(0,0), sm(0,0);
				
				for(std::size_t i=0;i<ms1.size();++i)
				{
					cm.x+=ms1[i].x; cm.y+=ms1[i].y;
					cM.x+=ms2[i].x; cM.y+=ms2[i].y;
				}

				cm.x/=count_; cm.y/=count_;
				cM.x/=count_; cM.y/=count_;
				
				for(std::size_t i=0;i<ms1.size();++i)
				{
					sm.x+=fabs(ms1[i].x-cm.x);
					sm.y+=fabs(ms1[i].y-cm.y);
					sM.x+=fabs(ms2[i].x-cM.x);
					sM.y+=fabs(ms2[i].y-cM.y);
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
					value_t x=(ms1[i].x-cm.x)*sm.x, y=(ms1[i].y-cm.y)*sm.y;
					value_t X=(ms2[i].x-cM.x)*sM.x, Y=(ms2[i].y-cM.y)*sM.y;
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

				typedef nublas::matrix<value_t, nublas::row_major, nublas::shallow_array_adaptor<value_t> > shalow_adapted_mat_t;
				
				lapack::jacobi(
					shalow_adapted_mat_t(9, 9, nublas::shallow_array_adaptor<value_t>(9*9, &LtL[0][0]) ),
					shalow_adapted_mat_t(9, 9, nublas::shallow_array_adaptor<value_t>(9*9, &V[0][0]) ),
					shalow_adapted_mat_t(9, 9, nublas::shallow_array_adaptor<value_t>(9*9, &W[0][0]) )
				);

				shalow_adapted_mat_t _H0(3,3, nublas::shallow_array_adaptor<value_t>(3*3, V[8]));
				shalow_adapted_mat_t _Htemp(3,3, nublas::shallow_array_adaptor<value_t>(3*3, V[7]));
				shalow_adapted_mat_t _invHnorm(3,3, nublas::shallow_array_adaptor<value_t>(3*3, invHnorm));
				shalow_adapted_mat_t _Hnorm2(3,3, nublas::shallow_array_adaptor<value_t>(3*3, Hnorm2));
				
				_Htemp=nublas::prod(_invHnorm,_H0);
				_H0=nublas::prod(_Htemp, _Hnorm2);
				float d_H0=static_cast<float>(_H0(2,2));
				models=_H0*(detail::constants::one<value_t>()/_H0(2,2));
				return true;
			}

		private:
			feature_points_t const	&m_ops;
			feature_points_t const	&m_ips;
			matches_t const			&m_oim;
			matrix_t				&m_hm;
		};

		template < typename T, typename DT >
		T const homography_estimator<T,DT>::CONFIDENCE=0.995;
		template < typename T, typename DT >
		T const homography_estimator<T,DT>::OUTLIER_RATIO=0.45;
		template < typename T, typename DT >
		std::size_t const homography_estimator<T,DT>::MAX_ITERS=2000;

	}

	template < typename T, typename DT >
	bool find_homography(
		std::vector< feature_point<T,DT> > const &ops,
		std::vector< feature_point<T,DT> > const &ips,
		std::vector< std::pair<std::size_t, std::size_t> > const &oim,
		nublas::matrix< typename feature_point<T,DT>::value_type > &hm
	)
	{
		detail::homography_estimator<T,DT> hest(ops, ips, oim, hm);
		return hest();
	}

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_HOMOGRAPHY_H
