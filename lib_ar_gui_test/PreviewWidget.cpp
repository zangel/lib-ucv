#include "Prec.h"
#include "PreviewWidget.h"

PreviewWidget::PreviewWidget(QWidget *parent/* =0 */)
	: QGLWidget(parent)
	, m_VideoTexture(GL_INVALID_VALUE)
	, m_VideoTextureDirty(false)
	, m_VideoTextureSize(0,0)
	, m_Detected(false)
	, m_CameraPose(math::matrix44f::identity())
	, m_CameraProjection(math::matrix44f::identity())
	, m_MarkerSize(0,0)
{

}

PreviewWidget::~PreviewWidget()
{

}

void PreviewWidget::setVideoTextureView(ucv::gil::rgb8c_view_t rgb8v)
{
	boost::mutex::scoped_lock lock_vt(m_VideoTextureSync);
	m_VideoTextureView=rgb8v;
	m_VideoTextureDirty=true;
	update();
}

void PreviewWidget::setDetectedCameraPose(bool detected, math::matrix44f const &cpose, math::matrix44f const &cproj, math::size2ui const &ms)
{
	boost::mutex::scoped_lock lock_cp(m_CameraPoseSync);
	m_Detected=detected;
	m_CameraPose=cpose;
	m_CameraProjection=cproj;
	m_MarkerSize=ms;
}

void PreviewWidget::initializeGL()
{
	glGenTextures(1, &m_VideoTexture);
	if(m_VideoTexture!=GL_INVALID_VALUE)
	{
		glBindTexture(GL_TEXTURE_2D, m_VideoTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void PreviewWidget::resizeGL(int width,int height)
{
	glViewport(0,0,width,height);
}

void PreviewWidget::paintGL()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(-1, 1, -1, 1, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	glDisable(GL_NORMALIZE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	

	math::size2ui vtex_size(0,0);
	{
		boost::mutex::scoped_lock lock_vt(m_VideoTextureSync);
		math::size2ui view_size(m_VideoTextureView.width(),m_VideoTextureView.height());
		if(m_VideoTextureDirty)
		{
			glBindTexture(GL_TEXTURE_2D, m_VideoTexture);
			if(m_VideoTextureSize!=view_size)
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
					view_size.width(),
					view_size.height(),
					0,
					GL_RGB,
					GL_UNSIGNED_BYTE,
					&m_VideoTextureView[0][0]
				);
			}
			else
			{
				glTexSubImage2D(
					GL_TEXTURE_2D,
					0,
					0, 0,
					view_size.width(), view_size.height(),
					GL_RGB,
					GL_UNSIGNED_BYTE,
					&m_VideoTextureView[0][0]
				);
			}
			glBindTexture(GL_TEXTURE_2D, 0);
			m_VideoTextureSize=view_size;
			m_VideoTextureDirty=false;
		}
		vtex_size=m_VideoTextureSize;
	}

	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_VideoTexture);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS) ;
	{
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,-1.0f, 0.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,-1.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, 1.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, 1.0f, 0.0f);
	}
	glEnd();

	glEnable(GL_BLEND);
	{
		boost::mutex::scoped_lock lock_tfs(m_CameraPoseSync);
		if(m_Detected)
		{
			glEnable(GL_BLEND);
			glDisable(GL_TEXTURE_2D);
			glPointSize(4.0f);

			GLfloat model_view[16]=
			{
				m_CameraPose(0,0), m_CameraPose(1,0), m_CameraPose(2,0), m_CameraPose(3,0),
				m_CameraPose(0,1), m_CameraPose(1,1), m_CameraPose(2,1), m_CameraPose(3,1),
				m_CameraPose(0,2), m_CameraPose(1,2), m_CameraPose(2,2), m_CameraPose(3,2),
				m_CameraPose(0,3), m_CameraPose(1,3), m_CameraPose(2,3), m_CameraPose(3,3)
			};

			glMatrixMode(GL_MODELVIEW);
			glLoadMatrixf(model_view);
			
			GLfloat projection[16]=
			{
				m_CameraProjection(0,0), m_CameraProjection(1,0), m_CameraProjection(2,0), m_CameraProjection(3,0),
				m_CameraProjection(0,1), m_CameraProjection(1,1), m_CameraProjection(2,1), m_CameraProjection(3,1),
				m_CameraProjection(0,2), m_CameraProjection(1,2), m_CameraProjection(2,2), m_CameraProjection(3,2),
				m_CameraProjection(0,3), m_CameraProjection(1,3), m_CameraProjection(2,3), m_CameraProjection(3,3)
			};

			glMatrixMode(GL_PROJECTION);
			glLoadMatrixf(projection);

			float low_corners[]=
			{
				-float(m_MarkerSize.width())*0.5f,  float(m_MarkerSize.height())*0.5f, -std::sqrt(float(m_MarkerSize.area()))*0.5f,
				 float(m_MarkerSize.width())*0.5f,  float(m_MarkerSize.height())*0.5f, -std::sqrt(float(m_MarkerSize.area()))*0.5f,
				 float(m_MarkerSize.width())*0.5f, -float(m_MarkerSize.height())*0.5f, -std::sqrt(float(m_MarkerSize.area()))*0.5f,
				-float(m_MarkerSize.width())*0.5f, -float(m_MarkerSize.height())*0.5f, -std::sqrt(float(m_MarkerSize.area()))*0.5f
			};

			float high_corners[]=
			{
				-float(m_MarkerSize.width())*0.5f,  float(m_MarkerSize.height())*0.5f, std::sqrt(float(m_MarkerSize.area()))*0.5f,
				 float(m_MarkerSize.width())*0.5f,  float(m_MarkerSize.height())*0.5f, std::sqrt(float(m_MarkerSize.area()))*0.5f,
				 float(m_MarkerSize.width())*0.5f, -float(m_MarkerSize.height())*0.5f, std::sqrt(float(m_MarkerSize.area()))*0.5f,
				-float(m_MarkerSize.width())*0.5f, -float(m_MarkerSize.height())*0.5f, std::sqrt(float(m_MarkerSize.area()))*0.5f
			};

			float colors[]=
			{
				1.0f, 0.0f, 0.0f, 1.0f,
				0.0f, 1.0f, 0.0f, 1.0f,
				0.0f, 0.0f, 1.0f, 1.0f,
				1.0f, 1.0f, 0.0f, 1.0f
			};

			for(int i=0;i<4;++i)
			{
				glBegin(GL_LINES);
			
				glColor4fv(colors+i*4);
				glVertex3fv(low_corners+i*3);

				glColor4fv(colors+((i+1)%4)*4);
				glVertex3fv(low_corners+((i+1)%4)*3);

				glEnd();

#if 1
				glBegin(GL_LINES);
				glColor4fv(colors+i*4);
				glVertex3fv(high_corners+i*3);

				glColor4fv(colors+((i+1)%4)*4);
				glVertex3fv(high_corners+((i+1)%4)*3);
				glEnd();

				glBegin(GL_LINES);
				glColor4fv(colors+i*4);
				glVertex3fv(low_corners+i*3);
				glVertex3fv(high_corners+i*3);
				glEnd();
#endif
			}
		}
	}
	glDisable(GL_BLEND);
}