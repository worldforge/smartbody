
#include "vhcl.h"
#include "SbmDebuggerForm.h"

#include "vhmsg-tt.h"
#include "SBDebuggerClient.h"

#include "SettingsDialog.h"
#include "ResourceDialog.h"
#include "CommandDialog.h"
#include "DataViewerDialog.h"
#include "UtilsDialog.h"
#include "FaceDialog.h"
#include "BmlCreatorDialog.h"
#include "EntityCreatorDialog.h"

using std::vector;
using std::string;


SBDebuggerClient c;

bool GetSceneScale(void* caller, NetRequest* req);


SbmDebuggerForm::SbmDebuggerForm(QWidget *parent)
  : QMainWindow(parent)
{
  m_pSelectedSceneJointItem = NULL;
  m_pSelectedSceneJoint = NULL;

  m_pMainWindow = this;
  ui.setupUi(MainWindow());
  MainWindow()->show();
  m_pGLWidget = new GLWidget(c.GetScene(), this);

  c.AddOnCloseCallback(SbmDebuggerForm::EmitClose, this);

  ui.actionConnectionStatus->setIcon(QIcon("Images/ConnectionStatus_Disconnected.png"));

  // setup renderer size and positioning
  QPoint rendererPosition = ui.RenderView->pos();
  QSize rendererSize = ui.RenderView->size();
  m_pGLWidget->setGeometry(ui.RenderView->x() /*+ 40*/, ui.RenderView->y() + 25,
     rendererSize.width(), rendererSize.height());
  MainWindow()->setCentralWidget(m_pGLWidget); 
  
  // set vhmsg
  vhmsg::ttu_set_client_callback(VHMsgCallback);
  vhmsg::ttu_open();
  vhmsg::ttu_register("sbmdebugger");
  vhmsg::ttu_register("sbmlog");
  vhmsg::ttu_register("vrAllCall");
  vhmsg::ttu_register("vrKillComponent");
  vhmsg::ttu_register("vrKillAll");
  vhmsg::ttu_notify1("vrComponent sbmonitor");

  // setup scene tree
  ui.sceneDockWidget->setBaseSize(220, 767);
  ui.sceneTree->insertTopLevelItem(Characters, new QTreeWidgetItem(ui.sceneTree, QStringList(QString("Characters"))));
  ui.sceneTree->insertTopLevelItem(Pawns, new QTreeWidgetItem(ui.sceneTree, QStringList(QString("Pawns"))));

  QStringList headers;
  headers.append("Entities");
  headers.append("Position");
  headers.append("Rotation");
  ui.sceneTree->setHeaderLabels(headers);

  ui.sceneTree->setColumnWidth(Entity, 400);
  ui.sceneTree->setColumnWidth(Position, 170);
  ui.sceneTree->setColumnWidth(Rotation, 190);

  InitSignalsSlots();

  setUpdatesEnabled(true);

  m_msSinceLastFrame = m_StopWatch.GetTime();

  m_Utils = new UtilsDialog(&c, m_pGLWidget, ui);
}

SbmDebuggerForm::~SbmDebuggerForm()
{
   if (m_pGLWidget)
   {
      delete m_pGLWidget;
      m_pGLWidget = NULL;
   }

   if (m_Utils)
   {
      delete m_Utils;
      m_Utils = NULL;
   }

   SbmDebuggerForm::EmitClose(this);
   vhmsg::ttu_notify1("vrProcEnd sbmonitor");
   vhmsg::ttu_close();
}

void SbmDebuggerForm::EmitClose(void* form)
{
   SbmDebuggerForm* pForm = static_cast<SbmDebuggerForm*>(form);
   pForm->EmitClose();
}

void SbmDebuggerForm::EmitClose()
{
   emit close();
}

QSize SbmDebuggerForm::sizeHint() const
{
   return QSize(1274, 830);
}

