#include "Prec.h"
#include "ArTestDlg.h"
#include "FiducialTestDlg.h"

int main(int argc, char *argv[])
{
	QGLFormat defaultGLFormat=QGLFormat::defaultFormat();
	defaultGLFormat.setRgba(true);
	QGLFormat::setDefaultFormat(defaultGLFormat);
	
	QApplication a(argc, argv);
#if 0
	ArTestDlg w;
#else
	FiducialTestDlg w;
#endif
	w.show();
	return a.exec();
}
