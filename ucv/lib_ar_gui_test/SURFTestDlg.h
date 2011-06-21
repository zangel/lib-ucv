#ifndef SURF_TEST_DLG_H
#define SURF_TEST_DLG_H

#include "ui_SURFTestDlg.h"

class SURFTestDlg
	: public QDialog
	, protected Ui::SURFTestDlg
{
	Q_OBJECT
public:
	SURFTestDlg(QWidget *parent = 0, Qt::WFlags flags = 0);
	~SURFTestDlg();

protected slots:

	void		onMinMaxChanged();


};

#endif //SURF_TEST_DLG_H