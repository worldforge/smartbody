#include "EntityCreatorDialog.h"
#include "vhmsg-tt.h"
#include <QMessageBox>

bool GetSkeletonNamesCB(void* caller, NetRequest* req);

EntityCreatorDialog::EntityCreatorDialog(SBDebuggerClient* client, QWidget* parent) : QDialog(parent)
{
   ui.setupUi(this);

   m_client = client;
   m_pScene = client->GetScene();

   connect(ui.entityTypeBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(EntityTypeChanged(const QString&)));

   m_client->SendSBMCommand(NetRequest::Get_Skeleton_Names, "string-array", "scene.getSkeletonNames()", GetSkeletonNamesCB, this);
   
   ui.entityTypeBox->addItem("pawn");
   ui.entityTypeBox->addItem("char");
}

EntityCreatorDialog::~EntityCreatorDialog()
{

}

void EntityCreatorDialog::EntityTypeChanged(const QString& text)
{
   bool creatingChar = text == "char";
   ui.skeletonBox->setEnabled(creatingChar);
   ui.objectNameBox->setEnabled(creatingChar);
}

void EntityCreatorDialog::accept()
{
   if (ui.sbmNameBox->toPlainText().length() == 0)
   {
      QMessageBox::warning(this, "Bad Name", "All smartbody entities must have a name");
   }
   else
   {
      QString message;
      if (ui.entityTypeBox->currentText() == "char")
      {
         message = QString("sbm char %1 init %2 %3").arg(ui.sbmNameBox->toPlainText()).arg(ui.skeletonBox->currentText())
            .arg(ui.objectNameBox->toPlainText());
      }
      else
      {
         message = QString("sbm pawn %1 init loc 0 0 0").arg(ui.sbmNameBox->toPlainText());
      }
      
      vhmsg::ttu_notify1(message.toStdString().c_str());
      done(Accepted);
   }
}

void EntityCreatorDialog::reject()
{
   done(Rejected);
}

bool GetSkeletonNamesCB(void* caller, NetRequest* req)
{
   EntityCreatorDialog* dlg = req->getCaller<EntityCreatorDialog*>();
   vector<string> args = req->Args();
   QStringList list;

   for (unsigned int i = 0; i < args.size(); i++)
   {
      list.append(args[i].c_str());
   }

   dlg->ui.skeletonBox->addItems(list);

   return true;
}