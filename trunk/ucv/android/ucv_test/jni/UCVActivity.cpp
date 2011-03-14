#include "UCVActivity.h"
#include <android/graphics/PixelFormat.hpp>
#include <android/view/ViewGroup.hpp>

namespace {

	class Native
	{
	public:
		Native()
		{

		}

		~Native()
		{

		}
	};

} //namespace anonymous

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
