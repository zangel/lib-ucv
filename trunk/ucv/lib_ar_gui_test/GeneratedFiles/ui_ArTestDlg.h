/********************************************************************************
** Form generated from reading UI file 'ArTestDlg.ui'
**
** Created: Mon 11. Apr 15:21:28 2011
**      by: Qt User Interface Compiler version 4.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ARTESTDLG_H
#define UI_ARTESTDLG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QVBoxLayout>
#include "PreviewWidget.h"

QT_BEGIN_NAMESPACE

class Ui_ArTestDlgClass
{
public:
    QHBoxLayout *horizontalLayout_5;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QSpacerItem *horizontalSpacer_3;
    QComboBox *m_PreviewSize;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_3;
    QSpacerItem *horizontalSpacer_4;
    QSpinBox *m_PreviewRate;
    QLabel *m_MarkerPreview;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout_3;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *m_StartStop;
    QSpacerItem *horizontalSpacer;
    PreviewWidget *m_VideoPreview;
    QSpacerItem *horizontalSpacer_5;

    void setupUi(QDialog *ArTestDlgClass)
    {
        if (ArTestDlgClass->objectName().isEmpty())
            ArTestDlgClass->setObjectName(QString::fromUtf8("ArTestDlgClass"));
        ArTestDlgClass->resize(417, 279);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(ArTestDlgClass->sizePolicy().hasHeightForWidth());
        ArTestDlgClass->setSizePolicy(sizePolicy);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/ArTestDlg/lib_ar_gui_test.ico"), QSize(), QIcon::Normal, QIcon::Off);
        ArTestDlgClass->setWindowIcon(icon);
        ArTestDlgClass->setSizeGripEnabled(true);
        horizontalLayout_5 = new QHBoxLayout(ArTestDlgClass);
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        horizontalLayout_5->setSizeConstraint(QLayout::SetDefaultConstraint);
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(ArTestDlgClass);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_3);

        m_PreviewSize = new QComboBox(ArTestDlgClass);
        m_PreviewSize->setObjectName(QString::fromUtf8("m_PreviewSize"));
        m_PreviewSize->setEnabled(false);
        m_PreviewSize->setSizeAdjustPolicy(QComboBox::AdjustToContents);

        horizontalLayout->addWidget(m_PreviewSize);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_3 = new QLabel(ArTestDlgClass);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        horizontalLayout_4->addWidget(label_3);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_4);

        m_PreviewRate = new QSpinBox(ArTestDlgClass);
        m_PreviewRate->setObjectName(QString::fromUtf8("m_PreviewRate"));
        m_PreviewRate->setMinimum(1);
        m_PreviewRate->setMaximum(100);
        m_PreviewRate->setValue(30);

        horizontalLayout_4->addWidget(m_PreviewRate);


        verticalLayout->addLayout(horizontalLayout_4);

        m_MarkerPreview = new QLabel(ArTestDlgClass);
        m_MarkerPreview->setObjectName(QString::fromUtf8("m_MarkerPreview"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(m_MarkerPreview->sizePolicy().hasHeightForWidth());
        m_MarkerPreview->setSizePolicy(sizePolicy1);
        m_MarkerPreview->setFrameShape(QFrame::Panel);
        m_MarkerPreview->setFrameShadow(QFrame::Sunken);
        m_MarkerPreview->setTextFormat(Qt::PlainText);

        verticalLayout->addWidget(m_MarkerPreview);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);

        m_StartStop = new QPushButton(ArTestDlgClass);
        m_StartStop->setObjectName(QString::fromUtf8("m_StartStop"));
        m_StartStop->setEnabled(false);
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(m_StartStop->sizePolicy().hasHeightForWidth());
        m_StartStop->setSizePolicy(sizePolicy2);
        m_StartStop->setFlat(false);

        horizontalLayout_3->addWidget(m_StartStop);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout_3);


        horizontalLayout_5->addLayout(verticalLayout);

        m_VideoPreview = new PreviewWidget(ArTestDlgClass);
        m_VideoPreview->setObjectName(QString::fromUtf8("m_VideoPreview"));

        horizontalLayout_5->addWidget(m_VideoPreview);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_5);


        retranslateUi(ArTestDlgClass);

        QMetaObject::connectSlotsByName(ArTestDlgClass);
    } // setupUi

    void retranslateUi(QDialog *ArTestDlgClass)
    {
        ArTestDlgClass->setWindowTitle(QApplication::translate("ArTestDlgClass", "ArTestDlg", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("ArTestDlgClass", "Preview size:", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("ArTestDlgClass", "Preview rate (fps):", 0, QApplication::UnicodeUTF8));
        m_MarkerPreview->setText(QString());
        m_StartStop->setText(QApplication::translate("ArTestDlgClass", "Start", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ArTestDlgClass: public Ui_ArTestDlgClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ARTESTDLG_H
