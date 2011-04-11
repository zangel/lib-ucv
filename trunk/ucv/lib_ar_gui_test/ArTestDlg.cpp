#include "Prec.h"
#include "ArTestDlg.h"
#include <baldzarika/ucv/convert_scale.h>


ucv::size2ui const ArTestDlg::PREVIEW_SIZES[2]=
{
	ucv::size2ui(320,240),
	ucv::size2ui(640,480)
};


ArTestDlg::ArTestDlg(QWidget *parent, Qt::WFlags flags)
	: QDialog(parent, flags)
	, m_fg_ios()
	, m_fg_ios_work(m_fg_ios)
	, m_fg_worker()
	, m_fg_timer(m_fg_ios)
{
	setupUi(this);
	setAcceptDrops(true);
	QString strItemFmt("%1 x %2");
	for(std::size_t sz=0;sz<sizeof(PREVIEW_SIZES)/sizeof(ucv::size2ui);++sz)
		m_PreviewSize->addItem(strItemFmt.arg(PREVIEW_SIZES[sz].width()).arg(PREVIEW_SIZES[sz].height()));

	
	connect(m_PreviewSize, SIGNAL(currentIndexChanged(int)), this, SLOT(onPreviewSizeChanged(int)));
	m_PreviewSize->setCurrentIndex(0);
	onPreviewSizeChanged(0);

	connect(m_StartStop, SIGNAL(clicked(bool)), this, SLOT(onStartStop()));
	updateGUI();
}

ArTestDlg::~ArTestDlg()
{
}

void ArTestDlg::onStartStop()
{
	if(is_started())
		stop();
	else
		start();
	
	updateGUI();
}

void ArTestDlg::onPreviewSizeChanged(int index)
{
	m_VideoPreview->setFixedSize(PREVIEW_SIZES[index].width(),PREVIEW_SIZES[index].height());
}

void ArTestDlg::updateGUI()
{
	if(is_started())
	{
		m_StartStop->setEnabled(true);
		m_StartStop->setText("Stop");
		m_PreviewSize->setEnabled(false);
		m_PreviewRate->setEnabled(false);
	}
	else
	{
		m_StartStop->setEnabled(m_marker?true:false);
		m_StartStop->setText("Start");
		m_PreviewSize->setEnabled(true);
		m_PreviewRate->setEnabled(true);
	}
}

bool ArTestDlg::is_started() const
{
	return m_tracker?true:false;
}

void ArTestDlg::start()
{
	BOOST_ASSERT(m_marker);
	BOOST_ASSERT(!m_tracker);
	BOOST_ASSERT(!m_marker_state);

	int size_idx=m_PreviewSize->currentIndex();
	
	BOOST_ASSERT(
		m_VideoPreview->size().width()==PREVIEW_SIZES[size_idx].width() &&
		m_VideoPreview->size().height()==PREVIEW_SIZES[size_idx].height()
	);

	if(videoInput::listDevices())
	{
		m_vi.setIdealFramerate(0, m_PreviewRate->value());
		if(m_vi.setupDevice(0, PREVIEW_SIZES[size_idx].width(), PREVIEW_SIZES[size_idx].height()))
		{
			boost::shared_ptr<ar::tracker> new_tracker(
				new ar::tracker(PREVIEW_SIZES[size_idx])
			);

			new_tracker->marker_state_changed().connect(
				boost::bind(
					&ArTestDlg::onMarkerStateChanged, this, _1, _2
				)
			);

			if(boost::shared_ptr<ar::tracker::marker_state> pms=new_tracker->add_marker(m_marker))
			{
				if(new_tracker->start() && new_tracker->is_started())
				{
					m_tracker=new_tracker;
					m_marker_state=pms;

					m_rgb8_frame.recreate(PREVIEW_SIZES[size_idx].width(),PREVIEW_SIZES[size_idx].height());
					m_gray8_frame.recreate(PREVIEW_SIZES[size_idx].width(),PREVIEW_SIZES[size_idx].height());
					m_gray_frame.recreate(PREVIEW_SIZES[size_idx].width(),PREVIEW_SIZES[size_idx].height());
										
					m_fg_worker=boost::thread(
						boost::bind(
							&boost::asio::io_service::run,
							&m_fg_ios
						)
					).move();
								
					m_fg_timer.expires_from_now(
						boost::posix_time::seconds(0)
					);

					m_fg_timer.async_wait(
						boost::bind(&ArTestDlg::onGrabFrame, this)
					);
				}
				else
				{
					m_vi.stopDevice(0);
				}
			}
			else
			{
				m_vi.stopDevice(0);
			}
		}
	}
	else
	{

	}
}

