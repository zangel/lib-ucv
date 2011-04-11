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
	void			setTrackingFeatures(ar::tracker::marker_state::points2_t const &tfs);

	void			initializeGL();
	void			resizeGL(int width,int height);
	void			paintGL();

private:
	GLuint						m_VideoTexture;

	boost::mutex				m_VideoTextureSync;
	ucv::size2ui				m_VideoTextureSize;
	bool						m_VideoTextureDirty;
	ucv::gil::rgb8c_view_t		m_VideoTextureView;

	boost::mutex							m_TrackingFeaturesSync;
	ar::tracker::marker_state::points2_t	m_TrackingFeatures;

};

#endif //PREVIEW_WIDGET_H