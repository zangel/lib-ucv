#include "UCVActivity.h"
#include "baldzarika_ucv_test_UCVActivity.h"
#include <android/graphics/PixelFormat.hpp>
#include <android/view/ViewGroup.hpp>
//#define BALDZARIKA_UCV_FIXED_POINT_TRIGONO_USE_FPU
#include <baldzarika/ucv/config.h>
#include <baldzarika/ucv/surf.h>
#include <baldzarika/ucv/homography.h>
#include <baldzarika/ucv/integral.h>
#include <baldzarika/ucv/convert_scale.h>
#include <baldzarika/ucv/gles/shaders.h>
#include <baldzarika/ucv/klt_tracker.h>
#include <boost/date_time.hpp>

#define png_infopp_NULL (png_infopp)0
#define int_p_NULL (int*)0

#include <boost/gil/extension/io/png_io.hpp>

using namespace j2cpp;
namespace ucv=baldzarika::ucv;

void Java_baldzarika_ucv_test_UCVActivity_initialize(JNIEnv *, jobject jUCVActivity)
{
	UCVActivity(jUCVActivity).initialize();
}

void Java_baldzarika_ucv_test_UCVActivity_terminate(JNIEnv *, jobject jUCVActivity)
{
	UCVActivity(jUCVActivity).terminate();
}

void Java_baldzarika_ucv_test_UCVActivity_onSurfaceCreated(JNIEnv *, jobject jUCVActivity, jobject gl, jobject config)
{
	UCVActivity(jUCVActivity).onSurfaceCreated(
		local_ref<egl::EGL10>(gl),
		local_ref<egl::EGLConfig>(config)
	);
}

void Java_baldzarika_ucv_test_UCVActivity_onSurfaceChanged(JNIEnv *, jobject jUCVActivity, jobject gl, jint width, jint height)
{
	UCVActivity(jUCVActivity).onSurfaceChanged(
		local_ref<egl::EGL10>(gl),
		width,
		height
	);
}

jboolean Java_baldzarika_ucv_test_UCVActivity_onTouchEvent(JNIEnv *, jobject jUCVActivity, jobject me)
{
	return UCVActivity(jUCVActivity).onTouchEvent(local_ref<MotionEvent>(me))?JNI_TRUE:JNI_FALSE;
}

void Java_baldzarika_ucv_test_UCVActivity_onDrawFrame(JNIEnv *, jobject jUCVActivity, jobject gl)
{
	UCVActivity(jUCVActivity).onDrawFrame(local_ref<egl::EGL10>(gl));
}

void Java_baldzarika_ucv_test_UCVActivity_surfaceChanged(JNIEnv *, jobject jUCVActivity, jobject holder, jint format, jint width, jint height)
{
	UCVActivity(jUCVActivity).surfaceChanged(local_ref<SurfaceHolder>(holder), format, width, height);
}

void Java_baldzarika_ucv_test_UCVActivity_surfaceCreated(JNIEnv *, jobject jUCVActivity, jobject holder)
{
	UCVActivity(jUCVActivity).surfaceCreated(local_ref<SurfaceHolder>(holder));
}

void Java_baldzarika_ucv_test_UCVActivity_surfaceDestroyed(JNIEnv *, jobject jUCVActivity, jobject holder)
{
	UCVActivity(jUCVActivity).surfaceDestroyed(local_ref<SurfaceHolder>(holder));
}

void Java_baldzarika_ucv_test_UCVActivity_onPreviewFrame(JNIEnv *, jobject jUCVActivity, jbyteArray data, jobject camera)
{
	UCVActivity(jUCVActivity).onPreviewFrame(local_ref< array<jbyte,1> >(data), local_ref<Camera>(camera));
}

namespace {

	static char const *fp_vs_src=
		"attribute vec4 a_vtx_pos;\n"
		"\n"
		"void main()\n"
		"{\n"
		"	gl_Position=a_vtx_pos;\n"
		"	gl_PointSize=4.0;\n"
		"}\n"
	;

	static char const *fp_fs_src=
		"precision mediump float;\n"
		"void main()\n"
		"{\n"
		"	gl_FragColor=vec4(1.0, 0.0, 0.0, 0.5);\n"
		"}\n"
	;

	class Native
	{
	public:
		typedef ucv::surf::feature_point_t::value_type value_t;

