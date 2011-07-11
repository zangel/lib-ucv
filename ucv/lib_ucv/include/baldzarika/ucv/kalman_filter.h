#ifndef BALDZARIKA_UCV_KALMAN_FILTER_H
#define BALDZARIKA_UCV_KALMAN_FILTER_H

namespace baldzarika { namespace ucv {

	template < typename T, boost::uint32_t DP, boost::uint32_t MP >
	class kalman_filter
	{
	public:

		typedef math::matrix<T,DP,DP> transition_matrix_t;
		typedef math::matrix<T,MP,DP> measurement_matrix_t;
		typedef math::matrix<T,DP,DP> process_noise_cov_t;
		typedef math::matrix<T,MP,MP> measurement_noise_cov_t;
		typedef math::matrix<T,DP,DP> error_cov_post_t;
		typedef math::matrix<T,DP,1> state_post_t;

		typedef math::matrix<T,MP,1> measurement_t;

		
		
		kalman_filter()
			: m_state_pre(math::matrix<T,DP,1>::zero())
			, m_state_post(math::matrix<T,DP,1>::zero())
			, m_transition_matrix(math::matrix<T,DP,DP>::identity())
			, m_measurement_matrix(math::matrix<T,MP,DP>::zero())
			, m_process_noise_cov(math::matrix<T,DP,DP>::identity())
			, m_measurement_noise_cov(math::matrix<T,MP,MP>::identity())
			, m_error_cov_pre(math::matrix<T,DP,DP>::zero())
			, m_gain(math::matrix<T,DP,MP>::zero())
			, m_error_cov_post(math::matrix<T,DP,DP>::zero())
		{
		}

		inline void correct(measurement_t const &measurement)
		{
			m_temp2=m_measurement_matrix*m_error_cov_pre;
			m_temp3=m_temp2*m_measurement_matrix.transposed()+m_measurement_noise_cov;
			m_temp4=m_temp3.inverted()*m_temp2;
			m_gain=m_temp4.transposed();
			m_temp5=measurement-m_measurement_matrix*m_state_pre;
			m_state_post=m_state_pre+m_gain*m_temp5;
			m_error_cov_post=m_error_cov_pre-m_gain*m_temp2;

		}

		void predict()
		{
			m_state_pre=m_transition_matrix*m_state_post;
			m_temp1=m_transition_matrix*m_error_cov_post;
			m_error_cov_pre=m_temp1*m_transition_matrix.transposed()+m_process_noise_cov;
		}

	public:
		math::matrix<T,DP,1>		m_state_pre;
		math::matrix<T,DP,1>		m_state_post;
		math::matrix<T,DP,DP>		m_transition_matrix;
		math::matrix<T,MP,DP>		m_measurement_matrix;
		math::matrix<T,DP,DP>		m_process_noise_cov;
		math::matrix<T,MP,MP>		m_measurement_noise_cov;
		math::matrix<T,DP,DP>		m_error_cov_pre;
		math::matrix<T,DP,DP>		m_error_cov_post;

	private:
		math::matrix<T,DP,MP>		m_gain;
		math::matrix<T,DP,DP>		m_temp1;
		math::matrix<T,MP,DP>		m_temp2;
		math::matrix<T,MP,MP>		m_temp3;
		math::matrix<T,MP,DP>		m_temp4;
		math::matrix<T,MP,1>		m_temp5;
	};

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_KALMAN_FILTER_H