#ifndef BML_CREATOR_DIALOG_H_
#define BML_CREATOR_DIALOG_H_

#include "ui_BMLCreatorDialog.h"
#include "SBDebuggerCommon.h"
#include "SBDebuggerClient.h"

using std::string;

class BmlCreatorDialog : public QDialog
{
   Q_OBJECT

public:

   BmlCreatorDialog(SBDebuggerClient* client, QWidget *parent = 0);
   ~BmlCreatorDialog();

   Ui::BMLCreator ui;

public slots:
   void TypingTextChanged();
   void TypingSelectionChanged();
   void ComboCurrentIndexChanged(const QString & text);
   void CharacterSelectionChanged(const QString & text);
   void SliderMoved(int value);
   void SpinValueChanged ( const QString & text );
   void CheckBoxStateChanged(int state);
   void Refresh();
   void RunBml();
   void ResetBml();
   void ChangedTab(int currTab);

private:
   SBDebuggerClient* m_client;
   Scene* m_pScene;

   void BuildConnections(QObject* widget);
   QString GetSelectedChar();
   void AppendBml(const QString& name, const QString& attribute);
   void SetupComboBoxes();
};

#endif