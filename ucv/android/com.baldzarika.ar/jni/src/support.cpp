#include <config.h>

#include <java/lang/String.hpp>
#define J2CPP_INCLUDE_IMPLEMENTATION
#include <java/lang/String.hpp>

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	if(!j2cpp::environment::get().init(vm))
		return JNI_ERR;
	return JNI_VERSION_1_6;
}
