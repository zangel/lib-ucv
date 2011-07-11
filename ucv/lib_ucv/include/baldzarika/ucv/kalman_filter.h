#ifndef BALDZARIKA_UCV_KALMAN_FILTER_H
#define BALDZARIKA_UCV_KALMAN_FILTER_H

namespace baldzarika { namespace ucv {

	template < typename T, boost::uint32_t DP, boost::uint32_t MP >
	struct kalman_filter
	{
		kalman_filter()
		{
		}

		math::matrix<T,DP,1>		m_state_pre;
		math::matrix<T,DP,1>		m_state_post;
		math::matrix<T,DP,DP>		m_transition_matrix;
		math::matrix<T,MP,DP>		m_measurement_matrix;
		math::matrix<T,DP,DP>		m_process_noise_cov;
		math::matrix<T,MP,MP>		m_measurement_noise_cov;
		math::matrix<T,DP,DP>		m_error_cov_pre;
		math::matrix<T,DP,MP>		m_gain;
		math::matrix<T,DP,DP>		m_error_cov_post;


		math::matrix<T,DP,DP>		m_temp1;
		math::matrix<T,MP,DP>		m_temp2;
		math::matrix<T,MP,MP>		m_temp3;
		math::matrix<T,MP,DP>		m_temp4;
		math::matrix<T,MP,1>		m_temp5;
	};

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_KALMAN_FILTER_H