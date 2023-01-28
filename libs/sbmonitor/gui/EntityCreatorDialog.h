#ifndef ENTITY_CREATOR_DIALOG_H_
#define ENTITY_CREATOR_DIALOG_H_

#include "ui_EntityCreatorDialog.h"
#include "SBDebuggerClient.h"

class EntityCreatorDialog : public QDialog
{
   Q_OBJECT

public:
   EntityCreatorDialog(SBDebuggerClient* client, QWidget* parent = 0);
   ~EntityCreatorDialog();
   Ui::EntityCreator ui;

private:
   std::string GetSelectedChar();
   SBDebuggerClient* m_client;
   Scene* m_pScene;

private slots:
   void EntityTypeChanged(const QString& text);
   void accept();
   void reject();
};

#endif