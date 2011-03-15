#ifndef BALDZARIKA_UCV_FB_OBJECTS_H
#define BALDZARIKA_UCV_FB_OBJECTS_H

#if defined(HAVE_GLES2)

#include <baldzarika/ucv/size2.h>
#include <baldzarika/ucv/point2.h>
#include <baldzarika/ucv/gles/texture.h>

namespace baldzarika { namespace ucv { namespace gles {

	class render_buffer
		: protected boost::noncopyable
	{
	public:
		render_buffer();
		virtual ~render_buffer();

		operator bool() const;
		operator GLuint() const;


		static GLuint	current_binding();
		bool			bind();
		static void		bind(GLuint rb);

	protected:
		GLuint		m_gl_handle;
	};


	template <int>
	struct depth_buffer_traits {};

	template <>
	struct depth_buffer_traits<16>
	{
		enum { FMT=GL_DEPTH_COMPONENT16 };
	};

	class frame_buffer;

	template <int DS>
	class depth_buffer
		: public render_buffer
	{
	public:
		depth_buffer()
			: render_buffer()
			, m_size(0,0)
		{
		}

		depth_buffer(size2ui const &ds)
			: render_buffer()
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

		~depth_buffer()
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
			glRenderbufferStorage(GL_RENDERBUFFER, depth_buffer_traits<DS>::FMT, m_size.width(), m_size.height());
			return true;
		}

		bool attach(frame_buffer const &fb) const
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

	typedef depth_buffer<16> depth_buffer_16;



	class frame_buffer
		: protected boost::noncopyable
	{
	public:
		frame_buffer();
		~frame_buffer();

		operator bool() const;
		operator GLuint() const;

		
		static GLuint	current_binding();
		bool			bind();
		static void		bind(GLuint fb);
		static bool		is_complete();

		bool			detach_color();
		bool			detach_depth();

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

		template <typename DVT>
		bool download(point2ui const &o, DVT dst)
		{
			if(m_gl_handle==GL_INVALID_VALUE)
				return false;
			glPixelStorei(GL_PACK_ALIGNMENT, pixel_traits<typename DVT::value_type>::PACK_ALIGNMENT);
			glReadPixels(
				o.x, o.y,
				dst.width(), dst.height(),
				pixel_traits<typename DVT::value_type>::PIXEL_FORMAT,
				pixel_traits<typename DVT::value_type>::PIXEL_TYPE,
				&dst[0][0]
			);
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