void SbmDebuggerForm::InitSignalsSlots()
{
   // File Menu
   connect(ui.actionConnect, SIGNAL(triggered()), this, SLOT(ShowConnectDialog()));
   connect(ui.actionConnect_Single, SIGNAL(triggered()), this, SLOT(ConnectToFirstFoundProcess()));
   connect(ui.actionDisconnect, SIGNAL(triggered()), this, SLOT(Disconnect()));
   connect(ui.actionSettings, SIGNAL(triggered()), this, SLOT(ShowSettingsDialog()));
   connect(ui.actionExit, SIGNAL(triggered()), MainWindow(), SLOT(close()));
   connect(ui.actionConnectionStatus, SIGNAL(triggered()), this, SLOT(ConnectionStatusButtonPressed()));

   // Tool bar
   connect(ui.actionToggleFreeLookCamera, SIGNAL(triggered()), m_pGLWidget, SLOT(ToggleFreeLook()));

   // Sbm Menu
   connect(ui.actionResource_Viewer, SIGNAL(triggered()), this, SLOT(ShowResourceDialog()));
   connect(ui.actionCommand_Window, SIGNAL(triggered()), this, SLOT(ShowCommandDialog()));
   connect(ui.actionData_Viewer, SIGNAL(triggered()), this, SLOT(ShowDataViewerDialog()));
   connect(ui.actionFace_Viewer, SIGNAL(triggered()), this, SLOT(ShowFaceDialog()));
   connect(ui.actionBML_Creator, SIGNAL(triggered()), this, SLOT(ShowBmlCreatorDialog()));
   connect(ui.actionEnity_Creator, SIGNAL(triggered()), this, SLOT(ShowEntityCreatorDialog()));

   // Scene Tree
   //selection changes shall trigger a slot
   connect(ui.sceneTree, SIGNAL(currentItemChanged (QTreeWidgetItem*, QTreeWidgetItem *)),
             m_pGLWidget, SLOT(sceneTreeCurrentItemChanged(QTreeWidgetItem*, QTreeWidgetItem *)));
   connect(ui.sceneTree, SIGNAL(itemDoubleClicked (QTreeWidgetItem*, int)),
             m_pGLWidget, SLOT(sceneTreeItemDoubleClicked(QTreeWidgetItem*, int)));
   connect(ui.sceneTree, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
             this, SLOT(sceneTreeItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)));

   // Renderer
   connect(m_pGLWidget, SIGNAL(JointPicked(const Pawn*, const Joint*)),
      this, SLOT(SetSelectedSceneTreeItem(const Pawn*, const Joint*)));

   timer.start(10, this);
}

void SbmDebuggerForm::ShowConnectDialog()
{
   ConnectDialog dlg(this); 
   if (dlg.exec() == QDialog::Accepted)
   {
      // they hit ok to connect to a renderer
      QListWidget * listWidget = dlg.GetListWidget();
      if (listWidget->currentItem())
      {
         string sbmId = listWidget->currentItem()->text().toStdString();
         ConnectToSbmProcess(sbmId);
      }
   }
}

void SbmDebuggerForm::ConnectToFirstFoundProcess()
{
   c.QuerySbmProcessIds();
   vhcl::Sleep(2);
   vhmsg::ttu_poll();
   vector<string> ids = c.GetSbmProcessIds();

   if (ids.size() == 0)
   {
      // no sbm processes found
      QMessageBox box(QMessageBox::Warning, "No Sbm Processes Found", "There aren't any active running Smartbody processes", QMessageBox::NoButton, this);
      box.exec();
   }
   else
   {
      ConnectToSbmProcess(ids[0]);
   }
}

void SbmDebuggerForm::ConnectionStatusButtonPressed()
{
   if (ui.actionDisconnect->isEnabled())
   {
      // you're connected to a process
      Disconnect();
   }
   else
   {
      ConnectToFirstFoundProcess();
   }
}

void SbmDebuggerForm::ConnectToSbmProcess(string processId)
{
   // disconnect from any previous process
   Disconnect();

   c.Connect(processId);
   vhcl::Sleep(2);
   vhmsg::ttu_poll();
   if (c.GetConnectResult())
   {
      printf("Connect succeeded to id: %s\n", processId.c_str());
   }

   c.Init();

   c.StartUpdates(0.01f);

   // since you're connected, enable the disconnect button
   ui.actionDisconnect->setEnabled(true);
   ui.actionConnectionStatus->setToolTip(vhcl::Format("Connected to %s", processId.c_str()).c_str());

   ui.actionConnectionStatus->setIcon(QIcon("Images/ConnectionStatus_Connected.png"));

   c.SendSBMCommand(NetRequest::Get_Scene_Scale, "float", "scene.getScale()", GetSceneScale, this);
}

void SbmDebuggerForm::ShowSettingsDialog()
{
   SettingsDialog dlg(this);
   connect(&dlg, SIGNAL(DialogFinished(const SettingsDialog*, int)), m_pGLWidget, SLOT(OnCloseSettingsDialog(const SettingsDialog*, int)));
   dlg.exec();
}

void SbmDebuggerForm::ShowResourceDialog()
{
   ResourceDialog dlg(&c, this);
   dlg.exec();
}

void SbmDebuggerForm::ShowCommandDialog()
{
   CommandDialog dlg(&c, this);
   dlg.exec();
}

void SbmDebuggerForm::ShowDataViewerDialog()
{
   DataViewerDialog dlg(&c, this);
   dlg.exec();
}

void SbmDebuggerForm::ShowFaceDialog()
{
   FaceDialog dlg(&c, this);
   dlg.exec();
}

