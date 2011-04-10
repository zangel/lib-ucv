#ifndef PREVIEW_WIDGET_H
#define PREVIEW_WIDGET_H

class PreviewWidget
	: public QGLWidget
{
	Q_OBJECT
public:
	PreviewWidget(QWidget *parent=0);
	~PreviewWidget();
};

#endif //PREVIEW_WIDGET_H