		typedef ucv::fixed_point<10, 21> gray_t;
		typedef ucv::gil::pixel<gray_t, ucv::gil::gray_layout_t> gray_pixel_t;
		typedef ucv::gil::image< gray_pixel_t, false, std::allocator<unsigned char> > gray_image_t;
		typedef gray_image_t::view_t gray_view_t;

		typedef ucv::klt_tracker<ucv::surf::integral_t::IS,ucv::surf::integral_t::FS> klt_tracker_t;

		Native()
		: m_homography_ok(false)
		, m_curr(0)
		{
			ucv::gil::gray8_image_t marker_img;
			ucv::gil::png_read_and_convert_image("/sdcard/markers/joker.png", marker_img);
			if(marker_img.width()*marker_img.height())
			{
				m_marker_size=ucv::size2ui(marker_img.width(), marker_img.height());
				__android_log_print(ANDROID_LOG_INFO, J2CPP_NAME, "marker image loaded, size=%dx%d, ...",marker_img.width(), marker_img.height());

				gray_image_t marker_gray_img(marker_img.width(), marker_img.height());

				ucv::convert_scale(
					ucv::gil::view(marker_img),
					ucv::gil::view(marker_gray_img),
					ucv::surf::integral_t(1.0f/255.0f)
				);
				ucv::surf::integral_image_t marker_integral_img(marker_img.width(), marker_img.height());
				ucv::integral(
					ucv::gil::const_view(marker_gray_img),
					ucv::gil::view(marker_integral_img)
				);
				ucv::surf surf_marker(ucv::size2ui(marker_img.width(), marker_img.height()), 3, 4, 2, 1.0e-4f);
				surf_marker.set_integral_view(ucv::gil::const_view(marker_integral_img));
				surf_marker.build_response_layers();
				surf_marker.detect(m_marker_features);
				surf_marker.describe(m_marker_features);
				__android_log_print(ANDROID_LOG_INFO, J2CPP_NAME, "detected %d marker feature points", m_marker_features.size());
			}
			else
				__android_log_print(ANDROID_LOG_ERROR, J2CPP_NAME, "failed to load marker image!");
		}

		~Native()
		{
		}

		//marker stuff
		std::vector<ucv::surf::feature_point_t> m_marker_features;
		std::vector<ucv::surf::feature_point_t::point2_t> m_next_pts;
		ucv::size2ui m_marker_size;

		//detection stuff
		boost::scoped_ptr<ucv::surf> m_surf;
		boost::scoped_ptr<klt_tracker_t> m_tracker;
		gray_image_t m_gray_img;
		std::vector<ucv::surf::feature_point_t> m_features;
		std::vector< std::pair<std::size_t,std::size_t> > m_feature_matches;
		bool m_homography_ok;
		boost::uint32_t	m_curr;
		ucv::matrix33f	m_homography[2];
		ucv::surf::integral_image_t m_integral_img[2];

		boost::scoped_ptr<ucv::gles::program> m_features_program;

	};

} //namespace anonymous


J2CPP_DEFINE_CLASS(UCVActivity,"baldzarika/ucv/test/UCVActivity")
J2CPP_DEFINE_METHOD(UCVActivity,0,"makeContextFactory","()Landroid/opengl/GLSurfaceView$EGLContextFactory;")
J2CPP_DEFINE_METHOD(UCVActivity,1,"makeConfigChooser","(IIIIII)Landroid/opengl/GLSurfaceView$EGLConfigChooser;")
J2CPP_DEFINE_FIELD(UCVActivity,0,"m_GLView","Landroid/opengl/GLSurfaceView;")
J2CPP_DEFINE_FIELD(UCVActivity,1,"m_CameraPreview","Landroid/view/SurfaceView;")
J2CPP_DEFINE_FIELD(UCVActivity,2,"m_Camera","Landroid/hardware/Camera;")
J2CPP_DEFINE_FIELD(UCVActivity,3,"m_Native","J")

UCVActivity::operator local_ref<Activity>() const
{
	return local_ref<Activity>(get_jobject());
}

UCVActivity::operator local_ref<SurfaceHolder::Callback>() const
{
	return local_ref<SurfaceHolder::Callback>(get_jobject());
}

