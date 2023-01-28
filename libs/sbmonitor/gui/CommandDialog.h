#ifndef COMMAND_DIALOG_H_
#define COMMAND_DIALOG_H_

#include "ui_CommandDialog.h"
#include "SBDebuggerClient.h"

using std::string;
using std::vector;

// callbacks
bool SbmCommandReturned(void* caller, NetRequest* req);

class CommandDialog : public QDialog
{
   Q_OBJECT

public slots:
   void RunCode();
   void ClearOutputBox();

public:
   CommandDialog(SBDebuggerClient* client, QWidget *parent = 0);
   ~CommandDialog();

   QPlainTextEdit* CurrentTextEditor();

   Ui::CommandDialog ui;

private:
   enum TabType
   {
      Sbm,
      Python,
      VHMsg,
   };

   SBDebuggerClient* m_client;
   vector<string> m_previousCommands;

   void SaveCommand(string& command);
};

#endif