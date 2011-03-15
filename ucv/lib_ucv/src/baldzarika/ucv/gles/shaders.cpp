#if defined(HAVE_GLES2)

#include <baldzarika/ucv/config.h>
#include <baldzarika/ucv/gles/shaders.h>

namespace baldzarika { namespace ucv { namespace gles {

	shader::shader(GLenum sht)
		: m_gl_handle(glCreateShader(sht))
	{
	}

	shader::~shader()
	{
		if(m_gl_handle!=GL_INVALID_VALUE)
			glDeleteShader(m_gl_handle);
	}

	shader::operator bool() const
	{
		return m_gl_handle!=GL_INVALID_VALUE;
	}

	shader::operator GLuint() const
	{
		return m_gl_handle;
	}

	bool shader::set_source(std::string const &src)
	{
		if(m_gl_handle==GL_INVALID_VALUE)
			return false;

		GLint src_length=src.size();
		const char* src_txt=src.c_str();
		glShaderSource(m_gl_handle, 1, &src_txt, &src_length);
		return true;
	}


	bool shader::compile()
	{
		if(m_gl_handle==GL_INVALID_VALUE)
			return false;
		glCompileShader(m_gl_handle);
		return true;
	}

	bool shader::is_compiled() const
	{
		if(m_gl_handle==GL_INVALID_VALUE)
			return false;
		GLint compile_status(GL_FALSE);
		glGetShaderiv(m_gl_handle, GL_COMPILE_STATUS, &compile_status);
		if(compile_status!=GL_TRUE)
		{
			char shader_compile_log[1024];
			GLint log_length(0);
			glGetShaderInfoLog(m_gl_handle, 1024, &log_length, shader_compile_log);
#if defined(ANDROID)
			__android_log_print(ANDROID_LOG_INFO, UCV_NAME, "shader::is_compiled: status=%d, len=%d, %s", int(compile_status), log_length, shader_compile_log);
#endif
			return false;
		}
		return true;
	}


	GLuint const vertex_shader::attribute::INVALID_ATTRIBUTE_LOCATION=GLuint(-1);

	vertex_shader::attribute::attribute()
		: m_location(INVALID_ATTRIBUTE_LOCATION)
		, m_size(0)
		, m_type(GLenum(-1))
	{
	}

	vertex_shader::attribute::attribute(GLuint l, std::string const &n, GLuint s, GLenum t)
		: m_location(l)
		, m_name(n)
		, m_size(s)
		, m_type(t)
	{
	}

	vertex_shader::attribute::attribute(attribute const &that)
		: m_location(that.m_location)
		, m_name(that.m_name)
		, m_size(that.m_size)
		, m_type(that.m_type)
	{
	}

	vertex_shader::attribute::~attribute()
	{
	}

	vertex_shader::attribute::operator bool() const
	{
		return m_location!=INVALID_ATTRIBUTE_LOCATION;
	}

	GLuint vertex_shader::attribute::get_location() const
	{
		return m_location;
	}

	std::string const& vertex_shader::attribute::get_name() const
	{
		return m_name;
	}

	GLuint vertex_shader::attribute::get_size() const
	{
		return m_size;
	}

	GLenum vertex_shader::attribute::get_type() const
	{
		return m_type;
	}

	vertex_shader::vertex_shader()
		: shader(GL_VERTEX_SHADER)
	{
	}

	vertex_shader::~vertex_shader()
	{
	}

	fragment_shader::fragment_shader()
		: shader(GL_FRAGMENT_SHADER)
	{
	}

	fragment_shader::~fragment_shader()
	{
	}

	
	GLuint const program::uniform::INVALID_UNIFORM_LOCATION=GLuint(-1);

	program::uniform::uniform()
		: m_location(INVALID_UNIFORM_LOCATION)
		, m_size(0)
		, m_type(GLenum(-1))
	{
	}

	program::uniform::uniform(GLuint l, std::string const &n, GLuint s, GLenum t)
		: m_location(l)
		, m_name(n)
		, m_size(s)
		, m_type(t)
	{
	}

	program::uniform::uniform(uniform const &that)
		: m_location(that.m_location)
		, m_name(that.m_name)
		, m_size(that.m_size)
		, m_type(that.m_type)
	{
	}

	program::uniform::~uniform()
	{
	}

	program::uniform::operator bool() const
	{
		return m_location!=INVALID_UNIFORM_LOCATION;
	}

	GLuint program::uniform::get_location() const
	{
		return m_location;
	}

	std::string const& program::uniform::get_name() const
	{
		return m_name;
	}

	GLuint program::uniform::get_size() const
	{
		return m_size;
	}

	GLenum program::uniform::get_type() const
	{
		return m_type;
	}

	program::program()
		: m_gl_handle(glCreateProgram())
	{
	}

	program::~program()
	{
		if(m_gl_handle!=GL_INVALID_VALUE)
			glDeleteProgram(m_gl_handle);
	}

	program::operator bool() const
	{
		return m_gl_handle!=GL_INVALID_VALUE;
	}

	program::operator GLuint() const
	{
		return m_gl_handle;
	}

