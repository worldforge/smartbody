/********************************************************************************
** Form generated from reading UI file 'BMLCreatorDialog.ui'
**
** Created: Tue May 29 15:01:03 2012
**      by: Qt User Interface Compiler version 4.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BMLCREATORDIALOG_H
#define UI_BMLCREATORDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QFrame>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtGui/QTabWidget>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_BMLCreator
{
public:
    QPlainTextEdit *bmlCommandBox;
    QFrame *frame;
    QFrame *middleFrame;
    QComboBox *characterBox;
    QPushButton *refreshButton;
    QPushButton *resetBml;
    QPushButton *runButton;
    QLabel *characterNameLabel;
    QTabWidget *tabWidget;
    QWidget *bodyTab;
    QLabel *postureLabel;
    QLabel *startLabel;
    QLabel *readyLabel;
    QLabel *idLabel;
    QPlainTextEdit *bodyIdBox;
    QPlainTextEdit *bodyReadyBox;
    QPlainTextEdit *bodyStartBox;
    QPlainTextEdit *bodyPostureBox;
    QLabel *idLabel_2;
    QFrame *line;
    QWidget *locomotionTab;
    QPlainTextEdit *locomotionFacingBox;
    QLabel *readyLabel_2;
    QPlainTextEdit *locomotionTargetBox;
    QLabel *idLabel_3;
    QPlainTextEdit *locomotionFollowBox;
    QLabel *startLabel_2;
    QLabel *postureLabel_2;
    QPlainTextEdit *locomotionAccelBox;
    QFrame *line_2;
    QLabel *idLabel_4;
    QLabel *idLabel_5;
    QLabel *idLabel_6;
    QLabel *idLabel_7;
    QLabel *idLabel_8;
    QLabel *idLabel_9;
    QLabel *idLabel_10;
    QLabel *idLabel_11;
    QLabel *idLabel_12;
    QComboBox *locomotionTypeBox;
    QComboBox *locomotionMannerBox;
    QLabel *idLabel_13;
    QPlainTextEdit *locomotionProximityBox;
    QPlainTextEdit *locomotionScootAccelBox;
    QPlainTextEdit *locomotionAngleAccelBox;
    QPlainTextEdit *locomotionNumStepsBox;
    QPlainTextEdit *locomotionStartBox;
    QPlainTextEdit *locomotionIdBox;
    QSlider *locomotionSbmBreakingSlider;
    QDoubleSpinBox *locomotionSbmBreakingBox;
    QWidget *animationTab;
    QLabel *idLabel_14;
    QFrame *line_3;
    QLabel *postureLabel_3;
    QPlainTextEdit *animationNameBox;
    QLabel *postureLabel_4;
    QPlainTextEdit *animationStartBox;
    QLabel *postureLabel_5;
    QPlainTextEdit *animationReadyBox;
    QLabel *postureLabel_6;
    QLabel *postureLabel_7;
    QPlainTextEdit *animationStrokeBox;
    QLabel *postureLabel_8;
    QPlainTextEdit *animationRelaxBox;
    QLabel *postureLabel_9;
    QPlainTextEdit *animationsEndBox;
    QLabel *postureLabel_10;
    QPlainTextEdit *animationIdBox;
    QSlider *animationSpeedSlider;
    QDoubleSpinBox *animationSpeedBox;
    QWidget *gestureTab;
    QLabel *postureLabel_11;
    QPlainTextEdit *gestureLexemeBox;
    QFrame *line_4;
    QLabel *idLabel_15;
    QLabel *postureLabel_12;
    QPlainTextEdit *gestureNameBox;
    QComboBox *gestureModeBox;
    QLabel *postureLabel_13;
    QPlainTextEdit *gestureReadyBox;
    QLabel *postureLabel_14;
    QPlainTextEdit *gestureTargetBox;
    QLabel *postureLabel_15;
    QPlainTextEdit *gestureStartBox;
    QLabel *postureLabel_16;
    QPlainTextEdit *gestureStrokeBox;
    QLabel *postureLabel_17;
    QPlainTextEdit *gestureRelaxBox;
    QLabel *postureLabel_18;
    QPlainTextEdit *gestureEndBox;
    QLabel *postureLabel_19;
    QPlainTextEdit *gestureIdBox;
    QLabel *postureLabel_20;
    QWidget *sbmreachTab;
    QLabel *idLabel_17;
    QPlainTextEdit *sbmReachSbmHandle;
    QLabel *postureLabel_24;
    QPlainTextEdit *sbmReachTarget;
    QLabel *postureLabel_25;
    QFrame *line_6;
    QLabel *postureLabel_26;
    QComboBox *sbmReachSbmAction;
    QCheckBox *sbmReachSbmFootIk;
    QCheckBox *sbmReachSbmReachFinish;
    QSlider *sbmReachsbmVelocity;
    QLabel *postureLabel_27;
    QLabel *postureLabel_28;
    QSlider *sbmReachSbmDuration;
    QPlainTextEdit *sbmReachStart;
    QLabel *postureLabel_29;
    QLabel *postureLabel_30;
    QPlainTextEdit *sbmReachId;
    QDoubleSpinBox *doubleSpinBox;
    QDoubleSpinBox *doubleSpinBox_2;
    QWidget *gazeTab;
    QPlainTextEdit *gazeTarget;
    QFrame *line_7;
    QLabel *postureLabel_31;
    QLabel *idLabel_18;
    QLabel *postureLabel_32;
    QComboBox *gazeSbmJointRange;
    QLabel *postureLabel_33;
    QComboBox *gazeDirection;
    QLabel *postureLabel_67;
    QSlider *gazeAngleSlider;
    QLabel *postureLabel_68;
    QComboBox *gazeSbmPriorityJoint;
    QPlainTextEdit *gazeSbmHandle;
    QLabel *postureLabel_69;
    QDoubleSpinBox *gazeAngleBox;
    QDoubleSpinBox *gazeSbmTimeHintBox;
    QLabel *postureLabel_106;
    QSlider *gazeSbmTimeHintSlider;
    QPlainTextEdit *gazeSbmJointSpeed;
    QLabel *postureLabel_107;
    QPlainTextEdit *gazeSbmJointSmooth;
    QLabel *postureLabel_108;
    QLabel *postureLabel_148;
    QSlider *gazeSbmFadeInSlider;
    QDoubleSpinBox *gazeSbmFadeInBox;
    QLabel *postureLabel_149;
    QLabel *postureLabel_150;
    QPlainTextEdit *gazeId;
    QPlainTextEdit *gazeStart;
    QLabel *postureLabel_151;
    QDoubleSpinBox *gazeSbmFadeOutBox;
    QSlider *gazeSbmFadeOutSlider;
    QWidget *sbmconstraintTab;
    QPlainTextEdit *sbmConstraintTarget;
    QLabel *postureLabel_208;
    QLabel *postureLabel_209;
    QPlainTextEdit *sbmConstraintSbmHandle;
    QLabel *idLabel_78;
    QFrame *line_34;
    QPlainTextEdit *sbmConstraintSbmRoot;
    QPlainTextEdit *sbmConstraintEffector;
    QLabel *postureLabel_210;
    QLabel *postureLabel_211;
    QPlainTextEdit *sbmConstraintSbmEffectRoot;
    QLabel *postureLabel_212;
    QPlainTextEdit *sbmConstraintStart;
    QLabel *postureLabel_213;
    QLabel *postureLabel_214;
    QPlainTextEdit *sbmConstraintReady;
    QPlainTextEdit *sbmConstraintId;
    QLabel *postureLabel_215;
    QLabel *postureLabel_216;
    QComboBox *sbmConstraintSbmConstraintType;
    QDoubleSpinBox *sbmConstraintRotZBox;
    QSlider *sbmConstraintSbmFadeOutSlider;
    QSlider *sbmConstraintPosXSlider;
    QLabel *postureLabel_217;
    QLabel *postureLabel_218;
    QLabel *postureLabel_219;
    QDoubleSpinBox *sbmConstraintPosYBox;
    QSlider *sbmConstraintRotYSlider;
    QDoubleSpinBox *sbmConstraintSbmFadeInBox;
    QSlider *sbmConstraintSbmFadeInSlider;
    QSlider *sbmConstraintRotZSlider;
    QSlider *sbmConstraintPosYSlider;
    QSlider *sbmConstraintPosZSlider;
    QLabel *postureLabel_220;
    QDoubleSpinBox *sbmConstraintPosXBox;
    QDoubleSpinBox *sbmConstraintRotXBox;
    QDoubleSpinBox *sbmConstraintPosZBox;
    QLabel *postureLabel_221;
    QLabel *postureLabel_222;
    QDoubleSpinBox *sbmConstraintRotYBox;
    QSlider *sbmConstraintRotXSlider;
    QDoubleSpinBox *sbmConstraintSbmFadeOutBox;
    QLabel *postureLabel_223;
    QLabel *postureLabel_224;
    QWidget *headTab;
    QLabel *idLabel_77;
    QFrame *line_33;
    QComboBox *headType;
    QLabel *postureLabel_193;
    QPlainTextEdit *headId;
    QLabel *postureLabel_194;
    QLabel *postureLabel_195;
    QPlainTextEdit *headEnd;
    QLabel *postureLabel_196;
    QPlainTextEdit *headStroke;
    QLabel *postureLabel_197;
    QLabel *postureLabel_199;
    QPlainTextEdit *headReady;
    QPlainTextEdit *headRelax;
    QPlainTextEdit *headStart;
    QLabel *postureLabel_201;
    QDoubleSpinBox *headRepeatsBox;
    QLabel *postureLabel_198;
    QSlider *headRepeatsSlider;
    QDoubleSpinBox *headAmountBox;
    QLabel *postureLabel_200;
    QSlider *headAmountSlider;
    QDoubleSpinBox *headSbmSmoothBox;
    QLabel *postureLabel_202;
    QSlider *headSbmSmoothSlider;
    QLabel *postureLabel_203;
    QDoubleSpinBox *headSbmWarpBox;
    QDoubleSpinBox *headSbmPeriodBox;
    QLabel *postureLabel_204;
    QSlider *headSbmWarpSlider;
    QSlider *headSbmPeriodSlider;
    QLabel *postureLabel_205;
    QDoubleSpinBox *headSbmAccelBox;
    QSlider *headSbmAccelSlider;
    QLabel *postureLabel_206;
    QLabel *postureLabel_207;
    QDoubleSpinBox *headSbmPitchBox;
    QSlider *headSbmPitchSlider;
    QSlider *headSbmDecaySlider;
    QDoubleSpinBox *headSbmDecayBox;
    QWidget *faceTab;
    QLabel *postureLabel_181;
    QLabel *idLabel_76;
    QFrame *line_32;
    QComboBox *faceType;
    QComboBox *faceSide;
    QLabel *postureLabel_182;
    QPlainTextEdit *faceAu;
    QLabel *postureLabel_183;
    QLabel *postureLabel_184;
    QSlider *faceAmountSlider;
    QDoubleSpinBox *faceAmountBox;
    QLabel *postureLabel_185;
    QPlainTextEdit *faceStart;
    QPlainTextEdit *faceReady;
    QLabel *postureLabel_186;
    QLabel *postureLabel_187;
    QPlainTextEdit *faceStroke;
    QLabel *postureLabel_188;
    QPlainTextEdit *faceRelax;
    QLabel *postureLabel_189;
    QPlainTextEdit *faceEnd;
    QLabel *postureLabel_190;
    QPlainTextEdit *faceSbmRampUp;
    QLabel *postureLabel_191;
    QPlainTextEdit *faceSbmRampDown;
    QLabel *postureLabel_192;
    QPlainTextEdit *faceId;
    QWidget *speechTab;
    QLabel *postureLabel_152;
    QPlainTextEdit *speechContent;
    QLabel *postureLabel_153;
    QLabel *idLabel_73;
    QFrame *line_29;
    QComboBox *speech;
    QPlainTextEdit *speechRef;
    QLabel *postureLabel_154;
    QPlainTextEdit *speechId;
    QLabel *postureLabel_155;
    QWidget *saccadeTab;
    QLabel *postureLabel_164;
    QComboBox *saccadeFinish;
    QLabel *idLabel_75;
    QFrame *line_31;
    QComboBox *saccadeMode;
    QLabel *postureLabel_165;
    QPlainTextEdit *saccadeAngleLimit;
    QLabel *postureLabel_166;
    QLabel *postureLabel_167;
    QPlainTextEdit *saccadeSbmBin0;
    QLabel *postureLabel_168;
    QPlainTextEdit *saccadeSbmBin45;
    QLabel *postureLabel_169;
    QPlainTextEdit *saccadeSbmBin90;
    QLabel *postureLabel_170;
    QPlainTextEdit *saccadeSbmBin135;
    QLabel *postureLabel_171;
    QPlainTextEdit *saccadeSbmBin180;
    QLabel *postureLabel_172;
    QPlainTextEdit *saccadeSbmBin225;
    QLabel *postureLabel_173;
    QPlainTextEdit *saccadeSbmBin270;
    QLabel *postureLabel_174;
    QPlainTextEdit *saccadeSbmBin315;
    QLabel *postureLabel_175;
    QPlainTextEdit *saccadeSbmMean;
    QLabel *postureLabel_176;
    QPlainTextEdit *saccadeSbmVariant;
    QLabel *postureLabel_177;
    QPlainTextEdit *saccadeDirection;
    QLabel *postureLabel_178;
    QPlainTextEdit *saccadeMagnitude;
    QLabel *postureLabel_179;
    QPlainTextEdit *saccadeSbmDuration;
    QPlainTextEdit *saccadeId;
    QLabel *postureLabel_180;
    QWidget *eventTab;
    QLabel *idLabel_16;
    QPlainTextEdit *eventStroke;
    QLabel *postureLabel_21;
    QLabel *postureLabel_22;
    QPlainTextEdit *eventId;
    QPlainTextEdit *eventMessage;
    QLabel *postureLabel_23;
    QFrame *line_5;
    QWidget *sbmstatesTab;
    QPlainTextEdit *sbmStatesName;
    QLabel *postureLabel_156;
    QLabel *idLabel_74;
    QFrame *line_30;
    QLabel *postureLabel_157;
    QComboBox *sbmStatesMode;
    QComboBox *sbmStatesLoop;
    QLabel *postureLabel_158;
    QComboBox *sbmStatesSbmStartNow;
    QLabel *postureLabel_159;
    QLabel *postureLabel_160;
    QPlainTextEdit *sbmStatesX;
    QLabel *postureLabel_161;
    QPlainTextEdit *sbmStatesY;
    QLabel *postureLabel_162;
    QPlainTextEdit *sbmStatesZ;
    QLabel *postureLabel_163;
    QPlainTextEdit *sbmStatesId;

    void setupUi(QDialog *BMLCreator)
    {
        if (BMLCreator->objectName().isEmpty())
            BMLCreator->setObjectName(QString::fromUtf8("BMLCreator"));
        BMLCreator->resize(840, 589);
        bmlCommandBox = new QPlainTextEdit(BMLCreator);
        bmlCommandBox->setObjectName(QString::fromUtf8("bmlCommandBox"));
        bmlCommandBox->setGeometry(QRect(10, 430, 821, 151));
        frame = new QFrame(BMLCreator);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setGeometry(QRect(10, 10, 821, 411));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        middleFrame = new QFrame(frame);
        middleFrame->setObjectName(QString::fromUtf8("middleFrame"));
        middleFrame->setGeometry(QRect(0, 360, 761, 51));
        middleFrame->setFrameShape(QFrame::StyledPanel);
        middleFrame->setFrameShadow(QFrame::Raised);
        characterBox = new QComboBox(middleFrame);
        characterBox->setObjectName(QString::fromUtf8("characterBox"));
        characterBox->setGeometry(QRect(70, 20, 151, 22));
        refreshButton = new QPushButton(middleFrame);
        refreshButton->setObjectName(QString::fromUtf8("refreshButton"));
        refreshButton->setGeometry(QRect(230, 20, 61, 23));
        resetBml = new QPushButton(middleFrame);
        resetBml->setObjectName(QString::fromUtf8("resetBml"));
        resetBml->setGeometry(QRect(300, 20, 61, 23));
        runButton = new QPushButton(middleFrame);
        runButton->setObjectName(QString::fromUtf8("runButton"));
        runButton->setGeometry(QRect(370, 20, 61, 23));
        characterNameLabel = new QLabel(middleFrame);
        characterNameLabel->setObjectName(QString::fromUtf8("characterNameLabel"));
        characterNameLabel->setGeometry(QRect(10, 20, 61, 16));
        tabWidget = new QTabWidget(frame);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setGeometry(QRect(0, 0, 821, 371));
        bodyTab = new QWidget();
        bodyTab->setObjectName(QString::fromUtf8("bodyTab"));
        postureLabel = new QLabel(bodyTab);
        postureLabel->setObjectName(QString::fromUtf8("postureLabel"));
        postureLabel->setGeometry(QRect(20, 30, 61, 20));
        QFont font;
        font.setPointSize(12);
        postureLabel->setFont(font);
        postureLabel->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        startLabel = new QLabel(bodyTab);
        startLabel->setObjectName(QString::fromUtf8("startLabel"));
        startLabel->setGeometry(QRect(20, 90, 41, 16));
        startLabel->setFont(font);
        startLabel->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        readyLabel = new QLabel(bodyTab);
        readyLabel->setObjectName(QString::fromUtf8("readyLabel"));
        readyLabel->setGeometry(QRect(20, 150, 41, 16));
        readyLabel->setFont(font);
        readyLabel->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        idLabel = new QLabel(bodyTab);
        idLabel->setObjectName(QString::fromUtf8("idLabel"));
        idLabel->setGeometry(QRect(20, 210, 31, 16));
        idLabel->setFont(font);
        idLabel->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        bodyIdBox = new QPlainTextEdit(bodyTab);
        bodyIdBox->setObjectName(QString::fromUtf8("bodyIdBox"));
        bodyIdBox->setGeometry(QRect(20, 230, 131, 31));
        bodyReadyBox = new QPlainTextEdit(bodyTab);
        bodyReadyBox->setObjectName(QString::fromUtf8("bodyReadyBox"));
        bodyReadyBox->setGeometry(QRect(20, 170, 131, 31));
        bodyStartBox = new QPlainTextEdit(bodyTab);
        bodyStartBox->setObjectName(QString::fromUtf8("bodyStartBox"));
        bodyStartBox->setGeometry(QRect(20, 110, 131, 31));
        bodyPostureBox = new QPlainTextEdit(bodyTab);
        bodyPostureBox->setObjectName(QString::fromUtf8("bodyPostureBox"));
        bodyPostureBox->setGeometry(QRect(20, 50, 131, 31));
        bodyPostureBox->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        idLabel_2 = new QLabel(bodyTab);
        idLabel_2->setObjectName(QString::fromUtf8("idLabel_2"));
        idLabel_2->setGeometry(QRect(340, 10, 41, 16));
        idLabel_2->setFont(font);
        idLabel_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        line = new QFrame(bodyTab);
        line->setObjectName(QString::fromUtf8("line"));
        line->setGeometry(QRect(310, 30, 118, 3));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        tabWidget->addTab(bodyTab, QString());
        locomotionTab = new QWidget();
        locomotionTab->setObjectName(QString::fromUtf8("locomotionTab"));
        locomotionFacingBox = new QPlainTextEdit(locomotionTab);
        locomotionFacingBox->setObjectName(QString::fromUtf8("locomotionFacingBox"));
        locomotionFacingBox->setGeometry(QRect(20, 240, 131, 31));
        readyLabel_2 = new QLabel(locomotionTab);
        readyLabel_2->setObjectName(QString::fromUtf8("readyLabel_2"));
        readyLabel_2->setGeometry(QRect(20, 160, 71, 16));
        readyLabel_2->setFont(font);
        readyLabel_2->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        locomotionTargetBox = new QPlainTextEdit(locomotionTab);
        locomotionTargetBox->setObjectName(QString::fromUtf8("locomotionTargetBox"));
        locomotionTargetBox->setGeometry(QRect(20, 60, 131, 31));
        idLabel_3 = new QLabel(locomotionTab);
        idLabel_3->setObjectName(QString::fromUtf8("idLabel_3"));
        idLabel_3->setGeometry(QRect(20, 220, 51, 21));
        idLabel_3->setFont(font);
        idLabel_3->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        locomotionFollowBox = new QPlainTextEdit(locomotionTab);
        locomotionFollowBox->setObjectName(QString::fromUtf8("locomotionFollowBox"));
        locomotionFollowBox->setGeometry(QRect(170, 120, 131, 31));
        startLabel_2 = new QLabel(locomotionTab);
        startLabel_2->setObjectName(QString::fromUtf8("startLabel_2"));
        startLabel_2->setGeometry(QRect(20, 100, 41, 16));
        startLabel_2->setFont(font);
        startLabel_2->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        postureLabel_2 = new QLabel(locomotionTab);
        postureLabel_2->setObjectName(QString::fromUtf8("postureLabel_2"));
        postureLabel_2->setGeometry(QRect(20, 40, 61, 20));
        postureLabel_2->setFont(font);
        postureLabel_2->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        locomotionAccelBox = new QPlainTextEdit(locomotionTab);
        locomotionAccelBox->setObjectName(QString::fromUtf8("locomotionAccelBox"));
        locomotionAccelBox->setGeometry(QRect(170, 180, 131, 31));
        line_2 = new QFrame(locomotionTab);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setGeometry(QRect(300, 30, 118, 3));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);
        idLabel_4 = new QLabel(locomotionTab);
        idLabel_4->setObjectName(QString::fromUtf8("idLabel_4"));
        idLabel_4->setGeometry(QRect(310, 10, 91, 16));
        idLabel_4->setFont(font);
        idLabel_4->setAlignment(Qt::AlignCenter);
        idLabel_5 = new QLabel(locomotionTab);
        idLabel_5->setObjectName(QString::fromUtf8("idLabel_5"));
        idLabel_5->setGeometry(QRect(20, 280, 71, 21));
        idLabel_5->setFont(font);
        idLabel_5->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        idLabel_6 = new QLabel(locomotionTab);
        idLabel_6->setObjectName(QString::fromUtf8("idLabel_6"));
        idLabel_6->setGeometry(QRect(170, 100, 81, 21));
        idLabel_6->setFont(font);
        idLabel_6->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        idLabel_7 = new QLabel(locomotionTab);
        idLabel_7->setObjectName(QString::fromUtf8("idLabel_7"));
        idLabel_7->setGeometry(QRect(170, 160, 81, 21));
        idLabel_7->setFont(font);
        idLabel_7->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        idLabel_8 = new QLabel(locomotionTab);
        idLabel_8->setObjectName(QString::fromUtf8("idLabel_8"));
        idLabel_8->setGeometry(QRect(170, 220, 121, 21));
        idLabel_8->setFont(font);
        idLabel_8->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        idLabel_9 = new QLabel(locomotionTab);
        idLabel_9->setObjectName(QString::fromUtf8("idLabel_9"));
        idLabel_9->setGeometry(QRect(170, 280, 121, 21));
        idLabel_9->setFont(font);
        idLabel_9->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        idLabel_10 = new QLabel(locomotionTab);
        idLabel_10->setObjectName(QString::fromUtf8("idLabel_10"));
        idLabel_10->setGeometry(QRect(320, 40, 121, 21));
        idLabel_10->setFont(font);
        idLabel_10->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        idLabel_11 = new QLabel(locomotionTab);
        idLabel_11->setObjectName(QString::fromUtf8("idLabel_11"));
        idLabel_11->setGeometry(QRect(320, 100, 121, 21));
        idLabel_11->setFont(font);
        idLabel_11->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        idLabel_12 = new QLabel(locomotionTab);
        idLabel_12->setObjectName(QString::fromUtf8("idLabel_12"));
        idLabel_12->setGeometry(QRect(320, 160, 121, 21));
        idLabel_12->setFont(font);
        idLabel_12->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        locomotionTypeBox = new QComboBox(locomotionTab);
        locomotionTypeBox->setObjectName(QString::fromUtf8("locomotionTypeBox"));
        locomotionTypeBox->setGeometry(QRect(20, 120, 131, 31));
        locomotionMannerBox = new QComboBox(locomotionTab);
        locomotionMannerBox->setObjectName(QString::fromUtf8("locomotionMannerBox"));
        locomotionMannerBox->setGeometry(QRect(20, 180, 131, 31));
        idLabel_13 = new QLabel(locomotionTab);
        idLabel_13->setObjectName(QString::fromUtf8("idLabel_13"));
        idLabel_13->setGeometry(QRect(170, 40, 101, 21));
        idLabel_13->setFont(font);
        idLabel_13->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        locomotionProximityBox = new QPlainTextEdit(locomotionTab);
        locomotionProximityBox->setObjectName(QString::fromUtf8("locomotionProximityBox"));
        locomotionProximityBox->setGeometry(QRect(20, 300, 131, 31));
        locomotionScootAccelBox = new QPlainTextEdit(locomotionTab);
        locomotionScootAccelBox->setObjectName(QString::fromUtf8("locomotionScootAccelBox"));
        locomotionScootAccelBox->setGeometry(QRect(170, 240, 131, 31));
        locomotionAngleAccelBox = new QPlainTextEdit(locomotionTab);
        locomotionAngleAccelBox->setObjectName(QString::fromUtf8("locomotionAngleAccelBox"));
        locomotionAngleAccelBox->setGeometry(QRect(170, 300, 131, 31));
        locomotionNumStepsBox = new QPlainTextEdit(locomotionTab);
        locomotionNumStepsBox->setObjectName(QString::fromUtf8("locomotionNumStepsBox"));
        locomotionNumStepsBox->setGeometry(QRect(320, 60, 131, 31));
        locomotionStartBox = new QPlainTextEdit(locomotionTab);
        locomotionStartBox->setObjectName(QString::fromUtf8("locomotionStartBox"));
        locomotionStartBox->setGeometry(QRect(320, 120, 131, 31));
        locomotionIdBox = new QPlainTextEdit(locomotionTab);
        locomotionIdBox->setObjectName(QString::fromUtf8("locomotionIdBox"));
        locomotionIdBox->setGeometry(QRect(320, 180, 131, 31));
        locomotionSbmBreakingSlider = new QSlider(locomotionTab);
        locomotionSbmBreakingSlider->setObjectName(QString::fromUtf8("locomotionSbmBreakingSlider"));
        locomotionSbmBreakingSlider->setGeometry(QRect(170, 70, 71, 19));
        locomotionSbmBreakingSlider->setMinimum(0);
        locomotionSbmBreakingSlider->setValue(0);
        locomotionSbmBreakingSlider->setOrientation(Qt::Horizontal);
        locomotionSbmBreakingBox = new QDoubleSpinBox(locomotionTab);
        locomotionSbmBreakingBox->setObjectName(QString::fromUtf8("locomotionSbmBreakingBox"));
        locomotionSbmBreakingBox->setGeometry(QRect(250, 70, 51, 22));
        locomotionSbmBreakingBox->setMinimum(-99);
        tabWidget->addTab(locomotionTab, QString());
        animationTab = new QWidget();
        animationTab->setObjectName(QString::fromUtf8("animationTab"));
        idLabel_14 = new QLabel(animationTab);
        idLabel_14->setObjectName(QString::fromUtf8("idLabel_14"));
        idLabel_14->setGeometry(QRect(310, 10, 91, 16));
        idLabel_14->setFont(font);
        idLabel_14->setAlignment(Qt::AlignCenter);
        line_3 = new QFrame(animationTab);
        line_3->setObjectName(QString::fromUtf8("line_3"));
        line_3->setGeometry(QRect(300, 30, 118, 3));
        line_3->setFrameShape(QFrame::HLine);
        line_3->setFrameShadow(QFrame::Sunken);
        postureLabel_3 = new QLabel(animationTab);
        postureLabel_3->setObjectName(QString::fromUtf8("postureLabel_3"));
        postureLabel_3->setGeometry(QRect(20, 40, 61, 20));
        postureLabel_3->setFont(font);
        postureLabel_3->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        animationNameBox = new QPlainTextEdit(animationTab);
        animationNameBox->setObjectName(QString::fromUtf8("animationNameBox"));
        animationNameBox->setGeometry(QRect(20, 60, 131, 31));
        postureLabel_4 = new QLabel(animationTab);
        postureLabel_4->setObjectName(QString::fromUtf8("postureLabel_4"));
        postureLabel_4->setGeometry(QRect(20, 100, 61, 20));
        postureLabel_4->setFont(font);
        postureLabel_4->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        animationStartBox = new QPlainTextEdit(animationTab);
        animationStartBox->setObjectName(QString::fromUtf8("animationStartBox"));
        animationStartBox->setGeometry(QRect(20, 180, 131, 31));
        postureLabel_5 = new QLabel(animationTab);
        postureLabel_5->setObjectName(QString::fromUtf8("postureLabel_5"));
        postureLabel_5->setGeometry(QRect(20, 160, 61, 20));
        postureLabel_5->setFont(font);
        postureLabel_5->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        animationReadyBox = new QPlainTextEdit(animationTab);
        animationReadyBox->setObjectName(QString::fromUtf8("animationReadyBox"));
        animationReadyBox->setGeometry(QRect(20, 240, 131, 31));
        postureLabel_6 = new QLabel(animationTab);
        postureLabel_6->setObjectName(QString::fromUtf8("postureLabel_6"));
        postureLabel_6->setGeometry(QRect(20, 220, 61, 20));
        postureLabel_6->setFont(font);
        postureLabel_6->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        postureLabel_7 = new QLabel(animationTab);
        postureLabel_7->setObjectName(QString::fromUtf8("postureLabel_7"));
        postureLabel_7->setGeometry(QRect(170, 40, 61, 20));
        postureLabel_7->setFont(font);
        postureLabel_7->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        animationStrokeBox = new QPlainTextEdit(animationTab);
        animationStrokeBox->setObjectName(QString::fromUtf8("animationStrokeBox"));
        animationStrokeBox->setGeometry(QRect(170, 60, 131, 31));
        postureLabel_8 = new QLabel(animationTab);
        postureLabel_8->setObjectName(QString::fromUtf8("postureLabel_8"));
        postureLabel_8->setGeometry(QRect(170, 100, 61, 20));
        postureLabel_8->setFont(font);
        postureLabel_8->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        animationRelaxBox = new QPlainTextEdit(animationTab);
        animationRelaxBox->setObjectName(QString::fromUtf8("animationRelaxBox"));
        animationRelaxBox->setGeometry(QRect(170, 120, 131, 31));
        postureLabel_9 = new QLabel(animationTab);
        postureLabel_9->setObjectName(QString::fromUtf8("postureLabel_9"));
        postureLabel_9->setGeometry(QRect(170, 160, 61, 20));
        postureLabel_9->setFont(font);
        postureLabel_9->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        animationsEndBox = new QPlainTextEdit(animationTab);
        animationsEndBox->setObjectName(QString::fromUtf8("animationsEndBox"));
        animationsEndBox->setGeometry(QRect(170, 180, 131, 31));
        postureLabel_10 = new QLabel(animationTab);
        postureLabel_10->setObjectName(QString::fromUtf8("postureLabel_10"));
        postureLabel_10->setGeometry(QRect(170, 220, 61, 20));
        postureLabel_10->setFont(font);
        postureLabel_10->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        animationIdBox = new QPlainTextEdit(animationTab);
        animationIdBox->setObjectName(QString::fromUtf8("animationIdBox"));
        animationIdBox->setGeometry(QRect(170, 240, 131, 31));
        animationSpeedSlider = new QSlider(animationTab);
        animationSpeedSlider->setObjectName(QString::fromUtf8("animationSpeedSlider"));
        animationSpeedSlider->setGeometry(QRect(20, 130, 71, 19));
        animationSpeedSlider->setOrientation(Qt::Horizontal);
        animationSpeedBox = new QDoubleSpinBox(animationTab);
        animationSpeedBox->setObjectName(QString::fromUtf8("animationSpeedBox"));
        animationSpeedBox->setGeometry(QRect(100, 130, 51, 22));
        animationSpeedBox->setMinimum(-99);
        tabWidget->addTab(animationTab, QString());
        gestureTab = new QWidget();
        gestureTab->setObjectName(QString::fromUtf8("gestureTab"));
        postureLabel_11 = new QLabel(gestureTab);
        postureLabel_11->setObjectName(QString::fromUtf8("postureLabel_11"));
        postureLabel_11->setGeometry(QRect(20, 40, 61, 20));
        postureLabel_11->setFont(font);
        postureLabel_11->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        gestureLexemeBox = new QPlainTextEdit(gestureTab);
        gestureLexemeBox->setObjectName(QString::fromUtf8("gestureLexemeBox"));
        gestureLexemeBox->setGeometry(QRect(20, 60, 131, 31));
        line_4 = new QFrame(gestureTab);
        line_4->setObjectName(QString::fromUtf8("line_4"));
        line_4->setGeometry(QRect(310, 30, 118, 3));
        line_4->setFrameShape(QFrame::HLine);
        line_4->setFrameShadow(QFrame::Sunken);
        idLabel_15 = new QLabel(gestureTab);
        idLabel_15->setObjectName(QString::fromUtf8("idLabel_15"));
        idLabel_15->setGeometry(QRect(320, 10, 91, 21));
        idLabel_15->setFont(font);
        idLabel_15->setAlignment(Qt::AlignCenter);
        postureLabel_12 = new QLabel(gestureTab);
        postureLabel_12->setObjectName(QString::fromUtf8("postureLabel_12"));
        postureLabel_12->setGeometry(QRect(20, 100, 61, 20));
        postureLabel_12->setFont(font);
        postureLabel_12->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        gestureNameBox = new QPlainTextEdit(gestureTab);
        gestureNameBox->setObjectName(QString::fromUtf8("gestureNameBox"));
        gestureNameBox->setGeometry(QRect(20, 120, 131, 31));
        gestureModeBox = new QComboBox(gestureTab);
        gestureModeBox->setObjectName(QString::fromUtf8("gestureModeBox"));
        gestureModeBox->setGeometry(QRect(20, 180, 131, 31));
        postureLabel_13 = new QLabel(gestureTab);
        postureLabel_13->setObjectName(QString::fromUtf8("postureLabel_13"));
        postureLabel_13->setGeometry(QRect(20, 160, 91, 20));
        postureLabel_13->setFont(font);
        postureLabel_13->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        gestureReadyBox = new QPlainTextEdit(gestureTab);
        gestureReadyBox->setObjectName(QString::fromUtf8("gestureReadyBox"));
        gestureReadyBox->setGeometry(QRect(170, 60, 131, 31));
        postureLabel_14 = new QLabel(gestureTab);
        postureLabel_14->setObjectName(QString::fromUtf8("postureLabel_14"));
        postureLabel_14->setGeometry(QRect(20, 220, 61, 20));
        postureLabel_14->setFont(font);
        postureLabel_14->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        gestureTargetBox = new QPlainTextEdit(gestureTab);
        gestureTargetBox->setObjectName(QString::fromUtf8("gestureTargetBox"));
        gestureTargetBox->setGeometry(QRect(20, 240, 131, 31));
        postureLabel_15 = new QLabel(gestureTab);
        postureLabel_15->setObjectName(QString::fromUtf8("postureLabel_15"));
        postureLabel_15->setGeometry(QRect(20, 280, 61, 20));
        postureLabel_15->setFont(font);
        postureLabel_15->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        gestureStartBox = new QPlainTextEdit(gestureTab);
        gestureStartBox->setObjectName(QString::fromUtf8("gestureStartBox"));
        gestureStartBox->setGeometry(QRect(20, 300, 131, 31));
        postureLabel_16 = new QLabel(gestureTab);
        postureLabel_16->setObjectName(QString::fromUtf8("postureLabel_16"));
        postureLabel_16->setGeometry(QRect(170, 40, 61, 20));
        postureLabel_16->setFont(font);
        postureLabel_16->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        gestureStrokeBox = new QPlainTextEdit(gestureTab);
        gestureStrokeBox->setObjectName(QString::fromUtf8("gestureStrokeBox"));
        gestureStrokeBox->setGeometry(QRect(170, 120, 131, 31));
        postureLabel_17 = new QLabel(gestureTab);
        postureLabel_17->setObjectName(QString::fromUtf8("postureLabel_17"));
        postureLabel_17->setGeometry(QRect(170, 100, 61, 20));
        postureLabel_17->setFont(font);
        postureLabel_17->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        gestureRelaxBox = new QPlainTextEdit(gestureTab);
        gestureRelaxBox->setObjectName(QString::fromUtf8("gestureRelaxBox"));
        gestureRelaxBox->setGeometry(QRect(170, 180, 131, 31));
        postureLabel_18 = new QLabel(gestureTab);
        postureLabel_18->setObjectName(QString::fromUtf8("postureLabel_18"));
        postureLabel_18->setGeometry(QRect(170, 160, 61, 20));
        postureLabel_18->setFont(font);
        postureLabel_18->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        gestureEndBox = new QPlainTextEdit(gestureTab);
        gestureEndBox->setObjectName(QString::fromUtf8("gestureEndBox"));
        gestureEndBox->setGeometry(QRect(170, 240, 131, 31));
        postureLabel_19 = new QLabel(gestureTab);
        postureLabel_19->setObjectName(QString::fromUtf8("postureLabel_19"));
        postureLabel_19->setGeometry(QRect(170, 220, 61, 20));
        postureLabel_19->setFont(font);
        postureLabel_19->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        gestureIdBox = new QPlainTextEdit(gestureTab);
        gestureIdBox->setObjectName(QString::fromUtf8("gestureIdBox"));
        gestureIdBox->setGeometry(QRect(170, 300, 131, 31));
        postureLabel_20 = new QLabel(gestureTab);
        postureLabel_20->setObjectName(QString::fromUtf8("postureLabel_20"));
        postureLabel_20->setGeometry(QRect(170, 280, 61, 20));
        postureLabel_20->setFont(font);
        postureLabel_20->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        tabWidget->addTab(gestureTab, QString());
        sbmreachTab = new QWidget();
        sbmreachTab->setObjectName(QString::fromUtf8("sbmreachTab"));
        idLabel_17 = new QLabel(sbmreachTab);
        idLabel_17->setObjectName(QString::fromUtf8("idLabel_17"));
        idLabel_17->setGeometry(QRect(320, 10, 91, 21));
        idLabel_17->setFont(font);
        idLabel_17->setAlignment(Qt::AlignCenter);
        sbmReachSbmHandle = new QPlainTextEdit(sbmreachTab);
        sbmReachSbmHandle->setObjectName(QString::fromUtf8("sbmReachSbmHandle"));
        sbmReachSbmHandle->setGeometry(QRect(20, 180, 131, 31));
        postureLabel_24 = new QLabel(sbmreachTab);
        postureLabel_24->setObjectName(QString::fromUtf8("postureLabel_24"));
        postureLabel_24->setGeometry(QRect(20, 40, 61, 20));
        postureLabel_24->setFont(font);
        postureLabel_24->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        sbmReachTarget = new QPlainTextEdit(sbmreachTab);
        sbmReachTarget->setObjectName(QString::fromUtf8("sbmReachTarget"));
        sbmReachTarget->setGeometry(QRect(20, 60, 131, 31));
        postureLabel_25 = new QLabel(sbmreachTab);
        postureLabel_25->setObjectName(QString::fromUtf8("postureLabel_25"));
        postureLabel_25->setGeometry(QRect(20, 160, 91, 20));
        postureLabel_25->setFont(font);
        postureLabel_25->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        line_6 = new QFrame(sbmreachTab);
        line_6->setObjectName(QString::fromUtf8("line_6"));
        line_6->setGeometry(QRect(310, 30, 118, 3));
        line_6->setFrameShape(QFrame::HLine);
        line_6->setFrameShadow(QFrame::Sunken);
        postureLabel_26 = new QLabel(sbmreachTab);
        postureLabel_26->setObjectName(QString::fromUtf8("postureLabel_26"));
        postureLabel_26->setGeometry(QRect(20, 100, 91, 20));
        postureLabel_26->setFont(font);
        postureLabel_26->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        sbmReachSbmAction = new QComboBox(sbmreachTab);
        sbmReachSbmAction->setObjectName(QString::fromUtf8("sbmReachSbmAction"));
        sbmReachSbmAction->setGeometry(QRect(20, 120, 131, 31));
        sbmReachSbmFootIk = new QCheckBox(sbmreachTab);
        sbmReachSbmFootIk->setObjectName(QString::fromUtf8("sbmReachSbmFootIk"));
        sbmReachSbmFootIk->setGeometry(QRect(20, 220, 81, 17));
        sbmReachSbmFootIk->setLayoutDirection(Qt::LeftToRight);
        sbmReachSbmReachFinish = new QCheckBox(sbmreachTab);
        sbmReachSbmReachFinish->setObjectName(QString::fromUtf8("sbmReachSbmReachFinish"));
        sbmReachSbmReachFinish->setGeometry(QRect(20, 250, 111, 17));
        sbmReachSbmReachFinish->setLayoutDirection(Qt::LeftToRight);
        sbmReachsbmVelocity = new QSlider(sbmreachTab);
        sbmReachsbmVelocity->setObjectName(QString::fromUtf8("sbmReachsbmVelocity"));
        sbmReachsbmVelocity->setGeometry(QRect(170, 70, 71, 19));
        sbmReachsbmVelocity->setOrientation(Qt::Horizontal);
        postureLabel_27 = new QLabel(sbmreachTab);
        postureLabel_27->setObjectName(QString::fromUtf8("postureLabel_27"));
        postureLabel_27->setGeometry(QRect(170, 40, 141, 20));
        postureLabel_27->setFont(font);
        postureLabel_27->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        postureLabel_28 = new QLabel(sbmreachTab);
        postureLabel_28->setObjectName(QString::fromUtf8("postureLabel_28"));
        postureLabel_28->setGeometry(QRect(170, 100, 141, 20));
        postureLabel_28->setFont(font);
        postureLabel_28->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        sbmReachSbmDuration = new QSlider(sbmreachTab);
        sbmReachSbmDuration->setObjectName(QString::fromUtf8("sbmReachSbmDuration"));
        sbmReachSbmDuration->setGeometry(QRect(170, 130, 71, 19));
        sbmReachSbmDuration->setOrientation(Qt::Horizontal);
        sbmReachStart = new QPlainTextEdit(sbmreachTab);
        sbmReachStart->setObjectName(QString::fromUtf8("sbmReachStart"));
        sbmReachStart->setGeometry(QRect(170, 180, 131, 31));
        postureLabel_29 = new QLabel(sbmreachTab);
        postureLabel_29->setObjectName(QString::fromUtf8("postureLabel_29"));
        postureLabel_29->setGeometry(QRect(170, 160, 61, 20));
        postureLabel_29->setFont(font);
        postureLabel_29->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        postureLabel_30 = new QLabel(sbmreachTab);
        postureLabel_30->setObjectName(QString::fromUtf8("postureLabel_30"));
        postureLabel_30->setGeometry(QRect(170, 220, 61, 20));
        postureLabel_30->setFont(font);
        postureLabel_30->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        sbmReachId = new QPlainTextEdit(sbmreachTab);
        sbmReachId->setObjectName(QString::fromUtf8("sbmReachId"));
        sbmReachId->setGeometry(QRect(170, 240, 131, 31));
        doubleSpinBox = new QDoubleSpinBox(sbmreachTab);
        doubleSpinBox->setObjectName(QString::fromUtf8("doubleSpinBox"));
        doubleSpinBox->setGeometry(QRect(250, 70, 51, 22));
        doubleSpinBox->setMinimum(-99);
        doubleSpinBox_2 = new QDoubleSpinBox(sbmreachTab);
        doubleSpinBox_2->setObjectName(QString::fromUtf8("doubleSpinBox_2"));
        doubleSpinBox_2->setGeometry(QRect(250, 130, 51, 22));
        doubleSpinBox_2->setMinimum(-99);
        tabWidget->addTab(sbmreachTab, QString());
        gazeTab = new QWidget();
        gazeTab->setObjectName(QString::fromUtf8("gazeTab"));
        gazeTarget = new QPlainTextEdit(gazeTab);
        gazeTarget->setObjectName(QString::fromUtf8("gazeTarget"));
        gazeTarget->setGeometry(QRect(20, 60, 131, 31));
        line_7 = new QFrame(gazeTab);
        line_7->setObjectName(QString::fromUtf8("line_7"));
        line_7->setGeometry(QRect(310, 30, 118, 3));
        line_7->setFrameShape(QFrame::HLine);
        line_7->setFrameShadow(QFrame::Sunken);
        postureLabel_31 = new QLabel(gazeTab);
        postureLabel_31->setObjectName(QString::fromUtf8("postureLabel_31"));
        postureLabel_31->setGeometry(QRect(20, 40, 61, 20));
        postureLabel_31->setFont(font);
        postureLabel_31->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        idLabel_18 = new QLabel(gazeTab);
        idLabel_18->setObjectName(QString::fromUtf8("idLabel_18"));
        idLabel_18->setGeometry(QRect(320, 10, 91, 21));
        idLabel_18->setFont(font);
        idLabel_18->setAlignment(Qt::AlignCenter);
        postureLabel_32 = new QLabel(gazeTab);
        postureLabel_32->setObjectName(QString::fromUtf8("postureLabel_32"));
        postureLabel_32->setGeometry(QRect(320, 40, 121, 20));
        postureLabel_32->setFont(font);
        postureLabel_32->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        gazeSbmJointRange = new QComboBox(gazeTab);
        gazeSbmJointRange->setObjectName(QString::fromUtf8("gazeSbmJointRange"));
        gazeSbmJointRange->setGeometry(QRect(320, 60, 131, 31));
        postureLabel_33 = new QLabel(gazeTab);
        postureLabel_33->setObjectName(QString::fromUtf8("postureLabel_33"));
        postureLabel_33->setGeometry(QRect(320, 100, 91, 20));
        postureLabel_33->setFont(font);
        postureLabel_33->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        gazeDirection = new QComboBox(gazeTab);
        gazeDirection->setObjectName(QString::fromUtf8("gazeDirection"));
        gazeDirection->setGeometry(QRect(320, 120, 131, 31));
        postureLabel_67 = new QLabel(gazeTab);
        postureLabel_67->setObjectName(QString::fromUtf8("postureLabel_67"));
        postureLabel_67->setGeometry(QRect(170, 160, 141, 20));
        postureLabel_67->setFont(font);
        postureLabel_67->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        gazeAngleSlider = new QSlider(gazeTab);
        gazeAngleSlider->setObjectName(QString::fromUtf8("gazeAngleSlider"));
        gazeAngleSlider->setGeometry(QRect(170, 190, 71, 19));
        gazeAngleSlider->setOrientation(Qt::Horizontal);
        postureLabel_68 = new QLabel(gazeTab);
        postureLabel_68->setObjectName(QString::fromUtf8("postureLabel_68"));
        postureLabel_68->setGeometry(QRect(320, 160, 131, 20));
        postureLabel_68->setFont(font);
        postureLabel_68->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        gazeSbmPriorityJoint = new QComboBox(gazeTab);
        gazeSbmPriorityJoint->setObjectName(QString::fromUtf8("gazeSbmPriorityJoint"));
        gazeSbmPriorityJoint->setGeometry(QRect(320, 180, 131, 31));
        gazeSbmHandle = new QPlainTextEdit(gazeTab);
        gazeSbmHandle->setObjectName(QString::fromUtf8("gazeSbmHandle"));
        gazeSbmHandle->setGeometry(QRect(170, 60, 131, 31));
        postureLabel_69 = new QLabel(gazeTab);
        postureLabel_69->setObjectName(QString::fromUtf8("postureLabel_69"));
        postureLabel_69->setGeometry(QRect(170, 40, 91, 20));
        postureLabel_69->setFont(font);
        postureLabel_69->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        gazeAngleBox = new QDoubleSpinBox(gazeTab);
        gazeAngleBox->setObjectName(QString::fromUtf8("gazeAngleBox"));
        gazeAngleBox->setGeometry(QRect(250, 190, 51, 22));
        gazeAngleBox->setMinimum(-99);
        gazeSbmTimeHintBox = new QDoubleSpinBox(gazeTab);
        gazeSbmTimeHintBox->setObjectName(QString::fromUtf8("gazeSbmTimeHintBox"));
        gazeSbmTimeHintBox->setGeometry(QRect(250, 130, 51, 22));
        gazeSbmTimeHintBox->setMinimum(-99);
        postureLabel_106 = new QLabel(gazeTab);
        postureLabel_106->setObjectName(QString::fromUtf8("postureLabel_106"));
        postureLabel_106->setGeometry(QRect(170, 100, 141, 20));
        postureLabel_106->setFont(font);
        postureLabel_106->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        gazeSbmTimeHintSlider = new QSlider(gazeTab);
        gazeSbmTimeHintSlider->setObjectName(QString::fromUtf8("gazeSbmTimeHintSlider"));
        gazeSbmTimeHintSlider->setGeometry(QRect(170, 130, 71, 19));
        gazeSbmTimeHintSlider->setOrientation(Qt::Horizontal);
        gazeSbmJointSpeed = new QPlainTextEdit(gazeTab);
        gazeSbmJointSpeed->setObjectName(QString::fromUtf8("gazeSbmJointSpeed"));
        gazeSbmJointSpeed->setGeometry(QRect(20, 120, 131, 31));
        postureLabel_107 = new QLabel(gazeTab);
        postureLabel_107->setObjectName(QString::fromUtf8("postureLabel_107"));
        postureLabel_107->setGeometry(QRect(20, 100, 121, 20));
        postureLabel_107->setFont(font);
        postureLabel_107->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        gazeSbmJointSmooth = new QPlainTextEdit(gazeTab);
        gazeSbmJointSmooth->setObjectName(QString::fromUtf8("gazeSbmJointSmooth"));
        gazeSbmJointSmooth->setGeometry(QRect(20, 180, 131, 31));
        postureLabel_108 = new QLabel(gazeTab);
        postureLabel_108->setObjectName(QString::fromUtf8("postureLabel_108"));
        postureLabel_108->setGeometry(QRect(20, 160, 131, 20));
        postureLabel_108->setFont(font);
        postureLabel_108->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        postureLabel_148 = new QLabel(gazeTab);
        postureLabel_148->setObjectName(QString::fromUtf8("postureLabel_148"));
        postureLabel_148->setGeometry(QRect(170, 220, 101, 20));
        postureLabel_148->setFont(font);
        postureLabel_148->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        gazeSbmFadeInSlider = new QSlider(gazeTab);
        gazeSbmFadeInSlider->setObjectName(QString::fromUtf8("gazeSbmFadeInSlider"));
        gazeSbmFadeInSlider->setGeometry(QRect(170, 250, 71, 19));
        gazeSbmFadeInSlider->setOrientation(Qt::Horizontal);
        gazeSbmFadeInBox = new QDoubleSpinBox(gazeTab);
        gazeSbmFadeInBox->setObjectName(QString::fromUtf8("gazeSbmFadeInBox"));
        gazeSbmFadeInBox->setGeometry(QRect(250, 250, 51, 22));
        gazeSbmFadeInBox->setMinimum(-99);
        postureLabel_149 = new QLabel(gazeTab);
        postureLabel_149->setObjectName(QString::fromUtf8("postureLabel_149"));
        postureLabel_149->setGeometry(QRect(20, 280, 61, 20));
        postureLabel_149->setFont(font);
        postureLabel_149->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        postureLabel_150 = new QLabel(gazeTab);
        postureLabel_150->setObjectName(QString::fromUtf8("postureLabel_150"));
        postureLabel_150->setGeometry(QRect(20, 220, 61, 20));
        postureLabel_150->setFont(font);
        postureLabel_150->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        gazeId = new QPlainTextEdit(gazeTab);
        gazeId->setObjectName(QString::fromUtf8("gazeId"));
        gazeId->setGeometry(QRect(20, 300, 131, 31));
        gazeStart = new QPlainTextEdit(gazeTab);
        gazeStart->setObjectName(QString::fromUtf8("gazeStart"));
        gazeStart->setGeometry(QRect(20, 240, 131, 31));
        postureLabel_151 = new QLabel(gazeTab);
        postureLabel_151->setObjectName(QString::fromUtf8("postureLabel_151"));
        postureLabel_151->setGeometry(QRect(170, 280, 101, 20));
        postureLabel_151->setFont(font);
        postureLabel_151->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        gazeSbmFadeOutBox = new QDoubleSpinBox(gazeTab);
        gazeSbmFadeOutBox->setObjectName(QString::fromUtf8("gazeSbmFadeOutBox"));
        gazeSbmFadeOutBox->setGeometry(QRect(250, 310, 51, 22));
        gazeSbmFadeOutBox->setMinimum(-99);
        gazeSbmFadeOutSlider = new QSlider(gazeTab);
        gazeSbmFadeOutSlider->setObjectName(QString::fromUtf8("gazeSbmFadeOutSlider"));
        gazeSbmFadeOutSlider->setGeometry(QRect(170, 310, 71, 19));
        gazeSbmFadeOutSlider->setOrientation(Qt::Horizontal);
        tabWidget->addTab(gazeTab, QString());
        sbmconstraintTab = new QWidget();
        sbmconstraintTab->setObjectName(QString::fromUtf8("sbmconstraintTab"));
        sbmConstraintTarget = new QPlainTextEdit(sbmconstraintTab);
        sbmConstraintTarget->setObjectName(QString::fromUtf8("sbmConstraintTarget"));
        sbmConstraintTarget->setGeometry(QRect(20, 50, 131, 31));
        sbmConstraintTarget->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        postureLabel_208 = new QLabel(sbmconstraintTab);
        postureLabel_208->setObjectName(QString::fromUtf8("postureLabel_208"));
        postureLabel_208->setGeometry(QRect(20, 90, 121, 20));
        postureLabel_208->setFont(font);
        postureLabel_208->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        postureLabel_209 = new QLabel(sbmconstraintTab);
        postureLabel_209->setObjectName(QString::fromUtf8("postureLabel_209"));
        postureLabel_209->setGeometry(QRect(20, 30, 61, 20));
        postureLabel_209->setFont(font);
        postureLabel_209->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        sbmConstraintSbmHandle = new QPlainTextEdit(sbmconstraintTab);
        sbmConstraintSbmHandle->setObjectName(QString::fromUtf8("sbmConstraintSbmHandle"));
        sbmConstraintSbmHandle->setGeometry(QRect(20, 110, 131, 31));
        idLabel_78 = new QLabel(sbmconstraintTab);
        idLabel_78->setObjectName(QString::fromUtf8("idLabel_78"));
        idLabel_78->setGeometry(QRect(310, 0, 121, 21));
        idLabel_78->setFont(font);
        idLabel_78->setAlignment(Qt::AlignCenter);
        line_34 = new QFrame(sbmconstraintTab);
        line_34->setObjectName(QString::fromUtf8("line_34"));
        line_34->setGeometry(QRect(310, 20, 118, 3));
        line_34->setFrameShape(QFrame::HLine);
        line_34->setFrameShadow(QFrame::Sunken);
        sbmConstraintSbmRoot = new QPlainTextEdit(sbmconstraintTab);
        sbmConstraintSbmRoot->setObjectName(QString::fromUtf8("sbmConstraintSbmRoot"));
        sbmConstraintSbmRoot->setGeometry(QRect(20, 170, 131, 31));
        sbmConstraintEffector = new QPlainTextEdit(sbmconstraintTab);
        sbmConstraintEffector->setObjectName(QString::fromUtf8("sbmConstraintEffector"));
        sbmConstraintEffector->setGeometry(QRect(20, 230, 131, 31));
        postureLabel_210 = new QLabel(sbmconstraintTab);
        postureLabel_210->setObjectName(QString::fromUtf8("postureLabel_210"));
        postureLabel_210->setGeometry(QRect(20, 150, 71, 20));
        postureLabel_210->setFont(font);
        postureLabel_210->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        postureLabel_211 = new QLabel(sbmconstraintTab);
        postureLabel_211->setObjectName(QString::fromUtf8("postureLabel_211"));
        postureLabel_211->setGeometry(QRect(20, 210, 121, 20));
        postureLabel_211->setFont(font);
        postureLabel_211->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        sbmConstraintSbmEffectRoot = new QPlainTextEdit(sbmconstraintTab);
        sbmConstraintSbmEffectRoot->setObjectName(QString::fromUtf8("sbmConstraintSbmEffectRoot"));
        sbmConstraintSbmEffectRoot->setGeometry(QRect(20, 290, 131, 31));
        postureLabel_212 = new QLabel(sbmconstraintTab);
        postureLabel_212->setObjectName(QString::fromUtf8("postureLabel_212"));
        postureLabel_212->setGeometry(QRect(20, 270, 121, 20));
        postureLabel_212->setFont(font);
        postureLabel_212->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        sbmConstraintStart = new QPlainTextEdit(sbmconstraintTab);
        sbmConstraintStart->setObjectName(QString::fromUtf8("sbmConstraintStart"));
        sbmConstraintStart->setGeometry(QRect(170, 50, 131, 31));
        postureLabel_213 = new QLabel(sbmconstraintTab);
        postureLabel_213->setObjectName(QString::fromUtf8("postureLabel_213"));
        postureLabel_213->setGeometry(QRect(170, 30, 121, 20));
        postureLabel_213->setFont(font);
        postureLabel_213->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        postureLabel_214 = new QLabel(sbmconstraintTab);
        postureLabel_214->setObjectName(QString::fromUtf8("postureLabel_214"));
        postureLabel_214->setGeometry(QRect(170, 90, 121, 20));
        postureLabel_214->setFont(font);
        postureLabel_214->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        sbmConstraintReady = new QPlainTextEdit(sbmconstraintTab);
        sbmConstraintReady->setObjectName(QString::fromUtf8("sbmConstraintReady"));
        sbmConstraintReady->setGeometry(QRect(170, 110, 131, 31));
        sbmConstraintId = new QPlainTextEdit(sbmconstraintTab);
        sbmConstraintId->setObjectName(QString::fromUtf8("sbmConstraintId"));
        sbmConstraintId->setGeometry(QRect(170, 170, 131, 31));
        postureLabel_215 = new QLabel(sbmconstraintTab);
        postureLabel_215->setObjectName(QString::fromUtf8("postureLabel_215"));
        postureLabel_215->setGeometry(QRect(170, 150, 121, 20));
        postureLabel_215->setFont(font);
        postureLabel_215->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        postureLabel_216 = new QLabel(sbmconstraintTab);
        postureLabel_216->setObjectName(QString::fromUtf8("postureLabel_216"));
        postureLabel_216->setGeometry(QRect(170, 210, 141, 20));
        postureLabel_216->setFont(font);
        postureLabel_216->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        sbmConstraintSbmConstraintType = new QComboBox(sbmconstraintTab);
        sbmConstraintSbmConstraintType->setObjectName(QString::fromUtf8("sbmConstraintSbmConstraintType"));
        sbmConstraintSbmConstraintType->setGeometry(QRect(170, 230, 131, 31));
        sbmConstraintRotZBox = new QDoubleSpinBox(sbmconstraintTab);
        sbmConstraintRotZBox->setObjectName(QString::fromUtf8("sbmConstraintRotZBox"));
        sbmConstraintRotZBox->setGeometry(QRect(550, 180, 51, 22));
        sbmConstraintRotZBox->setMinimum(-1000);
        sbmConstraintRotZBox->setMaximum(1000);
        sbmConstraintSbmFadeOutSlider = new QSlider(sbmconstraintTab);
        sbmConstraintSbmFadeOutSlider->setObjectName(QString::fromUtf8("sbmConstraintSbmFadeOutSlider"));
        sbmConstraintSbmFadeOutSlider->setGeometry(QRect(320, 120, 71, 19));
        sbmConstraintSbmFadeOutSlider->setOrientation(Qt::Horizontal);
        sbmConstraintPosXSlider = new QSlider(sbmconstraintTab);
        sbmConstraintPosXSlider->setObjectName(QString::fromUtf8("sbmConstraintPosXSlider"));
        sbmConstraintPosXSlider->setGeometry(QRect(320, 180, 71, 19));
        sbmConstraintPosXSlider->setOrientation(Qt::Horizontal);
        postureLabel_217 = new QLabel(sbmconstraintTab);
        postureLabel_217->setObjectName(QString::fromUtf8("postureLabel_217"));
        postureLabel_217->setGeometry(QRect(470, 30, 121, 20));
        postureLabel_217->setFont(font);
        postureLabel_217->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        postureLabel_218 = new QLabel(sbmconstraintTab);
        postureLabel_218->setObjectName(QString::fromUtf8("postureLabel_218"));
        postureLabel_218->setGeometry(QRect(470, 150, 121, 20));
        postureLabel_218->setFont(font);
        postureLabel_218->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        postureLabel_219 = new QLabel(sbmconstraintTab);
        postureLabel_219->setObjectName(QString::fromUtf8("postureLabel_219"));
        postureLabel_219->setGeometry(QRect(320, 90, 121, 20));
        postureLabel_219->setFont(font);
        postureLabel_219->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        sbmConstraintPosYBox = new QDoubleSpinBox(sbmconstraintTab);
        sbmConstraintPosYBox->setObjectName(QString::fromUtf8("sbmConstraintPosYBox"));
        sbmConstraintPosYBox->setGeometry(QRect(400, 240, 51, 22));
        sbmConstraintPosYBox->setMinimum(-1000);
        sbmConstraintPosYBox->setMaximum(1000);
        sbmConstraintRotYSlider = new QSlider(sbmconstraintTab);
        sbmConstraintRotYSlider->setObjectName(QString::fromUtf8("sbmConstraintRotYSlider"));
        sbmConstraintRotYSlider->setGeometry(QRect(470, 120, 71, 19));
        sbmConstraintRotYSlider->setOrientation(Qt::Horizontal);
        sbmConstraintSbmFadeInBox = new QDoubleSpinBox(sbmconstraintTab);
        sbmConstraintSbmFadeInBox->setObjectName(QString::fromUtf8("sbmConstraintSbmFadeInBox"));
        sbmConstraintSbmFadeInBox->setGeometry(QRect(400, 60, 51, 22));
        sbmConstraintSbmFadeInBox->setMinimum(-1000);
        sbmConstraintSbmFadeInBox->setMaximum(1000);
        sbmConstraintSbmFadeInSlider = new QSlider(sbmconstraintTab);
        sbmConstraintSbmFadeInSlider->setObjectName(QString::fromUtf8("sbmConstraintSbmFadeInSlider"));
        sbmConstraintSbmFadeInSlider->setGeometry(QRect(320, 60, 71, 19));
        sbmConstraintSbmFadeInSlider->setOrientation(Qt::Horizontal);
        sbmConstraintRotZSlider = new QSlider(sbmconstraintTab);
        sbmConstraintRotZSlider->setObjectName(QString::fromUtf8("sbmConstraintRotZSlider"));
        sbmConstraintRotZSlider->setGeometry(QRect(470, 180, 71, 19));
        sbmConstraintRotZSlider->setOrientation(Qt::Horizontal);
        sbmConstraintPosYSlider = new QSlider(sbmconstraintTab);
        sbmConstraintPosYSlider->setObjectName(QString::fromUtf8("sbmConstraintPosYSlider"));
        sbmConstraintPosYSlider->setGeometry(QRect(320, 240, 71, 19));
        sbmConstraintPosYSlider->setOrientation(Qt::Horizontal);
        sbmConstraintPosZSlider = new QSlider(sbmconstraintTab);
        sbmConstraintPosZSlider->setObjectName(QString::fromUtf8("sbmConstraintPosZSlider"));
        sbmConstraintPosZSlider->setGeometry(QRect(320, 300, 71, 19));
        sbmConstraintPosZSlider->setOrientation(Qt::Horizontal);
        postureLabel_220 = new QLabel(sbmconstraintTab);
        postureLabel_220->setObjectName(QString::fromUtf8("postureLabel_220"));
        postureLabel_220->setGeometry(QRect(320, 270, 121, 20));
        postureLabel_220->setFont(font);
        postureLabel_220->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        sbmConstraintPosXBox = new QDoubleSpinBox(sbmconstraintTab);
        sbmConstraintPosXBox->setObjectName(QString::fromUtf8("sbmConstraintPosXBox"));
        sbmConstraintPosXBox->setGeometry(QRect(400, 180, 51, 22));
        sbmConstraintPosXBox->setMinimum(-1000);
        sbmConstraintPosXBox->setMaximum(1000);
        sbmConstraintRotXBox = new QDoubleSpinBox(sbmconstraintTab);
        sbmConstraintRotXBox->setObjectName(QString::fromUtf8("sbmConstraintRotXBox"));
        sbmConstraintRotXBox->setGeometry(QRect(550, 60, 51, 22));
        sbmConstraintRotXBox->setMinimum(-1000);
        sbmConstraintRotXBox->setMaximum(1000);
        sbmConstraintPosZBox = new QDoubleSpinBox(sbmconstraintTab);
        sbmConstraintPosZBox->setObjectName(QString::fromUtf8("sbmConstraintPosZBox"));
        sbmConstraintPosZBox->setGeometry(QRect(400, 300, 51, 22));
        sbmConstraintPosZBox->setMinimum(-1000);
        sbmConstraintPosZBox->setMaximum(1000);
        postureLabel_221 = new QLabel(sbmconstraintTab);
        postureLabel_221->setObjectName(QString::fromUtf8("postureLabel_221"));
        postureLabel_221->setGeometry(QRect(320, 30, 121, 20));
        postureLabel_221->setFont(font);
        postureLabel_221->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        postureLabel_222 = new QLabel(sbmconstraintTab);
        postureLabel_222->setObjectName(QString::fromUtf8("postureLabel_222"));
        postureLabel_222->setGeometry(QRect(320, 150, 121, 20));
        postureLabel_222->setFont(font);
        postureLabel_222->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        sbmConstraintRotYBox = new QDoubleSpinBox(sbmconstraintTab);
        sbmConstraintRotYBox->setObjectName(QString::fromUtf8("sbmConstraintRotYBox"));
        sbmConstraintRotYBox->setGeometry(QRect(550, 120, 51, 22));
        sbmConstraintRotYBox->setMinimum(-1000);
        sbmConstraintRotYBox->setMaximum(1000);
        sbmConstraintRotXSlider = new QSlider(sbmconstraintTab);
        sbmConstraintRotXSlider->setObjectName(QString::fromUtf8("sbmConstraintRotXSlider"));
        sbmConstraintRotXSlider->setGeometry(QRect(470, 60, 71, 19));
        sbmConstraintRotXSlider->setOrientation(Qt::Horizontal);
        sbmConstraintSbmFadeOutBox = new QDoubleSpinBox(sbmconstraintTab);
        sbmConstraintSbmFadeOutBox->setObjectName(QString::fromUtf8("sbmConstraintSbmFadeOutBox"));
        sbmConstraintSbmFadeOutBox->setGeometry(QRect(400, 120, 51, 22));
        sbmConstraintSbmFadeOutBox->setMinimum(-1000);
        sbmConstraintSbmFadeOutBox->setMaximum(1000);
        postureLabel_223 = new QLabel(sbmconstraintTab);
        postureLabel_223->setObjectName(QString::fromUtf8("postureLabel_223"));
        postureLabel_223->setGeometry(QRect(320, 210, 121, 20));
        postureLabel_223->setFont(font);
        postureLabel_223->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        postureLabel_224 = new QLabel(sbmconstraintTab);
        postureLabel_224->setObjectName(QString::fromUtf8("postureLabel_224"));
        postureLabel_224->setGeometry(QRect(470, 90, 121, 20));
        postureLabel_224->setFont(font);
        postureLabel_224->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        tabWidget->addTab(sbmconstraintTab, QString());
        headTab = new QWidget();
        headTab->setObjectName(QString::fromUtf8("headTab"));
        idLabel_77 = new QLabel(headTab);
        idLabel_77->setObjectName(QString::fromUtf8("idLabel_77"));
        idLabel_77->setGeometry(QRect(320, 10, 91, 21));
        idLabel_77->setFont(font);
        idLabel_77->setAlignment(Qt::AlignCenter);
        line_33 = new QFrame(headTab);
        line_33->setObjectName(QString::fromUtf8("line_33"));
        line_33->setGeometry(QRect(310, 30, 118, 3));
        line_33->setFrameShape(QFrame::HLine);
        line_33->setFrameShadow(QFrame::Sunken);
        headType = new QComboBox(headTab);
        headType->setObjectName(QString::fromUtf8("headType"));
        headType->setGeometry(QRect(20, 60, 131, 31));
        postureLabel_193 = new QLabel(headTab);
        postureLabel_193->setObjectName(QString::fromUtf8("postureLabel_193"));
        postureLabel_193->setGeometry(QRect(20, 40, 121, 20));
        postureLabel_193->setFont(font);
        postureLabel_193->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        headId = new QPlainTextEdit(headTab);
        headId->setObjectName(QString::fromUtf8("headId"));
        headId->setGeometry(QRect(170, 120, 131, 31));
        postureLabel_194 = new QLabel(headTab);
        postureLabel_194->setObjectName(QString::fromUtf8("postureLabel_194"));
        postureLabel_194->setGeometry(QRect(20, 220, 61, 20));
        postureLabel_194->setFont(font);
        postureLabel_194->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        postureLabel_195 = new QLabel(headTab);
        postureLabel_195->setObjectName(QString::fromUtf8("postureLabel_195"));
        postureLabel_195->setGeometry(QRect(20, 160, 61, 20));
        postureLabel_195->setFont(font);
        postureLabel_195->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        headEnd = new QPlainTextEdit(headTab);
        headEnd->setObjectName(QString::fromUtf8("headEnd"));
        headEnd->setGeometry(QRect(170, 60, 131, 31));
        postureLabel_196 = new QLabel(headTab);
        postureLabel_196->setObjectName(QString::fromUtf8("postureLabel_196"));
        postureLabel_196->setGeometry(QRect(20, 280, 61, 20));
        postureLabel_196->setFont(font);
        postureLabel_196->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        headStroke = new QPlainTextEdit(headTab);
        headStroke->setObjectName(QString::fromUtf8("headStroke"));
        headStroke->setGeometry(QRect(20, 240, 131, 31));
        postureLabel_197 = new QLabel(headTab);
        postureLabel_197->setObjectName(QString::fromUtf8("postureLabel_197"));
        postureLabel_197->setGeometry(QRect(170, 100, 61, 20));
        postureLabel_197->setFont(font);
        postureLabel_197->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        postureLabel_199 = new QLabel(headTab);
        postureLabel_199->setObjectName(QString::fromUtf8("postureLabel_199"));
        postureLabel_199->setGeometry(QRect(170, 40, 61, 20));
        postureLabel_199->setFont(font);
        postureLabel_199->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        headReady = new QPlainTextEdit(headTab);
        headReady->setObjectName(QString::fromUtf8("headReady"));
        headReady->setGeometry(QRect(20, 180, 131, 31));
        headRelax = new QPlainTextEdit(headTab);
        headRelax->setObjectName(QString::fromUtf8("headRelax"));
        headRelax->setGeometry(QRect(20, 300, 131, 31));
        headStart = new QPlainTextEdit(headTab);
        headStart->setObjectName(QString::fromUtf8("headStart"));
        headStart->setGeometry(QRect(20, 120, 131, 31));
        postureLabel_201 = new QLabel(headTab);
        postureLabel_201->setObjectName(QString::fromUtf8("postureLabel_201"));
        postureLabel_201->setGeometry(QRect(20, 100, 61, 20));
        postureLabel_201->setFont(font);
        postureLabel_201->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        headRepeatsBox = new QDoubleSpinBox(headTab);
        headRepeatsBox->setObjectName(QString::fromUtf8("headRepeatsBox"));
        headRepeatsBox->setGeometry(QRect(250, 190, 51, 22));
        headRepeatsBox->setMinimum(-99);
        postureLabel_198 = new QLabel(headTab);
        postureLabel_198->setObjectName(QString::fromUtf8("postureLabel_198"));
        postureLabel_198->setGeometry(QRect(170, 160, 121, 20));
        postureLabel_198->setFont(font);
        postureLabel_198->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        headRepeatsSlider = new QSlider(headTab);
        headRepeatsSlider->setObjectName(QString::fromUtf8("headRepeatsSlider"));
        headRepeatsSlider->setGeometry(QRect(170, 190, 71, 19));
        headRepeatsSlider->setOrientation(Qt::Horizontal);
        headAmountBox = new QDoubleSpinBox(headTab);
        headAmountBox->setObjectName(QString::fromUtf8("headAmountBox"));
        headAmountBox->setGeometry(QRect(250, 250, 51, 22));
        headAmountBox->setMinimum(-99);
        postureLabel_200 = new QLabel(headTab);
        postureLabel_200->setObjectName(QString::fromUtf8("postureLabel_200"));
        postureLabel_200->setGeometry(QRect(170, 220, 121, 20));
        postureLabel_200->setFont(font);
        postureLabel_200->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        headAmountSlider = new QSlider(headTab);
        headAmountSlider->setObjectName(QString::fromUtf8("headAmountSlider"));
        headAmountSlider->setGeometry(QRect(170, 250, 71, 19));
        headAmountSlider->setOrientation(Qt::Horizontal);
        headSbmSmoothBox = new QDoubleSpinBox(headTab);
        headSbmSmoothBox->setObjectName(QString::fromUtf8("headSbmSmoothBox"));
        headSbmSmoothBox->setGeometry(QRect(250, 310, 51, 22));
        headSbmSmoothBox->setMinimum(-99);
        postureLabel_202 = new QLabel(headTab);
        postureLabel_202->setObjectName(QString::fromUtf8("postureLabel_202"));
        postureLabel_202->setGeometry(QRect(170, 280, 121, 20));
        postureLabel_202->setFont(font);
        postureLabel_202->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        headSbmSmoothSlider = new QSlider(headTab);
        headSbmSmoothSlider->setObjectName(QString::fromUtf8("headSbmSmoothSlider"));
        headSbmSmoothSlider->setGeometry(QRect(170, 310, 71, 19));
        headSbmSmoothSlider->setOrientation(Qt::Horizontal);
        postureLabel_203 = new QLabel(headTab);
        postureLabel_203->setObjectName(QString::fromUtf8("postureLabel_203"));
        postureLabel_203->setGeometry(QRect(320, 100, 121, 20));
        postureLabel_203->setFont(font);
        postureLabel_203->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        headSbmWarpBox = new QDoubleSpinBox(headTab);
        headSbmWarpBox->setObjectName(QString::fromUtf8("headSbmWarpBox"));
        headSbmWarpBox->setGeometry(QRect(400, 130, 51, 22));
        headSbmWarpBox->setMinimum(-99);
        headSbmPeriodBox = new QDoubleSpinBox(headTab);
        headSbmPeriodBox->setObjectName(QString::fromUtf8("headSbmPeriodBox"));
        headSbmPeriodBox->setGeometry(QRect(400, 70, 51, 22));
        headSbmPeriodBox->setMinimum(-99);
        postureLabel_204 = new QLabel(headTab);
        postureLabel_204->setObjectName(QString::fromUtf8("postureLabel_204"));
        postureLabel_204->setGeometry(QRect(320, 40, 121, 20));
        postureLabel_204->setFont(font);
        postureLabel_204->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        headSbmWarpSlider = new QSlider(headTab);
        headSbmWarpSlider->setObjectName(QString::fromUtf8("headSbmWarpSlider"));
        headSbmWarpSlider->setGeometry(QRect(320, 130, 71, 19));
        headSbmWarpSlider->setOrientation(Qt::Horizontal);
        headSbmPeriodSlider = new QSlider(headTab);
        headSbmPeriodSlider->setObjectName(QString::fromUtf8("headSbmPeriodSlider"));
        headSbmPeriodSlider->setGeometry(QRect(320, 70, 71, 19));
        headSbmPeriodSlider->setOrientation(Qt::Horizontal);
        postureLabel_205 = new QLabel(headTab);
        postureLabel_205->setObjectName(QString::fromUtf8("postureLabel_205"));
        postureLabel_205->setGeometry(QRect(320, 160, 121, 20));
        postureLabel_205->setFont(font);
        postureLabel_205->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        headSbmAccelBox = new QDoubleSpinBox(headTab);
        headSbmAccelBox->setObjectName(QString::fromUtf8("headSbmAccelBox"));
        headSbmAccelBox->setGeometry(QRect(400, 190, 51, 22));
        headSbmAccelBox->setMinimum(-99);
        headSbmAccelSlider = new QSlider(headTab);
        headSbmAccelSlider->setObjectName(QString::fromUtf8("headSbmAccelSlider"));
        headSbmAccelSlider->setGeometry(QRect(320, 190, 71, 19));
        headSbmAccelSlider->setOrientation(Qt::Horizontal);
        postureLabel_206 = new QLabel(headTab);
        postureLabel_206->setObjectName(QString::fromUtf8("postureLabel_206"));
        postureLabel_206->setGeometry(QRect(320, 220, 121, 20));
        postureLabel_206->setFont(font);
        postureLabel_206->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        postureLabel_207 = new QLabel(headTab);
        postureLabel_207->setObjectName(QString::fromUtf8("postureLabel_207"));
        postureLabel_207->setGeometry(QRect(320, 280, 121, 20));
        postureLabel_207->setFont(font);
        postureLabel_207->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        headSbmPitchBox = new QDoubleSpinBox(headTab);
        headSbmPitchBox->setObjectName(QString::fromUtf8("headSbmPitchBox"));
        headSbmPitchBox->setGeometry(QRect(400, 250, 51, 22));
        headSbmPitchBox->setMinimum(-99);
        headSbmPitchSlider = new QSlider(headTab);
        headSbmPitchSlider->setObjectName(QString::fromUtf8("headSbmPitchSlider"));
        headSbmPitchSlider->setGeometry(QRect(320, 250, 71, 19));
        headSbmPitchSlider->setOrientation(Qt::Horizontal);
        headSbmDecaySlider = new QSlider(headTab);
        headSbmDecaySlider->setObjectName(QString::fromUtf8("headSbmDecaySlider"));
        headSbmDecaySlider->setGeometry(QRect(320, 310, 71, 19));
        headSbmDecaySlider->setOrientation(Qt::Horizontal);
        headSbmDecayBox = new QDoubleSpinBox(headTab);
        headSbmDecayBox->setObjectName(QString::fromUtf8("headSbmDecayBox"));
        headSbmDecayBox->setGeometry(QRect(400, 310, 51, 22));
        headSbmDecayBox->setMinimum(-99);
        tabWidget->addTab(headTab, QString());
        faceTab = new QWidget();
        faceTab->setObjectName(QString::fromUtf8("faceTab"));
        postureLabel_181 = new QLabel(faceTab);
        postureLabel_181->setObjectName(QString::fromUtf8("postureLabel_181"));
        postureLabel_181->setGeometry(QRect(20, 40, 121, 20));
        postureLabel_181->setFont(font);
        postureLabel_181->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        idLabel_76 = new QLabel(faceTab);
        idLabel_76->setObjectName(QString::fromUtf8("idLabel_76"));
        idLabel_76->setGeometry(QRect(320, 10, 91, 21));
        idLabel_76->setFont(font);
        idLabel_76->setAlignment(Qt::AlignCenter);
        line_32 = new QFrame(faceTab);
        line_32->setObjectName(QString::fromUtf8("line_32"));
        line_32->setGeometry(QRect(310, 30, 118, 3));
        line_32->setFrameShape(QFrame::HLine);
        line_32->setFrameShadow(QFrame::Sunken);
        faceType = new QComboBox(faceTab);
        faceType->setObjectName(QString::fromUtf8("faceType"));
        faceType->setGeometry(QRect(20, 60, 131, 31));
        faceSide = new QComboBox(faceTab);
        faceSide->setObjectName(QString::fromUtf8("faceSide"));
        faceSide->setGeometry(QRect(20, 120, 131, 31));
        postureLabel_182 = new QLabel(faceTab);
        postureLabel_182->setObjectName(QString::fromUtf8("postureLabel_182"));
        postureLabel_182->setGeometry(QRect(20, 100, 121, 20));
        postureLabel_182->setFont(font);
        postureLabel_182->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        faceAu = new QPlainTextEdit(faceTab);
        faceAu->setObjectName(QString::fromUtf8("faceAu"));
        faceAu->setGeometry(QRect(20, 180, 131, 31));
        postureLabel_183 = new QLabel(faceTab);
        postureLabel_183->setObjectName(QString::fromUtf8("postureLabel_183"));
        postureLabel_183->setGeometry(QRect(20, 160, 61, 20));
        postureLabel_183->setFont(font);
        postureLabel_183->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        postureLabel_184 = new QLabel(faceTab);
        postureLabel_184->setObjectName(QString::fromUtf8("postureLabel_184"));
        postureLabel_184->setGeometry(QRect(320, 100, 141, 20));
        postureLabel_184->setFont(font);
        postureLabel_184->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        faceAmountSlider = new QSlider(faceTab);
        faceAmountSlider->setObjectName(QString::fromUtf8("faceAmountSlider"));
        faceAmountSlider->setGeometry(QRect(320, 130, 71, 19));
        faceAmountSlider->setOrientation(Qt::Horizontal);
        faceAmountBox = new QDoubleSpinBox(faceTab);
        faceAmountBox->setObjectName(QString::fromUtf8("faceAmountBox"));
        faceAmountBox->setGeometry(QRect(400, 130, 51, 22));
        faceAmountBox->setMinimum(-99);
        postureLabel_185 = new QLabel(faceTab);
        postureLabel_185->setObjectName(QString::fromUtf8("postureLabel_185"));
        postureLabel_185->setGeometry(QRect(20, 220, 61, 20));
        postureLabel_185->setFont(font);
        postureLabel_185->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        faceStart = new QPlainTextEdit(faceTab);
        faceStart->setObjectName(QString::fromUtf8("faceStart"));
        faceStart->setGeometry(QRect(20, 240, 131, 31));
        faceReady = new QPlainTextEdit(faceTab);
        faceReady->setObjectName(QString::fromUtf8("faceReady"));
        faceReady->setGeometry(QRect(20, 300, 131, 31));
        postureLabel_186 = new QLabel(faceTab);
        postureLabel_186->setObjectName(QString::fromUtf8("postureLabel_186"));
        postureLabel_186->setGeometry(QRect(20, 280, 61, 20));
        postureLabel_186->setFont(font);
        postureLabel_186->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        postureLabel_187 = new QLabel(faceTab);
        postureLabel_187->setObjectName(QString::fromUtf8("postureLabel_187"));
        postureLabel_187->setGeometry(QRect(170, 40, 61, 20));
        postureLabel_187->setFont(font);
        postureLabel_187->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        faceStroke = new QPlainTextEdit(faceTab);
        faceStroke->setObjectName(QString::fromUtf8("faceStroke"));
        faceStroke->setGeometry(QRect(170, 60, 131, 31));
        postureLabel_188 = new QLabel(faceTab);
        postureLabel_188->setObjectName(QString::fromUtf8("postureLabel_188"));
        postureLabel_188->setGeometry(QRect(170, 100, 61, 20));
        postureLabel_188->setFont(font);
        postureLabel_188->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        faceRelax = new QPlainTextEdit(faceTab);
        faceRelax->setObjectName(QString::fromUtf8("faceRelax"));
        faceRelax->setGeometry(QRect(170, 120, 131, 31));
        postureLabel_189 = new QLabel(faceTab);
        postureLabel_189->setObjectName(QString::fromUtf8("postureLabel_189"));
        postureLabel_189->setGeometry(QRect(170, 160, 61, 20));
        postureLabel_189->setFont(font);
        postureLabel_189->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        faceEnd = new QPlainTextEdit(faceTab);
        faceEnd->setObjectName(QString::fromUtf8("faceEnd"));
        faceEnd->setGeometry(QRect(170, 180, 131, 31));
        postureLabel_190 = new QLabel(faceTab);
        postureLabel_190->setObjectName(QString::fromUtf8("postureLabel_190"));
        postureLabel_190->setGeometry(QRect(170, 220, 91, 20));
        postureLabel_190->setFont(font);
        postureLabel_190->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        faceSbmRampUp = new QPlainTextEdit(faceTab);
        faceSbmRampUp->setObjectName(QString::fromUtf8("faceSbmRampUp"));
        faceSbmRampUp->setGeometry(QRect(170, 240, 131, 31));
        postureLabel_191 = new QLabel(faceTab);
        postureLabel_191->setObjectName(QString::fromUtf8("postureLabel_191"));
        postureLabel_191->setGeometry(QRect(170, 280, 121, 20));
        postureLabel_191->setFont(font);
        postureLabel_191->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        faceSbmRampDown = new QPlainTextEdit(faceTab);
        faceSbmRampDown->setObjectName(QString::fromUtf8("faceSbmRampDown"));
        faceSbmRampDown->setGeometry(QRect(170, 300, 131, 31));
        postureLabel_192 = new QLabel(faceTab);
        postureLabel_192->setObjectName(QString::fromUtf8("postureLabel_192"));
        postureLabel_192->setGeometry(QRect(320, 40, 61, 20));
        postureLabel_192->setFont(font);
        postureLabel_192->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        faceId = new QPlainTextEdit(faceTab);
        faceId->setObjectName(QString::fromUtf8("faceId"));
        faceId->setGeometry(QRect(320, 60, 131, 31));
        tabWidget->addTab(faceTab, QString());
        speechTab = new QWidget();
        speechTab->setObjectName(QString::fromUtf8("speechTab"));
        postureLabel_152 = new QLabel(speechTab);
        postureLabel_152->setObjectName(QString::fromUtf8("postureLabel_152"));
        postureLabel_152->setGeometry(QRect(20, 40, 121, 20));
        postureLabel_152->setFont(font);
        postureLabel_152->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        speechContent = new QPlainTextEdit(speechTab);
        speechContent->setObjectName(QString::fromUtf8("speechContent"));
        speechContent->setGeometry(QRect(20, 120, 131, 31));
        postureLabel_153 = new QLabel(speechTab);
        postureLabel_153->setObjectName(QString::fromUtf8("postureLabel_153"));
        postureLabel_153->setGeometry(QRect(20, 100, 61, 20));
        postureLabel_153->setFont(font);
        postureLabel_153->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        idLabel_73 = new QLabel(speechTab);
        idLabel_73->setObjectName(QString::fromUtf8("idLabel_73"));
        idLabel_73->setGeometry(QRect(320, 10, 91, 21));
        idLabel_73->setFont(font);
        idLabel_73->setAlignment(Qt::AlignCenter);
        line_29 = new QFrame(speechTab);
        line_29->setObjectName(QString::fromUtf8("line_29"));
        line_29->setGeometry(QRect(310, 30, 118, 3));
        line_29->setFrameShape(QFrame::HLine);
        line_29->setFrameShadow(QFrame::Sunken);
        speech = new QComboBox(speechTab);
        speech->setObjectName(QString::fromUtf8("speech"));
        speech->setGeometry(QRect(20, 60, 131, 31));
        speechRef = new QPlainTextEdit(speechTab);
        speechRef->setObjectName(QString::fromUtf8("speechRef"));
        speechRef->setGeometry(QRect(20, 180, 131, 31));
        postureLabel_154 = new QLabel(speechTab);
        postureLabel_154->setObjectName(QString::fromUtf8("postureLabel_154"));
        postureLabel_154->setGeometry(QRect(20, 160, 61, 20));
        postureLabel_154->setFont(font);
        postureLabel_154->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        speechId = new QPlainTextEdit(speechTab);
        speechId->setObjectName(QString::fromUtf8("speechId"));
        speechId->setGeometry(QRect(20, 240, 131, 31));
        postureLabel_155 = new QLabel(speechTab);
        postureLabel_155->setObjectName(QString::fromUtf8("postureLabel_155"));
        postureLabel_155->setGeometry(QRect(20, 220, 61, 20));
        postureLabel_155->setFont(font);
        postureLabel_155->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        tabWidget->addTab(speechTab, QString());
        saccadeTab = new QWidget();
        saccadeTab->setObjectName(QString::fromUtf8("saccadeTab"));
        postureLabel_164 = new QLabel(saccadeTab);
        postureLabel_164->setObjectName(QString::fromUtf8("postureLabel_164"));
        postureLabel_164->setGeometry(QRect(20, 40, 121, 20));
        postureLabel_164->setFont(font);
        postureLabel_164->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        saccadeFinish = new QComboBox(saccadeTab);
        saccadeFinish->setObjectName(QString::fromUtf8("saccadeFinish"));
        saccadeFinish->setGeometry(QRect(20, 60, 131, 31));
        idLabel_75 = new QLabel(saccadeTab);
        idLabel_75->setObjectName(QString::fromUtf8("idLabel_75"));
        idLabel_75->setGeometry(QRect(320, 10, 91, 21));
        idLabel_75->setFont(font);
        idLabel_75->setAlignment(Qt::AlignCenter);
        line_31 = new QFrame(saccadeTab);
        line_31->setObjectName(QString::fromUtf8("line_31"));
        line_31->setGeometry(QRect(310, 30, 118, 3));
        line_31->setFrameShape(QFrame::HLine);
        line_31->setFrameShadow(QFrame::Sunken);
        saccadeMode = new QComboBox(saccadeTab);
        saccadeMode->setObjectName(QString::fromUtf8("saccadeMode"));
        saccadeMode->setGeometry(QRect(20, 120, 131, 31));
        postureLabel_165 = new QLabel(saccadeTab);
        postureLabel_165->setObjectName(QString::fromUtf8("postureLabel_165"));
        postureLabel_165->setGeometry(QRect(20, 100, 121, 20));
        postureLabel_165->setFont(font);
        postureLabel_165->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        saccadeAngleLimit = new QPlainTextEdit(saccadeTab);
        saccadeAngleLimit->setObjectName(QString::fromUtf8("saccadeAngleLimit"));
        saccadeAngleLimit->setGeometry(QRect(20, 180, 131, 31));
        postureLabel_166 = new QLabel(saccadeTab);
        postureLabel_166->setObjectName(QString::fromUtf8("postureLabel_166"));
        postureLabel_166->setGeometry(QRect(20, 160, 81, 20));
        postureLabel_166->setFont(font);
        postureLabel_166->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        postureLabel_167 = new QLabel(saccadeTab);
        postureLabel_167->setObjectName(QString::fromUtf8("postureLabel_167"));
        postureLabel_167->setGeometry(QRect(20, 220, 91, 20));
        postureLabel_167->setFont(font);
        postureLabel_167->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        saccadeSbmBin0 = new QPlainTextEdit(saccadeTab);
        saccadeSbmBin0->setObjectName(QString::fromUtf8("saccadeSbmBin0"));
        saccadeSbmBin0->setGeometry(QRect(20, 240, 131, 31));
        postureLabel_168 = new QLabel(saccadeTab);
        postureLabel_168->setObjectName(QString::fromUtf8("postureLabel_168"));
        postureLabel_168->setGeometry(QRect(20, 280, 91, 21));
        postureLabel_168->setFont(font);
        postureLabel_168->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        saccadeSbmBin45 = new QPlainTextEdit(saccadeTab);
        saccadeSbmBin45->setObjectName(QString::fromUtf8("saccadeSbmBin45"));
        saccadeSbmBin45->setGeometry(QRect(20, 300, 131, 31));
        postureLabel_169 = new QLabel(saccadeTab);
        postureLabel_169->setObjectName(QString::fromUtf8("postureLabel_169"));
        postureLabel_169->setGeometry(QRect(170, 40, 91, 20));
        postureLabel_169->setFont(font);
        postureLabel_169->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        saccadeSbmBin90 = new QPlainTextEdit(saccadeTab);
        saccadeSbmBin90->setObjectName(QString::fromUtf8("saccadeSbmBin90"));
        saccadeSbmBin90->setGeometry(QRect(170, 60, 131, 31));
        postureLabel_170 = new QLabel(saccadeTab);
        postureLabel_170->setObjectName(QString::fromUtf8("postureLabel_170"));
        postureLabel_170->setGeometry(QRect(170, 100, 91, 20));
        postureLabel_170->setFont(font);
        postureLabel_170->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        saccadeSbmBin135 = new QPlainTextEdit(saccadeTab);
        saccadeSbmBin135->setObjectName(QString::fromUtf8("saccadeSbmBin135"));
        saccadeSbmBin135->setGeometry(QRect(170, 120, 131, 31));
        postureLabel_171 = new QLabel(saccadeTab);
        postureLabel_171->setObjectName(QString::fromUtf8("postureLabel_171"));
        postureLabel_171->setGeometry(QRect(170, 160, 91, 20));
        postureLabel_171->setFont(font);
        postureLabel_171->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        saccadeSbmBin180 = new QPlainTextEdit(saccadeTab);
        saccadeSbmBin180->setObjectName(QString::fromUtf8("saccadeSbmBin180"));
        saccadeSbmBin180->setGeometry(QRect(170, 180, 131, 31));
        postureLabel_172 = new QLabel(saccadeTab);
        postureLabel_172->setObjectName(QString::fromUtf8("postureLabel_172"));
        postureLabel_172->setGeometry(QRect(170, 220, 91, 20));
        postureLabel_172->setFont(font);
        postureLabel_172->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        saccadeSbmBin225 = new QPlainTextEdit(saccadeTab);
        saccadeSbmBin225->setObjectName(QString::fromUtf8("saccadeSbmBin225"));
        saccadeSbmBin225->setGeometry(QRect(170, 240, 131, 31));
        postureLabel_173 = new QLabel(saccadeTab);
        postureLabel_173->setObjectName(QString::fromUtf8("postureLabel_173"));
        postureLabel_173->setGeometry(QRect(170, 280, 91, 20));
        postureLabel_173->setFont(font);
        postureLabel_173->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        saccadeSbmBin270 = new QPlainTextEdit(saccadeTab);
        saccadeSbmBin270->setObjectName(QString::fromUtf8("saccadeSbmBin270"));
        saccadeSbmBin270->setGeometry(QRect(170, 300, 131, 31));
        postureLabel_174 = new QLabel(saccadeTab);
        postureLabel_174->setObjectName(QString::fromUtf8("postureLabel_174"));
        postureLabel_174->setGeometry(QRect(320, 40, 101, 20));
        postureLabel_174->setFont(font);
        postureLabel_174->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        saccadeSbmBin315 = new QPlainTextEdit(saccadeTab);
        saccadeSbmBin315->setObjectName(QString::fromUtf8("saccadeSbmBin315"));
        saccadeSbmBin315->setGeometry(QRect(320, 60, 131, 31));
        postureLabel_175 = new QLabel(saccadeTab);
        postureLabel_175->setObjectName(QString::fromUtf8("postureLabel_175"));
        postureLabel_175->setGeometry(QRect(320, 100, 81, 20));
        postureLabel_175->setFont(font);
        postureLabel_175->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        saccadeSbmMean = new QPlainTextEdit(saccadeTab);
        saccadeSbmMean->setObjectName(QString::fromUtf8("saccadeSbmMean"));
        saccadeSbmMean->setGeometry(QRect(320, 120, 131, 31));
        postureLabel_176 = new QLabel(saccadeTab);
        postureLabel_176->setObjectName(QString::fromUtf8("postureLabel_176"));
        postureLabel_176->setGeometry(QRect(320, 160, 91, 20));
        postureLabel_176->setFont(font);
        postureLabel_176->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        saccadeSbmVariant = new QPlainTextEdit(saccadeTab);
        saccadeSbmVariant->setObjectName(QString::fromUtf8("saccadeSbmVariant"));
        saccadeSbmVariant->setGeometry(QRect(320, 180, 131, 31));
        postureLabel_177 = new QLabel(saccadeTab);
        postureLabel_177->setObjectName(QString::fromUtf8("postureLabel_177"));
        postureLabel_177->setGeometry(QRect(320, 220, 101, 20));
        postureLabel_177->setFont(font);
        postureLabel_177->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        saccadeDirection = new QPlainTextEdit(saccadeTab);
        saccadeDirection->setObjectName(QString::fromUtf8("saccadeDirection"));
        saccadeDirection->setGeometry(QRect(320, 240, 131, 31));
        postureLabel_178 = new QLabel(saccadeTab);
        postureLabel_178->setObjectName(QString::fromUtf8("postureLabel_178"));
        postureLabel_178->setGeometry(QRect(320, 280, 81, 20));
        postureLabel_178->setFont(font);
        postureLabel_178->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        saccadeMagnitude = new QPlainTextEdit(saccadeTab);
        saccadeMagnitude->setObjectName(QString::fromUtf8("saccadeMagnitude"));
        saccadeMagnitude->setGeometry(QRect(320, 300, 131, 31));
        postureLabel_179 = new QLabel(saccadeTab);
        postureLabel_179->setObjectName(QString::fromUtf8("postureLabel_179"));
        postureLabel_179->setGeometry(QRect(470, 40, 111, 20));
        postureLabel_179->setFont(font);
        postureLabel_179->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        saccadeSbmDuration = new QPlainTextEdit(saccadeTab);
        saccadeSbmDuration->setObjectName(QString::fromUtf8("saccadeSbmDuration"));
        saccadeSbmDuration->setGeometry(QRect(470, 60, 131, 31));
        saccadeId = new QPlainTextEdit(saccadeTab);
        saccadeId->setObjectName(QString::fromUtf8("saccadeId"));
        saccadeId->setGeometry(QRect(470, 120, 131, 31));
        postureLabel_180 = new QLabel(saccadeTab);
        postureLabel_180->setObjectName(QString::fromUtf8("postureLabel_180"));
        postureLabel_180->setGeometry(QRect(470, 100, 61, 20));
        postureLabel_180->setFont(font);
        postureLabel_180->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        tabWidget->addTab(saccadeTab, QString());
        eventTab = new QWidget();
        eventTab->setObjectName(QString::fromUtf8("eventTab"));
        idLabel_16 = new QLabel(eventTab);
        idLabel_16->setObjectName(QString::fromUtf8("idLabel_16"));
        idLabel_16->setGeometry(QRect(320, 10, 91, 21));
        idLabel_16->setFont(font);
        idLabel_16->setAlignment(Qt::AlignCenter);
        eventStroke = new QPlainTextEdit(eventTab);
        eventStroke->setObjectName(QString::fromUtf8("eventStroke"));
        eventStroke->setGeometry(QRect(20, 120, 131, 31));
        postureLabel_21 = new QLabel(eventTab);
        postureLabel_21->setObjectName(QString::fromUtf8("postureLabel_21"));
        postureLabel_21->setGeometry(QRect(20, 40, 61, 20));
        postureLabel_21->setFont(font);
        postureLabel_21->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        postureLabel_22 = new QLabel(eventTab);
        postureLabel_22->setObjectName(QString::fromUtf8("postureLabel_22"));
        postureLabel_22->setGeometry(QRect(20, 160, 61, 20));
        postureLabel_22->setFont(font);
        postureLabel_22->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        eventId = new QPlainTextEdit(eventTab);
        eventId->setObjectName(QString::fromUtf8("eventId"));
        eventId->setGeometry(QRect(20, 180, 131, 31));
        eventMessage = new QPlainTextEdit(eventTab);
        eventMessage->setObjectName(QString::fromUtf8("eventMessage"));
        eventMessage->setGeometry(QRect(20, 60, 131, 31));
        postureLabel_23 = new QLabel(eventTab);
        postureLabel_23->setObjectName(QString::fromUtf8("postureLabel_23"));
        postureLabel_23->setGeometry(QRect(20, 100, 61, 20));
        postureLabel_23->setFont(font);
        postureLabel_23->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        line_5 = new QFrame(eventTab);
        line_5->setObjectName(QString::fromUtf8("line_5"));
        line_5->setGeometry(QRect(310, 30, 118, 3));
        line_5->setFrameShape(QFrame::HLine);
        line_5->setFrameShadow(QFrame::Sunken);
        tabWidget->addTab(eventTab, QString());
        sbmstatesTab = new QWidget();
        sbmstatesTab->setObjectName(QString::fromUtf8("sbmstatesTab"));
        sbmStatesName = new QPlainTextEdit(sbmstatesTab);
        sbmStatesName->setObjectName(QString::fromUtf8("sbmStatesName"));
        sbmStatesName->setGeometry(QRect(20, 240, 131, 31));
        postureLabel_156 = new QLabel(sbmstatesTab);
        postureLabel_156->setObjectName(QString::fromUtf8("postureLabel_156"));
        postureLabel_156->setGeometry(QRect(20, 220, 61, 20));
        postureLabel_156->setFont(font);
        postureLabel_156->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        idLabel_74 = new QLabel(sbmstatesTab);
        idLabel_74->setObjectName(QString::fromUtf8("idLabel_74"));
        idLabel_74->setGeometry(QRect(320, 10, 91, 21));
        idLabel_74->setFont(font);
        idLabel_74->setAlignment(Qt::AlignCenter);
        line_30 = new QFrame(sbmstatesTab);
        line_30->setObjectName(QString::fromUtf8("line_30"));
        line_30->setGeometry(QRect(310, 30, 118, 3));
        line_30->setFrameShape(QFrame::HLine);
        line_30->setFrameShadow(QFrame::Sunken);
        postureLabel_157 = new QLabel(sbmstatesTab);
        postureLabel_157->setObjectName(QString::fromUtf8("postureLabel_157"));
        postureLabel_157->setGeometry(QRect(20, 40, 121, 20));
        postureLabel_157->setFont(font);
        postureLabel_157->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        sbmStatesMode = new QComboBox(sbmstatesTab);
        sbmStatesMode->setObjectName(QString::fromUtf8("sbmStatesMode"));
        sbmStatesMode->setGeometry(QRect(20, 60, 131, 31));
        sbmStatesLoop = new QComboBox(sbmstatesTab);
        sbmStatesLoop->setObjectName(QString::fromUtf8("sbmStatesLoop"));
        sbmStatesLoop->setGeometry(QRect(20, 120, 131, 31));
        postureLabel_158 = new QLabel(sbmstatesTab);
        postureLabel_158->setObjectName(QString::fromUtf8("postureLabel_158"));
        postureLabel_158->setGeometry(QRect(20, 100, 121, 20));
        postureLabel_158->setFont(font);
        postureLabel_158->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        sbmStatesSbmStartNow = new QComboBox(sbmstatesTab);
        sbmStatesSbmStartNow->setObjectName(QString::fromUtf8("sbmStatesSbmStartNow"));
        sbmStatesSbmStartNow->setGeometry(QRect(20, 180, 131, 31));
        postureLabel_159 = new QLabel(sbmstatesTab);
        postureLabel_159->setObjectName(QString::fromUtf8("postureLabel_159"));
        postureLabel_159->setGeometry(QRect(20, 160, 121, 20));
        postureLabel_159->setFont(font);
        postureLabel_159->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        postureLabel_160 = new QLabel(sbmstatesTab);
        postureLabel_160->setObjectName(QString::fromUtf8("postureLabel_160"));
        postureLabel_160->setGeometry(QRect(170, 40, 61, 20));
        postureLabel_160->setFont(font);
        postureLabel_160->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        sbmStatesX = new QPlainTextEdit(sbmstatesTab);
        sbmStatesX->setObjectName(QString::fromUtf8("sbmStatesX"));
        sbmStatesX->setGeometry(QRect(170, 60, 131, 31));
        postureLabel_161 = new QLabel(sbmstatesTab);
        postureLabel_161->setObjectName(QString::fromUtf8("postureLabel_161"));
        postureLabel_161->setGeometry(QRect(170, 100, 61, 20));
        postureLabel_161->setFont(font);
        postureLabel_161->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        sbmStatesY = new QPlainTextEdit(sbmstatesTab);
        sbmStatesY->setObjectName(QString::fromUtf8("sbmStatesY"));
        sbmStatesY->setGeometry(QRect(170, 120, 131, 31));
        postureLabel_162 = new QLabel(sbmstatesTab);
        postureLabel_162->setObjectName(QString::fromUtf8("postureLabel_162"));
        postureLabel_162->setGeometry(QRect(170, 160, 61, 20));
        postureLabel_162->setFont(font);
        postureLabel_162->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        sbmStatesZ = new QPlainTextEdit(sbmstatesTab);
        sbmStatesZ->setObjectName(QString::fromUtf8("sbmStatesZ"));
        sbmStatesZ->setGeometry(QRect(170, 180, 131, 31));
        postureLabel_163 = new QLabel(sbmstatesTab);
        postureLabel_163->setObjectName(QString::fromUtf8("postureLabel_163"));
        postureLabel_163->setGeometry(QRect(170, 220, 61, 20));
        postureLabel_163->setFont(font);
        postureLabel_163->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        sbmStatesId = new QPlainTextEdit(sbmstatesTab);
        sbmStatesId->setObjectName(QString::fromUtf8("sbmStatesId"));
        sbmStatesId->setGeometry(QRect(170, 240, 131, 31));
        tabWidget->addTab(sbmstatesTab, QString());

        retranslateUi(BMLCreator);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(BMLCreator);
    } // setupUi

    void retranslateUi(QDialog *BMLCreator)
    {
        BMLCreator->setWindowTitle(QApplication::translate("BMLCreator", "BML Creator", 0, QApplication::UnicodeUTF8));
        refreshButton->setText(QApplication::translate("BMLCreator", "Refresh", 0, QApplication::UnicodeUTF8));
        resetBml->setText(QApplication::translate("BMLCreator", "Reset BML", 0, QApplication::UnicodeUTF8));
        runButton->setText(QApplication::translate("BMLCreator", "Run", 0, QApplication::UnicodeUTF8));
        characterNameLabel->setText(QApplication::translate("BMLCreator", "Characters", 0, QApplication::UnicodeUTF8));
        postureLabel->setText(QApplication::translate("BMLCreator", "posture", 0, QApplication::UnicodeUTF8));
        startLabel->setText(QApplication::translate("BMLCreator", "start", 0, QApplication::UnicodeUTF8));
        readyLabel->setText(QApplication::translate("BMLCreator", "ready", 0, QApplication::UnicodeUTF8));
        idLabel->setText(QApplication::translate("BMLCreator", "id", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        bodyIdBox->setAccessibleName(QApplication::translate("BMLCreator", "id", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        bodyReadyBox->setAccessibleName(QApplication::translate("BMLCreator", "ready", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        bodyStartBox->setAccessibleName(QApplication::translate("BMLCreator", "start", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        bodyPostureBox->setAccessibleName(QApplication::translate("BMLCreator", "posture", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        idLabel_2->setText(QApplication::translate("BMLCreator", "body", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(bodyTab), QApplication::translate("BMLCreator", "body", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        locomotionFacingBox->setAccessibleName(QApplication::translate("BMLCreator", "facing", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        readyLabel_2->setText(QApplication::translate("BMLCreator", "manner", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        locomotionTargetBox->setAccessibleName(QApplication::translate("BMLCreator", "target", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        idLabel_3->setText(QApplication::translate("BMLCreator", "facing", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        locomotionFollowBox->setAccessibleName(QApplication::translate("BMLCreator", "sbm:follow", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        startLabel_2->setText(QApplication::translate("BMLCreator", "type", 0, QApplication::UnicodeUTF8));
        postureLabel_2->setText(QApplication::translate("BMLCreator", "target", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        locomotionAccelBox->setAccessibleName(QApplication::translate("BMLCreator", "sbm:accel", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        idLabel_4->setText(QApplication::translate("BMLCreator", "locomotion", 0, QApplication::UnicodeUTF8));
        idLabel_5->setText(QApplication::translate("BMLCreator", "proximity", 0, QApplication::UnicodeUTF8));
        idLabel_6->setText(QApplication::translate("BMLCreator", "sbm:follow", 0, QApplication::UnicodeUTF8));
        idLabel_7->setText(QApplication::translate("BMLCreator", "sbm:accel", 0, QApplication::UnicodeUTF8));
        idLabel_8->setText(QApplication::translate("BMLCreator", "sbm:scootaccel", 0, QApplication::UnicodeUTF8));
        idLabel_9->setText(QApplication::translate("BMLCreator", "sbm:angleaccel", 0, QApplication::UnicodeUTF8));
        idLabel_10->setText(QApplication::translate("BMLCreator", "sbm:numsteps", 0, QApplication::UnicodeUTF8));
        idLabel_11->setText(QApplication::translate("BMLCreator", "start", 0, QApplication::UnicodeUTF8));
        idLabel_12->setText(QApplication::translate("BMLCreator", "id", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        locomotionTypeBox->setAccessibleName(QApplication::translate("BMLCreator", "type", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        locomotionMannerBox->setAccessibleName(QApplication::translate("BMLCreator", "manner", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        idLabel_13->setText(QApplication::translate("BMLCreator", "sbm:breaking", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        locomotionProximityBox->setAccessibleName(QApplication::translate("BMLCreator", "proximity", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        locomotionScootAccelBox->setAccessibleName(QApplication::translate("BMLCreator", "sbm:scootaccel", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        locomotionAngleAccelBox->setAccessibleName(QApplication::translate("BMLCreator", "sbm:angleaccel", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        locomotionNumStepsBox->setAccessibleName(QApplication::translate("BMLCreator", "sbm:numsteps", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        locomotionStartBox->setAccessibleName(QApplication::translate("BMLCreator", "start", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        locomotionIdBox->setAccessibleName(QApplication::translate("BMLCreator", "id", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        locomotionSbmBreakingSlider->setAccessibleName(QApplication::translate("BMLCreator", "sbm:breaking", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        locomotionSbmBreakingBox->setAccessibleName(QApplication::translate("BMLCreator", "sbm:breaking", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        tabWidget->setTabText(tabWidget->indexOf(locomotionTab), QApplication::translate("BMLCreator", "locomotion", 0, QApplication::UnicodeUTF8));
        idLabel_14->setText(QApplication::translate("BMLCreator", "animation", 0, QApplication::UnicodeUTF8));
        postureLabel_3->setText(QApplication::translate("BMLCreator", "name", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        animationNameBox->setAccessibleName(QApplication::translate("BMLCreator", "name", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_4->setText(QApplication::translate("BMLCreator", "speed", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        animationStartBox->setAccessibleName(QApplication::translate("BMLCreator", "start", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_5->setText(QApplication::translate("BMLCreator", "start", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        animationReadyBox->setAccessibleName(QApplication::translate("BMLCreator", "ready", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_6->setText(QApplication::translate("BMLCreator", "ready", 0, QApplication::UnicodeUTF8));
        postureLabel_7->setText(QApplication::translate("BMLCreator", "stroke", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        animationStrokeBox->setAccessibleName(QApplication::translate("BMLCreator", "stroke", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_8->setText(QApplication::translate("BMLCreator", "relax", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        animationRelaxBox->setAccessibleName(QApplication::translate("BMLCreator", "relax", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_9->setText(QApplication::translate("BMLCreator", "end", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        animationsEndBox->setAccessibleName(QApplication::translate("BMLCreator", "end", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_10->setText(QApplication::translate("BMLCreator", "id", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        animationIdBox->setAccessibleName(QApplication::translate("BMLCreator", "id", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        animationSpeedSlider->setAccessibleName(QApplication::translate("BMLCreator", "speed", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        animationSpeedBox->setAccessibleName(QApplication::translate("BMLCreator", "speed", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        tabWidget->setTabText(tabWidget->indexOf(animationTab), QApplication::translate("BMLCreator", "animation", 0, QApplication::UnicodeUTF8));
        postureLabel_11->setText(QApplication::translate("BMLCreator", "lexeme", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        gestureLexemeBox->setAccessibleName(QApplication::translate("BMLCreator", "lexeme", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        idLabel_15->setText(QApplication::translate("BMLCreator", "gesture", 0, QApplication::UnicodeUTF8));
        postureLabel_12->setText(QApplication::translate("BMLCreator", "name", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        gestureNameBox->setAccessibleName(QApplication::translate("BMLCreator", "name", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        gestureModeBox->setAccessibleName(QApplication::translate("BMLCreator", "mode", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_13->setText(QApplication::translate("BMLCreator", "mode", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        gestureReadyBox->setAccessibleName(QApplication::translate("BMLCreator", "ready", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_14->setText(QApplication::translate("BMLCreator", "target", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        gestureTargetBox->setAccessibleName(QApplication::translate("BMLCreator", "target", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_15->setText(QApplication::translate("BMLCreator", "start", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        gestureStartBox->setAccessibleName(QApplication::translate("BMLCreator", "start", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_16->setText(QApplication::translate("BMLCreator", "ready", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        gestureStrokeBox->setAccessibleName(QApplication::translate("BMLCreator", "stroke", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_17->setText(QApplication::translate("BMLCreator", "stroke", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        gestureRelaxBox->setAccessibleName(QApplication::translate("BMLCreator", "relax", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_18->setText(QApplication::translate("BMLCreator", "relax", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        gestureEndBox->setAccessibleName(QApplication::translate("BMLCreator", "end", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_19->setText(QApplication::translate("BMLCreator", "end", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        gestureIdBox->setAccessibleName(QApplication::translate("BMLCreator", "id", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_20->setText(QApplication::translate("BMLCreator", "id", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(gestureTab), QApplication::translate("BMLCreator", "gesture", 0, QApplication::UnicodeUTF8));
        idLabel_17->setText(QApplication::translate("BMLCreator", "sbm:reach", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        sbmReachSbmHandle->setAccessibleName(QApplication::translate("BMLCreator", "sbm:handle", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_24->setText(QApplication::translate("BMLCreator", "target", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        sbmReachTarget->setAccessibleName(QApplication::translate("BMLCreator", "target", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_25->setText(QApplication::translate("BMLCreator", "sbm:handle", 0, QApplication::UnicodeUTF8));
        postureLabel_26->setText(QApplication::translate("BMLCreator", "sbm:action", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        sbmReachSbmAction->setAccessibleName(QApplication::translate("BMLCreator", "sbm:action", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        sbmReachSbmFootIk->setAccessibleName(QApplication::translate("BMLCreator", "sbm:foot-ik", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        sbmReachSbmFootIk->setText(QApplication::translate("BMLCreator", "sbm:foot-ik", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        sbmReachSbmReachFinish->setAccessibleName(QApplication::translate("BMLCreator", "sbm:reach-finish", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        sbmReachSbmReachFinish->setText(QApplication::translate("BMLCreator", "sbm:reach-finish", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        sbmReachsbmVelocity->setAccessibleName(QApplication::translate("BMLCreator", "sbm:reach-velocity", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_27->setText(QApplication::translate("BMLCreator", "sbm:reach-velocity", 0, QApplication::UnicodeUTF8));
        postureLabel_28->setText(QApplication::translate("BMLCreator", "sbm:reach-duration", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        sbmReachSbmDuration->setAccessibleName(QApplication::translate("BMLCreator", "sbm:reach-duration", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        sbmReachStart->setAccessibleName(QApplication::translate("BMLCreator", "start", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_29->setText(QApplication::translate("BMLCreator", "start", 0, QApplication::UnicodeUTF8));
        postureLabel_30->setText(QApplication::translate("BMLCreator", "id", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        sbmReachId->setAccessibleName(QApplication::translate("BMLCreator", "id", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        doubleSpinBox->setAccessibleName(QApplication::translate("BMLCreator", "sbm:reach-velocity", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        doubleSpinBox_2->setAccessibleName(QApplication::translate("BMLCreator", "sbm:reach-duration", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        tabWidget->setTabText(tabWidget->indexOf(sbmreachTab), QApplication::translate("BMLCreator", "sbm:reach", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        gazeTarget->setAccessibleName(QApplication::translate("BMLCreator", "target", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_31->setText(QApplication::translate("BMLCreator", "target", 0, QApplication::UnicodeUTF8));
        idLabel_18->setText(QApplication::translate("BMLCreator", "gaze", 0, QApplication::UnicodeUTF8));
        postureLabel_32->setText(QApplication::translate("BMLCreator", "sbm:joint-range", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        gazeSbmJointRange->setAccessibleName(QApplication::translate("BMLCreator", "sbm:joint-range", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_33->setText(QApplication::translate("BMLCreator", "direction", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        gazeDirection->setAccessibleName(QApplication::translate("BMLCreator", "direction", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_67->setText(QApplication::translate("BMLCreator", "angle", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        gazeAngleSlider->setAccessibleName(QApplication::translate("BMLCreator", "angle", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_68->setText(QApplication::translate("BMLCreator", "sbm:priority-joint", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        gazeSbmPriorityJoint->setAccessibleName(QApplication::translate("BMLCreator", "sbm:priority-joint", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        gazeSbmHandle->setAccessibleName(QApplication::translate("BMLCreator", "sbm:handle", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_69->setText(QApplication::translate("BMLCreator", "sbm:handle", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        gazeAngleBox->setAccessibleName(QApplication::translate("BMLCreator", "angle", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        gazeSbmTimeHintBox->setAccessibleName(QApplication::translate("BMLCreator", "sbm:time-hint", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_106->setText(QApplication::translate("BMLCreator", "sbm:time-hint", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        gazeSbmTimeHintSlider->setAccessibleName(QApplication::translate("BMLCreator", "sbm:time-hint", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        gazeSbmJointSpeed->setAccessibleName(QApplication::translate("BMLCreator", "sbm:joint-speed", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_107->setText(QApplication::translate("BMLCreator", "sbm:joint-speed", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        gazeSbmJointSmooth->setAccessibleName(QApplication::translate("BMLCreator", "sbm:joint-smooth", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_108->setText(QApplication::translate("BMLCreator", "sbm:joint-smooth", 0, QApplication::UnicodeUTF8));
        postureLabel_148->setText(QApplication::translate("BMLCreator", "sbm:fade-in", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        gazeSbmFadeInSlider->setAccessibleName(QApplication::translate("BMLCreator", "sbm:fade-in", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        gazeSbmFadeInBox->setAccessibleName(QApplication::translate("BMLCreator", "sbm:fade-in", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_149->setText(QApplication::translate("BMLCreator", "id", 0, QApplication::UnicodeUTF8));
        postureLabel_150->setText(QApplication::translate("BMLCreator", "start", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        gazeId->setAccessibleName(QApplication::translate("BMLCreator", "id", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        gazeStart->setAccessibleName(QApplication::translate("BMLCreator", "start", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_151->setText(QApplication::translate("BMLCreator", "sbm:fade-out", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        gazeSbmFadeOutBox->setAccessibleName(QApplication::translate("BMLCreator", "sbm:fade-out", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        gazeSbmFadeOutSlider->setAccessibleName(QApplication::translate("BMLCreator", "sbm:fade-out", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        tabWidget->setTabText(tabWidget->indexOf(gazeTab), QApplication::translate("BMLCreator", "gaze", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        sbmConstraintTarget->setAccessibleName(QApplication::translate("BMLCreator", "target", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_208->setText(QApplication::translate("BMLCreator", "sbm:handle", 0, QApplication::UnicodeUTF8));
        postureLabel_209->setText(QApplication::translate("BMLCreator", "target", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        sbmConstraintSbmHandle->setAccessibleName(QApplication::translate("BMLCreator", "sbm:handle", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        idLabel_78->setText(QApplication::translate("BMLCreator", "sbm:contraint", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        sbmConstraintSbmRoot->setAccessibleName(QApplication::translate("BMLCreator", "sbm:root", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        sbmConstraintEffector->setAccessibleName(QApplication::translate("BMLCreator", "effector", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_210->setText(QApplication::translate("BMLCreator", "sbm:root", 0, QApplication::UnicodeUTF8));
        postureLabel_211->setText(QApplication::translate("BMLCreator", "effector", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        sbmConstraintSbmEffectRoot->setAccessibleName(QApplication::translate("BMLCreator", "sbm:effector-root", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_212->setText(QApplication::translate("BMLCreator", "sbm:effect-root", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        sbmConstraintStart->setAccessibleName(QApplication::translate("BMLCreator", "start", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_213->setText(QApplication::translate("BMLCreator", "start", 0, QApplication::UnicodeUTF8));
        postureLabel_214->setText(QApplication::translate("BMLCreator", "ready", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        sbmConstraintReady->setAccessibleName(QApplication::translate("BMLCreator", "ready", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        sbmConstraintId->setAccessibleName(QApplication::translate("BMLCreator", "id", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_215->setText(QApplication::translate("BMLCreator", "id", 0, QApplication::UnicodeUTF8));
        postureLabel_216->setText(QApplication::translate("BMLCreator", "sbm:constraint-type", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        sbmConstraintSbmConstraintType->setAccessibleName(QApplication::translate("BMLCreator", "sbm:constraint-type", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        sbmConstraintRotZBox->setAccessibleName(QApplication::translate("BMLCreator", "rot-z", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        sbmConstraintSbmFadeOutSlider->setAccessibleName(QApplication::translate("BMLCreator", "sbm:fade-out", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        sbmConstraintPosXSlider->setAccessibleName(QApplication::translate("BMLCreator", "pos-x", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_217->setText(QApplication::translate("BMLCreator", "rot-x", 0, QApplication::UnicodeUTF8));
        postureLabel_218->setText(QApplication::translate("BMLCreator", "rot-z", 0, QApplication::UnicodeUTF8));
        postureLabel_219->setText(QApplication::translate("BMLCreator", "sbm:fade-out", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        sbmConstraintPosYBox->setAccessibleName(QApplication::translate("BMLCreator", "pos-y", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        sbmConstraintRotYSlider->setAccessibleName(QApplication::translate("BMLCreator", "rot-y", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        sbmConstraintSbmFadeInBox->setAccessibleName(QApplication::translate("BMLCreator", "sbm:fade-in", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        sbmConstraintSbmFadeInSlider->setAccessibleName(QApplication::translate("BMLCreator", "sbm:fade-in", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        sbmConstraintRotZSlider->setAccessibleName(QApplication::translate("BMLCreator", "rot-z", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        sbmConstraintPosYSlider->setAccessibleName(QApplication::translate("BMLCreator", "pos-y", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        sbmConstraintPosZSlider->setAccessibleName(QApplication::translate("BMLCreator", "pos-z", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_220->setText(QApplication::translate("BMLCreator", "pos-z", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        sbmConstraintPosXBox->setAccessibleName(QApplication::translate("BMLCreator", "pos-x", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        sbmConstraintRotXBox->setAccessibleName(QApplication::translate("BMLCreator", "rot-x", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        sbmConstraintPosZBox->setAccessibleName(QApplication::translate("BMLCreator", "pos-z", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_221->setText(QApplication::translate("BMLCreator", "sbm:fade-in", 0, QApplication::UnicodeUTF8));
        postureLabel_222->setText(QApplication::translate("BMLCreator", "pos-x", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        sbmConstraintRotYBox->setAccessibleName(QApplication::translate("BMLCreator", "rot-y", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        sbmConstraintRotXSlider->setAccessibleName(QApplication::translate("BMLCreator", "rot-x", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        sbmConstraintSbmFadeOutBox->setAccessibleName(QApplication::translate("BMLCreator", "sbm:fade-out", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_223->setText(QApplication::translate("BMLCreator", "pos-y", 0, QApplication::UnicodeUTF8));
        postureLabel_224->setText(QApplication::translate("BMLCreator", "rot-y", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(sbmconstraintTab), QApplication::translate("BMLCreator", "sbm:constraint", 0, QApplication::UnicodeUTF8));
        idLabel_77->setText(QApplication::translate("BMLCreator", "head", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        headType->setAccessibleName(QApplication::translate("BMLCreator", "type", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_193->setText(QApplication::translate("BMLCreator", "type", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        headId->setAccessibleName(QApplication::translate("BMLCreator", "id", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_194->setText(QApplication::translate("BMLCreator", "stroke", 0, QApplication::UnicodeUTF8));
        postureLabel_195->setText(QApplication::translate("BMLCreator", "ready", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        headEnd->setAccessibleName(QApplication::translate("BMLCreator", "end", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_196->setText(QApplication::translate("BMLCreator", "relax", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        headStroke->setAccessibleName(QApplication::translate("BMLCreator", "stroke", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_197->setText(QApplication::translate("BMLCreator", "id", 0, QApplication::UnicodeUTF8));
        postureLabel_199->setText(QApplication::translate("BMLCreator", "end", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        headReady->setAccessibleName(QApplication::translate("BMLCreator", "ready", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        headRelax->setAccessibleName(QApplication::translate("BMLCreator", "relax", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        headStart->setAccessibleName(QApplication::translate("BMLCreator", "start", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_201->setText(QApplication::translate("BMLCreator", "start", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        headRepeatsBox->setAccessibleName(QApplication::translate("BMLCreator", "repeats", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_198->setText(QApplication::translate("BMLCreator", "repeats", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        headRepeatsSlider->setAccessibleName(QApplication::translate("BMLCreator", "repeats", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        headAmountBox->setAccessibleName(QApplication::translate("BMLCreator", "amount", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_200->setText(QApplication::translate("BMLCreator", "amount", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        headAmountSlider->setAccessibleName(QApplication::translate("BMLCreator", "amount", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        headSbmSmoothBox->setAccessibleName(QApplication::translate("BMLCreator", "sbm:smooth", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_202->setText(QApplication::translate("BMLCreator", "sbm:smooth", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        headSbmSmoothSlider->setAccessibleName(QApplication::translate("BMLCreator", "sbm:smooth", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_203->setText(QApplication::translate("BMLCreator", "sbm:warp", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        headSbmWarpBox->setAccessibleName(QApplication::translate("BMLCreator", "sbm:warp", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        headSbmPeriodBox->setAccessibleName(QApplication::translate("BMLCreator", "sbm:period", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_204->setText(QApplication::translate("BMLCreator", "sbm:period", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        headSbmWarpSlider->setAccessibleName(QApplication::translate("BMLCreator", "sbm:warp", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        headSbmPeriodSlider->setAccessibleName(QApplication::translate("BMLCreator", "sbm:period", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_205->setText(QApplication::translate("BMLCreator", "sbm:accel", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        headSbmAccelBox->setAccessibleName(QApplication::translate("BMLCreator", "sbm:accel", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        headSbmAccelSlider->setAccessibleName(QApplication::translate("BMLCreator", "sbm:accel", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_206->setText(QApplication::translate("BMLCreator", "sbm:pitch", 0, QApplication::UnicodeUTF8));
        postureLabel_207->setText(QApplication::translate("BMLCreator", "sbm:decay", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        headSbmPitchBox->setAccessibleName(QApplication::translate("BMLCreator", "sbm:pitch", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        headSbmPitchSlider->setAccessibleName(QApplication::translate("BMLCreator", "sbm:pitch", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        headSbmDecaySlider->setAccessibleName(QApplication::translate("BMLCreator", "sbm:decay", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        headSbmDecayBox->setAccessibleName(QApplication::translate("BMLCreator", "sbm:decay", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        tabWidget->setTabText(tabWidget->indexOf(headTab), QApplication::translate("BMLCreator", "head", 0, QApplication::UnicodeUTF8));
        postureLabel_181->setText(QApplication::translate("BMLCreator", "type", 0, QApplication::UnicodeUTF8));
        idLabel_76->setText(QApplication::translate("BMLCreator", "face", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        faceType->setAccessibleName(QApplication::translate("BMLCreator", "type", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        faceSide->setAccessibleName(QApplication::translate("BMLCreator", "side", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_182->setText(QApplication::translate("BMLCreator", "side", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        faceAu->setAccessibleName(QApplication::translate("BMLCreator", "au", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_183->setText(QApplication::translate("BMLCreator", "au", 0, QApplication::UnicodeUTF8));
        postureLabel_184->setText(QApplication::translate("BMLCreator", "amount", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        faceAmountSlider->setAccessibleName(QApplication::translate("BMLCreator", "amount", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        faceAmountBox->setAccessibleName(QApplication::translate("BMLCreator", "amount", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_185->setText(QApplication::translate("BMLCreator", "start", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        faceStart->setAccessibleName(QApplication::translate("BMLCreator", "start", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        faceReady->setAccessibleName(QApplication::translate("BMLCreator", "ready", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_186->setText(QApplication::translate("BMLCreator", "ready", 0, QApplication::UnicodeUTF8));
        postureLabel_187->setText(QApplication::translate("BMLCreator", "stroke", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        faceStroke->setAccessibleName(QApplication::translate("BMLCreator", "stroke", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_188->setText(QApplication::translate("BMLCreator", "relax", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        faceRelax->setAccessibleName(QApplication::translate("BMLCreator", "relax", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_189->setText(QApplication::translate("BMLCreator", "end", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        faceEnd->setAccessibleName(QApplication::translate("BMLCreator", "end", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_190->setText(QApplication::translate("BMLCreator", "sbm:rampup", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        faceSbmRampUp->setAccessibleName(QApplication::translate("BMLCreator", "sbm:rampup", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_191->setText(QApplication::translate("BMLCreator", "sbm:rampdown", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        faceSbmRampDown->setAccessibleName(QApplication::translate("BMLCreator", "sbm:rampdown", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_192->setText(QApplication::translate("BMLCreator", "id", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        faceId->setAccessibleName(QApplication::translate("BMLCreator", "id", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        tabWidget->setTabText(tabWidget->indexOf(faceTab), QApplication::translate("BMLCreator", "face", 0, QApplication::UnicodeUTF8));
        postureLabel_152->setText(QApplication::translate("BMLCreator", "type", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        speechContent->setAccessibleName(QApplication::translate("BMLCreator", "content", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_153->setText(QApplication::translate("BMLCreator", "content", 0, QApplication::UnicodeUTF8));
        idLabel_73->setText(QApplication::translate("BMLCreator", "speech", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        speech->setAccessibleName(QApplication::translate("BMLCreator", "type", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        speechRef->setAccessibleName(QApplication::translate("BMLCreator", "ref", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_154->setText(QApplication::translate("BMLCreator", "ref", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        speechId->setAccessibleName(QApplication::translate("BMLCreator", "id", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_155->setText(QApplication::translate("BMLCreator", "id", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(speechTab), QApplication::translate("BMLCreator", "speech", 0, QApplication::UnicodeUTF8));
        postureLabel_164->setText(QApplication::translate("BMLCreator", "finish", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        saccadeFinish->setAccessibleName(QApplication::translate("BMLCreator", "finish", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        idLabel_75->setText(QApplication::translate("BMLCreator", "saccade", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        saccadeMode->setAccessibleName(QApplication::translate("BMLCreator", "mode", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_165->setText(QApplication::translate("BMLCreator", "mode", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        saccadeAngleLimit->setAccessibleName(QApplication::translate("BMLCreator", "angle-limit", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_166->setText(QApplication::translate("BMLCreator", "angle-limit", 0, QApplication::UnicodeUTF8));
        postureLabel_167->setText(QApplication::translate("BMLCreator", "sbm:bin0", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        saccadeSbmBin0->setAccessibleName(QApplication::translate("BMLCreator", "sbm:bin0", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_168->setText(QApplication::translate("BMLCreator", "sbm:bin45", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        saccadeSbmBin45->setAccessibleName(QApplication::translate("BMLCreator", "sbm:bin45", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_169->setText(QApplication::translate("BMLCreator", "sbm:bin90", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        saccadeSbmBin90->setAccessibleName(QApplication::translate("BMLCreator", "sbm:bin90", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_170->setText(QApplication::translate("BMLCreator", "sbm:bin135", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        saccadeSbmBin135->setAccessibleName(QApplication::translate("BMLCreator", "sbm:bin135", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_171->setText(QApplication::translate("BMLCreator", "sbm:bin180", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        saccadeSbmBin180->setAccessibleName(QApplication::translate("BMLCreator", "sbm:bin180", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_172->setText(QApplication::translate("BMLCreator", "sbm:bin225", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        saccadeSbmBin225->setAccessibleName(QApplication::translate("BMLCreator", "sbm:bin225", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_173->setText(QApplication::translate("BMLCreator", "sbm:bin270", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        saccadeSbmBin270->setAccessibleName(QApplication::translate("BMLCreator", "sbm:bin270", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_174->setText(QApplication::translate("BMLCreator", "sbm:bin315", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        saccadeSbmBin315->setAccessibleName(QApplication::translate("BMLCreator", "sbm:bin315", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_175->setText(QApplication::translate("BMLCreator", "sbm:mean", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        saccadeSbmMean->setAccessibleName(QApplication::translate("BMLCreator", "sbm:mean", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_176->setText(QApplication::translate("BMLCreator", "sbm:variant", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        saccadeSbmVariant->setAccessibleName(QApplication::translate("BMLCreator", "sbm:variant", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_177->setText(QApplication::translate("BMLCreator", "direction", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        saccadeDirection->setAccessibleName(QApplication::translate("BMLCreator", "direction", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_178->setText(QApplication::translate("BMLCreator", "magnitude", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        saccadeMagnitude->setAccessibleName(QApplication::translate("BMLCreator", "magnitude", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_179->setText(QApplication::translate("BMLCreator", "sbm:duration", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        saccadeSbmDuration->setAccessibleName(QApplication::translate("BMLCreator", "sbm:duration", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        saccadeId->setAccessibleName(QApplication::translate("BMLCreator", "id", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_180->setText(QApplication::translate("BMLCreator", "id", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(saccadeTab), QApplication::translate("BMLCreator", "saccade", 0, QApplication::UnicodeUTF8));
        idLabel_16->setText(QApplication::translate("BMLCreator", "event", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        eventStroke->setAccessibleName(QApplication::translate("BMLCreator", "stroke", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_21->setText(QApplication::translate("BMLCreator", "message", 0, QApplication::UnicodeUTF8));
        postureLabel_22->setText(QApplication::translate("BMLCreator", "id", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        eventId->setAccessibleName(QApplication::translate("BMLCreator", "id", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        eventMessage->setAccessibleName(QApplication::translate("BMLCreator", "message", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_23->setText(QApplication::translate("BMLCreator", "stroke", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(eventTab), QApplication::translate("BMLCreator", "event", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        sbmStatesName->setAccessibleName(QApplication::translate("BMLCreator", "name", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_156->setText(QApplication::translate("BMLCreator", "name", 0, QApplication::UnicodeUTF8));
        idLabel_74->setText(QApplication::translate("BMLCreator", "sbm:states", 0, QApplication::UnicodeUTF8));
        postureLabel_157->setText(QApplication::translate("BMLCreator", "mode", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        sbmStatesMode->setAccessibleName(QApplication::translate("BMLCreator", "mode", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
#ifndef QT_NO_ACCESSIBILITY
        sbmStatesLoop->setAccessibleName(QApplication::translate("BMLCreator", "loop", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_158->setText(QApplication::translate("BMLCreator", "loop", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        sbmStatesSbmStartNow->setAccessibleName(QApplication::translate("BMLCreator", "sbm:startnow", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_159->setText(QApplication::translate("BMLCreator", "sbm:startnow", 0, QApplication::UnicodeUTF8));
        postureLabel_160->setText(QApplication::translate("BMLCreator", "x", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        sbmStatesX->setAccessibleName(QApplication::translate("BMLCreator", "x", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_161->setText(QApplication::translate("BMLCreator", "y", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        sbmStatesY->setAccessibleName(QApplication::translate("BMLCreator", "y", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_162->setText(QApplication::translate("BMLCreator", "z", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        sbmStatesZ->setAccessibleName(QApplication::translate("BMLCreator", "z", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        postureLabel_163->setText(QApplication::translate("BMLCreator", "id", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_ACCESSIBILITY
        sbmStatesId->setAccessibleName(QApplication::translate("BMLCreator", "id", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_ACCESSIBILITY
        tabWidget->setTabText(tabWidget->indexOf(sbmstatesTab), QApplication::translate("BMLCreator", "sbm:states", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class BMLCreator: public Ui_BMLCreator {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BMLCREATORDIALOG_H
