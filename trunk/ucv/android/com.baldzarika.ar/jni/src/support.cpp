#include <config.h>
#include <com/baldzarika/ar/Size2.h>

#define J2CPP_INCLUDE_IMPLEMENTATION
#include <java/lang/Class.hpp>
#include <java/lang/Object.hpp>
#include <java/lang/String.hpp>
#include <java/lang/ClassLoader.hpp>
#include <java/lang/Thread.hpp>

static j2cpp::global_ref<j2cpp::java::lang::Thread> const& get_loader_thread()
{
	static j2cpp::global_ref<j2cpp::java::lang::Thread> loader_thread=
		j2cpp::java::lang::Thread::currentThread();
	return loader_thread;
}

static j2cpp::global_ref<j2cpp::java::lang::ClassLoader> const& get_class_loader()
{
	static j2cpp::global_ref<j2cpp::java::lang::ClassLoader> class_loader=
		get_loader_thread()->getContextClassLoader();
	return class_loader;
}

static bool current_thread_set_class_loader()
{
	if(j2cpp::local_ref<j2cpp::java::lang::Thread> current_thread=j2cpp::java::lang::Thread::currentThread())
	{
		if(j2cpp::local_ref<j2cpp::java::lang::ClassLoader> cler=get_class_loader())
		{
			current_thread->setContextClassLoader(cler);
			if(j2cpp::local_ref<j2cpp::java::lang::ClassLoader> cler=current_thread->getContextClassLoader())
			{
				__android_log_print(ANDROID_LOG_INFO, J2CPP_NAME, "ok to change current thread's class loader");
				return true;
			}
		}
	}
	__android_log_print(ANDROID_LOG_INFO, J2CPP_NAME, "failed to change current thread's class loader");
	return false;
}

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	if(j2cpp::environment::get().init(vm, &current_thread_set_class_loader))
	{
		__android_log_print(ANDROID_LOG_INFO, J2CPP_NAME, "getting loader thread ...");
		if(j2cpp::local_ref<j2cpp::java::lang::Thread> loader_thread=get_loader_thread())
		{
			__android_log_print(ANDROID_LOG_INFO, J2CPP_NAME, "getting loader thread [OK]");
			__android_log_print(ANDROID_LOG_INFO, J2CPP_NAME, "getting class loader ...");
			if(j2cpp::local_ref<j2cpp::java::lang::ClassLoader> cler=get_class_loader())
			{
				//loader_thread->setContextClassLoader(cler);
				__android_log_print(ANDROID_LOG_INFO, J2CPP_NAME, "getting class loader [OK]");
				return JNI_VERSION_1_6;
			}
		}
	}
	return JNI_ERR;
}
