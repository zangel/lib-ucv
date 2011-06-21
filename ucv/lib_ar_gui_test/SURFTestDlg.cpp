#include "Prec.h"
#include "SURFTestDlg.h"

SURFTestDlg::SURFTestDlg(QWidget *parent /* = 0 */, Qt::WFlags flags /* = 0 */)
	: QDialog(parent,flags)
{
	setupUi(this);

	connect(m_MinScale, SIGNAL(valueChanged(int)), this, SLOT(onMinMaxChanged()));
	connect(m_MaxScale, SIGNAL(valueChanged(int)), this, SLOT(onMinMaxChanged()));
}

SURFTestDlg::~SURFTestDlg()
{
}

void SURFTestDlg::onMinMaxChanged()
{
	m_SURFTestWidget->setMinMaxScale(
		float(m_MinScale->value())/10.0f,
		float(m_MaxScale->value())/10.0f
	);
}
