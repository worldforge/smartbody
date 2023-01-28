/********************************************************************************
** Form generated from reading UI file 'UtilsDialog.ui'
**
** Created: Tue May 29 15:01:03 2012
**      by: Qt User Interface Compiler version 4.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_UTILSDIALOG_H
#define UI_UTILSDIALOG_H

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
#include <QtGui/QPlainTextEdit>
#include <QtGui/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_UtilsDialog
{
public:
    QFrame *frame;
    QComboBox *bmlFilesBox;
    QLabel *ttsLabel;
    QLabel *bmlFilesLabel;
    QPlainTextEdit *ttsBox;
    QPushButton *SpeakButton;
    QFrame *line_2;
    QLabel *speechLabel;
    QPushButton *runBmlButton;
    QFrame *frame_2;
    QLabel *animationNamesLabel;
    QFrame *line;
    QPushButton *QueryAnimButton;
    QComboBox *animationNamesBox;
    QLabel *animationsLabel;
    QPlainTextEdit *animFilterBox;
    QLabel *filterLabel;
    QPushButton *PlayAnimButton;
    QPushButton *SetPostureButton;
    QFrame *frame_3;
    QLabel *renderingLabel;
    QCheckBox *showAxesBox;
    QFrame *line_3;
    QCheckBox *showEyeBeamsBox;
    QCheckBox *allowBoneUpdatesBox;
    QFrame *line_4;
    QFrame *frame_4;
    QPushButton *GazeAtButton;
    QLabel *gazeTargetLabel;
    QComboBox *gazeTargetBox;
    QFrame *line_5;
    QLabel *controlLabel;
    QPushButton *StopGazeButton;
    QFrame *line_6;
    QComboBox *selectedCharacterBox;
    QLabel *selectedCharacterLabel;
    QFrame *line_7;
    QFrame *line_8;
    QPushButton *refreshButton;

    void setupUi(QDialog *UtilsDialog)
    {
        if (UtilsDialog->objectName().isEmpty())
            UtilsDialog->setObjectName(QString::fromUtf8("UtilsDialog"));
        UtilsDialog->resize(488, 498);
        frame = new QFrame(UtilsDialog);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setGeometry(QRect(250, 70, 211, 221));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        bmlFilesBox = new QComboBox(frame);
        bmlFilesBox->setObjectName(QString::fromUtf8("bmlFilesBox"));
        bmlFilesBox->setGeometry(QRect(10, 160, 191, 22));
        bmlFilesBox->setEditable(true);
        ttsLabel = new QLabel(frame);
        ttsLabel->setObjectName(QString::fromUtf8("ttsLabel"));
        ttsLabel->setGeometry(QRect(10, 40, 91, 16));
        bmlFilesLabel = new QLabel(frame);
        bmlFilesLabel->setObjectName(QString::fromUtf8("bmlFilesLabel"));
        bmlFilesLabel->setGeometry(QRect(10, 140, 51, 16));
        ttsBox = new QPlainTextEdit(frame);
        ttsBox->setObjectName(QString::fromUtf8("ttsBox"));
        ttsBox->setGeometry(QRect(10, 60, 191, 41));
        SpeakButton = new QPushButton(frame);
        SpeakButton->setObjectName(QString::fromUtf8("SpeakButton"));
        SpeakButton->setGeometry(QRect(10, 110, 61, 23));
        line_2 = new QFrame(frame);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setGeometry(QRect(10, 30, 118, 3));
        line_2->setLineWidth(1);
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);
        speechLabel = new QLabel(frame);
        speechLabel->setObjectName(QString::fromUtf8("speechLabel"));
        speechLabel->setGeometry(QRect(10, 10, 71, 16));
        QFont font;
        font.setPointSize(10);
        speechLabel->setFont(font);
        runBmlButton = new QPushButton(frame);
        runBmlButton->setObjectName(QString::fromUtf8("runBmlButton"));
        runBmlButton->setGeometry(QRect(10, 190, 61, 23));
        frame_2 = new QFrame(UtilsDialog);
        frame_2->setObjectName(QString::fromUtf8("frame_2"));
        frame_2->setGeometry(QRect(10, 70, 211, 221));
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);
        animationNamesLabel = new QLabel(frame_2);
        animationNamesLabel->setObjectName(QString::fromUtf8("animationNamesLabel"));
        animationNamesLabel->setGeometry(QRect(10, 100, 91, 16));
        line = new QFrame(frame_2);
        line->setObjectName(QString::fromUtf8("line"));
        line->setGeometry(QRect(10, 30, 118, 3));
        line->setLineWidth(1);
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        QueryAnimButton = new QPushButton(frame_2);
        QueryAnimButton->setObjectName(QString::fromUtf8("QueryAnimButton"));
        QueryAnimButton->setGeometry(QRect(10, 180, 75, 23));
        animationNamesBox = new QComboBox(frame_2);
        animationNamesBox->setObjectName(QString::fromUtf8("animationNamesBox"));
        animationNamesBox->setGeometry(QRect(10, 120, 191, 22));
        animationsLabel = new QLabel(frame_2);
        animationsLabel->setObjectName(QString::fromUtf8("animationsLabel"));
        animationsLabel->setGeometry(QRect(10, 10, 71, 16));
        animationsLabel->setFont(font);
        animFilterBox = new QPlainTextEdit(frame_2);
        animFilterBox->setObjectName(QString::fromUtf8("animFilterBox"));
        animFilterBox->setGeometry(QRect(10, 60, 191, 31));
        filterLabel = new QLabel(frame_2);
        filterLabel->setObjectName(QString::fromUtf8("filterLabel"));
        filterLabel->setGeometry(QRect(10, 40, 71, 16));
        PlayAnimButton = new QPushButton(frame_2);
        PlayAnimButton->setObjectName(QString::fromUtf8("PlayAnimButton"));
        PlayAnimButton->setGeometry(QRect(10, 150, 71, 23));
        SetPostureButton = new QPushButton(frame_2);
        SetPostureButton->setObjectName(QString::fromUtf8("SetPostureButton"));
        SetPostureButton->setGeometry(QRect(90, 150, 71, 23));
        frame_3 = new QFrame(UtilsDialog);
        frame_3->setObjectName(QString::fromUtf8("frame_3"));
        frame_3->setGeometry(QRect(250, 310, 141, 91));
        frame_3->setFrameShape(QFrame::StyledPanel);
        frame_3->setFrameShadow(QFrame::Raised);
        renderingLabel = new QLabel(frame_3);
        renderingLabel->setObjectName(QString::fromUtf8("renderingLabel"));
        renderingLabel->setGeometry(QRect(10, 0, 71, 16));
        renderingLabel->setFont(font);
        showAxesBox = new QCheckBox(frame_3);
        showAxesBox->setObjectName(QString::fromUtf8("showAxesBox"));
        showAxesBox->setGeometry(QRect(10, 30, 81, 17));
        line_3 = new QFrame(frame_3);
        line_3->setObjectName(QString::fromUtf8("line_3"));
        line_3->setGeometry(QRect(10, 20, 118, 3));
        line_3->setLineWidth(1);
        line_3->setFrameShape(QFrame::HLine);
        line_3->setFrameShadow(QFrame::Sunken);
        showEyeBeamsBox = new QCheckBox(frame_3);
        showEyeBeamsBox->setObjectName(QString::fromUtf8("showEyeBeamsBox"));
        showEyeBeamsBox->setGeometry(QRect(10, 50, 111, 17));
        allowBoneUpdatesBox = new QCheckBox(frame_3);
        allowBoneUpdatesBox->setObjectName(QString::fromUtf8("allowBoneUpdatesBox"));
        allowBoneUpdatesBox->setGeometry(QRect(10, 70, 121, 17));
        line_4 = new QFrame(UtilsDialog);
        line_4->setObjectName(QString::fromUtf8("line_4"));
        line_4->setGeometry(QRect(230, 70, 20, 231));
        line_4->setFrameShape(QFrame::VLine);
        line_4->setFrameShadow(QFrame::Sunken);
        frame_4 = new QFrame(UtilsDialog);
        frame_4->setObjectName(QString::fromUtf8("frame_4"));
        frame_4->setGeometry(QRect(20, 310, 211, 111));
        frame_4->setFrameShape(QFrame::StyledPanel);
        frame_4->setFrameShadow(QFrame::Raised);
        GazeAtButton = new QPushButton(frame_4);
        GazeAtButton->setObjectName(QString::fromUtf8("GazeAtButton"));
        GazeAtButton->setGeometry(QRect(0, 80, 61, 23));
        gazeTargetLabel = new QLabel(frame_4);
        gazeTargetLabel->setObjectName(QString::fromUtf8("gazeTargetLabel"));
        gazeTargetLabel->setGeometry(QRect(0, 30, 61, 16));
        gazeTargetBox = new QComboBox(frame_4);
        gazeTargetBox->setObjectName(QString::fromUtf8("gazeTargetBox"));
        gazeTargetBox->setGeometry(QRect(0, 50, 131, 22));
        line_5 = new QFrame(frame_4);
        line_5->setObjectName(QString::fromUtf8("line_5"));
        line_5->setGeometry(QRect(0, 20, 118, 3));
        line_5->setLineWidth(1);
        line_5->setFrameShape(QFrame::HLine);
        line_5->setFrameShadow(QFrame::Sunken);
        controlLabel = new QLabel(frame_4);
        controlLabel->setObjectName(QString::fromUtf8("controlLabel"));
        controlLabel->setGeometry(QRect(0, 0, 71, 16));
        controlLabel->setFont(font);
        StopGazeButton = new QPushButton(frame_4);
        StopGazeButton->setObjectName(QString::fromUtf8("StopGazeButton"));
        StopGazeButton->setGeometry(QRect(70, 80, 61, 23));
        line_6 = new QFrame(UtilsDialog);
        line_6->setObjectName(QString::fromUtf8("line_6"));
        line_6->setGeometry(QRect(20, 290, 441, 16));
        line_6->setLineWidth(1);
        line_6->setFrameShape(QFrame::HLine);
        line_6->setFrameShadow(QFrame::Sunken);
        selectedCharacterBox = new QComboBox(UtilsDialog);
        selectedCharacterBox->setObjectName(QString::fromUtf8("selectedCharacterBox"));
        selectedCharacterBox->setGeometry(QRect(10, 30, 131, 22));
        selectedCharacterLabel = new QLabel(UtilsDialog);
        selectedCharacterLabel->setObjectName(QString::fromUtf8("selectedCharacterLabel"));
        selectedCharacterLabel->setGeometry(QRect(10, 10, 91, 16));
        line_7 = new QFrame(UtilsDialog);
        line_7->setObjectName(QString::fromUtf8("line_7"));
        line_7->setGeometry(QRect(20, 60, 441, 16));
        line_7->setLineWidth(1);
        line_7->setFrameShape(QFrame::HLine);
        line_7->setFrameShadow(QFrame::Sunken);
        line_8 = new QFrame(UtilsDialog);
        line_8->setObjectName(QString::fromUtf8("line_8"));
        line_8->setGeometry(QRect(230, 300, 20, 121));
        line_8->setFrameShape(QFrame::VLine);
        line_8->setFrameShadow(QFrame::Sunken);
        refreshButton = new QPushButton(UtilsDialog);
        refreshButton->setObjectName(QString::fromUtf8("refreshButton"));
        refreshButton->setGeometry(QRect(150, 30, 61, 23));

        retranslateUi(UtilsDialog);

        QMetaObject::connectSlotsByName(UtilsDialog);
    } // setupUi

    void retranslateUi(QDialog *UtilsDialog)
    {
        UtilsDialog->setWindowTitle(QApplication::translate("UtilsDialog", "Utils", 0, QApplication::UnicodeUTF8));
        ttsLabel->setText(QApplication::translate("UtilsDialog", "Text to Speech", 0, QApplication::UnicodeUTF8));
        bmlFilesLabel->setText(QApplication::translate("UtilsDialog", "BML Files", 0, QApplication::UnicodeUTF8));
        SpeakButton->setText(QApplication::translate("UtilsDialog", "Speak", 0, QApplication::UnicodeUTF8));
        speechLabel->setText(QApplication::translate("UtilsDialog", "Speech", 0, QApplication::UnicodeUTF8));
        runBmlButton->setText(QApplication::translate("UtilsDialog", "Run", 0, QApplication::UnicodeUTF8));
        animationNamesLabel->setText(QApplication::translate("UtilsDialog", "Animation Names", 0, QApplication::UnicodeUTF8));
        QueryAnimButton->setText(QApplication::translate("UtilsDialog", "Query Anims", 0, QApplication::UnicodeUTF8));
        animationsLabel->setText(QApplication::translate("UtilsDialog", "Animations", 0, QApplication::UnicodeUTF8));
        filterLabel->setText(QApplication::translate("UtilsDialog", "Search Filter", 0, QApplication::UnicodeUTF8));
        PlayAnimButton->setText(QApplication::translate("UtilsDialog", "Play", 0, QApplication::UnicodeUTF8));
        SetPostureButton->setText(QApplication::translate("UtilsDialog", "Set Posture", 0, QApplication::UnicodeUTF8));
        renderingLabel->setText(QApplication::translate("UtilsDialog", "Rendering", 0, QApplication::UnicodeUTF8));
        showAxesBox->setText(QApplication::translate("UtilsDialog", "Show Axes", 0, QApplication::UnicodeUTF8));
        showEyeBeamsBox->setText(QApplication::translate("UtilsDialog", "Show Eye Beams", 0, QApplication::UnicodeUTF8));
        allowBoneUpdatesBox->setText(QApplication::translate("UtilsDialog", "Allow Bone Updates", 0, QApplication::UnicodeUTF8));
        GazeAtButton->setText(QApplication::translate("UtilsDialog", "Gaze At", 0, QApplication::UnicodeUTF8));
        gazeTargetLabel->setText(QApplication::translate("UtilsDialog", "Gaze Target", 0, QApplication::UnicodeUTF8));
        controlLabel->setText(QApplication::translate("UtilsDialog", "Control", 0, QApplication::UnicodeUTF8));
        StopGazeButton->setText(QApplication::translate("UtilsDialog", "Stop Gaze", 0, QApplication::UnicodeUTF8));
        selectedCharacterLabel->setText(QApplication::translate("UtilsDialog", "Selected Character", 0, QApplication::UnicodeUTF8));
        refreshButton->setText(QApplication::translate("UtilsDialog", "Refresh", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class UtilsDialog: public Ui_UtilsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_UTILSDIALOG_H
