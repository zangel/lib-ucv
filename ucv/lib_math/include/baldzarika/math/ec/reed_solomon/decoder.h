#ifndef BALDZARIKA_MATH_EC_REED_SOLOMON_DECODER_H
#define BALDZARIKA_MATH_EC_REED_SOLOMON_DECODER_H

#include <baldzarika/math/ec/galois/field_symbol.h>
#include <baldzarika/math/ec/galois/field.h>
#include <baldzarika/math/ec/galois/field_element.h>
#include <baldzarika/math/ec/galois/field_polynomial.h>
#include <baldzarika/math/ec/reed_solomon/block.h>

namespace baldzarika { namespace math { namespace ec { namespace reed_solomon {

	template < boost::int32_t PWR, boost::int32_t PP, boost::int32_t CL, boost::int32_t FECL >
	class decoder
	{
	public:
		typedef galois::field_symbol_traits<PWR> field_symbol_traits_t;
		typedef typename field_symbol_traits_t::field_symbol_t field_symbol_t;
		typedef galois::field<PWR,PP> field_t;
		typedef galois::field_element<PWR,PP> field_element_t;
		typedef galois::field_polynomial<PWR,PP> field_polynomial_t;
		typedef block<PWR,CL,FECL> block_t;

		decoder()
		{
		}

		bool decode(boost::dynamic_bitset<> const &in, boost::dynamic_bitset<> &out, boost::uint32_t N) const
		{
			if(block_t::num_blocks(in)!=N) return false;
			for(boost::uint32_t i=0;i<N;++i)
			{
				block_t nth_block;
				if(!(nth_block.get(in,i,N) && decode(nth_block) && nth_block.put(out,i)))
					return false;
			}
			return true;
		}

		bool decode(block_t& rcvd_block) const
		{
			field_polynomial_t received(rcvd_block.m_data,true);

			field_polynomial_t syndrome;

			if(compute_syndrom(received,syndrome))
			{
				rcvd_block.m_errors_detected=0;
				rcvd_block.m_errors_corrected=0;
				rcvd_block.m_unrecoverable=false;
				return true;
			}

			field_polynomial_t sigma, omega;

			if(!run_equclidean_algorithm(field_polynomial_t(block_t::FEC_LENGTH,1), syndrome, sigma, omega))
			{
				rcvd_block.m_errors_detected=0;
				rcvd_block.m_errors_corrected=0;
				rcvd_block.m_unrecoverable=true;
				return false;
			}

			std::vector<field_element_t> error_locations;
			if(!find_error_locations(sigma, error_locations))
			{
				rcvd_block.m_errors_detected=error_locations.size();
				rcvd_block.m_errors_corrected=0;
				rcvd_block.m_unrecoverable=true;
				return false;
			}

			std::vector<field_element_t> error_magnitudes;
			if(!find_error_magnitudes(omega,error_locations,error_magnitudes) || error_magnitudes.size()!=error_locations.size())
			{
				rcvd_block.m_errors_detected=error_locations.size();
				rcvd_block.m_errors_corrected=0;
				rcvd_block.m_unrecoverable=true;
				return false;
			}

			if(!correct_errors(rcvd_block, error_locations, error_magnitudes))
			{
				rcvd_block.m_errors_detected=error_locations.size();
				rcvd_block.m_errors_corrected=0;
				rcvd_block.m_unrecoverable=true;
				return false;
			}

			rcvd_block.m_errors_detected=error_locations.size();
			rcvd_block.m_errors_corrected=error_locations.size();
			rcvd_block.m_unrecoverable=false;
			return true;
		}

	private:
		bool compute_syndrom(field_polynomial_t const &received, field_polynomial_t &syndrome) const
		{
			field_symbol_t err_flag=0;

			field_element_t syndome_coefficients[block_t::FEC_LENGTH];

			for(boost::int32_t i=0;i<block_t::FEC_LENGTH;++i)
			{
				syndome_coefficients[i]=received(field_t::get().alpha(i));
				err_flag|=static_cast<field_symbol_t>(syndome_coefficients[i]);
			}
			syndrome=field_polynomial_t(syndome_coefficients);
			return err_flag==0;
		}

