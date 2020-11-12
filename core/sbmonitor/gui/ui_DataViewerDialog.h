/********************************************************************************
** Form generated from reading UI file 'DataViewerDialog.ui'
**
** Created: Tue May 29 15:01:03 2012
**      by: Qt User Interface Compiler version 4.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DATAVIEWERDIALOG_H
#define UI_DATAVIEWERDIALOG_H

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
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DataViewerDialog
{
public:
    QFrame *frame;
    QLabel *characterNameLabel;
    QComboBox *characterNameBox;
    QLabel *controllerLabel;
    QComboBox *controllerBox;
    QLabel *motionLabel;
    QComboBox *motionBox;
    QPushButton *addChannelButton;
    QPushButton *removeChannelButton;
    QLabel *numFramesLabel;
    QSpinBox *numFramesBox;
    QLabel *showRotationAsLabel;
    QComboBox *showRotaionAsBox;
    QCheckBox *xCheckBox;
    QCheckBox *yCheckBox;
    QCheckBox *zCheckBox;
    QCheckBox *wCheckBox;
    QPushButton *refreshButton;
    QPushButton *freezeButton;
    QListWidget *channelListBox;
    QListWidget *monitoredListBox;
    QWidget *renderSize;

    void setupUi(QDialog *DataViewerDialog)
    {
        if (DataViewerDialog->objectName().isEmpty())
            DataViewerDialog->setObjectName(QString::fromUtf8("DataViewerDialog"));
        DataViewerDialog->resize(806, 599);
        frame = new QFrame(DataViewerDialog);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setGeometry(QRect(9, 9, 788, 211));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        characterNameLabel = new QLabel(frame);
        characterNameLabel->setObjectName(QString::fromUtf8("characterNameLabel"));
        characterNameLabel->setGeometry(QRect(10, 10, 61, 16));
        QFont font;
        font.setPointSize(10);
        characterNameLabel->setFont(font);
        characterNameBox = new QComboBox(frame);
        characterNameBox->setObjectName(QString::fromUtf8("characterNameBox"));
        characterNameBox->setGeometry(QRect(10, 30, 181, 22));
        controllerLabel = new QLabel(frame);
        controllerLabel->setObjectName(QString::fromUtf8("controllerLabel"));
        controllerLabel->setGeometry(QRect(10, 60, 61, 16));
        controllerLabel->setFont(font);
        controllerBox = new QComboBox(frame);
        controllerBox->setObjectName(QString::fromUtf8("controllerBox"));
        controllerBox->setGeometry(QRect(10, 80, 181, 22));
        motionLabel = new QLabel(frame);
        motionLabel->setObjectName(QString::fromUtf8("motionLabel"));
        motionLabel->setGeometry(QRect(10, 110, 41, 16));
        motionLabel->setFont(font);
        motionBox = new QComboBox(frame);
        motionBox->setObjectName(QString::fromUtf8("motionBox"));
        motionBox->setGeometry(QRect(10, 130, 181, 22));
        addChannelButton = new QPushButton(frame);
        addChannelButton->setObjectName(QString::fromUtf8("addChannelButton"));
        addChannelButton->setGeometry(QRect(460, 20, 61, 23));
        removeChannelButton = new QPushButton(frame);
        removeChannelButton->setObjectName(QString::fromUtf8("removeChannelButton"));
        removeChannelButton->setGeometry(QRect(460, 50, 61, 23));
        numFramesLabel = new QLabel(frame);
        numFramesLabel->setObjectName(QString::fromUtf8("numFramesLabel"));
        numFramesLabel->setGeometry(QRect(10, 170, 121, 16));
        numFramesLabel->setFont(font);
        numFramesBox = new QSpinBox(frame);
        numFramesBox->setObjectName(QString::fromUtf8("numFramesBox"));
        numFramesBox->setGeometry(QRect(130, 170, 61, 22));
        numFramesBox->setMaximum(1000);
        numFramesBox->setValue(800);
        showRotationAsLabel = new QLabel(frame);
        showRotationAsLabel->setObjectName(QString::fromUtf8("showRotationAsLabel"));
        showRotationAsLabel->setGeometry(QRect(220, 170, 111, 16));
        showRotationAsLabel->setFont(font);
        showRotaionAsBox = new QComboBox(frame);
        showRotaionAsBox->setObjectName(QString::fromUtf8("showRotaionAsBox"));
        showRotaionAsBox->setGeometry(QRect(330, 170, 111, 22));
        xCheckBox = new QCheckBox(frame);
        xCheckBox->setObjectName(QString::fromUtf8("xCheckBox"));
        xCheckBox->setGeometry(QRect(450, 170, 31, 17));
        xCheckBox->setChecked(true);
        xCheckBox->setTristate(false);
        yCheckBox = new QCheckBox(frame);
        yCheckBox->setObjectName(QString::fromUtf8("yCheckBox"));
        yCheckBox->setGeometry(QRect(480, 170, 31, 17));
        yCheckBox->setChecked(true);
        zCheckBox = new QCheckBox(frame);
        zCheckBox->setObjectName(QString::fromUtf8("zCheckBox"));
        zCheckBox->setGeometry(QRect(510, 170, 31, 17));
        zCheckBox->setChecked(true);
        wCheckBox = new QCheckBox(frame);
        wCheckBox->setObjectName(QString::fromUtf8("wCheckBox"));
        wCheckBox->setGeometry(QRect(540, 170, 31, 17));
        wCheckBox->setChecked(true);
        refreshButton = new QPushButton(frame);
        refreshButton->setObjectName(QString::fromUtf8("refreshButton"));
        refreshButton->setGeometry(QRect(630, 170, 61, 23));
        freezeButton = new QPushButton(frame);
        freezeButton->setObjectName(QString::fromUtf8("freezeButton"));
        freezeButton->setGeometry(QRect(700, 170, 61, 23));
        channelListBox = new QListWidget(frame);
        channelListBox->setObjectName(QString::fromUtf8("channelListBox"));
        channelListBox->setGeometry(QRect(200, 10, 251, 141));
        channelListBox->setSelectionMode(QAbstractItemView::MultiSelection);
        monitoredListBox = new QListWidget(frame);
        monitoredListBox->setObjectName(QString::fromUtf8("monitoredListBox"));
        monitoredListBox->setGeometry(QRect(530, 10, 251, 141));
        monitoredListBox->setSelectionMode(QAbstractItemView::MultiSelection);
        renderSize = new QWidget(DataViewerDialog);
        renderSize->setObjectName(QString::fromUtf8("renderSize"));
        renderSize->setGeometry(QRect(9, 229, 788, 361));

        retranslateUi(DataViewerDialog);

        QMetaObject::connectSlotsByName(DataViewerDialog);
    } // setupUi

    void retranslateUi(QDialog *DataViewerDialog)
    {
        DataViewerDialog->setWindowTitle(QApplication::translate("DataViewerDialog", "Data Viewer", 0, QApplication::UnicodeUTF8));
        characterNameLabel->setText(QApplication::translate("DataViewerDialog", "Character", 0, QApplication::UnicodeUTF8));
        controllerLabel->setText(QApplication::translate("DataViewerDialog", "Controller", 0, QApplication::UnicodeUTF8));
        motionLabel->setText(QApplication::translate("DataViewerDialog", "Motion", 0, QApplication::UnicodeUTF8));
        addChannelButton->setText(QApplication::translate("DataViewerDialog", ">>>", 0, QApplication::UnicodeUTF8));
        removeChannelButton->setText(QApplication::translate("DataViewerDialog", "<<<", 0, QApplication::UnicodeUTF8));
        numFramesLabel->setText(QApplication::translate("DataViewerDialog", "Num Frames Shown", 0, QApplication::UnicodeUTF8));
        showRotationAsLabel->setText(QApplication::translate("DataViewerDialog", "Show Rotation As", 0, QApplication::UnicodeUTF8));
        xCheckBox->setText(QApplication::translate("DataViewerDialog", "X", 0, QApplication::UnicodeUTF8));
        yCheckBox->setText(QApplication::translate("DataViewerDialog", "Y", 0, QApplication::UnicodeUTF8));
        zCheckBox->setText(QApplication::translate("DataViewerDialog", "Z", 0, QApplication::UnicodeUTF8));
        wCheckBox->setText(QApplication::translate("DataViewerDialog", "W", 0, QApplication::UnicodeUTF8));
        refreshButton->setText(QApplication::translate("DataViewerDialog", "Refresh", 0, QApplication::UnicodeUTF8));
        freezeButton->setText(QApplication::translate("DataViewerDialog", "Freeze", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class DataViewerDialog: public Ui_DataViewerDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DATAVIEWERDIALOG_H
