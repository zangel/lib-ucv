#ifndef ARTESTDLG_H
#define ARTESTDLG_H


#include "ui_ArTestDlg.h"

class ArTestDlg
	: public QDialog
	, protected Ui::ArTestDlgClass
{
	Q_OBJECT

public:
	ArTestDlg(QWidget *parent = 0, Qt::WFlags flags = 0);
	~ArTestDlg();

private:
};

#endif // ARTESTDLG_H
