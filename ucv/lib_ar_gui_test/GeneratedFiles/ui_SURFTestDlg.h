/********************************************************************************
** Form generated from reading UI file 'SURFTestDlg.ui'
**
** Created: Tue Aug 30 00:13:07 2011
**      by: Qt User Interface Compiler version 4.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SURFTESTDLG_H
#define UI_SURFTESTDLG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QSlider>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include "SURFTestWidget.h"

QT_BEGIN_NAMESPACE

class Ui_SURFTestDlg
{
public:
    QGridLayout *gridLayout;
    SURFTestWidget *m_SURFTestWidget;
    QGroupBox *m_SettingsBox;
    QVBoxLayout *verticalLayout;
    QSlider *m_MinScale;
    QSlider *m_MaxScale;
    QSpacerItem *verticalSpacer;

    void setupUi(QDialog *SURFTestDlg)
    {
        if (SURFTestDlg->objectName().isEmpty())
            SURFTestDlg->setObjectName(QString::fromUtf8("SURFTestDlg"));
        SURFTestDlg->resize(439, 310);
        gridLayout = new QGridLayout(SURFTestDlg);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        m_SURFTestWidget = new SURFTestWidget(SURFTestDlg);
        m_SURFTestWidget->setObjectName(QString::fromUtf8("m_SURFTestWidget"));
        QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(m_SURFTestWidget->sizePolicy().hasHeightForWidth());
        m_SURFTestWidget->setSizePolicy(sizePolicy);

        gridLayout->addWidget(m_SURFTestWidget, 0, 0, 1, 1);

        m_SettingsBox = new QGroupBox(SURFTestDlg);
        m_SettingsBox->setObjectName(QString::fromUtf8("m_SettingsBox"));
        verticalLayout = new QVBoxLayout(m_SettingsBox);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        m_MinScale = new QSlider(m_SettingsBox);
        m_MinScale->setObjectName(QString::fromUtf8("m_MinScale"));
        m_MinScale->setMaximum(200);
        m_MinScale->setOrientation(Qt::Horizontal);

        verticalLayout->addWidget(m_MinScale);

        m_MaxScale = new QSlider(m_SettingsBox);
        m_MaxScale->setObjectName(QString::fromUtf8("m_MaxScale"));
        m_MaxScale->setMaximum(200);
        m_MaxScale->setValue(200);
        m_MaxScale->setOrientation(Qt::Horizontal);

        verticalLayout->addWidget(m_MaxScale);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        gridLayout->addWidget(m_SettingsBox, 0, 1, 1, 1);


        retranslateUi(SURFTestDlg);

        QMetaObject::connectSlotsByName(SURFTestDlg);
    } // setupUi

    void retranslateUi(QDialog *SURFTestDlg)
    {
        SURFTestDlg->setWindowTitle(QApplication::translate("SURFTestDlg", "Dialog", 0, QApplication::UnicodeUTF8));
        m_SettingsBox->setTitle(QApplication::translate("SURFTestDlg", "Settings", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class SURFTestDlg: public Ui_SURFTestDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SURFTESTDLG_H
