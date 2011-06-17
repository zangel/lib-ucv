#include <config.h>
#include <com_baldzarika_ar_markerless_Marker.h>
#include <com/baldzarika/ar/markerless/Marker.h>
#include <com/baldzarika/ar/Size2.h>

#pragma GCC visibility push(default)

void Java_com_baldzarika_ar_markerless_Marker_initialize(JNIEnv */*e*/, jobject m, jlong px)
{
	using namespace com::baldzarika::ar::markerless;
	using namespace j2cpp;
	Marker(m).initialize(px);
}

jlong Java_com_baldzarika_ar_markerless_Marker_create__(JNIEnv */*e*/, jclass /*clazz*/)
{
	using namespace com::baldzarika::ar::markerless;
	using namespace j2cpp;
	return Marker::create();
}

jlong Java_com_baldzarika_ar_markerless_Marker_create__Lcom_baldzarika_ar_Size2_2(JNIEnv */*e*/, jclass /*clazz*/, jobject frameSize)
{
	using namespace com::baldzarika::ar::markerless;
	using namespace j2cpp;
	return Marker::create(local_ref<com::baldzarika::ar::Size2>(frameSize));
}

void Java_com_baldzarika_ar_markerless_Marker_destroy(JNIEnv */*e*/, jobject m)
{
	using namespace com::baldzarika::ar::markerless;
	using namespace j2cpp;
	Marker(m).destroy();
}

jobject Java_com_baldzarika_ar_markerless_Marker_getSize(JNIEnv */*e*/, jobject m)
{
	using namespace com::baldzarika::ar::markerless;
	using namespace j2cpp;
	return Marker(m).getSize().get_jobject();
}

jboolean Java_com_baldzarika_ar_markerless_Marker_loadImage(JNIEnv */*e*/, jobject m, jstring fileName)
{
	using namespace com::baldzarika::ar::markerless;
	using namespace j2cpp;
	return Marker(m).loadImage(local_ref<java::lang::String>(fileName));
}

jboolean Java_com_baldzarika_ar_markerless_Marker_setImage(JNIEnv */*e*/, jobject m, jobject bitmap)
{
	using namespace com::baldzarika::ar::markerless;
	using namespace j2cpp;
	return Marker(m).setImage(local_ref<android::graphics::Bitmap>(bitmap));
}

jboolean Java_com_baldzarika_ar_markerless_Marker_saveImage(JNIEnv */*e*/, jobject m, jstring fileName)
{
	using namespace com::baldzarika::ar::markerless;
	using namespace j2cpp;
	return Marker(m).saveImage(local_ref<java::lang::String>(fileName));

}

#pragma GCC visibility pop

namespace com { namespace baldzarika { namespace ar { namespace markerless {


	J2CPP_DEFINE_CLASS(Marker,"com/baldzarika/ar/markerless/Marker")
	J2CPP_DEFINE_FIELD(Marker,0,"m_px","J")

	Marker::jx_t Marker::get(Marker::px_t const &px)
	{
		if(px->m_any_data.type()==typeid(jref_t))
			return jx_t(boost::any_cast<jref_t>(px->m_any_data));
		return jx_t();
	}

	Marker::px_t Marker::get(Marker::jx_t const &jx)
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(jx->m_px)))
			return *ppx;
		return px_t();
	}

	Marker::Marker(jobject jobj)
		: j2cpp::object<Marker>(jobj)
		, m_px(get_jobject())
	{
	}

	void Marker::initialize(jlong px)
	{
		px_t *ppx=reinterpret_cast<px_t *>(px);
		(*ppx)->m_any_data=jref_t(get_jobject());
		m_px=reinterpret_cast<jlong>(ppx);
	}

	jlong Marker::create()
	{
		return reinterpret_cast<jlong>(
			new px_t( new ::baldzarika::ar::markerless::marker())
		);
	}

	jlong Marker::create(j2cpp::local_ref<Size2> const &ms)
	{
		return reinterpret_cast<jlong>(
			new px_t( new ::baldzarika::ar::markerless::marker(
				::baldzarika::math::size2ui(
					jint(ms->m_Width), jint(ms->m_Height)
				)
			))
		);
	}

	void Marker::destroy()
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
		{
			(*ppx)->m_any_data=boost::any();
			m_px=0;
			delete ppx;
		}
	}

	j2cpp::local_ref<Size2>	Marker::getSize()
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
			return j2cpp::local_ref<Size2>(
				Size2((*ppx)->get_size().width(),(*ppx)->get_size().height())
			);
		return j2cpp::local_ref<Size2>();
	}

	jboolean Marker::loadImage(j2cpp::local_ref<j2cpp::java::lang::String> const &fileName)
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
		{
			j2cpp::local_ref< j2cpp::array<jbyte,1> > str_bytes=fileName->getBytes();
			return (*ppx)->load(std::string(
					str_bytes->data(),
					str_bytes->data()+str_bytes->size()
				)
			)?JNI_TRUE:JNI_FALSE;
		}
		return JNI_FALSE;
	}

	jboolean Marker::saveImage(j2cpp::local_ref<j2cpp::java::lang::String> const &fileName)
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
		{
			j2cpp::local_ref< j2cpp::array<jbyte,1> > str_bytes=fileName->getBytes();
			return (*ppx)->save(std::string(
					str_bytes->data(),
					str_bytes->data()+str_bytes->size()
				)
			)?JNI_TRUE:JNI_FALSE;
		}
		return JNI_FALSE;
	}


	jboolean Marker::setImage(j2cpp::local_ref<j2cpp::android::graphics::Bitmap> const &bitmap)
	{
		using namespace j2cpp;
		using namespace j2cpp::android::graphics;
		using namespace j2cpp::java::lang;

		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
		{
			if(bitmap)
			{
				::baldzarika::math::size2ui bm_size(bitmap->getWidth(), bitmap->getHeight());
				if(bm_size.area())
				{
					if(local_ref<Bitmap::Config> bm_cfg=bitmap->getConfig())
					{
						if(0==static_cast< local_ref< Comparable > >(bm_cfg)->compareTo(local_ref<Bitmap::Config>(Bitmap::Config::ARGB_8888)))
						{
							local_ref< array<jint,1> > bm_buffer(bm_size.area());
							if(bm_buffer->length()==bm_size.area())
							{
								::baldzarika::ucv::gil::gray8_image_t bm_gray(bm_size.width(), bm_size.height());

								bitmap->getPixels(bm_buffer, 0, bm_size.width(), 0, 0, bm_size.width(), bm_size.height());
								::baldzarika::ucv::gil::copy_and_convert_pixels(
									::baldzarika::ucv::gil::interleaved_view(
										bm_size.width(), bm_size.height(),
										reinterpret_cast< ::baldzarika::ucv::gil::argb8_pixel_t * >( bm_buffer->data() ),
										bm_size.width()*4
									),
									::baldzarika::ucv::gil::view(bm_gray)
								);

								return (*ppx)->set(
										::baldzarika::ucv::gil::const_view(bm_gray)
								)?JNI_TRUE:JNI_FALSE;
							}
						}
					}
				}
			}
		}
		return JNI_FALSE;
	}



} //namespace markerless
} //namespace ar
} //namespace baldzarika
} //namespace com
