#ifndef FIDUCIAL_TEST_DLG_H
#define FIDUCIAL_TEST_DLG_H

#include "ui_FiducialTestDlg.h"

class FiducialTestDlg
	: public QDialog
	, protected Ui::FiducialTestDlg
{
	Q_OBJECT
public:
	FiducialTestDlg(QWidget *parent = 0, Qt::WFlags flags = 0);
	~FiducialTestDlg();

};

#endif //FIDUCIAL_TEST_DLG_H