UCVActivity::operator local_ref<Camera::PreviewCallback>() const
{
	return local_ref<Camera::PreviewCallback>(get_jobject());
}

UCVActivity::operator local_ref<GLSurfaceView::Renderer>() const
{
	return local_ref<GLSurfaceView::Renderer>(get_jobject());
}

UCVActivity::operator local_ref<Context>() const
{
	return local_ref<Context>(get_jobject());
}

local_ref<GLSurfaceView::EGLContextFactory> UCVActivity::makeContextFactory()
{
	return call_method<
		UCVActivity::J2CPP_CLASS_NAME,
		UCVActivity::J2CPP_METHOD_NAME(0),
		UCVActivity::J2CPP_METHOD_SIGNATURE(0),
		local_ref<GLSurfaceView::EGLContextFactory>
	>(get_jobject());
}

local_ref<GLSurfaceView::EGLConfigChooser> UCVActivity::makeConfigChooser(int r, int g, int b, int a, int depth, int stencil)
{
	return call_method<
		UCVActivity::J2CPP_CLASS_NAME,
		UCVActivity::J2CPP_METHOD_NAME(1),
		UCVActivity::J2CPP_METHOD_SIGNATURE(1),
		local_ref<GLSurfaceView::EGLConfigChooser>
	>(get_jobject(), r, g, b, a, depth, stencil);
}


UCVActivity::UCVActivity(jobject jobj)
: j2cpp::object<UCVActivity>(jobj)
, m_CameraPreview(jobj)
, m_GLView(jobj)
, m_Camera(jobj)
, m_Native(jobj)
{
}

void UCVActivity::initialize()
{
	using namespace j2cpp;

	Native *pn=new Native();
	m_Native=reinterpret_cast<jlong>(pn);

	local_ref<GLSurfaceView> pGLView=GLSurfaceView(*this);
	m_GLView=pGLView;
	pGLView->setEGLConfigChooser(makeConfigChooser(8,8,8,8,16,0));
	pGLView->setEGLContextFactory(makeContextFactory());

	pGLView->setRenderer(*this);
	pGLView->setRenderMode(GLSurfaceView::RENDERMODE_CONTINUOUSLY);
	local_ref<SurfaceView>(pGLView)->getHolder()->setFormat(PixelFormat::TRANSPARENT);
	local_ref<Activity>(*this)->addContentView(*pGLView, ViewGroup::LayoutParams(ViewGroup::LayoutParams::FILL_PARENT, ViewGroup::LayoutParams::FILL_PARENT));

	local_ref<SurfaceView> pCameraPreview=SurfaceView(*this);
	m_CameraPreview=pCameraPreview;
	pCameraPreview->getHolder()->setFormat(PixelFormat::TRANSLUCENT);
	pCameraPreview->getHolder()->setType(SurfaceHolder::SURFACE_TYPE_PUSH_BUFFERS);
	pCameraPreview->getHolder()->addCallback(*this);
	local_ref<Activity>(*this)->addContentView(pCameraPreview, ViewGroup::LayoutParams(ViewGroup::LayoutParams::FILL_PARENT, ViewGroup::LayoutParams::FILL_PARENT));
}

void UCVActivity::terminate()
{
	if(Native *pn=reinterpret_cast<Native*>((jlong)(m_Native)))
	{
		delete pn;
		m_Native=0;
	}
}

void UCVActivity::onSurfaceCreated(local_ref<egl::EGL10> const &gl, local_ref<egl::EGLConfig> const &config)
{

}

void UCVActivity::onSurfaceChanged(local_ref<egl::EGL10> const &gl, int width, int height)
{
	glViewport(0, 0, width, height);
	if(Native *pn=reinterpret_cast<Native*>((jlong)(m_Native)))
	{
		pn->m_surf.reset(new ucv::surf(ucv::size2ui(width*2,height*2), 2, 4, 2, 1.0e-3f));
		pn->m_tracker.reset(new Native::klt_tracker_t(ucv::size2ui(width*2, height*2), ucv::size2ui(5,5), 4, 10));

		//pn->m_gil_gray_img.recreate(width*2,height*2);
		pn->m_gray_img.recreate(width*2,height*2);
		pn->m_integral_img[0].recreate(width*2,height*2);
		pn->m_integral_img[1].recreate(width*2,height*2);

		pn->m_features_program.reset(new ucv::gles::program());
		ucv::gles::vertex_shader features_vs;
		ucv::gles::fragment_shader features_fs;
		features_vs.set_source(fp_vs_src);
		features_vs.compile();
		features_fs.set_source(fp_fs_src);
		features_fs.compile();

		pn->m_features_program->attach(features_vs);
		pn->m_features_program->attach(features_fs);
		pn->m_features_program->link();
	}
}

