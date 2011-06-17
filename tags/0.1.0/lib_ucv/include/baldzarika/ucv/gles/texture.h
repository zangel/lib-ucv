#ifndef BALDZARIKA_UCV_GLES_TEXTURE_H
#define BALDZARIKA_UCV_GLES_TEXTURE_H

#if defined(HAVE_GLES2)

#include <baldzarika/ucv/size2.h>
#include <baldzarika/ucv/point2.h>

namespace baldzarika { namespace ucv { namespace gles {

	//typedef gil::packed_pixel_type<boost::uint16_t, boost::mpl::vector3_c<unsigned,5,6,5>, gil::rgb_layout_t>  rgb565_pixel_t;
	
	template <typename GILPT>
	struct pixel_traits {};

	template <>
	struct pixel_traits<gil::rgba8_pixel_t>
	{
		enum { PIXEL_FORMAT=GL_RGBA };
		enum { PIXEL_TYPE=GL_UNSIGNED_BYTE };
		enum { UNPACK_ALIGNMENT=1 };
		enum { PACK_ALIGNMENT=1 };
	};

	class frame_buffer;

	class texture
		: private boost::noncopyable
	{
	public:
		texture();
		virtual ~texture();

		operator bool() const;
		operator GLuint() const;

		bool	set_parameter(GLenum n, GLint v);


	protected:
		GLuint	m_gl_handle;
	};

	
	template <typename GILPT>
	class texture_2d
		: public texture
	{
	public:
		typedef GILPT *pixel_iterator_t;
		typedef typename ucv::gil::type_from_x_iterator<pixel_iterator_t>::view_t view_t;

		texture_2d()
			: texture()
			, m_size(0,0)
		{
		}

		texture_2d(math::size2ui const &ts)
			: texture()
			, m_size(0,0)
		{
			if(m_gl_handle!=GL_INVALID_VALUE)
			{
				GLuint cur_tex=current_binding();
				if(bind())
				{
					resize(ts);
					bind(cur_tex);
				}
			}
		}

		~texture_2d()
		{
		}

		bool set_parameter(GLenum n, GLint v)
		{
			if(m_gl_handle==GL_INVALID_VALUE)
				return false;
			glTexParameteri(GL_TEXTURE_2D, n, v);
			return true;
		}


		static GLuint current_binding()
		{
			GLuint current_tex(GL_INVALID_VALUE);
			glGetIntegerv(GL_TEXTURE_BINDING_2D, reinterpret_cast<GLint*>(&current_tex));
			return current_tex;
		}

		bool bind() const
		{
			if(m_gl_handle==GL_INVALID_VALUE)
				return false;
			glBindTexture(GL_TEXTURE_2D, m_gl_handle);
			return true;
		}

		static void bind(GLuint tex)
		{
			if(tex!=GL_INVALID_VALUE)
				glBindTexture(GL_TEXTURE_2D, tex);
		}

		math::size2ui const& get_size() const
		{
			return m_size;
		}

		bool resize(math::size2ui const &ts)
		{
			if(m_gl_handle==GL_INVALID_VALUE)
				return false;
			if(m_size==ts)
				return true;
			if(ts.empty())
				return false;
			m_size=ts;
			
			glTexImage2D(GL_TEXTURE_2D,
				0,
				pixel_traits<GILPT>::PIXEL_FORMAT,
				m_size.width(),
				m_size.height(),
				0,
				pixel_traits<GILPT>::PIXEL_FORMAT,
				pixel_traits<GILPT>::PIXEL_TYPE,
				0
			);

			return true;
		}

		bool upload(GLint level, point2ui const &o, view_t d)
		{
			if(m_gl_handle==GL_INVALID_VALUE)
				return false;
			glTexSubImage2D(
				GL_TEXTURE_2D,
				level,
				o.x, o.y,
				d.width(), d.height(),
				pixel_traits<GILPT>::PIXEL_FORMAT,
				pixel_traits<GILPT>::PIXEL_TYPE,
				&d[0][0]
			);
			return true;
		}

		bool attach(frame_buffer const &fb) const
		{
			if(m_gl_handle==GL_INVALID_VALUE)
				return false;
			glFramebufferTexture2D(
				GL_FRAMEBUFFER,
				GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_2D,
				m_gl_handle,
				0
			);
			return true;
		}

	
	private:
		math::size2ui	m_size;
	};

	typedef texture_2d<ucv::gil::rgba8_pixel_t> rgba8_texture_2d;

} //namespace gl
} //namespace ucv
} //namespace baldzarika

#endif //HAVE_GLES2

#endif //BALDZARIKA_UCV_GLES_TEXTURE_H