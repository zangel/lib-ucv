#ifndef ARTESTDLG_H
#define ARTESTDLG_H


#include "ui_ArTestDlg.h"

class ArTestDlg
	: public QDialog
	, protected Ui::ArTestDlgClass
{
	Q_OBJECT

public:
	static math::size2ui	const			PREVIEW_SIZES[2];


	ArTestDlg(QWidget *parent = 0, Qt::WFlags flags = 0);
	~ArTestDlg();

protected slots:
	void			onStartStop();
	void			onPreviewSizeChanged(int index);
	void			updateGUI();



	
protected:
	void			onMarkerStateChanged(boost::shared_ptr<ar::tracker::marker_state> const &ms, ar::tracker::marker_state::eSC sc);
	void			onGrabFrame();
	
	bool			is_started() const;
	void			start();
	void			stop();


	void			closeEvent(QCloseEvent *e);
	void			dragEnterEvent(QDragEnterEvent *e);
	void			dropEvent(QDropEvent *e);




	
private:
	boost::shared_ptr<ar::markerless::tracker>		m_tracker;
	boost::shared_ptr<ar::markerless::marker>		m_marker;
	
	videoInput										m_vi;

	boost::asio::io_service							m_fg_ios;
	boost::asio::io_service::work					m_fg_ios_work;
	boost::thread									m_fg_worker;
	boost::asio::deadline_timer						m_fg_timer;
	
	boost::mutex									m_frame_sync;
	ucv::gil::rgb8_image_t							m_rgb8_frame;
	ucv::gil::gray8_image_t							m_gray8_frame;
};

#endif // ARTESTDLG_H
