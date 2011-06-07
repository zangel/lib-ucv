#include "Prec.h"
#include "ArTestDlg.h"
#include "FiducialTestDlg.h"

int main(int argc, char *argv[])
{
	QGLFormat defaultGLFormat=QGLFormat::defaultFormat();
	defaultGLFormat.setRgba(true);
	QGLFormat::setDefaultFormat(defaultGLFormat);
	
	QApplication a(argc, argv);

	wchar_t moduleFileName[MAX_PATH];

	GetModuleFileName(GetModuleHandle(0), moduleFileName, MAX_PATH);

	qApp->addLibraryPath(QFileInfo(QString::fromWCharArray(moduleFileName)).absoluteDir().path()+"/plugins");

	QDialog *pDialog(0);

	switch(QMessageBox::question(0,
		"Choose marker test :",
		"Do you want to run markerless (natural features) or fiducial marker detection/tracking test?\n (Yes=markerless, No=fiducial)",
		QMessageBox::Yes|QMessageBox::No))
	{
	case QMessageBox::Yes:
		pDialog=new ArTestDlg(); break;
	case QMessageBox::No:
		pDialog=new FiducialTestDlg(); break;
	}
	pDialog->show();
	return pDialog->exec();
}
