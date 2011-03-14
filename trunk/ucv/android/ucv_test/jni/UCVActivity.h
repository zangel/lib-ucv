#ifndef UCV_ACTIVITY_H
#define UCV_ACTIVITY_H

#include <j2cpp/j2cpp.hpp>
#include <android/app/Activity.hpp>
#include <android/view/SurfaceView.hpp>
#include <android/opengl/GLSurfaceView.hpp>
#include <android/hardware/Camera.hpp>
#include <javax/microedition/khronos/egl/EGLConfig.hpp>
#include <javax/microedition/khronos/egl/EGL10.hpp>

using namespace j2cpp::android::app;
using namespace j2cpp::android::content;
using namespace j2cpp::android::opengl;
using namespace j2cpp::android::graphics;
using namespace j2cpp::android::hardware;
using namespace j2cpp::android::view;
using namespace j2cpp::javax::microedition::khronos;


class UCVActivity
: public j2cpp::object<UCVActivity>
{
public:
	J2CPP_DECLARE_CLASS

	J2CPP_DECLARE_METHOD(0)
	J2CPP_DECLARE_METHOD(1)

	J2CPP_DECLARE_FIELD(0)
	J2CPP_DECLARE_FIELD(1)
	J2CPP_DECLARE_FIELD(2)
	J2CPP_DECLARE_FIELD(3)




	explicit UCVActivity(jobject jobj);

	operator j2cpp::local_ref<Activity>() const;
	operator j2cpp::local_ref<SurfaceHolder::Callback>() const;
	operator j2cpp::local_ref<Camera::PreviewCallback>() const;
	operator j2cpp::local_ref<GLSurfaceView::Renderer>() const;
	operator j2cpp::local_ref<Context>() const;


	void		initialize();
	void		terminate();
	void		onSurfaceCreated(j2cpp::local_ref<egl::EGL10> const &gl, j2cpp::local_ref<egl::EGLConfig> const &config);
	void		onSurfaceChanged(j2cpp::local_ref<egl::EGL10> const &gl, int width, int height);
	void		onDrawFrame(j2cpp::local_ref<egl::EGL10> const &gl);
	void		surfaceChanged(j2cpp::local_ref<SurfaceHolder> const &holder, int format, int width, int height);
	void		surfaceCreated(j2cpp::local_ref<SurfaceHolder> const &holder);
	bool		onTouchEvent(j2cpp::local_ref<MotionEvent> const &me);
	void		surfaceDestroyed(j2cpp::local_ref<SurfaceHolder> const &holder);
	void		onPreviewFrame(j2cpp::local_ref< j2cpp::array<jbyte,1> > const &data, j2cpp::local_ref<Camera> const &camera);


	j2cpp::local_ref<GLSurfaceView::EGLContextFactory>	makeContextFactory();
	j2cpp::local_ref<GLSurfaceView::EGLConfigChooser>	makeConfigChooser(int r, int g, int b, int a, int depth, int stencil);




	j2cpp::field< J2CPP_CLASS_NAME, J2CPP_FIELD_NAME(0), J2CPP_FIELD_SIGNATURE(0),
		j2cpp::local_ref<GLSurfaceView>
	> m_GLView;

	j2cpp::field< J2CPP_CLASS_NAME, J2CPP_FIELD_NAME(1), J2CPP_FIELD_SIGNATURE(1),
		j2cpp::local_ref<SurfaceView>
	> m_CameraPreview;

	j2cpp::field< J2CPP_CLASS_NAME, J2CPP_FIELD_NAME(2), J2CPP_FIELD_SIGNATURE(2),
		j2cpp::local_ref<Camera>
	> m_Camera;

	j2cpp::field< J2CPP_CLASS_NAME, J2CPP_FIELD_NAME(3), J2CPP_FIELD_SIGNATURE(3),
		jlong
	> m_Native;
};

#endif //UCV_ACTIVITY_H
