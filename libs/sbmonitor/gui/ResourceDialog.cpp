#include "ResourceDialog.h"

#include "vhmsg-tt.h"
#include "vhcl.h"

// callbacks
bool GetPathsCB(void* caller, NetRequest* req);

ResourceDialog::ResourceDialog(SBDebuggerClient* client, QWidget *parent) 
: QDialog(parent)
{
   m_client = client;
   m_pScene = client->GetScene();
   ui.setupUi(this);

   connect(ui.refreshButton, SIGNAL(pressed()), this, SLOT(Refresh()));

   ui.resourceTree->setHeaderLabel("Resources");

   QTreeWidgetItem* paths = new QTreeWidgetItem(ui.resourceTree, QStringList(QString("Paths")));
   ui.resourceTree->insertTopLevelItem(Paths, paths);
   paths->addChild(new QTreeWidgetItem(paths, QStringList(QString("Sequence Paths"))));
   paths->addChild(new QTreeWidgetItem(paths, QStringList(QString("ME Paths"))));
   paths->addChild(new QTreeWidgetItem(paths, QStringList(QString("Audio Paths"))));
   paths->addChild(new QTreeWidgetItem(paths, QStringList(QString("Mesh Paths"))));

   ui.resourceTree->insertTopLevelItem(Seq_Files, new QTreeWidgetItem(ui.resourceTree, QStringList(QString("Seq Files"))));
   ui.resourceTree->insertTopLevelItem(Skeletons, new QTreeWidgetItem(ui.resourceTree, QStringList(QString("Skeletons"))));
   ui.resourceTree->insertTopLevelItem(Bone_Map, new QTreeWidgetItem(ui.resourceTree, QStringList(QString("Bone Map"))));
   ui.resourceTree->insertTopLevelItem(Motions, new QTreeWidgetItem(ui.resourceTree, QStringList(QString("Motions"))));
   ui.resourceTree->insertTopLevelItem(Face_Definition, new QTreeWidgetItem(ui.resourceTree, QStringList(QString("Face Definition"))));
   ui.resourceTree->insertTopLevelItem(Event_Handlers, new QTreeWidgetItem(ui.resourceTree, QStringList(QString("Event Handlers"))));
   ui.resourceTree->insertTopLevelItem(Pawns, new QTreeWidgetItem(ui.resourceTree, QStringList(QString("Pawns"))));
   ui.resourceTree->insertTopLevelItem(Characters, new QTreeWidgetItem(ui.resourceTree, QStringList(QString("Characters"))));
   ui.resourceTree->insertTopLevelItem(_Scene, new QTreeWidgetItem(ui.resourceTree, QStringList(QString("Scene"))));
   ui.resourceTree->insertTopLevelItem(Services, new QTreeWidgetItem(ui.resourceTree, QStringList(QString("Services"))));

   Refresh();
}

ResourceDialog::~ResourceDialog()
{
   
}

void ResourceDialog::SendGetAssetPathCommand(string assetType, NetRequest::RequestId rid)
{
   string command = vhcl::Format("scene.getAssetPaths(\'%s\')", assetType.c_str());
   m_client->SendSBMCommand(rid, "string-array", command, GetPathsCB, this);
}