void UCVActivity::onDrawFrame(local_ref<egl::EGL10> const &gl)
{
	if(Native *pn=reinterpret_cast<Native*>((jlong)(m_Native)))
	{
		if(pn->m_surf && pn->m_features_program)
		{
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_BLEND);
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#if 1
			if(pn->m_homography_ok)
			{
				ucv::matrix33f hm(pn->m_homography[pn->m_curr]);

				float marker_corners[4][3]=
				{
					{
						0.0f,
						0.0f,
						1.0f
					},
					{
						pn->m_marker_size.width(),
						0.0f,
						1.0f
					},
					{
						0.0f,
						pn->m_marker_size.height(),
						1.0f
					},
					{
						pn->m_marker_size.width(),
						pn->m_marker_size.height(),
						1.0f
					}
				};

				ucv::point2f image_corners[4]=
				{
					(hm*ucv::vector3f(marker_corners[0])).homogenized(),
					(hm*ucv::vector3f(marker_corners[1])).homogenized(),
					(hm*ucv::vector3f(marker_corners[2])).homogenized(),
					(hm*ucv::vector3f(marker_corners[3])).homogenized()
				};

				boost::scoped_array<float> corner_pts( new float[4*4]);
				float *p_cpts=corner_pts.get();
				for(std::size_t c=0;c<4;++c)
				{
					p_cpts[0]=2.0f*(static_cast<float>(image_corners[c][0])/float(pn->m_gray_img.width())-0.5f);
					p_cpts[1]=-2.0f*(static_cast<float>(image_corners[c][1])/float(pn->m_gray_img.height())-0.5f);
					p_cpts[2]=0.0;
					p_cpts[3]=1.0f;
					p_cpts+=4;
				}

				if(ucv::gles::vertex_shader::attribute a_vtx_pos=pn->m_features_program->get_attribute("a_vtx_pos"))
				{
					pn->m_features_program->use();
					glVertexAttribPointer(
						a_vtx_pos.get_location(),
						4,
						GL_FLOAT,
						0, 0,
						corner_pts.get()
					);
					glEnableVertexAttribArray(a_vtx_pos.get_location());
					glDrawArrays(GL_TRIANGLE_STRIP,0,4);
				}
			}
#else
			std::vector<ucv::surf::feature_point_t> fps(pn->m_features);
			//std::vector< std::pair<std::size_t,std::size_t> > matched_fps(pn->m_feature_matches);

			//if(matched_fps.size())
			if(fps.size())
			{
				//boost::scoped_array<float> fp_points( new float[4*matched_fps.size()]);
				boost::scoped_array<float> fp_points( new float[4*fps.size()]);

				float *p_fpts=fp_points.get();
				//for(std::size_t ifp=0;ifp<matched_fps.size();++ifp)
				for(std::size_t ifp=0;ifp<fps.size();++ifp)
				{
					//ucv::surf::feature_point_t const &fp=fps[matched_fps[ifp].second];
					ucv::surf::feature_point_t const &fp=fps[ifp];
					p_fpts[0]=2.0f*(static_cast<float>(fp.x)/float(pn->m_gray_img.width())-0.5f);
					p_fpts[1]=-2.0f*(static_cast<float>(fp.y)/float(pn->m_gray_img.height())-0.5f);
					p_fpts[2]=0.0;
					p_fpts[3]=1.0f;
					p_fpts+=4;
				}

				if(ucv::gles::vertex_shader::attribute a_vtx_pos=pn->m_features_program->get_attribute("a_vtx_pos"))
				{
					pn->m_features_program->use();
					glVertexAttribPointer(
						a_vtx_pos.get_location(),
						4,
						GL_FLOAT,
						0, 0,
						fp_points.get()
					);
					glEnableVertexAttribArray(a_vtx_pos.get_location());
					glDrawArrays(GL_POINTS,0,fps.size());
				}
			}
#endif
		}
	}
}

