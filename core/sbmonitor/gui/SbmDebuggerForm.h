#ifndef SBM_DEBUGGER_FORM_H_
#define SBM_DEBUGGER_FORM_H_

#include "QtCrtDbgOff.h"
#include <QtGui>
#include <QWidget>
#include "QtCrtDbgOn.h"

#include "ConnectDialog.h"
#include "GLWidget.h"
#include "ui_SbmDebuggerForm.h"
#include "ui_ConnectDialog.h"
#include "UtilsDialog.h"

class SbmDebuggerForm : public QMainWindow
 {
     Q_OBJECT

 public:
     SbmDebuggerForm(QWidget *parent = 0);
     ~SbmDebuggerForm();
     QSize sizeHint() const;
     static QTreeWidgetItem* FindTreeWidgetItemByName(const QTreeWidgetItem* subTree, const std::string& name);
     static Pawn* FindSbmEntityFromTreeSelection(const QTreeWidgetItem* treeWidget, Scene* pScene);
     GLWidget* GetGLWidget() { return m_pGLWidget; }
     UtilsDialog* m_Utils;
     void EmitClose();
     

 private slots:
       void ShowConnectDialog();
       void ConnectToFirstFoundProcess();
       void ConnectionStatusButtonPressed();
       void ShowSettingsDialog();
       void ShowResourceDialog();
       void ShowCommandDialog();
       void ShowDataViewerDialog();
       void ShowFaceDialog();
       void ShowBmlCreatorDialog();
       void ShowEntityCreatorDialog();
       void Disconnect();
       void sceneTreeItemChanged(QTreeWidgetItem * current, QTreeWidgetItem * previous);
       void SetSelectedSceneTreeItem(const Pawn* selectedObj, const Joint* selectedJoint);

 private:
     Ui::MainWindow ui;
     QMainWindow* m_pMainWindow;
     GLWidget* m_pGLWidget;
     std::string GetFpsAsString();
     QTreeWidgetItem* m_pSelectedSceneJointItem;
     Joint* m_pSelectedSceneJoint;
     QMainWindow* MainWindow() { return m_pMainWindow; }

     double m_msSinceLastFrame;
     double m_msSinceLastFramePrev;

     QBasicTimer timer;
     vhcl::Timer m_StopWatch;

 protected:
    enum SceneTreeIndex
    {
      Characters,
      Pawns,
    };

    enum SceneTreeColumns
    {
      Entity,
      Position,
      Rotation,
    };

    void ConnectToSbmProcess(string processId);
    void InitSignalsSlots();
    void Update();
    void UpdateSceneTree();
    void UpdateLabels();
    void closeEvent(QCloseEvent *event);
    void AddJointToSceneTree(QTreeWidgetItem* parent, const Joint* joint);
    static void EmitClose(void* form);

    virtual void timerEvent(QTimerEvent * event);

    static void VHMsgCallback( const char * op, const char * args, void * userData );
 };

#endif
