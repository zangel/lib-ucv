/********************************************************************************
** Form generated from reading UI file 'CannyTestDlg.ui'
**
** Created: Tue Aug 30 00:13:07 2011
**      by: Qt User Interface Compiler version 4.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CANNYTESTDLG_H
#define UI_CANNYTESTDLG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include "CannyTestWidget.h"

QT_BEGIN_NAMESPACE

class Ui_CannyTestDlg
{
public:
    QGridLayout *gridLayout;
    CannyTestWidget *m_CannyTestWidget;

    void setupUi(QDialog *CannyTestDlg)
    {
        if (CannyTestDlg->objectName().isEmpty())
            CannyTestDlg->setObjectName(QString::fromUtf8("CannyTestDlg"));
        CannyTestDlg->resize(387, 348);
        gridLayout = new QGridLayout(CannyTestDlg);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        m_CannyTestWidget = new CannyTestWidget(CannyTestDlg);
        m_CannyTestWidget->setObjectName(QString::fromUtf8("m_CannyTestWidget"));

        gridLayout->addWidget(m_CannyTestWidget, 0, 0, 1, 1);


        retranslateUi(CannyTestDlg);

        QMetaObject::connectSlotsByName(CannyTestDlg);
    } // setupUi

    void retranslateUi(QDialog *CannyTestDlg)
    {
        CannyTestDlg->setWindowTitle(QApplication::translate("CannyTestDlg", "Dialog", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class CannyTestDlg: public Ui_CannyTestDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CANNYTESTDLG_H
