#include "Prec.h"
#include "PreviewWidget.h"

PreviewWidget::PreviewWidget(QWidget *parent/* =0 */)
	: QGLWidget(parent)
	, m_VideoTexture(GL_INVALID_VALUE)
	, m_VideoTextureDirty(false)
	, m_VideoTextureSize(0,0)
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

void PreviewWidget::setTrackingFeatures(ar::tracker::marker_state::points2_t const &tfs)
{
	boost::mutex::scoped_lock lock_tfs(m_TrackingFeaturesSync);
	m_TrackingFeatures=tfs;
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
	

	ucv::size2ui vtex_size(0,0);
	{
		boost::mutex::scoped_lock lock_vt(m_VideoTextureSync);
		ucv::size2ui view_size(m_VideoTextureView.width(),m_VideoTextureView.height());
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

	glColor4f(1.0f, 1.0f, 1.0f, 1.0);
	glBegin(GL_QUADS) ;
	{
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,-1.0f, 0.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,-1.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, 1.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, 1.0f, 0.0f);
	}
	glEnd();


	{
		boost::mutex::scoped_lock lock_tfs(m_TrackingFeaturesSync);
		if(!m_TrackingFeatures.empty())
		{
			boost::scoped_array<float> feature_coords( new float[3*m_TrackingFeatures.size()] );
			float *p_coords=feature_coords.get();
			for(std::size_t p=0;p<m_TrackingFeatures.size();++p)
			{
				p_coords[0]= 2.0f*(static_cast<float>(m_TrackingFeatures[p].x)/float(vtex_size.width())-0.5f);
				p_coords[1]=-2.0f*(static_cast<float>(m_TrackingFeatures[p].y)/float(vtex_size.height())-0.5f);
				p_coords[2]=0.0;
				p_coords+=3;
			}
			
			glEnableClientState(GL_VERTEX_ARRAY);
			glDisable(GL_TEXTURE_2D);

			glColor4ub(0, 255, 0, 127);
			glPointSize(4.0f);
			glVertexPointer(3, GL_FLOAT, 0, feature_coords.get());
			glDrawArrays(GL_POINTS, 0, m_TrackingFeatures.size());
		}
	}
}