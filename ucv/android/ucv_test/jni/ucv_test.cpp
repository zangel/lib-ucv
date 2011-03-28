#include <j2cpp/j2cpp.hpp>

#include <android/app/Activity.hpp>
#include <android/view/SurfaceView.hpp>
#include <android/view/SurfaceHolder.hpp>
#include <android/opengl/GLSurfaceView.hpp>
#include <android/hardware/Camera.hpp>
#include <android/graphics/PixelFormat.hpp>
#include <android/view/ViewGroup.hpp>
#include <android/view/MotionEvent.hpp>
#include <java/util/List.hpp>
#include <java/lang/String.hpp>
#define J2CPP_INCLUDE_IMPLEMENTATION
#include <android/app/Activity.hpp>
#include <android/view/SurfaceView.hpp>
#include <android/view/SurfaceHolder.hpp>
#include <android/opengl/GLSurfaceView.hpp>
#include <android/hardware/Camera.hpp>
#include <android/graphics/PixelFormat.hpp>
#include <android/view/ViewGroup.hpp>
#include <android/view/MotionEvent.hpp>
#include <java/util/List.hpp>
#include <java/lang/String.hpp>

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	if(!j2cpp::environment::get().init(vm))
		return JNI_ERR;
	return JNI_VERSION_1_6;
}
