#include "Prec.h"
#include "SURFTestWidget.h"

#define SURF_TEST_WIDTH 320
#define SURF_TEST_HEIGHT 240


SURFTestWidget::SURFTestWidget(QWidget *parent/* =0 */)
	: QGLWidget(parent)
	, m_scene_surf(math::size2ui(SURF_TEST_WIDTH,SURF_TEST_HEIGHT),3,4,2,4.0e-4f)
	, m_MinScale(0.0f)
	, m_MaxScale(20.0f)
{
	ucv::gil::gray8_image_t gray8_obj_img;
	ucv::gil::png_read_and_convert_image("../lib_ucv_test/test_img.png", gray8_obj_img);

	ucv::surf::integral_image_t gray_obj_img(gray8_obj_img.width(), gray8_obj_img.height());
	
	ucv::surf::integral_t median;
	ucv::convert(
		ucv::gil::view(gray8_obj_img),
		ucv::gil::view(gray_obj_img),
		ucv::detail::grayscale_convert_and_median<ucv::surf::integral_t>(
			median,
			gray8_obj_img.width(),
			gray8_obj_img.height()
		)
	);

	ucv::surf::integral_image_t gray_obj_int_img(gray8_obj_img.width(), gray8_obj_img.height());

	ucv::integral(ucv::gil::const_view(gray_obj_img), ucv::gil::view(gray_obj_int_img), median);

	ucv::surf obj_surf(math::size2ui(gray8_obj_img.width(), gray8_obj_img.height()), 3, 4, 2, 1.0e-4f);
	obj_surf.set_integral_view(ucv::gil::const_view(gray_obj_int_img));
	obj_surf.build_response_layers();
	obj_surf.detect(m_obj_features);
	obj_surf.describe(m_obj_features);
	m_obj_features.optimise();



	setFixedSize(SURF_TEST_WIDTH,SURF_TEST_HEIGHT);
	
	m_frame.recreate(SURF_TEST_WIDTH,SURF_TEST_HEIGHT);
	m_gray8_frame.recreate(SURF_TEST_WIDTH,SURF_TEST_HEIGHT);
	m_gray_frame.recreate(SURF_TEST_WIDTH,SURF_TEST_HEIGHT);
	m_integral_frame.recreate(SURF_TEST_WIDTH,SURF_TEST_HEIGHT);
	
	m_vi.setIdealFramerate(0, 30);
	m_vi.setupDevice(0, SURF_TEST_WIDTH, SURF_TEST_HEIGHT);

	m_render_timer= new QTimer(this);

	connect(m_render_timer, SIGNAL(timeout()), this, SLOT(update()));

	m_render_timer->start(0);
}

SURFTestWidget::~SURFTestWidget()
{
	m_render_timer->stop();
	m_vi.stopDevice(0);
}