void ArTestDlg::stop()
{
	BOOST_ASSERT(m_tracker);
	m_fg_ios.stop();
	m_fg_worker.join();
	BOOST_ASSERT(m_fg_ios.stopped());
	m_fg_ios.reset();
	m_tracker->stop();
	BOOST_ASSERT(!m_tracker->is_started());
	m_marker_state.reset();
	m_tracker.reset();
	m_vi.stopDevice(0);
}

void ArTestDlg::closeEvent(QCloseEvent *e)
{
	if(is_started())
		stop();
	updateGUI();
	QDialog::closeEvent(e);
}

void ArTestDlg::dragEnterEvent(QDragEnterEvent *e)
{
	e->setAccepted(false);
	if(!is_started())
	{
		if(e->mimeData()->hasUrls())
		{
			QList<QUrl> urls=e->mimeData()->urls();
			for(std::size_t u=0;u<urls.size();++u)
			{
				QString qstr_marker_fname=urls.at(u).toLocalFile();
				QByteArray asciiData=qstr_marker_fname.toAscii();
				std::string str_marker_fname(asciiData.constBegin(), asciiData.constEnd());
			
				if(ar::marker::can_load(str_marker_fname))
				{
					e->setAccepted(true);
					break;
				}
			}
		}
	}
}

void ArTestDlg::dropEvent(QDropEvent *e)
{
	if(!is_started())
	{
		if(e->mimeData()->hasUrls())
		{
			QList<QUrl> urls=e->mimeData()->urls();
			for(std::size_t u=0;u<urls.size();++u)
			{
				QString qstr_marker_fname=urls.at(u).toLocalFile();
				QByteArray asciiData=qstr_marker_fname.toAscii();
				std::string str_marker_fname(asciiData.constBegin(), asciiData.constEnd());

				if(ar::marker::can_load(str_marker_fname))
				{
					if(boost::shared_ptr<ar::marker> p_new_marker=ar::marker::load(str_marker_fname))
					{
						m_marker=p_new_marker;
						m_MarkerPreview->setPixmap(QPixmap(qstr_marker_fname));
						m_MarkerPreview->setFixedSize(m_marker->get_size().width(),m_marker->get_size().height());
						updateGUI();
					}
				}
			}
		}
	}
}

void ArTestDlg::onGrabFrame()
{
	if(m_vi.isFrameNew(0))
	{
		m_vi.getPixels(0, &ucv::gil::view(m_rgb8_frame)[0][0], true, true);
		m_VideoPreview->setVideoTextureView(ucv::gil::const_view(m_rgb8_frame));

		ucv::gil::copy_and_convert_pixels(
			ucv::gil::const_view(m_rgb8_frame),
			ucv::gil::view(m_gray8_frame)
		);
		ar::tracker::gray_t median_value;
		ucv::convert_scale(
			ucv::gil::const_view(m_gray8_frame),
			ucv::gil::view(m_gray_frame),
			1.0f/255.0f,
			median_value
		);
		m_tracker->update(ucv::gil::const_view(m_gray_frame), median_value);
	}
	m_fg_timer.expires_at(
		m_fg_timer.expires_at()+boost::posix_time::microseconds(
			static_cast<boost::int64_t>(0.5f*(1.0e+6f/float(m_PreviewRate->value())))
		)
	);

	m_fg_timer.async_wait(
		boost::bind(&ArTestDlg::onGrabFrame, this)
	);
}

void ArTestDlg::onMarkerStateChanged(boost::shared_ptr<ar::tracker::marker_state> const &ms, ar::tracker::marker_state::eSC sc)
{
	if(ms)
	{
		switch(sc)
		{
		case ar::tracker::marker_state::SC_DETECTION:
		case ar::tracker::marker_state::SC_POSE:
			{
				if(ms->is_detected())
					m_VideoPreview->setTrackingFeatures(ms->get_frame_features());
				else
					m_VideoPreview->setTrackingFeatures(ar::tracker::marker_state::points2_t());
			}
			break;
		}
	}
}