void UCVActivity::surfaceChanged(local_ref<SurfaceHolder> const &holder, int format, int width, int height)
{
	if(local_ref<Camera> pCamera=m_Camera)
	{
		local_ref<Camera::Parameters> parameters=pCamera->getParameters();
		local_ref<java::util::List> previewSizes=parameters->getSupportedPreviewSizes();
		for(int ps=0;ps<previewSizes->size();++ps)
		{
			local_ref<Camera::Size> previewSize(previewSizes->get(ps).get_jobject());
			__android_log_print(ANDROID_LOG_INFO, J2CPP_NAME, "supports: %dx%d",
					static_cast<jint>(previewSize->width),
					static_cast<jint>(previewSize->height)
			);
		}

		//parameters->set(java::lang::String(array<jbyte,1>("orientation")), 180);
		parameters->setPreviewSize(width*2,height*2);
		//parameters->setPreviewFormat(0x10);
		//parameters->setPreviewFormat(0x11);
		pCamera->setParameters(parameters);
		pCamera->startPreview();
	}
}

void UCVActivity::surfaceCreated(local_ref<SurfaceHolder> const &holder)
{
	if(local_ref<Camera> pCamera=Camera::open())
	{
		m_Camera=pCamera;
		local_ref<SurfaceView> pCameraPreview=m_CameraPreview;
		pCamera->setPreviewDisplay(pCameraPreview->getHolder());
		pCamera->setPreviewCallback(*this);
	}
}

bool UCVActivity::onTouchEvent(j2cpp::local_ref<MotionEvent> const &me)
{
	if(me->getAction()==MotionEvent::ACTION_DOWN)
	{
		if(Native *pn=reinterpret_cast<Native*>((jlong)(m_Native)))
		{
			if(pn->m_surf && pn->m_gray_img.width()*pn->m_gray_img.height())
			{
				//ucv::convert_scale(ucv::gil::view(pn->m_gray_img), ucv::gil::view(pn->m_gil_gray_img), float(255));
				//ucv::gil::png_write_view("/sdcard/markers/capture.png",ucv::gil::view(pn->m_gil_gray_img));
			}
		}
	}
}

void UCVActivity::surfaceDestroyed(j2cpp::local_ref<SurfaceHolder> const &holder)
{
	if(local_ref<Camera> pCamera=m_Camera)
	{
		pCamera->stopPreview();
		pCamera->setPreviewDisplay(local_ref<SurfaceHolder>());
		pCamera->setPreviewCallback(local_ref<Camera::PreviewCallback>());
		pCamera->release();
		m_Camera=local_ref<Camera>();
	}
}

