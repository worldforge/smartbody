/********************************************************************************
** Form generated from reading UI file 'FaceDialog.ui'
**
** Created: Tue May 29 15:01:03 2012
**      by: Qt User Interface Compiler version 4.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FACEDIALOG_H
#define UI_FACEDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QFrame>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QScrollArea>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FaceDialog
{
public:
    QFrame *frame_2;
    QComboBox *characterNameBox;
    QPushButton *resetButton;
    QLabel *characterLabel;
    QCheckBox *interactiveModeCheckBox;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QLabel *facialExpressionsLabel;

    void setupUi(QDialog *FaceDialog)
    {
        if (FaceDialog->objectName().isEmpty())
            FaceDialog->setObjectName(QString::fromUtf8("FaceDialog"));
        FaceDialog->resize(370, 652);
        frame_2 = new QFrame(FaceDialog);
        frame_2->setObjectName(QString::fromUtf8("frame_2"));
        frame_2->setGeometry(QRect(10, 10, 341, 51));
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);
        characterNameBox = new QComboBox(frame_2);
        characterNameBox->setObjectName(QString::fromUtf8("characterNameBox"));
        characterNameBox->setGeometry(QRect(10, 20, 131, 20));
        characterNameBox->setMaximumSize(QSize(150, 16777215));
        resetButton = new QPushButton(frame_2);
        resetButton->setObjectName(QString::fromUtf8("resetButton"));
        resetButton->setGeometry(QRect(150, 20, 75, 21));
        resetButton->setMaximumSize(QSize(100, 16777215));
        characterLabel = new QLabel(frame_2);
        characterLabel->setObjectName(QString::fromUtf8("characterLabel"));
        characterLabel->setGeometry(QRect(10, 0, 48, 16));
        characterLabel->setMaximumSize(QSize(50, 16777215));
        interactiveModeCheckBox = new QCheckBox(frame_2);
        interactiveModeCheckBox->setObjectName(QString::fromUtf8("interactiveModeCheckBox"));
        interactiveModeCheckBox->setGeometry(QRect(230, 10, 101, 41));
        interactiveModeCheckBox->setChecked(true);
        scrollArea = new QScrollArea(FaceDialog);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        scrollArea->setGeometry(QRect(10, 90, 341, 551));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 339, 549));
        scrollArea->setWidget(scrollAreaWidgetContents);
        facialExpressionsLabel = new QLabel(FaceDialog);
        facialExpressionsLabel->setObjectName(QString::fromUtf8("facialExpressionsLabel"));
        facialExpressionsLabel->setGeometry(QRect(10, 70, 131, 16));
        QFont font;
        font.setPointSize(12);
        facialExpressionsLabel->setFont(font);

        retranslateUi(FaceDialog);

        QMetaObject::connectSlotsByName(FaceDialog);
    } // setupUi

    void retranslateUi(QDialog *FaceDialog)
    {
        FaceDialog->setWindowTitle(QApplication::translate("FaceDialog", "Face Dialog", 0, QApplication::UnicodeUTF8));
        resetButton->setText(QApplication::translate("FaceDialog", "Reset", 0, QApplication::UnicodeUTF8));
        characterLabel->setText(QApplication::translate("FaceDialog", "Character", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        interactiveModeCheckBox->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        interactiveModeCheckBox->setText(QApplication::translate("FaceDialog", "Receive Updates\n"
"From Smartbody", 0, QApplication::UnicodeUTF8));
        facialExpressionsLabel->setText(QApplication::translate("FaceDialog", "Facial Expressions", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class FaceDialog: public Ui_FaceDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FACEDIALOG_H
