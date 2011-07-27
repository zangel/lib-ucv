#ifndef SURF_TEST_WIDGET_H
#define SURF_TEST_WIDGET_H

class SURFTestWidget
	: public QGLWidget
{
	Q_OBJECT

public:
	friend class SURFTestDlg;

	typedef ucv::surf::feature_point_t feature_point_t;
	typedef feature_point_t::point2_t point2_t;

	typedef KDTree::KDTree<2, feature_point_t, feature_point_t::position_accessor> matched_points_t;


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

	ucv::surf::fps_by_desc_tree_t				m_obj_features;

	ucv::surf									m_scene_surf;
	ucv::surf::fps_by_pos_tree_t				m_scene_features;

	matched_points_t							m_matched_points;
	
	float										m_MinScale;
	float										m_MaxScale;

};

#endif //FIDUCIAL_TEST_PREVIEW_H