void SbmDebuggerForm::ShowBmlCreatorDialog()
{
   BmlCreatorDialog dlg(&c, this);
   dlg.exec();
}

void SbmDebuggerForm::ShowEntityCreatorDialog()
{
   EntityCreatorDialog dlg(&c, this);
   dlg.exec();
}

void SbmDebuggerForm::Disconnect()
{
   ui.actionDisconnect->setEnabled(false);

   ui.sceneTree->clear();
   ui.sceneTree->insertTopLevelItem(Characters, new QTreeWidgetItem(ui.sceneTree, QStringList(QString("Characters"))));
   ui.sceneTree->insertTopLevelItem(Pawns, new QTreeWidgetItem(ui.sceneTree, QStringList(QString("Pawns"))));
   m_pSelectedSceneJointItem = NULL;

   ui.actionConnectionStatus->setIcon(QIcon("Images/ConnectionStatus_Disconnected.png"));
   ui.actionConnectionStatus->setToolTip("Disconnected");
   c.Disconnect();
}

void SbmDebuggerForm::sceneTreeItemChanged(QTreeWidgetItem * current, QTreeWidgetItem * previous)
{
   // clear out the unselected data
   if (previous)
   {
      previous->setText(Position, "");
      previous->setText(Rotation, "");
   } 

   if (current)
   {
      m_pSelectedSceneJointItem = current;
      Pawn* entity = FindSbmEntityFromTreeSelection(current, c.GetScene());
      if (entity)
      {
         Joint* selectedJoint = entity->FindJoint(current->text(Entity).toStdString());
         if (selectedJoint)
         {
            current->setText(Position, selectedJoint->GetPositionAsString(false).c_str());
            current->setText(Rotation, selectedJoint->GetRotationAsString(false).c_str());
            m_pSelectedSceneJoint = selectedJoint;
         }
      }
   }
}

void SbmDebuggerForm::SetSelectedSceneTreeItem(const Pawn* selectedObj, const Joint* selectedJoint)
{
   if (!selectedObj)
      return;

   m_pSelectedSceneJoint = const_cast<Joint*>(selectedJoint);

   // check if it's a pawn or a character so we search in the correct tree sub-section
   QTreeWidgetItem* subTree = ui.sceneTree->topLevelItem(dynamic_cast<const Character*>(selectedObj) ? Characters : Pawns);
   QTreeWidgetItem* rootNameItem = FindTreeWidgetItemByName(subTree, selectedObj->m_name.c_str());
   if (rootNameItem && selectedJoint)
   {
      // try to find the joint that is selected under this specific character
      QTreeWidgetItem* jointWidget = FindTreeWidgetItemByName(rootNameItem, selectedJoint->m_name);
      if (jointWidget)
      {  
         // select the specific joint
         ui.sceneTree->setCurrentItem(jointWidget);
         m_pSelectedSceneJointItem = jointWidget;
      }
      else
      {
         // there is no joint, so just select the entity's name in the tree view
         ui.sceneTree->setCurrentItem(rootNameItem);
         m_pSelectedSceneJointItem = rootNameItem;
      }
   }
   else
   {
      // there is no joint, so just select the entity's name in the tree view
      ui.sceneTree->setCurrentItem(rootNameItem);
      m_pSelectedSceneJointItem = rootNameItem;
   }
}

QTreeWidgetItem* SbmDebuggerForm::FindTreeWidgetItemByName(const QTreeWidgetItem* subTree, const std::string& name)
{
   QTreeWidgetItem* retVal = NULL;
   for (int i = 0; i < subTree->childCount(); i++)
   {
      if (subTree->child(i)->text(0) == name.c_str())
      {
         return subTree->child(i);
      }

      retVal = FindTreeWidgetItemByName(subTree->child(i), name);
      if (retVal)
      {
         return retVal;
      }
   }

   return NULL;
}

Pawn* SbmDebuggerForm::FindSbmEntityFromTreeSelection(const QTreeWidgetItem* treeWidget, Scene* pScene)
{
   const QTreeWidgetItem* parent = treeWidget;
   while (parent)
   {
      for (unsigned int i = 0; i < pScene->m_characters.size(); i++)
      {
         if (parent->text(0).toStdString() == pScene->m_characters[i].m_name)
            return &pScene->m_characters[i];
      }
      for (unsigned int i = 0; i < pScene->m_pawns.size(); i++)
      {
         if (parent->text(0).toStdString() == pScene->m_pawns[i].m_name)
            return &pScene->m_pawns[i];
      }
      parent = parent->parent();
   }

   return NULL;
}

void SbmDebuggerForm::Update()
{
   int err = vhmsg::ttu_poll();
   if (err == vhmsg::TTU_ERROR)
   {
      printf("ttu_poll ERR\n");
   }

   c.Update();
   UpdateSceneTree();
   UpdateLabels();
}

