#ifndef DATA_VIEWER_DIALOG_H_
#define DATA_VIEWER_DIALOG_H_

#include "QtCrtDbgOff.h"
#include "ui_DataViewerDialog.h"
#include "QtCrtDbgOn.h"

#include "SBDebuggerCommon.h"
#include "GLGraphWidget.h"
#include "SBDebuggerClient.h"

using std::string;

class DataViewerDialog : public QDialog
{
   Q_OBJECT

public:
   DataViewerDialog(SBDebuggerClient* client, QWidget* parent = 0);
   ~DataViewerDialog();
   Ui::DataViewerDialog ui;

private slots:
   void AddSelectedChannels();
   void RemoveSelectedChannels();
   void ChangedSelectedMotion(const QString&);
   void ChangedSelectedCharacter(const QString&);
   void ChangedRotationDisplay(const QString&);
   void Refresh();

private:
   Scene* m_pScene;
   GLGraphWidget* m_pGraphWidget;
   SBDebuggerClient* m_client;
   QBasicTimer timer;
   float m_DrawIntervals;
   float m_AccumTime;
   //void AddAllJointsToList(QListWidget* list, std::vector<Joint*>& joints);

protected:
   virtual void timerEvent(QTimerEvent * event);
   void Update();
};

#endif
