#include "CommandDialog.h"
#include "vhcl.h"
#include "vhmsg-tt.h"

CommandDialog::CommandDialog(SBDebuggerClient* client, QWidget *parent) : QDialog(parent)
{
   m_client = client;
   ui.setupUi(this);

   connect(ui.runButton, SIGNAL(pressed()), this, SLOT(RunCode()));
   connect(ui.clearTopButton, SIGNAL(pressed()), this, SLOT(ClearOutputBox()));
}
   
CommandDialog::~CommandDialog()
{
   
}

void CommandDialog::RunCode()
{
   QString codeToRun = CurrentTextEditor()->toPlainText();
   ui.outputTextEdit->appendPlainText(codeToRun);

   if (ui.languageTab->currentIndex() == Python)
   {
      vector<string> split;
      vhcl::Tokenize(codeToRun.toStdString(), split, " ");

      string entireCommand = codeToRun.toStdString();
      string retVal = "";
      int firstSpace = entireCommand.find_first_of(" ");
      if (firstSpace != string::npos)
      {
         retVal = entireCommand.substr(0, firstSpace);
         entireCommand = entireCommand.erase(0, firstSpace);
      }

      if (retVal != "")
         m_client->SendSBMCommand(NetRequest::Send_Python_Command, retVal, entireCommand, SbmCommandReturned, this);
      else
         m_client->SendSBMCommand(NetRequest::Send_Python_Command, "void", entireCommand, SbmCommandReturned, this);
      
   }
   else
   {
      if (ui.languageTab->currentIndex() == Sbm)
         codeToRun.insert(0, "sbm ");

      vhmsg::ttu_notify1(codeToRun.toStdString().c_str());
   }
}

void CommandDialog::ClearOutputBox()
{
   ui.outputTextEdit->clear();
}

void CommandDialog::SaveCommand(string& command)
{
   for (unsigned int i = 0; i < m_previousCommands.size(); i++)
   {
      if (m_previousCommands[i] == command)
         return; // the command is already stored
   }

   m_previousCommands.push_back(command);
}

QPlainTextEdit* CommandDialog::CurrentTextEditor()
{
   switch (ui.languageTab->currentIndex())
   {
   case Sbm:
      return ui.sbmTextEdit;
      break;

   case Python:
      return ui.pythonTextEdit;
      break;

   case VHMsg:
      return ui.vhmsgTextEdit;
      break;
   }

   return NULL;
}

// callbacks
//
///////////////
bool SbmCommandReturned(void* caller, NetRequest* req)
{
   CommandDialog* dlg = req->getCaller<CommandDialog*>();
   
   string returnedValue = req->ArgsAsString();

   dlg->ui.outputTextEdit->appendPlainText(returnedValue.c_str());

   return true;
}