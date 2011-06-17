#if defined(HAVE_GLES2)

#include <baldzarika/ucv/config.h>
#include <baldzarika/ucv/gles/texture.h>


namespace baldzarika { namespace ucv { namespace gles {

	texture::texture()
		: m_gl_handle(GL_INVALID_VALUE)
	{
		glGenTextures(1, &m_gl_handle);
	}

	texture::~texture()
	{
		if(m_gl_handle!=GL_INVALID_VALUE)
			glDeleteTextures(1,&m_gl_handle);
	}

	texture::operator bool() const
	{
		return m_gl_handle!=GL_INVALID_VALUE;
	}

	texture::operator GLuint() const
	{
		return m_gl_handle;
	}

} //namespace gl
} //namespace ucv
} //namespace baldzarika

#endif //HAVE_GLES2
