#ifndef BALDZARIKA_UCV_SURF_H
#define BALDZARIKA_UCV_SURF_H

#include <baldzarika/ucv/feature_point.h>
#include <baldzarika/ucv/point2.h>
#include <baldzarika/ucv/size2.h>
#include <baldzarika/ucv/fixed_point.h>
#include <baldzarika/ucv/gil_channel_traits.h>

namespace baldzarika { namespace ucv {

	class surf
		: public boost::noncopyable
	{
	public:

		typedef feature_point< decimal_t, fixed_point<10, 21> > feature_point_t;
		typedef KDTree::KDTree<2, feature_point_t, feature_point_t::accessor> feature_point_tree_t;
		
		typedef fixed_point<10, 21> gray_t;
		typedef gil::pixel<gray_t, ucv::gil::gray_layout_t> gray_pixel_t;
		typedef gil::image< gray_pixel_t, false, std::allocator<unsigned char> > gray_image_t;
		typedef gray_image_t::view_t gray_view_t;


		typedef fixed_point<10, 21> integral_t;
		typedef gil::pixel<integral_t, ucv::gil::gray_layout_t> integral_pixel_t;
		typedef gil::image< integral_pixel_t, false, std::allocator<unsigned char> > integral_image_t;
		typedef integral_image_t::view_t integral_view_t;

		typedef fixed_point<10, 21>	response_t;
		typedef gil::pixel<response_t, ucv::gil::gray_layout_t> response_pixel_t;
		typedef ucv::gil::image< response_pixel_t, false, std::allocator<unsigned char> > response_image_t;
		typedef response_image_t::view_t response_view_t;

		typedef boost::function<void (feature_point_t::point2_t const&, feature_point_t::desc_value_type const&)> point_detected_t;
		typedef std::pair<point2i,size2ui> range_t;
		typedef std::pair<range_t, point_detected_t> ranged_detect_params_t;

		

		class response_layer
		{
			friend class surf;
		public:
			response_layer(response_layer const &that);
			response_layer(surf &sr, point2ui const &ro, response_view_t rv, boost::uint32_t ss, boost::uint32_t fs);
			~response_layer();

			operator bool() const;

			bool				build();
			bool				detect(response_layer &bl, response_layer &ml, ranged_detect_params_t const &rdp);
						
			response_t			get_response(boost::int32_t x, boost::int32_t y, response_layer const &src) const;
			
			point2ui			get_offset() const;
			size2ui				get_size() const;
			response_view_t		get_response_view() const;
			boost::uint32_t		get_sample_step() const;
			boost::uint32_t		get_filter_size() const;

			response_layer&		operator =(response_layer const &rhs);


		protected:
			surf				&m_surf;
			point2ui			m_response_offset;
			response_view_t		m_response_view;
			
			boost::uint32_t		m_sample_step;
			boost::uint32_t		m_filter_size;
		};
		typedef std::vector<response_layer> response_layers_t;

		static boost::uint32_t const	MAX_OCTAVES=5;
		static boost::uint32_t const	MAX_INTERVALS=4;
		
		static boost::uint32_t const 	FILTER_MAP[MAX_OCTAVES][MAX_INTERVALS];
		

		surf(size2ui const &is, boost::uint32_t o, boost::uint32_t i, boost::uint32_t s, float t, boost::uint32_t mf=64);
		~surf();

		boost::uint32_t				octaves() const;
		boost::uint32_t				intervals() const;
		boost::uint32_t				sample_step() const;
		float						treshold() const;
		
		bool						resize(size2ui const &is);
		size2ui						size() const;
		bool						update(gray_view_t gi);
		bool						build_response_layers();
		bool						detect(std::vector<feature_point_t> &fps);
		bool						find(std::vector<feature_point_t::point2_t> const &gp, boost::uint32_t ws, feature_point_tree_t &fps);

		bool						describe(std::vector<feature_point_t> &fps);

		


		static void					match_feature_points(std::vector<feature_point_t> const &fps1, std::vector<feature_point_t> const &fps2, std::vector< std::pair<std::size_t, std::size_t> > &m, feature_point_t::value_type const &d=feature_point_t::value_type(0.65f));


	protected:
		void						ranged_detect(std::vector< ranged_detect_params_t > const &rdp);
		bool						compute_orientations(std::vector<feature_point_t> &fps);
		bool						compute_descriptors(std::vector<feature_point_t> &fps);
		


		template < typename T > T			haar_x(point2i const &p, boost::uint32_t s);
		template < typename T > T			haar_y(point2i const &p, boost::uint32_t s);

		template < boost::uint32_t I, boost::uint32_t F >
		fixed_point<I,F>					get_angle(fixed_point<I,F> const &x, fixed_point<I,F> const &y);

		float								get_angle(float x, float y);

		template < boost::uint32_t I, boost::uint32_t F >
		static fixed_point<I,F>				gaussian(boost::int32_t x, boost::int32_t y, fixed_point<I,F> const &sig);

		template < boost::uint32_t I, boost::uint32_t F >
		static fixed_point<I,F>				gaussian(fixed_point<I,F> const &x, fixed_point<I,F> const &y, fixed_point<I,F> const &sig);

	private:
		integral_image_t			m_integral_img;
		boost::uint32_t				m_octaves;
		boost::uint32_t				m_intervals;
		boost::uint32_t				m_sample_step;
		float						m_treshold;
		boost::uint32_t				m_max_features;

		response_image_t			m_response_img;
		response_layers_t			m_response_layers;
	};

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_SURF_H
