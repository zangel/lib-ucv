#include "Prec.h"
#include "ArTestDlg.h"

int main(int argc, char *argv[])
{
	QGLFormat defaultGLFormat=QGLFormat::defaultFormat();
	defaultGLFormat.setRgba(true);
	QGLFormat::setDefaultFormat(defaultGLFormat);
	
	QApplication a(argc, argv);
	ArTestDlg w;
	w.show();
	return a.exec();
}
