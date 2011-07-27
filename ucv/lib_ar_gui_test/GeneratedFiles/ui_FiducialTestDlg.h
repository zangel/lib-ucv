/********************************************************************************
** Form generated from reading UI file 'FiducialTestDlg.ui'
**
** Created: Mon Jul 25 23:13:49 2011
**      by: Qt User Interface Compiler version 4.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FIDUCIALTESTDLG_H
#define UI_FIDUCIALTESTDLG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include "FiducialTestPreviewWidget.h"

QT_BEGIN_NAMESPACE

class Ui_FiducialTestDlg
{
public:
    QGridLayout *gridLayout;
    FiducialTestPreviewWidget *m_FiducialPreview;

    void setupUi(QDialog *FiducialTestDlg)
    {
        if (FiducialTestDlg->objectName().isEmpty())
            FiducialTestDlg->setObjectName(QString::fromUtf8("FiducialTestDlg"));
        FiducialTestDlg->resize(312, 268);
        gridLayout = new QGridLayout(FiducialTestDlg);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        m_FiducialPreview = new FiducialTestPreviewWidget(FiducialTestDlg);
        m_FiducialPreview->setObjectName(QString::fromUtf8("m_FiducialPreview"));

        gridLayout->addWidget(m_FiducialPreview, 0, 0, 1, 1);


        retranslateUi(FiducialTestDlg);

        QMetaObject::connectSlotsByName(FiducialTestDlg);
    } // setupUi

    void retranslateUi(QDialog *FiducialTestDlg)
    {
        FiducialTestDlg->setWindowTitle(QApplication::translate("FiducialTestDlg", "Dialog", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class FiducialTestDlg: public Ui_FiducialTestDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FIDUCIALTESTDLG_H
