#include <config.h>
#include <j2cpp/raw_environment.hpp>
#include <com/baldzarika/ar/Size2.h>

#define J2CPP_INCLUDE_IMPLEMENTATION
#include <java/lang/Class.hpp>
#include <java/lang/Object.hpp>
#include <java/lang/Comparable.hpp>
#include <java/lang/String.hpp>
#include <java/lang/ClassLoader.hpp>
#include <java/lang/Thread.hpp>
#include <android/graphics/Bitmap.hpp>

namespace {

	using namespace j2cpp;
	using namespace j2cpp::java::lang;

	class custom_environment
		: public j2cpp::raw_environment
	{
	public:
		custom_environment(JavaVM *jvm)
			: j2cpp::raw_environment(jvm)
		{
		}

		~custom_environment()
		{
		}

		bool init()
		{
			//__android_log_print(ANDROID_LOG_INFO, J2CPP_NAME, "init().0");
			if(local_ref<Thread> currThread=Thread::currentThread())
			{
				//__android_log_print(ANDROID_LOG_INFO, J2CPP_NAME, "init().1");
				if(local_ref<ClassLoader> cler=currThread->getContextClassLoader())
				{
					//__android_log_print(ANDROID_LOG_INFO, J2CPP_NAME, "init().2");
					//load any class to cache loadClass method
					currThread->setContextClassLoader(cler);
					if(local_ref<Class> dummyClass=cler->loadClass(local_ref<String>(jenv()->NewStringUTF("java/lang/String"))))
					{
						//__android_log_print(ANDROID_LOG_INFO, J2CPP_NAME, "init().3");
						m_class_loader=cler;
					}
				}
			}
			return static_cast<bool>(m_class_loader);
		}

		bool attach_current_thread()
		{
			if(raw_environment::attach_current_thread())
			{
				if(m_class_loader)
				{
					if(local_ref<Thread> currThread=Thread::currentThread())
					{
						//currThread->setContextClassLoader(m_class_loader);
						currThread->setPriority(10);
						__android_log_print(ANDROID_LOG_INFO, J2CPP_NAME, "attach_current_thread(): currentThread()->getPriority()=%d", currThread->getPriority());
						return true;
					}
				}
			}
			return false;
		}

		bool detach_current_thread()
		{
			if(local_ref<Thread> currThread=Thread::currentThread())
				__android_log_print(ANDROID_LOG_INFO, J2CPP_NAME, "detach_current_thread(): currentThread()->getPriority()=%d", currThread->getPriority());
			return raw_environment::detach_current_thread();
		}

		jclass find_class(char const *cn)
		{
			//__android_log_print(ANDROID_LOG_INFO, J2CPP_NAME, "custom_environment::find_class(%s) ...", cn);
			if(m_class_loader)
			{
				//__android_log_print(ANDROID_LOG_INFO, J2CPP_NAME, "custom_environment::find_class(%s) from m_class_loader ...", cn);
				if(local_ref<Class> found_class=m_class_loader->loadClass(local_ref<String>(jenv()->NewStringUTF(cn))))
				{
					//__android_log_print(ANDROID_LOG_INFO, J2CPP_NAME, "custom_environment::find_class(%s) [OK]", cn);
					return reinterpret_cast<jclass>(found_class->get_jobject());
				}
			}
			//__android_log_print(ANDROID_LOG_INFO, J2CPP_NAME, "custom_environment::find_class(%s) from raw_environment", cn);
			return raw_environment::find_class(cn);
		}
		global_ref<ClassLoader> m_class_loader;
	};

} //namespace anonymous


JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
	j2cpp::shared_ptr<custom_environment> custom_env(new custom_environment(vm));
	if(j2cpp::environment::init(custom_env) && custom_env->init())
		return JNI_VERSION_1_4;
	return JNI_ERR;
}