void SURFTestWidget::initializeGL()
{
	glGenTextures(1, &m_video_texture);
	glBindTexture(GL_TEXTURE_2D, m_video_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
		SURF_TEST_WIDTH,
		SURF_TEST_HEIGHT,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		0
	);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void SURFTestWidget::resizeGL(int width,int height)
{
	glViewport(0,0,width,height);
}

void SURFTestWidget::paintGL()
{
	if(m_vi.isFrameNew(0))
	{
		m_vi.getPixels(0, &ucv::gil::view(m_frame)[0][0], true, true);

		ucv::gil::copy_and_convert_pixels(
			ucv::gil::const_view(m_frame),
			ucv::gil::view(m_gray8_frame)
		);

		ucv::surf::integral_t median;
		ucv::convert(
			ucv::gil::const_view(m_gray8_frame),
			ucv::gil::view(m_gray_frame),
			ucv::detail::grayscale_convert_and_median<ucv::surf::integral_t>(
				median,
				m_frame.width(),
				m_frame.height()
			)
		);
		ucv::integral(ucv::gil::const_view(m_gray_frame), ucv::gil::view(m_integral_frame), median);
		m_scene_surf.set_integral_view(ucv::gil::const_view(m_integral_frame));
		m_scene_surf.build_response_layers();
		m_scene_surf.detect(m_scene_features);
		m_scene_surf.describe(m_scene_features);

		typedef  std::vector<
			std::pair<
				ucv::surf::fps_by_desc_tree_t::const_iterator,
				ucv::surf::fps_by_pos_tree_t::const_iterator
			>
		> marker_matches_t;

		marker_matches_t marker_matches;

		ucv::match_feature_points<
			ucv::surf::feature_point_t,
			ucv::surf::fps_by_desc_tree_t,
			ucv::surf::fps_by_pos_tree_t
		>(m_obj_features, m_scene_features, marker_matches, 0.6f);

		m_matched_points.clear();


		for(marker_matches_t::const_iterator imm=marker_matches.begin();imm!=marker_matches.end();++imm)
			m_matched_points.insert(*imm->second);




		glBindTexture(GL_TEXTURE_2D, m_video_texture);
		glTexSubImage2D(
			GL_TEXTURE_2D,
			0,
			0, 0,
			SURF_TEST_WIDTH, SURF_TEST_HEIGHT,
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


	if(!m_scene_features.empty())
	{
		glEnable(GL_BLEND);
		glDisable(GL_TEXTURE_2D);
		glLineWidth(1.0f);

#if 1
		for(matched_points_t::const_iterator imp=m_matched_points.begin();imp!=m_matched_points.end();++imp)
		{
			ucv::surf::feature_point_t const &feature=*imp;
			float scale=2.0f*static_cast<float>(feature.m_scale);

			if(scale<m_MinScale || scale>m_MaxScale) continue;


			float laplacian=static_cast<float>(feature.m_laplacian);
			
			float orientation=static_cast<float>(feature.m_orientation);

			float x=static_cast<float>(feature.x());
			float y=static_cast<float>(feature.y());

			
			if(laplacian<=0.0f)
				glColor4f(0.0f,0.0f,1.0f,1.0f);
			else
				glColor4f(1.0f,0.0f,0.0f,1.0f);

			//draw circle
			glBegin(GL_LINE_LOOP);
			for(boost::uint32_t a=0;a<36;++a)
			{
				float angle=float(a)/36.0f*math::constant::pi_2<float>();
				float sin_a=std::sin(angle);
				float cos_a=std::cos(angle);

				float const vertex[3]=
				{
					 2.0f*((x+scale*cos_a)/float(SURF_TEST_WIDTH)-0.5f),
					-2.0f*((y+scale*sin_a)/float(SURF_TEST_HEIGHT)-0.5f),
					0.0f
				};
				glVertex3fv(vertex);
			}
			glEnd();

			glBegin(GL_LINES);
			{
				glVertex3f(
					2.0f*(x/float(SURF_TEST_WIDTH)-0.5f),
					-2.0f*(y/float(SURF_TEST_HEIGHT)-0.5f),
					0.0f
				);

				glVertex3f(
					2.0f*((x+scale*std::cos(orientation))/float(SURF_TEST_WIDTH)-0.5f),
					-2.0f*((y+scale*std::sin(orientation))/float(SURF_TEST_HEIGHT)-0.5f),
					0.0f
				);
			}
			glEnd();
		}
#endif

#if 0
		for(int i=0;i<4;++i)
		{
			glBegin(GL_LINES);
			
			glColor4fv(colors+i*4);
			glVertex3fv(low_corners+i*3);

			glColor4fv(colors+((i+1)%4)*4);
			glVertex3fv(low_corners+((i+1)%4)*3);

			glEnd();
		}
	}
#endif
	}
}

void SURFTestWidget::setMinMaxScale(float minScale, float maxScale)
{
	m_MinScale=minScale;
	m_MaxScale=maxScale;
}
