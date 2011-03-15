#ifndef BALDZARIKA_UCV_GLES_SHADERS_H
#define BALDZARIKA_UCV_GLES_SHADERS_H

#if defined(HAVE_GLES2)

namespace baldzarika { namespace ucv { namespace gles {

	class shader
		: public boost::noncopyable
	{
	protected:
		shader(GLenum sht);
	public:
		virtual ~shader();

		operator bool() const;
		operator GLuint() const;

		bool	set_source(std::string const &src);
		bool	compile();
		bool	is_compiled() const;


	protected:
		GLuint	m_gl_handle;
	};

	class vertex_shader
		: public shader
	{
	public:
		class attribute
		{
		protected:
			static GLuint const INVALID_ATTRIBUTE_LOCATION;

		public:

			attribute();
			attribute(GLuint l, std::string const &n, GLuint s, GLenum t);
			attribute(attribute const &that);
			~attribute();

			operator bool() const;


			GLuint				get_location() const;
			std::string const&	get_name() const;
			GLuint				get_size() const;
			GLenum				get_type() const;

		private:
			GLuint			m_location;
			std::string		m_name;
			GLuint			m_size;
			GLenum			m_type;
		};

		vertex_shader();
		~vertex_shader();
	};

	class fragment_shader
		: public shader
	{
	public:
		fragment_shader();
		~fragment_shader();
	};

	class program
		: protected boost::noncopyable
	{
	public:

		class uniform
		{
		protected:
			static GLuint const INVALID_UNIFORM_LOCATION;

		public:

			uniform();
			uniform(GLuint l, std::string const &n, GLuint s, GLenum t);
			uniform(uniform const &that);
			~uniform();

			operator bool() const;


			GLuint				get_location() const;
			std::string const&	get_name() const;
			GLuint				get_size() const;
			GLenum				get_type() const;

		private:
			GLuint			m_location;
			std::string		m_name;
			GLuint			m_size;
			GLenum			m_type;
		};

		struct order_tag {};
		struct location_tag {};
		struct name_tag {};
		struct type_tag {};

		typedef boost::multi_index_container
		<
			vertex_shader::attribute,
			boost::multi_index::indexed_by
			<
				boost::multi_index::random_access
				<
					boost::multi_index::tag<order_tag>
				>,
				boost::multi_index::hashed_unique
				<
					boost::multi_index::tag<location_tag>,
					BOOST_MULTI_INDEX_CONST_MEM_FUN(vertex_shader::attribute, GLuint, get_location)
				>,
				boost::multi_index::hashed_unique
				<
					boost::multi_index::tag<name_tag>,
					BOOST_MULTI_INDEX_CONST_MEM_FUN(vertex_shader::attribute, std::string const&, get_name)
				>,
				boost::multi_index::hashed_non_unique
				<
					boost::multi_index::tag<type_tag>,
					BOOST_MULTI_INDEX_CONST_MEM_FUN(vertex_shader::attribute, GLenum, get_type)
				>
			>
		> attributes_t;

		typedef boost::multi_index_container
		<
			uniform,
			boost::multi_index::indexed_by
			<
				boost::multi_index::random_access
				<
					boost::multi_index::tag<order_tag>
				>,
				boost::multi_index::hashed_unique
				<
					boost::multi_index::tag<location_tag>,
					BOOST_MULTI_INDEX_CONST_MEM_FUN(uniform, GLuint, get_location)
				>,
				boost::multi_index::hashed_unique
				<
					boost::multi_index::tag<name_tag>,
					BOOST_MULTI_INDEX_CONST_MEM_FUN(uniform, std::string const&, get_name)
				>,
				boost::multi_index::hashed_non_unique
				<
					boost::multi_index::tag<type_tag>,
					BOOST_MULTI_INDEX_CONST_MEM_FUN(uniform, GLenum, get_type)
				>
			>
		> uniforms_t;

		program();
		virtual ~program();

		
		operator bool() const;
		operator GLuint() const;

		template <typename ST>
		bool		attach(ST const &);
				
		bool		link();
		bool		is_linked() const;
		bool		use();


		std::size_t					num_attributes() const;
		vertex_shader::attribute	get_attribute(std::size_t idx) const;
		vertex_shader::attribute	get_attribute(std::string const &n) const;


		std::size_t					num_uniforms() const;
		uniform						get_uniform(std::size_t idx) const;
		uniform						get_uniform(std::string const &n) const;



	protected:
		GLuint				m_gl_handle;
		attributes_t		m_attributes;
		uniforms_t			m_uniforms;
	};
	
} //namespace gles
} //namespace ucv
} //namespace baldzarika

#endif //HAVE_GLES2

#endif //BALDZARIKA_UCV_GLES_SHADERS_H