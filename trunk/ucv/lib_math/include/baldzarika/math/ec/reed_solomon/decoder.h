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
			: m_x(field_polynomial_t::get_x())
		{
			create_lookup_tables();
		}

		bool decode(boost::dynamic_bitset<> const &in, boost::dynamic_bitset<> &out)
		{
			for(boost::uint32_t i=0;i<block_t::num_blocks(in);++i)
			{
				block_t nth_block;
				if(!(nth_block.get(in,i) && decode(nth_block) && nth_block.put(out,i)))
					return false;
			}
			return true;
		}

		bool decode(block_t& rsblock) const
		{
			std::vector<boost::uint32_t> erasures;
			return decode(rsblock,erasures);
        }

		bool decode(block_t& rsblock, std::vector<boost::uint32_t> const &erasures) const
		{
			if((erasures.size()>block_t::FEC_LENGTH))
			{
				rsblock.m_errors_detected=0;
				rsblock.m_errors_corrected=0;
				rsblock.m_unrecoverable=true;
				return false;
			}

			field_polynomial_t received(block_t::CODE_LENGTH-1);
			load_message(received,rsblock);

            field_polynomial_t syndrome;

            if(compute_syndrome(received,syndrome)==0)
            {
				rsblock.m_errors_detected=0;
				rsblock.m_errors_corrected=0;
				rsblock.m_unrecoverable=false;
				return true;
            }

			std::vector<boost::uint32_t> erasure_locations;
            prepare_erasure_list(erasure_locations,erasures);

            field_polynomial_t gamma;

            compute_gamma(gamma, erasure_locations);

            if(erasures.size()<block_t::FEC_LENGTH)
               modified_berlekamp_massey_algorithm(gamma,syndrome,erasures.size());
            
			std::vector<boost::uint32_t> error_locations;

            find_roots(gamma,error_locations);

            if(error_locations.empty())
            {
				rsblock.m_errors_detected=0;
				rsblock.m_errors_corrected=0;
				rsblock.m_unrecoverable=true;
				return false;
            }
			else
			if(((2*error_locations.size())-erasures.size())>block_t::FEC_LENGTH)
            {
				rsblock.m_errors_detected=error_locations.size();
				rsblock.m_errors_corrected=0;
				rsblock.m_unrecoverable=true;
				return false;
            }
            else
               rsblock.m_errors_detected=error_locations.size();
			return forney_algorithm(error_locations,gamma,syndrome,rsblock);
         }

	protected:

		void create_lookup_tables()
		{
			for(boost::int32_t i=0;i<=static_cast<boost::int32_t>(block_t::CODE_LENGTH);++i)
				m_root_exponent_table[i]=field_t::get().exp(field_t::get().alpha(block_t::CODE_LENGTH-i),1);

			for(boost::int32_t i=0;i<static_cast<boost::int32_t>(block_t::FEC_LENGTH);++i)
				m_syndrome_exponent_table[i]=field_t::get().alpha(i);
			
			for(boost::int32_t i=0;i<static_cast<boost::int32_t>(field_t::FIELD_SIZE+1);++i)
				m_gamma_table[i]=(1 + (m_x*field_element_t(field_t::get().alpha(i))));
		}

		void load_message(field_polynomial_t& received, block_t const &rsblock) const
		{
			for(boost::uint32_t i=0;i<block_t::CODE_LENGTH;++i)
				received[block_t::CODE_LENGTH-1-i]=rsblock[i];
		}
		
		boost::int32_t compute_syndrome(field_polynomial_t const &received, field_polynomial_t &syndrome) const
		{
			boost::int32_t error_flag=0;
			syndrome=field_polynomial_t(block_t::FEC_LENGTH-1);
			for(boost::uint32_t i=0;i<block_t::FEC_LENGTH;++i)
			{
				syndrome[i]=received(m_syndrome_exponent_table[i]);
				error_flag|=syndrome[i].poly();
			}
			return error_flag;
		}

		void prepare_erasure_list(std::vector<boost::uint32_t> &erasure_locations, std::vector<boost::uint32_t> const &erasures) const
		{
			erasure_locations.resize(erasures.size());
			for(boost::uint32_t i=0;i<erasures.size();++i)
				erasure_locations[i]=(block_t::CODE_LENGTH-1-erasures[i]);
		}

		void compute_gamma(field_polynomial_t& gamma, std::vector<boost::uint32_t> const &erasure_locations) const
		{
			gamma=field_element_t(1);
			for(boost::uint32_t i=0;i<erasure_locations.size();++i)
				gamma*=m_gamma_table[erasure_locations[i]];
		}

		void modified_berlekamp_massey_algorithm(field_polynomial_t &lambda, field_polynomial_t const &syndrome, boost::uint32_t erasure_count) const
		{
			boost::int32_t i=-1;
            boost::uint32_t l=erasure_count;
			
			field_element_t discrepancy(0);
            field_polynomial_t previous_lambda=lambda<<1;
			
			for(boost::uint32_t round=erasure_count;round<block_t::FEC_LENGTH;++round)
            {
				compute_discrepancy(discrepancy,lambda,syndrome,l,round);
				if(discrepancy!=0)
				{
					field_polynomial_t tau=lambda-discrepancy*previous_lambda;
					if(static_cast<boost::int32_t>(l)<static_cast<boost::int32_t>(round)-i)
					{
						boost::uint32_t tmp=round-i;
						i=static_cast<boost::int32_t>(round-l);
						l=tmp;
						previous_lambda=lambda/discrepancy;
					}
					lambda=tau;
				}
				previous_lambda<<=1;
			}
		}

		void compute_discrepancy(field_element_t &discrepancy, field_polynomial_t const &lambda, field_polynomial_t const &syndrome, boost::uint32_t l, boost::uint32_t round) const
		{
			boost::int32_t upper_bound=std::min(static_cast<boost::int32_t>(l),lambda.degree());
			discrepancy=0;
			for(boost::int32_t i=0;i<=upper_bound;++i)
				discrepancy+=lambda[i]*syndrome[round-i];
		}


		void find_roots(field_polynomial_t const &poly, std::vector<boost::uint32_t> &root_list) const
		{
			root_list.reserve(block_t::FEC_LENGTH<<1);
			root_list.resize(0);
			boost::uint32_t polynomial_degree=poly.degree();
			boost::uint32_t root_list_size=0;
            for(boost::int32_t i=1;i<=static_cast<boost::int32_t>(block_t::CODE_LENGTH);++i)
			{
				if(0==poly(field_t::get().alpha(i)).poly())
				{
					root_list.push_back(i);
					root_list_size++;
					if(root_list_size==polynomial_degree)
						break;
				}
			}
		}

		bool forney_algorithm(std::vector<boost::uint32_t> const &error_locations, field_polynomial_t const &lambda, field_polynomial_t const &syndrome, block_t &rsblock) const
		{
			field_polynomial_t omega=(lambda*syndrome)%block_t::FEC_LENGTH;
			field_polynomial_t lambda_derivative=lambda.derivative();

            rsblock.m_errors_corrected=0;
			
			for(boost::uint32_t  i=0;i<error_locations.size();++i)
			{
				boost::int32_t error_location=error_locations[i];
				field_symbol_t alpha_inverse=field_t::get().alpha(error_location);
				field_element_t numerator=(omega(alpha_inverse)*m_root_exponent_table[error_location]);
				field_element_t denominator=lambda_derivative(alpha_inverse);
				if(numerator!=0)
				{
					if(denominator==0)
					{
						rsblock.m_unrecoverable=true;
						return false;
					}
					rsblock[error_location-1]^=field_t::get().div(numerator.poly(), denominator.poly());
					rsblock.m_errors_corrected++;
				}
            }
            return true;
         }

	private:
		field_symbol_t						m_root_exponent_table[block_t::CODE_LENGTH+1];
		field_symbol_t						m_syndrome_exponent_table[block_t::FEC_LENGTH];
		field_polynomial_t					m_gamma_table[field_t::FIELD_SIZE+1];
		field_polynomial_t					m_x;
	};

} //namespace reed_solomon
} //namespace ec
} //namespace math
} //namespace baldzarika

#endif //BALDZARIKA_MATH_EC_REED_SOLOMON_DECODER_H