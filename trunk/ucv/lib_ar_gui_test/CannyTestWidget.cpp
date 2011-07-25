#include "Prec.h"
#include "CannyTestWidget.h"

#define CANNY_TEST_WIDTH 640
#define CANNY_TEST_HEIGHT 480


CannyTestWidget::CannyTestWidget(QWidget *parent/* =0 */)
	: QGLWidget(parent)
	, m_frame(CANNY_TEST_WIDTH,CANNY_TEST_HEIGHT)
	, m_gray8_frame(CANNY_TEST_WIDTH,CANNY_TEST_HEIGHT)
	, m_gray_frame(CANNY_TEST_WIDTH,CANNY_TEST_HEIGHT)
	, m_gray_blurred_frame(CANNY_TEST_WIDTH,CANNY_TEST_HEIGHT)
	, m_gaussian_blur(math::size2ui(CANNY_TEST_WIDTH,CANNY_TEST_HEIGHT))
	, m_canny(math::size2ui(CANNY_TEST_WIDTH, CANNY_TEST_HEIGHT), 0.1, 0.3)
{
	setFixedSize(CANNY_TEST_WIDTH,CANNY_TEST_HEIGHT);

	m_vi.setIdealFramerate(0, 30);
	m_vi.setupDevice(0, CANNY_TEST_WIDTH, CANNY_TEST_HEIGHT);

	m_render_timer=new QTimer(this);

	connect(m_render_timer, SIGNAL(timeout()), this, SLOT(update()));

	m_render_timer->start(0);
}

CannyTestWidget::~CannyTestWidget()
{
	m_render_timer->stop();
	m_vi.stopDevice(0);
}

void CannyTestWidget::initializeGL()
{
	glGenTextures(1, &m_video_texture);
	glBindTexture(GL_TEXTURE_2D, m_video_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
		CANNY_TEST_WIDTH,
		CANNY_TEST_HEIGHT,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		0
	);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void CannyTestWidget::resizeGL(int width, int height)
{
	glViewport(0,0,width,height);
}

void CannyTestWidget::doCanny()
{
	ucv::gil::copy_and_convert_pixels(
		ucv::gil::const_view(m_frame),
		ucv::gil::view(m_gray8_frame)
	);

	ucv::convert(
		ucv::gil::const_view(m_gray8_frame),
		ucv::gil::view(m_gray_frame),
		ucv::detail::grayscale_convert()
	);

	m_gaussian_blur(ucv::gil::const_view(m_gray_frame),ucv::gil::view(m_gray_blurred_frame));

	m_canny(ucv::gil::const_view(m_gray_blurred_frame), m_contours);

	std::list<contour_t>::iterator ic=m_contours.begin();

	float approx=0.007f*std::sqrt(float(CANNY_TEST_WIDTH*CANNY_TEST_HEIGHT));
	while(ic!=m_contours.end())
	{
		contour_t &contour=*ic;
		contour.aproximate(approx);

		if(!(contour.m_is_closed && contour.m_points.size()==4 && contour.m_is_convex))
		{
			ic=m_contours.erase(ic);
			continue;
		}
		ucv::corners_subpixel(ucv::gil::const_view(m_gray_blurred_frame),contour.m_points);
		ic++;
	}
	std::list<ar::fiducial::marker_model::detect_info> di;
	m_bch_marker_model.detect_markers(ucv::gil::const_view(m_gray_blurred_frame),m_contours,di);


	static boost::uint32_t n_empty=0;

	if(di.empty() && n_empty>=0)
	{
		n_empty++;
		di.clear();
	}

}

void CannyTestWidget::paintGL()
{
	if(m_vi.isFrameNew(0))
	{
		m_vi.getPixels(0, &ucv::gil::view(m_frame)[0][0], true, true);

		doCanny();


		glBindTexture(GL_TEXTURE_2D, m_video_texture);
		glTexSubImage2D(
			GL_TEXTURE_2D,
			0,
			0, 0,
			CANNY_TEST_WIDTH, CANNY_TEST_HEIGHT,
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

	if(!m_contours.empty())
	{
		glEnable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
		glLineWidth(1.0f);

		glColor4f(0.0f,1.0f,0.0f,1.0f);
		
		for(std::list<contour_t>::iterator ic=m_contours.begin();ic!=m_contours.end();++ic)
		{
			contour_t &contour=*ic;

			glBegin(GL_LINE_LOOP);
			for(boost::uint32_t cp=0;cp<contour.m_points.size();++cp)
			{
				float const vertex[3]=
				{
					+2.0f*(float(contour.m_points[cp].x())/float(CANNY_TEST_WIDTH)-0.5f),
					-2.0f*(float(contour.m_points[cp].y())/float(CANNY_TEST_HEIGHT)-0.5f),
					0.0f
				};
				glVertex3fv(vertex);
			}
			glEnd();
		}
	}
}
