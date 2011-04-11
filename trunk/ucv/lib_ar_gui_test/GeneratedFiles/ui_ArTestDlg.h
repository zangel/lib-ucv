/********************************************************************************
** Form generated from reading UI file 'ArTestDlg.ui'
**
** Created: Mon 11. Apr 23:31:05 2011
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
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
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
    QGridLayout *gridLayout_3;
    QGroupBox *m_gbSettings;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QComboBox *m_PreviewSize;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_3;
    QSpinBox *m_PreviewRate;
    QGroupBox *m_gbVideoPreview;
    QGridLayout *gridLayout_2;
    PreviewWidget *m_VideoPreview;
    QGroupBox *m_gbMarkerImage;
    QGridLayout *gridLayout;
    QLabel *label_2;
    QLabel *m_MarkerPreview;
    QSpacerItem *horizontalSpacer;
    QPushButton *m_StartStop;
    QSpacerItem *horizontalSpacer_2;
    QSpacerItem *verticalSpacer;

    void setupUi(QDialog *ArTestDlgClass)
    {
        if (ArTestDlgClass->objectName().isEmpty())
            ArTestDlgClass->setObjectName(QString::fromUtf8("ArTestDlgClass"));
        ArTestDlgClass->resize(533, 378);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(ArTestDlgClass->sizePolicy().hasHeightForWidth());
        ArTestDlgClass->setSizePolicy(sizePolicy);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/ArTestDlg/lib_ar_gui_test.ico"), QSize(), QIcon::Normal, QIcon::Off);
        ArTestDlgClass->setWindowIcon(icon);
        ArTestDlgClass->setSizeGripEnabled(true);
        gridLayout_3 = new QGridLayout(ArTestDlgClass);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        m_gbSettings = new QGroupBox(ArTestDlgClass);
        m_gbSettings->setObjectName(QString::fromUtf8("m_gbSettings"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(m_gbSettings->sizePolicy().hasHeightForWidth());
        m_gbSettings->setSizePolicy(sizePolicy1);
        verticalLayout = new QVBoxLayout(m_gbSettings);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(m_gbSettings);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        m_PreviewSize = new QComboBox(m_gbSettings);
        m_PreviewSize->setObjectName(QString::fromUtf8("m_PreviewSize"));
        m_PreviewSize->setEnabled(false);
        m_PreviewSize->setSizeAdjustPolicy(QComboBox::AdjustToContents);

        horizontalLayout->addWidget(m_PreviewSize);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_3 = new QLabel(m_gbSettings);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        horizontalLayout_4->addWidget(label_3);

        m_PreviewRate = new QSpinBox(m_gbSettings);
        m_PreviewRate->setObjectName(QString::fromUtf8("m_PreviewRate"));
        m_PreviewRate->setMinimum(1);
        m_PreviewRate->setMaximum(100);
        m_PreviewRate->setValue(30);

        horizontalLayout_4->addWidget(m_PreviewRate);


        verticalLayout->addLayout(horizontalLayout_4);


        gridLayout_3->addWidget(m_gbSettings, 0, 0, 1, 3);

        m_gbVideoPreview = new QGroupBox(ArTestDlgClass);
        m_gbVideoPreview->setObjectName(QString::fromUtf8("m_gbVideoPreview"));
        sizePolicy1.setHeightForWidth(m_gbVideoPreview->sizePolicy().hasHeightForWidth());
        m_gbVideoPreview->setSizePolicy(sizePolicy1);
        gridLayout_2 = new QGridLayout(m_gbVideoPreview);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        m_VideoPreview = new PreviewWidget(m_gbVideoPreview);
        m_VideoPreview->setObjectName(QString::fromUtf8("m_VideoPreview"));
        m_VideoPreview->setMinimumSize(QSize(320, 240));

        gridLayout_2->addWidget(m_VideoPreview, 0, 0, 1, 1);


        gridLayout_3->addWidget(m_gbVideoPreview, 0, 3, 4, 1);

        m_gbMarkerImage = new QGroupBox(ArTestDlgClass);
        m_gbMarkerImage->setObjectName(QString::fromUtf8("m_gbMarkerImage"));
        sizePolicy1.setHeightForWidth(m_gbMarkerImage->sizePolicy().hasHeightForWidth());
        m_gbMarkerImage->setSizePolicy(sizePolicy1);
        gridLayout = new QGridLayout(m_gbMarkerImage);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label_2 = new QLabel(m_gbMarkerImage);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setWordWrap(true);

        gridLayout->addWidget(label_2, 0, 0, 1, 1);

        m_MarkerPreview = new QLabel(m_gbMarkerImage);
        m_MarkerPreview->setObjectName(QString::fromUtf8("m_MarkerPreview"));
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(m_MarkerPreview->sizePolicy().hasHeightForWidth());
        m_MarkerPreview->setSizePolicy(sizePolicy2);
        m_MarkerPreview->setMinimumSize(QSize(128, 128));
        m_MarkerPreview->setSizeIncrement(QSize(0, 0));
        m_MarkerPreview->setFrameShape(QFrame::Panel);
        m_MarkerPreview->setFrameShadow(QFrame::Sunken);
        m_MarkerPreview->setTextFormat(Qt::PlainText);

        gridLayout->addWidget(m_MarkerPreview, 1, 0, 1, 1);


        gridLayout_3->addWidget(m_gbMarkerImage, 1, 0, 1, 3);

        horizontalSpacer = new QSpacerItem(28, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);

        gridLayout_3->addItem(horizontalSpacer, 2, 0, 1, 1);

        m_StartStop = new QPushButton(ArTestDlgClass);
        m_StartStop->setObjectName(QString::fromUtf8("m_StartStop"));
        m_StartStop->setEnabled(false);
        QSizePolicy sizePolicy3(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(m_StartStop->sizePolicy().hasHeightForWidth());
        m_StartStop->setSizePolicy(sizePolicy3);
        m_StartStop->setFlat(false);

        gridLayout_3->addWidget(m_StartStop, 2, 1, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(48, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);

        gridLayout_3->addItem(horizontalSpacer_2, 2, 2, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_3->addItem(verticalSpacer, 3, 1, 1, 1);


        retranslateUi(ArTestDlgClass);

        QMetaObject::connectSlotsByName(ArTestDlgClass);
    } // setupUi

    void retranslateUi(QDialog *ArTestDlgClass)
    {
        ArTestDlgClass->setWindowTitle(QApplication::translate("ArTestDlgClass", "ArTestDlg", 0, QApplication::UnicodeUTF8));
        m_gbSettings->setTitle(QApplication::translate("ArTestDlgClass", "Settings :", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("ArTestDlgClass", "Preview size:", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("ArTestDlgClass", "Preview rate (fps):", 0, QApplication::UnicodeUTF8));
        m_gbVideoPreview->setTitle(QApplication::translate("ArTestDlgClass", "Camera view:", 0, QApplication::UnicodeUTF8));
        m_gbMarkerImage->setTitle(QApplication::translate("ArTestDlgClass", "Marker image:", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("ArTestDlgClass", "drop an image file to be used as marker:", 0, QApplication::UnicodeUTF8));
        m_MarkerPreview->setText(QString());
        m_StartStop->setText(QApplication::translate("ArTestDlgClass", "Start", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ArTestDlgClass: public Ui_ArTestDlgClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ARTESTDLG_H
