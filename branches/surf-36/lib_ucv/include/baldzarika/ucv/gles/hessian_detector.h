#ifndef BALDZARIKA_UCV_GLES_HESSIAN_DETECTOR_H
#define BALDZARIKA_UCV_GLES_HESSIAN_DETECTOR_H

#if defined(HAVE_GLES2)

#include <baldzarika/ucv/gles/texture.h>
#include <baldzarika/ucv/gles/fbo.h>
#include <baldzarika/ucv/gles/shaders.h>
#include <baldzarika/ucv/feature_point.h>

namespace baldzarika { namespace ucv { namespace gles {


	class hessian_detector
		: private boost::noncopyable
	{
	public:
		class response_layer
		{
			friend class hessian_detector;
		public:
			response_layer();
			response_layer(response_layer const &that);
			response_layer(point2ui const &ro, math::size2ui const &rs, boost::uint32_t ss, boost::uint32_t fs);
			~response_layer();

			operator bool() const;

			point2ui const&		get_offset() const;
			math::size2ui const&		get_size() const;
			boost::uint32_t		get_sample_step() const;
			

			response_layer&		operator =(response_layer const &rhs);


		protected:
			point2ui			m_response_offset;
			math::size2ui				m_response_size;

			boost::uint32_t		m_sample_step;
			boost::uint32_t		m_filter_size;
		};
		
		typedef std::vector<response_layer> response_layers_t;

		class detection_layer
		{
			friend class hessian_detector;
		public:
			detection_layer();
			detection_layer(detection_layer const &that);
			detection_layer(point2ui const &o, math::size2ui const &s);
			~detection_layer();

			operator bool() const;

			point2ui const&		get_offset() const;
			math::size2ui const&		get_size() const;
			detection_layer&	operator =(detection_layer const &rhs);

		protected:
			point2ui			m_detection_offset;
			math::size2ui				m_detection_size;
		};

		typedef std::vector<detection_layer> detection_layers_t;

	public:
		hessian_detector(math::size2ui const &is, boost::uint32_t o, boost::uint32_t i, boost::uint32_t s, float t, boost::uint32_t mf=64);
		~hessian_detector();

		operator bool() const;

	
	protected:
		bool			create_programs();
		bool			create_hessian_response_program();
		bool			create_feature_detection_program();
		bool			create_orientation_program();
		bool			resize(math::size2ui const &is);
		bool			resize_response();
		bool			resize_detection();
		bool			resize_descript();


		
	public:
		bool						set_integral_img(ucv::gil::rgba8_view_t iv);
		bool						build_response_layers();
		bool						build_detection_layers();
		bool						collect_detected_points(std::vector<feature_point> &fps);
		bool						compute_orientations(std::vector<feature_point> &fps);
		

		rgba8_texture_2d const&		get_response_texture() const;
		rgba8_texture_2d const&		get_detection_texture() const;

		rgba8_texture_2d const&		get_integral_texture() const;
		response_layers_t const&	get_response_layers() const;
		detection_layers_t const&	get_detection_layers() const;
			
	private:
		math::size2ui						m_integral_img_size;
		boost::uint32_t				m_octaves;
		boost::uint32_t				m_intervals;
		boost::uint32_t				m_sample_step;
		float						m_treshold;
		boost::uint32_t				m_max_features;
		
		ucv::gles::program			m_hessian_response_program;
		ucv::gles::program			m_feature_detection_program;
		ucv::gles::program			m_orientation_1_program;
		ucv::gles::program			m_orientation_2_program;
		

		rgba8_texture_2d			m_integral_texture;
		rgba8_texture_2d			m_response_texture;
		rgba8_texture_2d			m_feature_detection_texture;
		frame_buffer				m_fbo;

		response_layers_t			m_response_layers;
		
		detection_layers_t			m_detection_layers;
		gil::rgba8_image_t			m_feature_detection_img;

		rgba8_texture_2d			m_orientation_1_texture;
		rgba8_texture_2d			m_orientation_2_texture;
		boost::scoped_array<float>	m_desc_vtx_pos;
		boost::scoped_array<float>	m_desc_vtx_tex_coords;
		rgba8_texture_2d			m_descript_texture;
	};

} //namespace gles
} //namespace ucv
} //namespace baldzarika

#endif //HAVE_GLES2

#endif //BALDZARIKA_UCV_GLES_HESSIAN_DETECTOR_H
