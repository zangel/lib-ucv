#ifndef SURF_TEST_WIDGET_H
#define SURF_TEST_WIDGET_H

class SURFTestWidget
	: public QGLWidget
{
	Q_OBJECT

public:
	friend class SURFTestDlg;

	typedef ar::markerless::tracker::feature_point_t feature_point_t;
	typedef ar::markerless::tracker::hessian_detector_t hessian_detector_t;
	typedef ar::markerless::tracker::orientation_estimator_t orientation_estimator_t;
	typedef ar::markerless::tracker::describer_t describer_t;

	
	typedef feature_point_t::base_type point2_t;
	typedef std::vector<feature_point_t> feature_points_t;
	typedef std::pair<feature_points_t::const_iterator,feature_points_t::const_iterator> fp_match_t;
	typedef std::vector<fp_match_t>	fp_matches_t;
	
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
	ar::gray_image_t							m_gray_frame;
	ar::gray_image_t							m_integral_frame;
	videoInput									m_vi;

	QTimer										*m_render_timer;

	hessian_detector_t							m_scene_detector;
	orientation_estimator_t						m_orientation_estimator;
	describer_t									m_describer;

	std::vector<feature_point_t>				m_obj_features;
	std::vector<feature_point_t>				m_scene_features;
	fp_matches_t								m_marker_matches;
	
	

	float										m_MinScale;
	float										m_MaxScale;

};

#endif //FIDUCIAL_TEST_PREVIEW_H
