#include "Prec.h"
#include "FiducialTestPreviewWidget.h"

#define FIDUCIAL_PREVIEW_WIDTH 640
#define FIDUCIAL_PREVIEW_HEIGHT 480

FiducialTestPreviewWidget::FiducialTestPreviewWidget(QWidget *parent/* =0 */)
	: QGLWidget(parent)
	, m_is_detected(false)
{
	setFixedSize(FIDUCIAL_PREVIEW_WIDTH,FIDUCIAL_PREVIEW_HEIGHT);
	
	m_frame.recreate(FIDUCIAL_PREVIEW_WIDTH,FIDUCIAL_PREVIEW_HEIGHT);
	m_gray_frame.recreate(FIDUCIAL_PREVIEW_WIDTH,FIDUCIAL_PREVIEW_HEIGHT);
		
	m_vi.setIdealFramerate(0, 30);
	m_vi.setupDevice(0, FIDUCIAL_PREVIEW_WIDTH, FIDUCIAL_PREVIEW_HEIGHT);

	m_tracker.reset(new ar::fiducial::tracker(math::size2ui(FIDUCIAL_PREVIEW_WIDTH,FIDUCIAL_PREVIEW_HEIGHT)));
	m_tracker->set_camera_z_far(1300.0f);
	

#if 1
	m_tracker->add_marker_model(
		boost::shared_ptr<ar::fiducial::marker_model>(
			new ar::fiducial::bch_marker_model()
		)
	);
#else
	m_tracker->add_marker_model(
		boost::shared_ptr<ar::fiducial::marker_model>(
			new ar::fiducial::qr_marker_model()
		)
	);
#endif

	m_tracker->marker_state_changed().connect(
		boost::bind(&FiducialTestPreviewWidget::onMarkerStateChanged, this, _1, _2)
	);

	m_tracker->start();

	m_render_timer= new QTimer(this);

	connect(m_render_timer, SIGNAL(timeout()), this, SLOT(update()));

	m_render_timer->start(0);
}

FiducialTestPreviewWidget::~FiducialTestPreviewWidget()
{
	m_tracker->stop();
	m_tracker->wait_to_stop();

	m_render_timer->stop();
	m_vi.stopDevice(0);
}

void FiducialTestPreviewWidget::onMarkerStateChanged(boost::shared_ptr<ar::tracker::marker_state> const &ms, ar::tracker::marker_state::eSC sc)
{
	if(boost::shared_ptr<ar::fiducial::tracker::marker_state> fms=boost::shared_ptr<ar::fiducial::tracker::marker_state>(ms, boost::detail::dynamic_cast_tag()))
	{
		if(!m_is_detected && fms->is_detected())
		{
			m_marker_id=fms->get_marker_id();
			m_is_detected=true;
			m_marker_size=fms->get_marker_size();
		}

		if(m_is_detected && fms->get_marker_id()==m_marker_id)
		{
			m_is_detected=fms->is_detected();
			m_model_view=fms->get_camera_pose();
		}
	}

}

void FiducialTestPreviewWidget::initializeGL()
{
	glGenTextures(1, &m_video_texture);
	glBindTexture(GL_TEXTURE_2D, m_video_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
		FIDUCIAL_PREVIEW_WIDTH,
		FIDUCIAL_PREVIEW_HEIGHT,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		0
	);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void FiducialTestPreviewWidget::resizeGL(int width,int height)
{
	glViewport(0,0,width,height);
}

void FiducialTestPreviewWidget::paintGL()
{
	if(m_vi.isFrameNew(0))
	{
		m_vi.getPixels(0, &ucv::gil::view(m_frame)[0][0], true, true);

		ucv::gil::copy_and_convert_pixels(
			ucv::gil::const_view(m_frame),
			ucv::gil::view(m_gray_frame)
		);

		m_tracker->process_frame(ucv::gil::const_view(m_gray_frame));
		
		glBindTexture(GL_TEXTURE_2D, m_video_texture);
		glTexSubImage2D(
			GL_TEXTURE_2D,
			0,
			0, 0,
			FIDUCIAL_PREVIEW_WIDTH, FIDUCIAL_PREVIEW_HEIGHT,
			GL_RGB,
			GL_UNSIGNED_BYTE,
			&ucv::gil::view(m_frame)[0][0]
		);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

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

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_video_texture);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS) ;
	{
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,-1.0f, 0.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,-1.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, 1.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, 1.0f, 0.0f);
	}
	glEnd();

	if(m_is_detected)
	{
		glEnable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
		glPointSize(4.0f);

		GLfloat model_view[16]=
		{
			m_model_view(0,0), m_model_view(1,0), m_model_view(2,0), m_model_view(3,0),
			m_model_view(0,1), m_model_view(1,1), m_model_view(2,1), m_model_view(3,1),
			m_model_view(0,2), m_model_view(1,2), m_model_view(2,2), m_model_view(3,2),
			m_model_view(0,3), m_model_view(1,3), m_model_view(2,3), m_model_view(3,3)
		};

		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(model_view);
		
		math::matrix44f const &proj=m_tracker->get_camera_projection();
		GLfloat projection[16]=
		{
			proj(0,0), proj(1,0), proj(2,0), proj(3,0),
			proj(0,1), proj(1,1), proj(2,1), proj(3,1),
			proj(0,2), proj(1,2), proj(2,2), proj(3,2),
			proj(0,3), proj(1,3), proj(2,3), proj(3,3)
		};

		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(projection);

		float half_width=float(m_marker_size.width())*0.5f;
		float half_length=float(m_marker_size.height())*0.5f;
		
		float half_height=std::sqrt(float(m_marker_size.area()))*0.5f;

		float low_corners[]=
		{
			-half_width,  half_length, -half_height,
			 half_width,  half_length, -half_height,
			 half_width, -half_length, -half_height,
			-half_width, -half_length, -half_height
		};

		float high_corners[]=
		{
			-half_width,  half_length,  half_height,
			 half_width,  half_length,  half_height,
			 half_width, -half_length,  half_height,
			-half_width, -half_length,  half_height
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