void SbmDebuggerForm::AddJointToSceneTree(QTreeWidgetItem* parent, const Joint* joint)
{
   QTreeWidgetItem* newParent = new QTreeWidgetItem(parent, QStringList(QString(joint->m_name.c_str())));
   parent->addChild(newParent);

   for (unsigned int i = 0; i < joint->m_joints.size(); i++)
   {
      AddJointToSceneTree(newParent, joint->m_joints[i]);
   }
}

void SbmDebuggerForm::UpdateSceneTree()
{
   string entityName = "";

   // character tree
   for (unsigned int i = 0; i < c.GetScene()->m_characters.size(); i++)
   {
      entityName = c.GetScene()->m_characters[i].m_name;
      QTreeWidgetItem* subTree = ui.sceneTree->topLevelItem(Characters);
      bool alreadyExistsInTree = false;
      for (int j = 0; j < subTree->childCount(); j++)
      {
         if (subTree->child(j)->text(0).toStdString() == entityName)
         {
            alreadyExistsInTree = true;
            break;
         }       
      }

      if (!alreadyExistsInTree)
      {
         // add character name
         QTreeWidgetItem* characterTreeRoot = new QTreeWidgetItem(subTree, QStringList(QString(entityName.c_str())));
         subTree->addChild(characterTreeRoot);

         // the entity isn't represented in the tree view, add it
         for (unsigned int j = 0; j < c.GetScene()->m_characters[i].m_joints.size(); j++)
         {
            AddJointToSceneTree(characterTreeRoot, c.GetScene()->m_characters[i].m_joints[j]);
         }
      } 
   }

   // pawn tree
   for (unsigned int i = 0; i < c.GetScene()->m_pawns.size(); i++)
   {
      entityName = c.GetScene()->m_pawns[i].m_name;
      QTreeWidgetItem* subTree = ui.sceneTree->topLevelItem(Pawns);
      bool alreadyExistsInTree = false;
      for (int j = 0; j < subTree->childCount(); j++)
      {
         if (subTree->child(j)->text(0).toStdString() == entityName)
         {
            alreadyExistsInTree = true;
            break;
         }       
      }

      if (!alreadyExistsInTree)
      {
         // add pawn name
         QTreeWidgetItem* pawnTreeRoot = new QTreeWidgetItem(subTree, QStringList(QString(entityName.c_str())));
         subTree->addChild(pawnTreeRoot);

         // the entity isn't represented in the tree view, add it
         for (unsigned int j = 0; j < c.GetScene()->m_pawns[i].m_joints.size(); j++)
         {
            AddJointToSceneTree(pawnTreeRoot, c.GetScene()->m_pawns[i].m_joints[j]);
         }
       } 
   }

   if (m_pSelectedSceneJointItem && m_pSelectedSceneJoint)
   {
      m_pSelectedSceneJointItem->setText(Position, m_pSelectedSceneJoint->GetPositionAsString(false).c_str());
      m_pSelectedSceneJointItem->setText(Rotation, m_pSelectedSceneJoint->GetRotationAsString(false).c_str());
   }
}

void SbmDebuggerForm::UpdateLabels()
{
   // calculate fps
   m_msSinceLastFramePrev = m_msSinceLastFrame;
   m_msSinceLastFrame = m_StopWatch.GetTime();

   ui.rendererFpsLabel->setText(m_pGLWidget->GetFpsAsString().c_str());
   ui.networkFpsLabel->setText(GetFpsAsString().c_str());
   ui.cameraPositionLabel->setText(m_pGLWidget->GetCameraPositionAsString().c_str());
}

std::string SbmDebuggerForm::GetFpsAsString()
{
   return vhcl::Format("Network Fps: %.2f", (1 / (m_msSinceLastFrame - m_msSinceLastFramePrev)));
}

void SbmDebuggerForm::closeEvent(QCloseEvent *event)
{
   Disconnect();
   event->accept();
}

void SbmDebuggerForm::timerEvent(QTimerEvent * event)
{
   if (event->timerId() == timer.timerId())
   {
      Update();
   }
   else
   {
      QWidget::timerEvent(event);
   }
}

void SbmDebuggerForm::VHMsgCallback( const char * op, const char * args, void * userData )
{
   c.ProcessVHMsgs(op, args);
}

bool GetSceneScale(void* caller, NetRequest* req)
{
   SbmDebuggerForm* dlg = req->getCaller<SbmDebuggerForm*>();
   float scale = vhcl::ToDouble(req->ArgsAsString());

   if (scale == 0)
      return false;

   dlg->GetGLWidget()->SetObjectScale(1 / scale);

   // refresh the utils tab with the character info
   dlg->m_Utils->Refresh();

   return true;
}