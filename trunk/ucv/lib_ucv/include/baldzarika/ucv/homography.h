#ifndef BALDZARIKA_UCV_HOMOGRAPHY_H
#define BALDZARIKA_UCV_HOMOGRAPHY_H

namespace baldzarika { namespace ucv {

	namespace detail {

		template < boost::uint32_t I, boost::uint32_t F, typename T, typename DT >
		class homography_estimator
		{
		public:
			homography_estimator();

		};

	}

	template < boost::uint32_t I, boost::uint32_t F, typename T, typename DT >
	bool find_homography(std::vector< feature_point<T,DT> > const &ops, std::vector< feature_point<T,DT> > const &ips, std::vector< std::pair<std::size_t, std::size_t> > const &oim, nublas::matrix< fixed_point<I,F> > &hm)
	{
		return true;
	}

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_HOMOGRAPHY_H