void ResourceDialog::Refresh()
{
   SendGetAssetPathCommand("script", NetRequest::Get_Seq_Asset_Paths);
   SendGetAssetPathCommand("motion", NetRequest::Get_ME_Asset_Paths);
   SendGetAssetPathCommand("audio", NetRequest::Get_Audio_Asset_Paths);
   SendGetAssetPathCommand("mesh", NetRequest::Get_Mesh_Asset_Paths);

   m_client->SendSBMCommand(NetRequest::Get_Script_Names, "string-array", "scene.getScriptNames()", GetPathsCB, this);
   m_client->SendSBMCommand(NetRequest::Get_Service_Names, "string-array", "scene.getServiceManager().getServiceNames()", GetPathsCB, this);
   m_client->SendSBMCommand(NetRequest::Get_Motion_Names, "string-array", "scene.getMotionNames()", GetPathsCB, this);

   m_client->SendSBMCommand(NetRequest::Get_Skeleton_Names, "string-array", "scene.getSkeletonNames()", GetPathsCB, this);
   m_client->SendSBMCommand(NetRequest::Get_BoneMap_Names, "string-array", "scene.getBoneMapNames()", GetPathsCB, this);
   m_client->SendSBMCommand(NetRequest::Get_EventHandler_Names, "string-array", "scene.getEventHandlerNames()", GetPathsCB, this);

   QStringList names;
   for (unsigned int i = 0; i < m_pScene->m_characters.size(); i++)
   {
      names.append(m_pScene->m_characters[i].m_name.c_str());
   }
   AddEntry(Characters, "Characters", names);
   names.clear();

   for (unsigned int i = 0; i < m_pScene->m_pawns.size(); i++)
   {
      names.append(m_pScene->m_pawns[i].m_name.c_str());
   }
   AddEntry(Pawns, "Pawns", names);
   names.clear();
}

void ResourceDialog::AddEntry(ResourceHeaders topLevelItem, const QString& pathType, QStringList& paths)
{
   QTreeWidgetItem* widget = ui.resourceTree->topLevelItem(topLevelItem);

   // first check the top level item before going to the children
   if (widget->text(0) == pathType)
   {
      AddChildWidgets(widget, paths, true);
      return;
   }

   // find the specified path
   for (int i = 0; i < widget->childCount(); i++)
   {
      QTreeWidgetItem* child = widget->child(i);
      if (child->text(0) == pathType)
      {
         AddChildWidgets(child, paths, true);
         return;
      }
   }
}

void ResourceDialog::AddChildWidgets(QTreeWidgetItem* parent, const QStringList& paths, bool removePreviousChildren)
{
   if (removePreviousChildren)
   {
      // remove all old children first
      while (parent->childCount() > 0)
      {
         parent->removeChild(parent->child(0));
      }
   }

   // add new children
   for (int j = 0; j < paths.size(); j++)
   {
      parent->addChild(new QTreeWidgetItem(QStringList(paths[j])));
   }
}

bool GetPathsCB(void* caller, NetRequest* req)
{
   ResourceDialog* dlg = req->getCaller<ResourceDialog*>();
   vector<string> args = req->Args();
   QStringList list;

   for (unsigned int i = 0; i < args.size(); i++)
   {
      list.append(args[i].c_str());
   }

   switch (req->Rid())
   {
   case NetRequest::Get_Seq_Asset_Paths:
      dlg->AddEntry(ResourceDialog::Paths, "Sequence Paths", list);
      break;

   case NetRequest::Get_ME_Asset_Paths:
      dlg->AddEntry(ResourceDialog::Paths,"ME Paths", list);
      break;

   case NetRequest::Get_Audio_Asset_Paths:
      dlg->AddEntry(ResourceDialog::Paths,"Audio Paths", list);
      break;

   case NetRequest::Get_Mesh_Asset_Paths:
      dlg->AddEntry(ResourceDialog::Paths, "Mesh Paths", list);
      break;

   case NetRequest::Get_Script_Names:
      dlg->AddEntry(ResourceDialog::Seq_Files, "Sequence Paths", list);
      break;

   case NetRequest::Get_Service_Names:
      dlg->AddEntry(ResourceDialog::Services, "Services", list);
      break;

   case NetRequest::Get_Motion_Names:
      dlg->AddEntry(ResourceDialog::Motions, "Motions", list);
      break;

   case NetRequest::Get_Skeleton_Names:
      dlg->AddEntry(ResourceDialog::Skeletons, "Skeletons", list);
      break;

   case NetRequest::Get_BoneMap_Names:
      dlg->AddEntry(ResourceDialog::Bone_Map, "Bone Map", list);
      break;

   case NetRequest::Get_EventHandler_Names:
      dlg->AddEntry(ResourceDialog::Event_Handlers, "Event Handlers", list);
      break;
   }
   
   return true;
}