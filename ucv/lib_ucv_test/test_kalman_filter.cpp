#include <boost/test/unit_test.hpp>
#include <baldzarika/ucv/config.h>
#include <baldzarika/ucv/kalman_filter.h>



BOOST_AUTO_TEST_CASE( kalman_filter_test )
{
	namespace ucv=baldzarika::ucv;
	namespace math=baldzarika::math;

	typedef ucv::kalman_filter<float,2,1> kalman_filter_t;

	float const pnc=1.0e-4f;
	float const mnc=1.0e-2f;


	kalman_filter_t kf;

	kf.m_transition_matrix=kalman_filter_t::transition_matrix_t::identity();
	kf.m_transition_matrix(0,1)=math::constant::one<float>();
	kf.m_measurement_matrix=kalman_filter_t::measurement_matrix_t::zero();
	kf.m_measurement_matrix(0,1)=math::constant::one<float>();

	kf.m_process_noise_cov=kalman_filter_t::process_noise_cov_t::identity()*pnc;
	kf.m_measurement_noise_cov=kalman_filter_t::measurement_noise_cov_t::identity()*mnc;

	kf.m_error_cov_post=kalman_filter_t::error_cov_post_t::identity()*1.0e+1f;
	kf.m_state_post=kalman_filter_t::state_post_t::identity()*0.0f;

		

	float velocity(1.0f);
	float predicted_pos=0.0f;
	float predicted_velocity=0.0f;
	for(int x=0;x<30;++x)
	{
		kf.predict();

		predicted_pos=kf.m_state_pre(0,0);
		predicted_velocity=kf.m_state_pre(1,0);
		
		kalman_filter_t::measurement_t measurement;
		measurement(0,0)=velocity;
		kf.correct(measurement);
		if(x>1)
			velocity=0.0f;
	}
}