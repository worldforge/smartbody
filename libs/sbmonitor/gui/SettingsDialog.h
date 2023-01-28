#ifndef SETTINGS_DIALOG_H_
#define SETTINGS_DIALOG_H_

#include "ui_SettingsDialog.h"

class SettingsDialog : public QDialog
{
   Q_OBJECT

public:
   SettingsDialog(QWidget* parent = 0);
   ~SettingsDialog();

   Ui::SettingsDialog ui;

signals:
      void DialogFinished(const SettingsDialog* dlg, int result);

private slots:
   void accept();
   void reject();
   void RendererTimeSetValue(int value);

};

#endif