#ifndef CANNY_TEST_WIDGET_H
#define CANNY_TEST_WIDGET_H

class CannyTestWidget
	: public QGLWidget
{
	Q_OBJECT

public:
	friend class CannyTestDlg;

	typedef ucv::gaussian_blur<ar::gray_t, 3> gaussian_blur_t;
	typedef ucv::canny<ar::gray_t, 3> canny_t;
	typedef gaussian_blur_t::gray_image_t gray_image_t;
	typedef ar::fiducial::contour_t	contour_t;



	CannyTestWidget(QWidget *parent=0);
	~CannyTestWidget();

protected:
	void						doCanny();
	void						initializeGL();
	void						resizeGL(int width,int height);
	void						paintGL();

private:
	GLuint										m_video_texture;
	ucv::gil::rgb8_image_t						m_frame;
	ucv::gil::gray8_image_t						m_gray8_frame;
	gray_image_t								m_gray_frame;
	gray_image_t								m_gray_blurred_frame;
	videoInput									m_vi;
	gaussian_blur_t								m_gaussian_blur;
	canny_t										m_canny;

	ar::fiducial::bch_marker_model				m_bch_marker_model;
	
	std::list< contour_t >						m_contours;

	QTimer										*m_render_timer;
};

#endif //CANNY_TEST_WIDGET_H