void UCVActivity::onPreviewFrame(local_ref< j2cpp::array<jbyte,1> > const &data, local_ref<Camera> const &camera)
{
	if(Native *pn=reinterpret_cast<Native*>((jlong)(m_Native)))
	{
		if(pn->m_surf)
		{
			local_ref<Camera::Parameters> parameters=camera->getParameters();
			ucv::size2ui fs(parameters->getPreviewSize()->width, parameters->getPreviewSize()->height);

			//ucv::gil::resize_view(
			//	ucv::gil::interleaved_view(
			//		fs.width(), fs.height(),
			//		reinterpret_cast<ucv::gil::gray8_pixel_t*>(data->data()),
			//		fs.width()
			//	),
			//	ucv::gil::view(pn->m_gil_gray_img),
			//	ucv::gil::bilinear_sampler()
			//);

			//ucv::convert_scale(
			//	//ucv::gil::view(pn->m_gil_gray_img),
			//	ucv::gil::interleaved_view(
			//		fs.width(), fs.height(),
			//		reinterpret_cast<ucv::gil::gray8_pixel_t*>(data->data()),
			//		fs.width()
			//	),
			//	ucv::gil::view(pn->m_gray_img),
			//	ucv::surf::integral_t(1.0f/255.0f)
			//);

			Native::gray_t median_value(-1);
			ucv::convert_nv16_to_gray(
				//y channel
				ucv::gil::interleaved_view(
					fs.width(), fs.height(),
					reinterpret_cast<ucv::gil::gray8_pixel_t*>(data->data()),
					fs.width()
				),
				//uv channels
				ucv::gil::interleaved_view(
					fs.width()/2, fs.height()/2,
					reinterpret_cast<ucv::gil::gray16_pixel_t*>(data->data()+fs.width()*fs.height()),
					(fs.width()/2)*2
				),
				ucv::gil::view(pn->m_gray_img),
				median_value
			);

			static int update_usecs=0, build_usecs=0, detect_usecs=0, describe_usecs=0, match_usecs=0, homography_usecs=0, counter=3;

			boost::posix_time::ptime start=boost::posix_time::microsec_clock::local_time();
			ucv::integral(
				ucv::gil::const_view(pn->m_gray_img),
				ucv::gil::view(pn->m_integral_img[pn->m_curr]),
				median_value
			);


			if(pn->m_homography_ok)
			{
				pn->m_tracker->set_integral_views(
					ucv::gil::const_view(pn->m_integral_img[(pn->m_curr+1)%2]),
					ucv::gil::const_view(pn->m_integral_img[pn->m_curr])
				);

				std::vector<bool> status;
				std::vector<ucv::surf::feature_point_t::point2_t> prev_pts;
				for(std::size_t ifp=0;ifp<pn->m_features.size();++ifp)
					prev_pts.push_back(
						ucv::surf::feature_point_t::point2_t(
							pn->m_features[ifp].x,
							pn->m_features[ifp].y
						)
					);

				boost::posix_time::ptime start_track=boost::posix_time::microsec_clock::local_time();
				pn->m_tracker->operator ()(prev_pts, pn->m_next_pts, status);
				boost::posix_time::ptime finish_track=boost::posix_time::microsec_clock::local_time();

				__android_log_print(ANDROID_LOG_INFO, J2CPP_NAME, "nf=%d, tt=%f",
						prev_pts.size(),
						float((finish_track-start_track).total_milliseconds())
				);

				pn->m_homography_ok=false;
			}
			else
			{
				pn->m_surf->set_integral_view(ucv::gil::const_view(pn->m_integral_img[pn->m_curr]));
				boost::posix_time::ptime finish_update=boost::posix_time::microsec_clock::local_time();
				pn->m_surf->build_response_layers();
				boost::posix_time::ptime finish_build=boost::posix_time::microsec_clock::local_time();
				pn->m_surf->detect(pn->m_features);
				boost::posix_time::ptime finish_detect=boost::posix_time::microsec_clock::local_time();
				pn->m_surf->describe(pn->m_features);
				boost::posix_time::ptime finish_describe=boost::posix_time::microsec_clock::local_time();
				ucv::surf::match_feature_points(pn->m_marker_features, pn->m_features, pn->m_feature_matches, ucv::surf::feature_point_t::value_type(0.65f));
				boost::posix_time::ptime finish_match=boost::posix_time::microsec_clock::local_time();
				//ucv::nublas::matrix< ucv::surf::feature_point_t::value_type, ucv::nublas::row_major > hm;
				pn->m_homography_ok=ucv::find_homography_ransac(pn->m_marker_features, pn->m_features, pn->m_feature_matches, pn->m_homography[pn->m_curr]);
				boost::posix_time::ptime finish_homography=boost::posix_time::microsec_clock::local_time();

				update_usecs+=(finish_update-start).total_milliseconds();
				build_usecs+=(finish_build-finish_update).total_milliseconds();
				detect_usecs+=(finish_detect-finish_build).total_milliseconds();
				describe_usecs+=(finish_describe-finish_detect).total_milliseconds();
				match_usecs+=(finish_match-finish_describe).total_milliseconds();
				homography_usecs+=(finish_homography-finish_match).total_milliseconds();

				if(!(--counter))
				{
					__android_log_print(ANDROID_LOG_INFO, J2CPP_NAME, "nf=%d, nm=%d, u=%f, b=%f, d=%f, des=%f, m=%f, h=%f",
							pn->m_features.size(),
							pn->m_feature_matches.size(),
							float(update_usecs)/3.0f,
							float(build_usecs)/3.0f,
							float(detect_usecs)/3.0f,
							float(describe_usecs/3.0f),
							float(match_usecs)/3.0f,
							float(homography_usecs)/3.0f
					);
					counter=3;
					update_usecs=build_usecs=detect_usecs=describe_usecs=match_usecs=homography_usecs=0;
				}
				pn->m_curr=(pn->m_curr+1)%2;
			}

		}
	}
}
