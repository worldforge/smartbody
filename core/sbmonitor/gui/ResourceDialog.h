#ifndef RESOURCE_DIALOG_H_
#define RESOURCE_DIALOG_H_

#include "ui_ResourceDialog.h"
#include "SBDebuggerCommon.h"
#include "SBDebuggerClient.h"

using std::string;

class ResourceDialog : public QDialog
{
   Q_OBJECT

public:
   enum ResourceHeaders
   {
      Paths,
      Seq_Files,
      Skeletons,
      Bone_Map,
      Motions,
      Face_Definition,
      Event_Handlers,
      Pawns,
      Characters,
      _Scene,
      Services
   };

   ResourceDialog(SBDebuggerClient* client, QWidget *parent = 0);
   ~ResourceDialog();

   Ui::ResourceDialog ui;
   void AddEntry(ResourceHeaders topLevelItem, const QString& pathType, QStringList& paths);
   void AddChildWidgets(QTreeWidgetItem* parent, const QStringList& paths, bool removePreviousChildren);

public slots:
   void Refresh();

private:
   SBDebuggerClient* m_client;
   Scene* m_pScene;

   void SendGetAssetPathCommand(string assetType, NetRequest::RequestId rid);
};

#endif