/********************************************************************************
** Form generated from reading UI file 'SbmDebuggerForm.ui'
**
** Created: Tue May 29 15:16:50 2012
**      by: Qt User Interface Compiler version 4.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SBMDEBUGGERFORM_H
#define UI_SBMDEBUGGERFORM_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDockWidget>
#include <QtGui/QFrame>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QPlainTextEdit>
#include <QtGui/QPushButton>
#include <QtGui/QScrollArea>
#include <QtGui/QStatusBar>
#include <QtGui/QTabWidget>
#include <QtGui/QToolBar>
#include <QtGui/QTreeWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *actionConnect;
    QAction *actionToggleFreeLookCamera;
    QAction *actionSettings;
    QAction *actionDisconnect;
    QAction *actionExit;
    QAction *actionResource_Viewer;
    QAction *actionCommand_Window;
    QAction *actionData_Viewer;
    QAction *actionUtils_Window;
    QAction *actionFace_Viewer;
    QAction *actionBML_Creator;
    QAction *actionEnity_Creator;
    QAction *actionConnect_Single;
    QAction *actionConnectionStatus;
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout_3;
    QWidget *RenderView;
    QHBoxLayout *horizontalLayout_2;
    QMenuBar *menubar;
    QMenu *menuFile;
    QMenu *menuEdit;
    QMenu *menuView;
    QMenu *menuSbm;
    QStatusBar *statusbar;
    QToolBar *toolBar;
    QDockWidget *sceneDockWidget;
    QWidget *dockWidgetContents;
    QVBoxLayout *verticalLayout;
    QTabWidget *sceneTab;
    QWidget *tab;
    QHBoxLayout *horizontalLayout_4;
    QTreeWidget *sceneTree;
    QWidget *statisticsTab;
    QLabel *cameraPositionLabel;
    QLabel *networkFpsLabel;
    QLabel *rendererFpsLabel;
    QWidget *utilsTab;
    QVBoxLayout *verticalLayout_2;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QPushButton *refreshButton;
    QLabel *selectedCharacterLabel;
    QComboBox *selectedCharacterBox;
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
    QFrame *frame_4;
    QPushButton *GazeAtButton;
    QLabel *gazeTargetLabel;
    QComboBox *gazeTargetBox;
    QFrame *line_5;
    QLabel *controlLabel;
    QPushButton *StopGazeButton;
    QFrame *frame;
    QComboBox *bmlFilesBox;
    QLabel *ttsLabel;
    QLabel *bmlFilesLabel;
    QPlainTextEdit *ttsBox;
    QPushButton *SpeakButton;
    QFrame *line_2;
    QLabel *speechLabel;
    QPushButton *runBmlButton;
    QFrame *frame_3;
    QLabel *renderingLabel;
    QCheckBox *showAxesBox;
    QFrame *line_3;
    QCheckBox *showEyeBeamsBox;
    QCheckBox *allowBoneUpdatesBox;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1264, 868);
        MainWindow->setBaseSize(QSize(1274, 830));
        actionConnect = new QAction(MainWindow);
        actionConnect->setObjectName(QString::fromUtf8("actionConnect"));
        actionToggleFreeLookCamera = new QAction(MainWindow);
        actionToggleFreeLookCamera->setObjectName(QString::fromUtf8("actionToggleFreeLookCamera"));
        QIcon icon;
        icon.addFile(QString::fromUtf8("images/monkey_on_128x128.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionToggleFreeLookCamera->setIcon(icon);
        actionSettings = new QAction(MainWindow);
        actionSettings->setObjectName(QString::fromUtf8("actionSettings"));
        actionDisconnect = new QAction(MainWindow);
        actionDisconnect->setObjectName(QString::fromUtf8("actionDisconnect"));
        actionDisconnect->setEnabled(false);
        actionExit = new QAction(MainWindow);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        actionResource_Viewer = new QAction(MainWindow);
        actionResource_Viewer->setObjectName(QString::fromUtf8("actionResource_Viewer"));
        actionCommand_Window = new QAction(MainWindow);
        actionCommand_Window->setObjectName(QString::fromUtf8("actionCommand_Window"));
        actionData_Viewer = new QAction(MainWindow);
        actionData_Viewer->setObjectName(QString::fromUtf8("actionData_Viewer"));
        actionUtils_Window = new QAction(MainWindow);
        actionUtils_Window->setObjectName(QString::fromUtf8("actionUtils_Window"));
        actionFace_Viewer = new QAction(MainWindow);
        actionFace_Viewer->setObjectName(QString::fromUtf8("actionFace_Viewer"));
        actionBML_Creator = new QAction(MainWindow);
        actionBML_Creator->setObjectName(QString::fromUtf8("actionBML_Creator"));
        actionEnity_Creator = new QAction(MainWindow);
        actionEnity_Creator->setObjectName(QString::fromUtf8("actionEnity_Creator"));
        actionConnect_Single = new QAction(MainWindow);
        actionConnect_Single->setObjectName(QString::fromUtf8("actionConnect_Single"));
        actionConnectionStatus = new QAction(MainWindow);
        actionConnectionStatus->setObjectName(QString::fromUtf8("actionConnectionStatus"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8("../../images/ConnectionStatus_Disconnected.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionConnectionStatus->setIcon(icon1);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        horizontalLayout_3 = new QHBoxLayout(centralwidget);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        RenderView = new QWidget(centralwidget);
        RenderView->setObjectName(QString::fromUtf8("RenderView"));
        RenderView->setMinimumSize(QSize(500, 500));
        RenderView->setMaximumSize(QSize(16777215, 16777215));
        horizontalLayout_2 = new QHBoxLayout(RenderView);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));

        horizontalLayout_3->addWidget(RenderView);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1264, 21));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        menuEdit = new QMenu(menubar);
        menuEdit->setObjectName(QString::fromUtf8("menuEdit"));
        menuView = new QMenu(menubar);
        menuView->setObjectName(QString::fromUtf8("menuView"));
        menuSbm = new QMenu(menubar);
        menuSbm->setObjectName(QString::fromUtf8("menuSbm"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        MainWindow->addToolBar(Qt::LeftToolBarArea, toolBar);
        sceneDockWidget = new QDockWidget(MainWindow);
        sceneDockWidget->setObjectName(QString::fromUtf8("sceneDockWidget"));
        sceneDockWidget->setMaximumSize(QSize(524287, 524287));
        sceneDockWidget->setLayoutDirection(Qt::LeftToRight);
        dockWidgetContents = new QWidget();
        dockWidgetContents->setObjectName(QString::fromUtf8("dockWidgetContents"));
        verticalLayout = new QVBoxLayout(dockWidgetContents);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        sceneTab = new QTabWidget(dockWidgetContents);
        sceneTab->setObjectName(QString::fromUtf8("sceneTab"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(sceneTab->sizePolicy().hasHeightForWidth());
        sceneTab->setSizePolicy(sizePolicy);
        sceneTab->setMinimumSize(QSize(0, 0));
        sceneTab->setMaximumSize(QSize(16777215, 16777215));
        sceneTab->setLayoutDirection(Qt::LeftToRight);
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        horizontalLayout_4 = new QHBoxLayout(tab);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        sceneTree = new QTreeWidget(tab);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(2, QString::fromUtf8("3"));
        __qtreewidgetitem->setText(1, QString::fromUtf8("2"));
        __qtreewidgetitem->setText(0, QString::fromUtf8("1"));
        sceneTree->setHeaderItem(__qtreewidgetitem);
        sceneTree->setObjectName(QString::fromUtf8("sceneTree"));
        sizePolicy.setHeightForWidth(sceneTree->sizePolicy().hasHeightForWidth());
        sceneTree->setSizePolicy(sizePolicy);
        sceneTree->setMinimumSize(QSize(0, 0));
        sceneTree->setMaximumSize(QSize(16777215, 16777215));
        sceneTree->setLayoutDirection(Qt::LeftToRight);
        sceneTree->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        sceneTree->setEditTriggers(QAbstractItemView::DoubleClicked|QAbstractItemView::EditKeyPressed);
        sceneTree->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
        sceneTree->setRootIsDecorated(true);
        sceneTree->setColumnCount(3);
        sceneTree->header()->setVisible(false);
        sceneTree->header()->setCascadingSectionResizes(false);
        sceneTree->header()->setDefaultSectionSize(300);

        horizontalLayout_4->addWidget(sceneTree);

        sceneTab->addTab(tab, QString());
        statisticsTab = new QWidget();
        statisticsTab->setObjectName(QString::fromUtf8("statisticsTab"));
        cameraPositionLabel = new QLabel(statisticsTab);
        cameraPositionLabel->setObjectName(QString::fromUtf8("cameraPositionLabel"));
        cameraPositionLabel->setGeometry(QRect(10, 50, 211, 31));
        networkFpsLabel = new QLabel(statisticsTab);
        networkFpsLabel->setObjectName(QString::fromUtf8("networkFpsLabel"));
        networkFpsLabel->setGeometry(QRect(10, 30, 121, 16));
        rendererFpsLabel = new QLabel(statisticsTab);
        rendererFpsLabel->setObjectName(QString::fromUtf8("rendererFpsLabel"));
        rendererFpsLabel->setEnabled(true);
        rendererFpsLabel->setGeometry(QRect(10, 10, 121, 16));
        sceneTab->addTab(statisticsTab, QString());
        utilsTab = new QWidget();
        utilsTab->setObjectName(QString::fromUtf8("utilsTab"));
        verticalLayout_2 = new QVBoxLayout(utilsTab);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        scrollArea = new QScrollArea(utilsTab);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        scrollArea->setWidgetResizable(false);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 479, 719));
        refreshButton = new QPushButton(scrollAreaWidgetContents);
        refreshButton->setObjectName(QString::fromUtf8("refreshButton"));
        refreshButton->setGeometry(QRect(160, 30, 61, 23));
        selectedCharacterLabel = new QLabel(scrollAreaWidgetContents);
        selectedCharacterLabel->setObjectName(QString::fromUtf8("selectedCharacterLabel"));
        selectedCharacterLabel->setGeometry(QRect(20, 10, 141, 16));
        QFont font;
        font.setPointSize(12);
        selectedCharacterLabel->setFont(font);
        selectedCharacterBox = new QComboBox(scrollAreaWidgetContents);
        selectedCharacterBox->setObjectName(QString::fromUtf8("selectedCharacterBox"));
        selectedCharacterBox->setGeometry(QRect(20, 30, 131, 22));
        frame_2 = new QFrame(scrollAreaWidgetContents);
        frame_2->setObjectName(QString::fromUtf8("frame_2"));
        frame_2->setGeometry(QRect(10, 60, 351, 221));
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
        animationNamesBox->setGeometry(QRect(10, 120, 291, 22));
        animationsLabel = new QLabel(frame_2);
        animationsLabel->setObjectName(QString::fromUtf8("animationsLabel"));
        animationsLabel->setGeometry(QRect(10, 10, 81, 16));
        animationsLabel->setFont(font);
        animFilterBox = new QPlainTextEdit(frame_2);
        animFilterBox->setObjectName(QString::fromUtf8("animFilterBox"));
        animFilterBox->setGeometry(QRect(10, 60, 291, 31));
        filterLabel = new QLabel(frame_2);
        filterLabel->setObjectName(QString::fromUtf8("filterLabel"));
        filterLabel->setGeometry(QRect(10, 40, 71, 16));
        PlayAnimButton = new QPushButton(frame_2);
        PlayAnimButton->setObjectName(QString::fromUtf8("PlayAnimButton"));
        PlayAnimButton->setGeometry(QRect(10, 150, 71, 23));
        SetPostureButton = new QPushButton(frame_2);
        SetPostureButton->setObjectName(QString::fromUtf8("SetPostureButton"));
        SetPostureButton->setGeometry(QRect(90, 150, 71, 23));
        frame_4 = new QFrame(scrollAreaWidgetContents);
        frame_4->setObjectName(QString::fromUtf8("frame_4"));
        frame_4->setGeometry(QRect(10, 290, 351, 111));
        frame_4->setFrameShape(QFrame::StyledPanel);
        frame_4->setFrameShadow(QFrame::Raised);
        GazeAtButton = new QPushButton(frame_4);
        GazeAtButton->setObjectName(QString::fromUtf8("GazeAtButton"));
        GazeAtButton->setGeometry(QRect(10, 80, 61, 23));
        gazeTargetLabel = new QLabel(frame_4);
        gazeTargetLabel->setObjectName(QString::fromUtf8("gazeTargetLabel"));
        gazeTargetLabel->setGeometry(QRect(10, 30, 61, 16));
        gazeTargetBox = new QComboBox(frame_4);
        gazeTargetBox->setObjectName(QString::fromUtf8("gazeTargetBox"));
        gazeTargetBox->setGeometry(QRect(10, 50, 131, 22));
        line_5 = new QFrame(frame_4);
        line_5->setObjectName(QString::fromUtf8("line_5"));
        line_5->setGeometry(QRect(10, 20, 118, 3));
        line_5->setLineWidth(1);
        line_5->setFrameShape(QFrame::HLine);
        line_5->setFrameShadow(QFrame::Sunken);
        controlLabel = new QLabel(frame_4);
        controlLabel->setObjectName(QString::fromUtf8("controlLabel"));
        controlLabel->setGeometry(QRect(10, 0, 71, 16));
        controlLabel->setFont(font);
        StopGazeButton = new QPushButton(frame_4);
        StopGazeButton->setObjectName(QString::fromUtf8("StopGazeButton"));
        StopGazeButton->setGeometry(QRect(80, 80, 61, 23));
        frame = new QFrame(scrollAreaWidgetContents);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setGeometry(QRect(10, 410, 351, 221));
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
        speechLabel->setFont(font);
        runBmlButton = new QPushButton(frame);
        runBmlButton->setObjectName(QString::fromUtf8("runBmlButton"));
        runBmlButton->setGeometry(QRect(10, 190, 61, 23));
        frame_3 = new QFrame(scrollAreaWidgetContents);
        frame_3->setObjectName(QString::fromUtf8("frame_3"));
        frame_3->setGeometry(QRect(10, 640, 351, 91));
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
        allowBoneUpdatesBox->setGeometry(QRect(130, 30, 121, 17));
        scrollArea->setWidget(scrollAreaWidgetContents);

        verticalLayout_2->addWidget(scrollArea);

        sceneTab->addTab(utilsTab, QString());

        verticalLayout->addWidget(sceneTab);

        sceneDockWidget->setWidget(dockWidgetContents);
        MainWindow->addDockWidget(static_cast<Qt::DockWidgetArea>(2), sceneDockWidget);

        menubar->addAction(menuFile->menuAction());
        menubar->addAction(menuEdit->menuAction());
        menubar->addAction(menuView->menuAction());
        menubar->addAction(menuSbm->menuAction());
        menuFile->addAction(actionConnect);
        menuFile->addAction(actionConnect_Single);
        menuFile->addAction(actionDisconnect);
        menuFile->addAction(actionSettings);
        menuFile->addAction(actionExit);
        menuSbm->addAction(actionData_Viewer);
        menuSbm->addAction(actionResource_Viewer);
        menuSbm->addAction(actionCommand_Window);
        menuSbm->addAction(actionUtils_Window);
        menuSbm->addAction(actionFace_Viewer);
        menuSbm->addAction(actionBML_Creator);
        menuSbm->addAction(actionEnity_Creator);
        toolBar->addAction(actionConnectionStatus);
        toolBar->addAction(actionToggleFreeLookCamera);

        retranslateUi(MainWindow);

        sceneTab->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "Smartbody Debugger", 0, QApplication::UnicodeUTF8));
        actionConnect->setText(QApplication::translate("MainWindow", "&Connect", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionConnect->setToolTip(QApplication::translate("MainWindow", "Connect to a Smartbody Process", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionConnect->setShortcut(QApplication::translate("MainWindow", "Ctrl+C", 0, QApplication::UnicodeUTF8));
        actionToggleFreeLookCamera->setText(QApplication::translate("MainWindow", "Toggle Free Look", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionToggleFreeLookCamera->setToolTip(QApplication::translate("MainWindow", "Toggle Free Look", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionSettings->setText(QApplication::translate("MainWindow", "&Settings", 0, QApplication::UnicodeUTF8));
        actionSettings->setShortcut(QApplication::translate("MainWindow", "Ctrl+S", 0, QApplication::UnicodeUTF8));
        actionDisconnect->setText(QApplication::translate("MainWindow", "&Disconnect", 0, QApplication::UnicodeUTF8));
        actionDisconnect->setShortcut(QApplication::translate("MainWindow", "Ctrl+D", 0, QApplication::UnicodeUTF8));
        actionExit->setText(QApplication::translate("MainWindow", "E&xit", 0, QApplication::UnicodeUTF8));
        actionExit->setShortcut(QApplication::translate("MainWindow", "Ctrl+Q", 0, QApplication::UnicodeUTF8));
        actionResource_Viewer->setText(QApplication::translate("MainWindow", "&Resource Viewer", 0, QApplication::UnicodeUTF8));
        actionResource_Viewer->setShortcut(QApplication::translate("MainWindow", "Ctrl+R", 0, QApplication::UnicodeUTF8));
        actionCommand_Window->setText(QApplication::translate("MainWindow", "Command Window", 0, QApplication::UnicodeUTF8));
        actionCommand_Window->setShortcut(QApplication::translate("MainWindow", "Ctrl+M", 0, QApplication::UnicodeUTF8));
        actionData_Viewer->setText(QApplication::translate("MainWindow", "D&ata Viewer", 0, QApplication::UnicodeUTF8));
        actionData_Viewer->setShortcut(QApplication::translate("MainWindow", "Ctrl+A", 0, QApplication::UnicodeUTF8));
        actionUtils_Window->setText(QApplication::translate("MainWindow", "U&tils Window", 0, QApplication::UnicodeUTF8));
        actionUtils_Window->setShortcut(QApplication::translate("MainWindow", "Ctrl+T", 0, QApplication::UnicodeUTF8));
        actionFace_Viewer->setText(QApplication::translate("MainWindow", "Face Viewer", 0, QApplication::UnicodeUTF8));
        actionFace_Viewer->setShortcut(QApplication::translate("MainWindow", "Ctrl+F", 0, QApplication::UnicodeUTF8));
        actionBML_Creator->setText(QApplication::translate("MainWindow", "&BML Creator", 0, QApplication::UnicodeUTF8));
        actionBML_Creator->setShortcut(QApplication::translate("MainWindow", "Ctrl+B", 0, QApplication::UnicodeUTF8));
        actionEnity_Creator->setText(QApplication::translate("MainWindow", "Entity Creator", 0, QApplication::UnicodeUTF8));
        actionEnity_Creator->setShortcut(QApplication::translate("MainWindow", "Ctrl+E", 0, QApplication::UnicodeUTF8));
        actionConnect_Single->setText(QApplication::translate("MainWindow", "Connect S&ingle", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionConnect_Single->setToolTip(QApplication::translate("MainWindow", "Connect to first sbm process found", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        actionConnect_Single->setShortcut(QApplication::translate("MainWindow", "Ctrl+I", 0, QApplication::UnicodeUTF8));
        actionConnectionStatus->setText(QApplication::translate("MainWindow", "Connection Status", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionConnectionStatus->setToolTip(QApplication::translate("MainWindow", "Disconnected", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        menuFile->setTitle(QApplication::translate("MainWindow", "File", 0, QApplication::UnicodeUTF8));
        menuEdit->setTitle(QApplication::translate("MainWindow", "Edit", 0, QApplication::UnicodeUTF8));
        menuView->setTitle(QApplication::translate("MainWindow", "View", 0, QApplication::UnicodeUTF8));
        menuSbm->setTitle(QApplication::translate("MainWindow", "Tools", 0, QApplication::UnicodeUTF8));
        toolBar->setWindowTitle(QApplication::translate("MainWindow", "toolBar", 0, QApplication::UnicodeUTF8));
        sceneTab->setTabText(sceneTab->indexOf(tab), QApplication::translate("MainWindow", "Scene", 0, QApplication::UnicodeUTF8));
        cameraPositionLabel->setText(QApplication::translate("MainWindow", "Camera Pos:", 0, QApplication::UnicodeUTF8));
        networkFpsLabel->setText(QApplication::translate("MainWindow", "Network Fps:", 0, QApplication::UnicodeUTF8));
        rendererFpsLabel->setText(QApplication::translate("MainWindow", "Renderer Fps:", 0, QApplication::UnicodeUTF8));
        sceneTab->setTabText(sceneTab->indexOf(statisticsTab), QApplication::translate("MainWindow", "Statistics", 0, QApplication::UnicodeUTF8));
        refreshButton->setText(QApplication::translate("MainWindow", "Refresh", 0, QApplication::UnicodeUTF8));
        selectedCharacterLabel->setText(QApplication::translate("MainWindow", "Selected Character", 0, QApplication::UnicodeUTF8));
        animationNamesLabel->setText(QApplication::translate("MainWindow", "Animation Names", 0, QApplication::UnicodeUTF8));
        QueryAnimButton->setText(QApplication::translate("MainWindow", "Query Anims", 0, QApplication::UnicodeUTF8));
        animationsLabel->setText(QApplication::translate("MainWindow", "Animations", 0, QApplication::UnicodeUTF8));
        filterLabel->setText(QApplication::translate("MainWindow", "Search Filter", 0, QApplication::UnicodeUTF8));
        PlayAnimButton->setText(QApplication::translate("MainWindow", "Play", 0, QApplication::UnicodeUTF8));
        SetPostureButton->setText(QApplication::translate("MainWindow", "Set Posture", 0, QApplication::UnicodeUTF8));
        GazeAtButton->setText(QApplication::translate("MainWindow", "Gaze At", 0, QApplication::UnicodeUTF8));
        gazeTargetLabel->setText(QApplication::translate("MainWindow", "Gaze Target", 0, QApplication::UnicodeUTF8));
        controlLabel->setText(QApplication::translate("MainWindow", "Control", 0, QApplication::UnicodeUTF8));
        StopGazeButton->setText(QApplication::translate("MainWindow", "Stop Gaze", 0, QApplication::UnicodeUTF8));
        ttsLabel->setText(QApplication::translate("MainWindow", "Text to Speech", 0, QApplication::UnicodeUTF8));
        bmlFilesLabel->setText(QApplication::translate("MainWindow", "BML Files", 0, QApplication::UnicodeUTF8));
        SpeakButton->setText(QApplication::translate("MainWindow", "Speak", 0, QApplication::UnicodeUTF8));
        speechLabel->setText(QApplication::translate("MainWindow", "Speech", 0, QApplication::UnicodeUTF8));
        runBmlButton->setText(QApplication::translate("MainWindow", "Run", 0, QApplication::UnicodeUTF8));
        renderingLabel->setText(QApplication::translate("MainWindow", "Rendering", 0, QApplication::UnicodeUTF8));
        showAxesBox->setText(QApplication::translate("MainWindow", "Show Axes", 0, QApplication::UnicodeUTF8));
        showEyeBeamsBox->setText(QApplication::translate("MainWindow", "Show Eye Beams", 0, QApplication::UnicodeUTF8));
        allowBoneUpdatesBox->setText(QApplication::translate("MainWindow", "Allow Bone Updates", 0, QApplication::UnicodeUTF8));
        sceneTab->setTabText(sceneTab->indexOf(utilsTab), QApplication::translate("MainWindow", "Utils", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SBMDEBUGGERFORM_H
