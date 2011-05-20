#include <config.h>
#include <com_baldzarika_ar_Frame.h>
#include <com/baldzarika/ar/Frame.h>
#include <com/baldzarika/ar/Size2.h>
#include <baldzarika/ucv/convert_scale.h>


void Java_com_baldzarika_ar_Frame_create__(JNIEnv */*e*/, jobject f)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;

	Frame(f).create();
}

void Java_com_baldzarika_ar_Frame_create__Lcom_baldzarika_ar_Size2_2(JNIEnv */*e*/, jobject f, jobject fs)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;

	Frame(f).create(local_ref<Size2>(fs));
}

void Java_com_baldzarika_ar_Frame_destroy(JNIEnv */*e*/, jobject f)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	Frame(f).destroy();
}

jobject Java_com_baldzarika_ar_Frame_getSize(JNIEnv */*e*/, jobject f)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Frame(f).getSize().get_jobject();
}

jdouble Java_com_baldzarika_ar_Frame_getMedianPixelValue(JNIEnv */*e*/, jobject f)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Frame(f).getMedianPixelValue();
}

jboolean Java_com_baldzarika_ar_Frame_setSize(JNIEnv */*e*/, jobject f, jobject fs)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Frame(f).setSize(local_ref<Size2>(fs));
}

jboolean Java_com_baldzarika_ar_Frame_setPixels(JNIEnv */*e*/, jobject f, jbyteArray data, jint pfmt)
{
	using namespace com::baldzarika::ar;
	using namespace j2cpp;
	return Frame(f).setPixels(local_ref< array< jbyte,1> >(data),pfmt);
}

namespace com { namespace baldzarika { namespace ar {

	J2CPP_DEFINE_CLASS(Frame,"com/baldzarika/ar/Frame")
	J2CPP_DEFINE_FIELD(Frame,0,"m_px","J")

	Frame::px_t* Frame::get(jx_t const &jx)
	{
		if(px_t *ppx=reinterpret_cast<px_t*>(static_cast<jlong>(jx->m_px)))
			return ppx;
		return 0;
	}

	Frame::Frame(jobject jobj)
		: j2cpp::object<Frame>(jobj)
		, m_px(get_jobject())
	{
	}

	void Frame::create()
	{
		m_px=reinterpret_cast<jlong>( new px_t(
			::baldzarika::ar::tracker::gray_t(-1),
			::baldzarika::ar::tracker::gray_image_t()
		));
	}

	void Frame::create(j2cpp::local_ref<Size2> const &fs)
	{
		m_px=reinterpret_cast<jlong>( new px_t(
			::baldzarika::ar::tracker::gray_t(-1),
			::baldzarika::ar::tracker::gray_image_t(jint(fs->m_Width), jint(fs->m_Height))
		));
	}

	void Frame::destroy()
	{
		if(px_t *p_px=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
		{
			delete p_px;
			m_px=0;
		}
	}

	j2cpp::local_ref<Size2> Frame::getSize()
	{
		if(px_t *p_px=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
		{
			return j2cpp::local_ref<Size2>(
				Size2(p_px->second.width(),p_px->second.height())
			);
		}
		return j2cpp::local_ref<Size2>(Size2(0,0));
	}

	jdouble Frame::getMedianPixelValue()
	{
		if(px_t *p_px=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
			return static_cast<jdouble>(p_px->first);
		return -1.0;
	}

	jboolean Frame::setSize(j2cpp::local_ref<Size2> const &fs)
	{
		if(px_t *p_px=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
		{
			p_px->second.recreate(jint(fs->m_Width), jint(fs->m_Height));
			return JNI_TRUE;
		}
		return JNI_FALSE;
	}

	jboolean Frame::setPixels(j2cpp::local_ref< j2cpp::array<jbyte,1> > const &data, jint pfmt)
	{
		if(px_t *p_px=reinterpret_cast<px_t*>(static_cast<jlong>(m_px)))
		{
			if(pfmt==17 && data->size()>=(p_px->second.width()*p_px->second.height()+p_px->second.width()*p_px->second.height()/2))
			{
#if 0
				return ::baldzarika::ucv::convert_nv16_to_gray(
					//y channel
					::baldzarika::ucv::gil::interleaved_view(
						p_px->second.width(), p_px->second.height(),
						reinterpret_cast< ::baldzarika::ucv::gil::gray8_pixel_t * >(data->data()),
						p_px->second.width()
					),
					//uv channels
					::baldzarika::ucv::gil::interleaved_view(
						p_px->second.width()/2, p_px->second.height()/2,
						reinterpret_cast< ::baldzarika::ucv::gil::gray16_pixel_t * >(data->data()+p_px->second.width()*p_px->second.height()),
						(p_px->second.width()/2)*2
					),
					::baldzarika::ucv::gil::view(p_px->second),
					p_px->first
				)?JNI_TRUE:JNI_FALSE;
#endif
			}
		}
		return JNI_FALSE;
	}

} //namespace ar
} //namespace baldzarika
} //namespace com
