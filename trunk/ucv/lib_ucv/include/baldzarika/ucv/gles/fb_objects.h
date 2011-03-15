#ifndef BALDZARIKA_UCV_FB_OBJECTS_H
#define BALDZARIKA_UCV_FB_OBJECTS_H

#if defined(HAVE_GLES2)

#include <baldzarika/ucv/size2.h>

namespace baldzarika { namespace ucv { namespace gles {

	class fb_render_buffer
		: protected boost::noncopyable
	{
	public:
		fb_render_buffer();
		virtual ~fb_render_buffer();

		operator bool() const;
		operator GLuint() const;


		static GLuint	current_binding();
		bool			bind();
		static void		bind(GLuint rb);

	protected:
		GLuint		m_gl_handle;
	};


	template <int>
	struct fb_depth_buffer_traits {};

	template <>
	struct fb_depth_buffer_traits<16>
	{
		enum { FMT=GL_DEPTH_COMPONENT16 };
	};

	class fb_frame_buffer;

	template <int DS>
	class fb_depth_buffer
		: public fb_render_buffer
	{
	public:
		fb_depth_buffer()
			: fb_render_buffer()
			, m_size(0,0)
		{
		}

		fb_depth_buffer(size2ui const &ds)
			: fb_render_buffer()
			, m_size(0,0)
		{
			if(m_gl_handle!=GL_INVALID_VALUE)
			{
				GLuint cur_rbuf=current_binding();
				if(bind())
				{
					resize(ds);
					bind(cur_rbuf);
				}
			}	
		}

		~fb_depth_buffer()
		{
		}

		size2ui const& get_size() const
		{
			return m_size;
		}

		bool resize(size2ui const &ds)
		{
			if(m_gl_handle==GL_INVALID_VALUE)
				return false;
			if(m_size==ds)
				return true;
			if(ds.empty())
				return false;
			m_size=ds;
			glRenderbufferStorage(GL_RENDERBUFFER, fb_depth_buffer_traits<DS>::FMT, m_size.width(), m_size.height());
			return true;
		}

		bool attach(fb_frame_buffer const &fb) const
		{
			if(m_gl_handle==GL_INVALID_VALUE)
				return false;
			glFramebufferRenderbuffer(GL_FRAMEBUFFER,
				GL_DEPTH_ATTACHMENT,
				GL_RENDERBUFFER,
				m_gl_handle
			);
		}

	private:
		size2ui		m_size;
	};

	typedef fb_depth_buffer<16> fb_depth16_buffer;



	class fb_frame_buffer
		: protected boost::noncopyable
	{
	public:
		fb_frame_buffer();
		~fb_frame_buffer();

		operator bool() const;
		operator GLuint() const;

		
		static GLuint	current_binding();
		bool			bind();
		static void		bind(GLuint fb);
		static bool		is_complete();

		template <typename CBUF>
		bool attach_buffer(CBUF const &cbuf)
		{
			if(!cbuf.attach(*this))
				return false;
			return true;
		}


		template <typename CBUF, typename DBUF>
		bool attach_buffers(CBUF const &cbuf, DBUF const &dbuf)
		{
			if(!(cbuf.attach(*this) && dbuf.attach(*this)))
				return false;
			return true;
		}

	protected:
		GLuint		m_gl_handle;
	};


} //namespace gl
} //namespace ucv
} //namespace baldzarika

#endif //HAVE_GLES2

#endif //BALDZARIKA_UCV_FB_OBJECT_H