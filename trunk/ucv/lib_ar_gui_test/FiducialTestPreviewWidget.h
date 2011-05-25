#ifndef FIDUCIAL_TEST_PREVIEW_H
#define FIDUCIAL_TEST_PREVIEW_H

class FiducialTestPreviewWidget
	: public QGLWidget
{
	Q_OBJECT

public:
	friend class FiducialTestDlg;

	FiducialTestPreviewWidget(QWidget *parent=0);
	~FiducialTestPreviewWidget();

protected:
	void						initializeGL();
	void						resizeGL(int width,int height);
	void						paintGL();

	void						onMarkerStateChanged(boost::shared_ptr<ar::fiducial::detector::marker_state> const &ms, ar::fiducial::detector::marker_state::eSC sc);


private:
	GLuint						m_video_texture;
	ucv::gil::rgb8_image_t		m_frame;
	ucv::gil::gray8_image_t		m_gray_frame;
	videoInput					m_vi;

	QTimer						*m_render_timer;

	boost::shared_ptr<ar::fiducial::detector>	m_detector;

	bool						m_is_detected;
	ucv::matrix44f				m_model_view;

};

#endif //FIDUCIAL_TEST_PREVIEW_H
