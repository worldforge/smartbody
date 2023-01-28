#ifndef UTILS_DIALOG_H_
#define UTILS_DIALOG_H_

#include "QtCrtDbgOff.h"
#include <QtGui>
#include "QtCrtDbgOn.h"

#include "ui_UtilsDialog.h"
#include "ui_SbmDebuggerForm.h"
#include "SBDebuggerClient.h"

class Scene;
class GLWidget;

using std::vector;
using std::string;

class UtilsDialog : public QObject
{
   Q_OBJECT

public:
   UtilsDialog(SBDebuggerClient* client, GLWidget* pRenderView, Ui::MainWindow& ui);
   ~UtilsDialog();
   vector<string> m_animations;
   Ui::MainWindow ui;

private:
   std::string GetSelectedChar();
   SBDebuggerClient* m_client;
   Scene* m_pScene;
   GLWidget* m_pRenderView;

private slots:
   void GazeAtPressed();
   void StopGazePressed();
   void RunBmlPressed();
   void PlayAnimPressed();
   void SetPosturePressed();
   void SpeakButtonPressed();
   void QueryAnimsPressed();
   void FilterAnims();

public slots:
   void Refresh();
};

#endif
