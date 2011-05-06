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
#include <android/graphics/Matrix.hpp>

namespace {

	using namespace j2cpp;
	using namespace j2cpp::java::lang;
	using namespace com::baldzarika::ar;


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
#if 0
			if(local_ref<Thread> currThread=Thread::currentThread())
			{
				if(local_ref<ClassLoader> cler=currThread->getContextClassLoader())
				{
					//currThread->setContextClassLoader(cler);
					if(local_ref<Class> dummyClass=cler->loadClass(local_ref<String>(jenv()->NewStringUTF("java/lang/String"))))
					{
						m_class_loader=cler;
					}
				}
			}
#else
			if(local_ref<Size2> dummySize2=local_ref<Size2>(Size2()))
			{
				if(local_ref< Object > dummySize2Obj=static_cast< local_ref<Object> >(dummySize2))
				{
					if(local_ref< Class > dummySize2Class=dummySize2Obj->getClass())
					{
						if(local_ref<ClassLoader> cler=dummySize2Class->getClassLoader())
						{
							if(local_ref<Class> dummyClass=cler->loadClass(local_ref<String>(jenv()->NewStringUTF("java/lang/String"))))
								m_class_loader=cler;
						}
					}
				}
			}
#endif
			return static_cast<bool>(m_class_loader);
		}

		bool attach_current_thread()
		{
			if(raw_environment::attach_current_thread())
			{
				if(m_class_loader)
				{
					//if(local_ref<Thread> currThread=Thread::currentThread())
					//{
						//currThread->setPriority(10);
						return true;
					//}
				}
			}
			return false;
		}

		bool detach_current_thread()
		{
			return raw_environment::detach_current_thread();
		}

		jclass find_class(char const *cn)
		{
			if(m_class_loader)
			{
				if(local_ref<Class> found_class=m_class_loader->loadClass(local_ref<String>(jenv()->NewStringUTF(cn))))
				{
					return reinterpret_cast<jclass>(found_class->get_jobject());
				}
			}
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
