#ifndef BALDZARIKA_UCV_SURF_FEATURE_POINT_H
#define BALDZARIKA_UCV_SURF_FEATURE_POINT_H

namespace baldzarika { namespace ucv { namespace surf {

	template < typename T, boost::uint32_t NB >
	class feature_point
		: public math::point2<T>
	{
	public:
		static boost::uint32_t const DESCRIPTOR_SIZE=4*NB*NB;

		typedef T value_t;
		typedef math::point2<value_t> base_t;
		typedef math::vector<value_t,DESCRIPTOR_SIZE> descriptor_t;

		
		struct position_accessor
		{
			typedef value_t result_type;
			result_type operator()(feature_point const &t, size_t k) const { return t[k]; }
			result_type operator()(base_t const &t, size_t k) const { return t[k]; }
		};

		struct description_accessor
		{
			typedef value_t result_type;
			result_type operator()(feature_point const &t, size_t k) const { return t.m_descriptor[k]; }
		};
		
		feature_point(base_t const &p, boost::int32_t s, bool lap)
			: base_t(p)
			, m_scale(s)
			, m_laplacian(lap)
			, m_orientation(math::constant::zero<value_t>())
		{
		}

		feature_point(feature_point const &that)
			: base_t(that)
			, m_scale(that.m_scale)
			, m_laplacian(that.m_laplacian)
			, m_orientation(that.m_orientation)
			, m_descriptor(that.m_descriptor)
		{
		}

		value_t operator -(feature_point const &rhs) const
		{
			return (m_descriptor-rhs.m_descriptor).norm_2();

		}

		bool operator&&(feature_point const &rhs) const
		{
			return !(!m_laplacian ^ !rhs.m_laplacian);
		}

		boost::int32_t	m_scale;
		bool			m_laplacian;
		value_type		m_orientation;
		descriptor_t	m_descriptor;
	};

	
} //namespace surf
} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_SURF_FEATURE_POINT_H
