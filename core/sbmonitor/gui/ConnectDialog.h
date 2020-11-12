#ifndef CONNECT_DIALOG_H_
#define CONNECT_DIALOG_H_

#include "QtCrtDbgOff.h"
#include <QtGui>
#include <QWidget>
#include "QtCrtDbgOn.h"

#include "ui_SbmDebuggerForm.h"
#include "ui_ConnectDialog.h"

class ConnectDialog : public QDialog
 {
     Q_OBJECT

 public:
     ConnectDialog(QWidget *parent = 0);

     QListWidget * GetListWidget() { return ui.listWidget; }

public Q_SLOTS:
   void keyPressEvent(QKeyEvent *key);
   void itemDoubleClicked(QListWidgetItem * item);

 private slots:
     void accept();
     void reject();

 private:
     Ui::ConnectDialog ui;
 };

#endif
