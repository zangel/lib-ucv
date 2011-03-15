#if defined(HAVE_GLES2)

#include <baldzarika/ucv/config.h>
#include <baldzarika/ucv/gles/fbo.h>

namespace baldzarika { namespace ucv { namespace gles {

	render_buffer::render_buffer()
		: m_gl_handle(GL_INVALID_VALUE)
	{
		glGenRenderbuffers(1,&m_gl_handle);
	}

	render_buffer::~render_buffer()
	{
		if(m_gl_handle!=GL_INVALID_VALUE)
			glDeleteRenderbuffers(1,&m_gl_handle);
	}

	render_buffer::operator bool() const
	{
		return 	m_gl_handle!=GL_INVALID_VALUE;
	}

	render_buffer::operator GLuint() const
	{
		return m_gl_handle;
	}

	GLuint render_buffer::current_binding()
	{
		GLuint current_rbuf(GL_INVALID_VALUE);
		glGetIntegerv(GL_RENDERBUFFER_BINDING, reinterpret_cast<GLint*>(&current_rbuf));
		return current_rbuf;
	}
	
	bool render_buffer::bind()
	{
		if(m_gl_handle==GL_INVALID_VALUE)
			return false;
		glBindRenderbuffer(GL_RENDERBUFFER, m_gl_handle);
		return true;
	}
	
	void render_buffer::bind(GLuint rb)
	{
		if(rb!=GL_INVALID_VALUE)
			glBindRenderbuffer(GL_RENDERBUFFER, rb);
	}


	frame_buffer::frame_buffer()
		: m_gl_handle(GL_INVALID_VALUE)
	{
		glGenFramebuffers(1,&m_gl_handle);
	}

	frame_buffer::~frame_buffer()
	{
		if(m_gl_handle!=GL_INVALID_VALUE)
			glDeleteFramebuffers(1,&m_gl_handle);
	}

	frame_buffer::operator bool() const
	{
		return 	m_gl_handle!=GL_INVALID_VALUE;	
	}

	frame_buffer::operator GLuint() const
	{
		return m_gl_handle;
	}


	GLuint frame_buffer::current_binding()
	{
		GLuint current_fbuf(GL_INVALID_VALUE);
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, reinterpret_cast<GLint*>(&current_fbuf));
		return current_fbuf;			
	}

	bool frame_buffer::bind()
	{
		if(m_gl_handle==GL_INVALID_VALUE)
			return false;
		glBindFramebuffer(GL_FRAMEBUFFER, m_gl_handle);
		return true;	
	}

	void frame_buffer::bind(GLuint fb)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, fb);
	}

	bool frame_buffer::is_complete()
	{
		GLenum fb_status=glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if(fb_status==GL_FRAMEBUFFER_COMPLETE)
			return true;

#if defined(ANDROID)
		switch(fb_status)
		{
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			__android_log_print(ANDROID_LOG_ERROR, UCV_NAME, "frame_buffer::is_complete: GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			__android_log_print(ANDROID_LOG_ERROR, UCV_NAME, "frame_buffer::is_complete: GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
			__android_log_print(ANDROID_LOG_ERROR, UCV_NAME, "frame_buffer::is_complete: GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS");
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED:
			__android_log_print(ANDROID_LOG_ERROR, UCV_NAME, "frame_buffer::is_complete: GL_FRAMEBUFFER_UNSUPPORTED");
			break;
		default:
			__android_log_print(ANDROID_LOG_ERROR, UCV_NAME, "frame_buffer::is_complete: glCheckFramebufferStatus()=%d",fb_status);
		}
#endif
		return false;
	}

	bool frame_buffer::detach_color()
	{
		if(m_gl_handle==GL_INVALID_VALUE)
			return false;
		glFramebufferRenderbuffer(m_gl_handle, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, 0);
		return true;
	}
	
	bool frame_buffer::detach_depth()
	{
		if(m_gl_handle==GL_INVALID_VALUE)
			return false;
		glFramebufferRenderbuffer(m_gl_handle, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
		return true;
	}
	
} //namespace gles
} //namespace ucv
} //namespace baldzarika

#endif //HAVE_GLES2
