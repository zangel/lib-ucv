#ifndef SURF_TEST_WIDGET_H
#define SURF_TEST_WIDGET_H

class SURFTestWidget
	: public QGLWidget
{
	Q_OBJECT

public:
	friend class SURFTestDlg;

	SURFTestWidget(QWidget *parent=0);
	~SURFTestWidget();

protected:
	void						initializeGL();
	void						resizeGL(int width,int height);
	void						paintGL();

	void						setMinMaxScale(float minScale, float maxScale);


private:
	GLuint										m_video_texture;
	ucv::gil::rgb8_image_t						m_frame;
	ucv::gil::gray8_image_t						m_gray8_frame;
	ucv::surf::integral_image_t					m_gray_frame;
	ucv::surf::integral_image_t					m_integral_frame;
	videoInput									m_vi;

	QTimer										*m_render_timer;
	ucv::surf									m_surf;
	std::vector<ucv::surf::feature_point_t>		m_features;

	float										m_MinScale;
	float										m_MaxScale;

};

#endif //FIDUCIAL_TEST_PREVIEW_H
