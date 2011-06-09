#ifndef PREVIEW_WIDGET_H
#define PREVIEW_WIDGET_H

class PreviewWidget
	: public QGLWidget
{
	Q_OBJECT
public:

	friend class ArTestDlg;
	
	PreviewWidget(QWidget *parent=0);
	~PreviewWidget();

protected:
	void			setVideoTextureView(ucv::gil::rgb8c_view_t rgb8v);
	void			setDetectedCameraPose(bool detected, math::matrix44f const &cpose, math::matrix44f const &cproj, math::size2ui const &ms);
	void			setMarkerSize(math::size2ui const &ms);
	
	
	void			initializeGL();
	void			resizeGL(int width,int height);
	void			paintGL();

private:
	GLuint						m_VideoTexture;

	boost::mutex				m_VideoTextureSync;
	math::size2ui				m_VideoTextureSize;
	bool						m_VideoTextureDirty;
	ucv::gil::rgb8c_view_t		m_VideoTextureView;


	boost::mutex				m_CameraPoseSync;
	bool						m_Detected;
	math::matrix44f				m_CameraPose;
	math::matrix44f				m_CameraProjection;
	math::size2ui				m_MarkerSize;
};

#endif //PREVIEW_WIDGET_H