	template<>
	bool program::attach<vertex_shader>(vertex_shader const &vs)
	{
		if(m_gl_handle==GL_INVALID_VALUE || !vs)
			return false;
		glAttachShader(m_gl_handle, vs);
		return true;
	}

	template<>
	bool program::attach<fragment_shader>(fragment_shader const &fs)
	{
		if(m_gl_handle==GL_INVALID_VALUE || !fs)
			return false;
		glAttachShader(m_gl_handle, fs);
		return true;
	}

	bool program::link()
	{
		if(m_gl_handle==GL_INVALID_VALUE)
			return false;
		glLinkProgram(m_gl_handle);
		
		if(!is_linked())
			return false;

		//collect active attributes
		m_attributes.clear();
		GLint num_attributes(0);
		glGetProgramiv(m_gl_handle, GL_ACTIVE_ATTRIBUTES, &num_attributes);
		
		if(num_attributes>0)
		{
			GLint max_attribute_name_length(0);
			glGetProgramiv(m_gl_handle, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max_attribute_name_length);
			if(!max_attribute_name_length)
				return false;
			
			boost::scoped_array<char> cstr_attribute_name(new char[max_attribute_name_length]);
			for(GLuint ai=0;ai<GLuint(num_attributes);++ai)
			{
				GLsizei attribute_name_length(0);
				GLint attribute_size(0);
				GLenum attribute_type(-1);
				glGetActiveAttrib(m_gl_handle, ai,
					max_attribute_name_length,
					&attribute_name_length,
					&attribute_size,
					&attribute_type,
					cstr_attribute_name.get()
				);
				if(attribute_name_length>0 && attribute_size>0)
				{
					GLuint attribute_loc=glGetAttribLocation(m_gl_handle,cstr_attribute_name.get());
					m_attributes.push_back(
						vertex_shader::attribute(
							attribute_loc,
							std::string(cstr_attribute_name.get(), cstr_attribute_name.get()+attribute_name_length),
							GLuint(attribute_size),
							attribute_type
						)
					);
				}
			}
		}

		//collect active uniforms
		m_uniforms.clear();
		GLint num_uniforms(0);
		glGetProgramiv(m_gl_handle, GL_ACTIVE_UNIFORMS, &num_uniforms);

		if(num_uniforms>0)
		{
			GLint max_uniform_name_length(0);
			glGetProgramiv(m_gl_handle, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_uniform_name_length);
		
			if(!max_uniform_name_length)
				return false;

			boost::scoped_array<char> cstr_uniform_name(new char[max_uniform_name_length]);
			for(GLuint ui=0;ui<GLuint(num_uniforms);++ui)
			{
				GLsizei uniform_name_length(0);
				GLint uniform_size(0);
				GLenum uniform_type(-1);
				glGetActiveUniform(m_gl_handle, ui,
					max_uniform_name_length,
					&uniform_name_length,
					&uniform_size,
					&uniform_type,
					cstr_uniform_name.get()
				);
				if(uniform_name_length>0 && uniform_size>0)
				{
					GLuint uniform_loc=glGetUniformLocation(m_gl_handle, cstr_uniform_name.get());
					m_uniforms.push_back(
						uniform(
							uniform_loc,
							std::string(cstr_uniform_name.get(), cstr_uniform_name.get()+uniform_name_length),
							GLuint(uniform_size),
							uniform_type
						)
					);
				}
			}
		}
		return true;
	}

	bool program::is_linked() const
	{
		if(m_gl_handle==GL_INVALID_VALUE)
			return false;
		GLint link_status(GL_FALSE);
		glGetProgramiv(m_gl_handle,GL_LINK_STATUS, &link_status);
		return link_status==GL_TRUE;
	}

	bool program::use()
	{
		if(m_gl_handle==GL_INVALID_VALUE)
			return false;
		glUseProgram(m_gl_handle);
		return true;
	}

	std::size_t program::num_attributes() const
	{
		return m_attributes.size();
	}

	vertex_shader::attribute program::get_attribute(std::size_t idx) const
	{
		return m_attributes[idx];
	}

	vertex_shader::attribute program::get_attribute(std::string const &n) const
	{
		attributes_t::index<name_tag>::type const &attributes_by_name=
			m_attributes.get<name_tag>();

		attributes_t::index<name_tag>::type::const_iterator it_attrib=
			attributes_by_name.find(n);

		if(it_attrib==attributes_by_name.end())
			return vertex_shader::attribute();
		return *it_attrib;
	}

	std::size_t program::num_uniforms() const
	{
		return m_uniforms.size();
	}

	program::uniform program::get_uniform(std::size_t idx) const
	{
		return m_uniforms[idx];
	}

	program::uniform program::get_uniform(std::string const &n) const
	{
		uniforms_t::index<name_tag>::type const &uniforms_by_name=
			m_uniforms.get<name_tag>();

		uniforms_t::index<name_tag>::type::const_iterator it_uniform=
			uniforms_by_name.find(n);

		if(it_uniform==uniforms_by_name.end())
			return uniform();
		return *it_uniform;
	}

} //namespace gles
} //namespace ucv
} //namespace baldzarika

#endif //HAVE_GLES2
