#ifndef BALDZARIKA_UCV_SURF_H
#define BALDZARIKA_UCV_SURF_H

#include <baldzarika/ucv/fixed_point.h>
#include <baldzarika/ucv/feature_point.h>
#include <baldzarika/ucv/point2.h>
#include <baldzarika/ucv/size2.h>
#include <baldzarika/ucv/gil_channel_traits.h>

namespace baldzarika { namespace ucv {

	class surf
		: public boost::noncopyable
	{
	public:

		static boost::uint32_t const SAMPLES_PER_BLOCK;
		typedef feature_point< decimal_t, fixed_point<10, 21>, 4 > feature_point_t;

		typedef std::vector< feature_point_t > fps_array_t;
		typedef KDTree::KDTree<2, feature_point_t, feature_point_t::position_accessor> fps_by_pos_tree_t;
		typedef KDTree::KDTree<feature_point_t::DESCRIPTOR_SIZE, feature_point_t, feature_point_t::description_accessor> fps_by_desc_tree_t;
		
		
		typedef fixed_point<10, 21> integral_t;
		typedef gil::pixel<integral_t, ucv::gil::gray_layout_t> integral_pixel_t;
		typedef gil::image< integral_pixel_t, false, std::allocator<unsigned char> > integral_image_t;
		typedef integral_image_t::view_t integral_view_t;
		typedef integral_image_t::const_view_t const_integral_view_t;

		typedef fixed_point<10, 21>	response_t;
		typedef gil::pixel<response_t, ucv::gil::gray_layout_t> response_pixel_t;
		typedef ucv::gil::image< response_pixel_t, false, std::allocator<unsigned char> > response_image_t;
		typedef response_image_t::view_t response_view_t;

		
		typedef gil::bit_aligned_image1_type< 1, gil::gray_layout_t >::type laplacian_image_t;
		typedef laplacian_image_t::view_t laplacian_view_t;



		typedef boost::function<void (feature_point_t::point2_t const&, feature_point_t::desc_value_type const&, bool)> point_detected_t;
		typedef std::pair<point2i,size2ui> range_t;
		typedef std::pair<range_t, point_detected_t> ranged_detect_params_t;

		

		class response_layer
		{
			friend class surf;
		public:
			response_layer(response_layer const &that);
			response_layer(surf &sr, point2ui const &ro, response_view_t rv, laplacian_view_t lv, boost::uint32_t ss, boost::uint32_t fs);
			~response_layer();

			operator bool() const;

			void				build();
			void				detect(response_layer &bl, response_layer &ml, ranged_detect_params_t const &rdp);
						
			response_t			get_response(boost::int32_t x, boost::int32_t y, response_layer const &src) const;
			bool				get_laplacian(boost::int32_t x, boost::int32_t y, response_layer const &src) const;
			
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
			laplacian_view_t	m_laplacian_view;
			
			boost::uint32_t		m_sample_step;
			boost::uint32_t		m_filter_size;
		};
		typedef std::vector<response_layer> response_layers_t;

		static boost::uint32_t const	MAX_OCTAVES=5;
		static boost::uint32_t const	MAX_INTERVALS=4;
		
		static boost::uint32_t const 	FILTER_MAP[MAX_OCTAVES][MAX_INTERVALS];
		

		surf(size2ui const &fs, boost::uint32_t o, boost::uint32_t i, boost::uint32_t s, float t, boost::uint32_t mf=64);
		~surf();

		boost::uint32_t				octaves() const;
		boost::uint32_t				intervals() const;
		boost::uint32_t				sample_step() const;
		
		float						treshold() const;
		void						set_treshold(float t);

		size2ui	const&				size() const;
		bool						resize(size2ui const &fs);

		bool						set_integral_view(const_integral_view_t iv);
		void						build_response_layers();

		void						detect(fps_array_t &fps);
		void						detect(fps_by_pos_tree_t &fps);
		void						detect(fps_by_desc_tree_t &fps);

		//bool						find(std::vector<feature_point_t::point2_t> const &gp, boost::uint32_t ws, fps_by_pos_tree_t &fps);

		void						describe(fps_array_t &fps);
		void						describe(fps_by_pos_tree_t &fps);
		void						describe(fps_by_desc_tree_t &fps);
		
		static void					match_feature_points(fps_array_t const &fps1, fps_array_t const &fps2, std::vector< std::pair<std::size_t,std::size_t> > &m, feature_point_t::value_type const &d=feature_point_t::value_type(0.65f));
		
	protected:
		void						ranged_detect(std::vector< ranged_detect_params_t > const &rdp);
		void						compute_orientation(feature_point_t &fp);
		void						compute_descriptor(feature_point_t &fp);
		


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
		size2ui					m_frame_size;
		const_integral_view_t	m_integral_view;
		boost::uint32_t			m_octaves;
		boost::uint32_t			m_intervals;
		boost::uint32_t			m_sample_step;
		float					m_treshold;
		boost::uint32_t			m_max_features;

		response_image_t		m_response_img;
		laplacian_image_t		m_laplacian_img;
		response_layers_t		m_response_layers;
	};

} //namespace ucv
} //namespace baldzarika

#endif //BALDZARIKA_UCV_SURF_H