		bool run_equclidean_algorithm(field_polynomial_t a, field_polynomial_t b, field_polynomial_t &sigma, field_polynomial_t &omega) const
		{
			if(a.degree()<b.degree())
				std::swap(a,b);

			field_polynomial_t r_last=a;
			field_polynomial_t r=b;
			field_polynomial_t s_last=field_polynomial_t::one();
			field_polynomial_t s=field_polynomial_t::zero();
			field_polynomial_t t_last=field_polynomial_t::zero();
			field_polynomial_t t=field_polynomial_t::one();
			
			while(r.degree()>=block_t::FEC_LENGTH/2)
			{
				field_polynomial_t r_last_last=r_last;
				field_polynomial_t s_last_last=s_last;
				field_polynomial_t t_last_last=t_last;
				r_last=r; s_last=s; t_last=t;
				
				if(r_last==field_polynomial_t::zero())
					return false;
				
				r=r_last_last;
				field_polynomial_t q=field_polynomial_t::zero();
				field_element_t denominator_leading_term=r_last[r_last.degree()];
				field_element_t dlt_inverse=denominator_leading_term.inverse();
				while(r.degree()>=r_last.degree() && r!=field_polynomial_t::zero())
				{
					boost::int32_t degree_diff=r.degree()-r_last.degree();
					field_element_t scale=r[r.degree()]*dlt_inverse;
					if(!scale) break;
					field_polynomial_t degree_diff_scale(degree_diff, scale);
					q+=degree_diff_scale;
					r+=r_last*degree_diff_scale;
				}
				
				s=q*s_last+s_last_last;
				t=q*t_last+t_last_last;
			}

			field_element_t sigma_tilde_at_zero=t[0];
			if(static_cast<field_symbol_t>(sigma_tilde_at_zero)==0)
				return false;
			
			field_element_t inverse=sigma_tilde_at_zero.inverse();
			sigma=t*inverse;
			omega=r*inverse;
			return true;
		}

		bool find_error_locations(field_polynomial_t const &sigma, std::vector<field_element_t> &error_locations) const
		{
			boost::uint32_t const n_errors=sigma.degree();
			if(n_errors==1)
			{
				error_locations.resize(1,sigma[1]);
				return true;
			}
			
			error_locations.resize(n_errors);
			boost::uint32_t e=0;
			for(boost::uint32_t i=1;i<(field_t::FIELD_SIZE+1) && e<n_errors;++i)
			{
				field_element_t ie(i);
				if(static_cast<field_symbol_t>(sigma(ie))==0)
					error_locations[e++]=ie.inverse();
			}
			return (e==n_errors);
		}

		bool find_error_magnitudes(field_polynomial_t const &omega, std::vector<field_element_t> const &error_locations, std::vector<field_element_t> &error_magnitudes) const
		{
			boost::uint32_t const n_errors=error_locations.size();
			error_magnitudes.resize(n_errors);
			for(boost::uint32_t i=0;i<n_errors;++i)
			{
				field_element_t xi_inverse=error_locations[i].inverse();
				field_element_t denominator=field_element_t::one();
				for(boost::uint32_t j=0;j<n_errors;++j)
					if(i!=j)
						denominator=denominator*(field_element_t::one()+error_locations[j]*xi_inverse);
				error_magnitudes[i]=omega(xi_inverse)*denominator.inverse();
			}
			return true;
		}

		bool correct_errors(block_t &rcv_block, std::vector<field_element_t> const &error_locations, std::vector<field_element_t> const &error_magnitudes) const
		{
			boost::uint32_t const n_errors=error_locations.size();
			
			for(boost::uint32_t i=0;i<n_errors;++i)
			{
				boost::int32_t error_position=block_t::CODE_LENGTH-1-error_locations[i].index();
				if(error_position<0)
					return false;
				rcv_block.m_data[error_position]=error_magnitudes[i]+rcv_block.m_data[error_position];
			}
			return true;
		}
	};

} //namespace reed_solomon
} //namespace ec
} //namespace math
} //namespace baldzarika

#endif //BALDZARIKA_MATH_EC_REED_SOLOMON_DECODER_H