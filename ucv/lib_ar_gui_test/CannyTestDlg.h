#ifndef CANNY_TEST_DLG_H
#define CANNY_TEST_DLG_H

#include "ui_CannyTestDlg.h"

class CannyTestDlg
	: public QDialog
	, protected Ui::CannyTestDlg
{
	Q_OBJECT
public:
	CannyTestDlg(QWidget *parent = 0, Qt::WFlags flags = 0);
	~CannyTestDlg();
};

#endif //CANNY_TEST_DLG_H