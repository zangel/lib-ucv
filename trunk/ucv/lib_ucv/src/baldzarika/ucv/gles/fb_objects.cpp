#if defined(HAVE_GLES2)
#include <baldzarika/ucv/config.h>
#include <baldzarika/ucv/gles/fb_objects.h>

namespace baldzarika { namespace ucv { namespace gles {

	fb_render_buffer::fb_render_buffer()
		: m_gl_handle(GL_INVALID_VALUE)
	{
		glGenRenderbuffers(1,&m_gl_handle);
	}

	fb_render_buffer::~fb_render_buffer()
	{
		if(m_gl_handle!=GL_INVALID_VALUE)
			glDeleteRenderbuffers(1,&m_gl_handle);
	}

	fb_render_buffer::operator bool() const
	{
		return 	m_gl_handle!=GL_INVALID_VALUE;
	}

	fb_render_buffer::operator GLuint() const
	{
		return m_gl_handle;
	}

	GLuint fb_render_buffer::current_binding()
	{
		GLuint current_rbuf(GL_INVALID_VALUE);
		glGetIntegerv(GL_RENDERBUFFER_BINDING, reinterpret_cast<GLint*>(&current_rbuf));
		return current_rbuf;
	}
	
	bool fb_render_buffer::bind()
	{
		if(m_gl_handle==GL_INVALID_VALUE)
			return false;
		glBindRenderbuffer(GL_RENDERBUFFER, m_gl_handle);
		return true;
	}
	
	void fb_render_buffer::bind(GLuint rb)
	{
		if(rb!=GL_INVALID_VALUE)
			glBindRenderbuffer(GL_RENDERBUFFER, rb);
	}


	fb_frame_buffer::fb_frame_buffer()
		: m_gl_handle(GL_INVALID_VALUE)
	{
		glGenFramebuffers(1,&m_gl_handle);
	}

	fb_frame_buffer::~fb_frame_buffer()
	{
		if(m_gl_handle!=GL_INVALID_VALUE)
			glDeleteFramebuffers(1,&m_gl_handle);
	}

	fb_frame_buffer::operator bool() const
	{
		return 	m_gl_handle!=GL_INVALID_VALUE;	
	}

	fb_frame_buffer::operator GLuint() const
	{
		return m_gl_handle;
	}


	GLuint fb_frame_buffer::current_binding()
	{
		GLuint current_fbuf(GL_INVALID_VALUE);
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, reinterpret_cast<GLint*>(&current_fbuf));
		return current_fbuf;			
	}

	bool fb_frame_buffer::bind()
	{
		if(m_gl_handle==GL_INVALID_VALUE)
			return false;
		glBindFramebuffer(GL_FRAMEBUFFER, m_gl_handle);
		return true;	
	}

	void fb_frame_buffer::bind(GLuint fb)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fb);
	}

	bool fb_frame_buffer::is_complete()
	{
		return glCheckFramebufferStatus(GL_FRAMEBUFFER)==GL_FRAMEBUFFER_COMPLETE;
	}



} //namespace gles
} //namespace ucv
} //namespace baldzarika

#endif //HAVE_GLES2
