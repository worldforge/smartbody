#include "SettingsDialog.h"

SettingsDialog::SettingsDialog(QWidget* parent) : QDialog(parent)
{
   ui.setupUi(this);

   ui.cameraControlBox->addItem("Free Look");
   ui.cameraControlBox->addItem("Follow Renderer");
   ui.unitsBox->addItem("1");
   ui.unitsBox->addItem("100");
   ui.unitsBox->setCurrentIndex(1);
   connect(ui.rendererSpeedSlider, SIGNAL(valueChanged (int)), this, SLOT(RendererTimeSetValue(int)));
}

SettingsDialog::~SettingsDialog()
{
   
}

void SettingsDialog::accept()
{
   done(Accepted);
   emit DialogFinished(this, Accepted);
}

void SettingsDialog::reject()
{
   done(Rejected);
   emit DialogFinished(this, Rejected);
}

void SettingsDialog::RendererTimeSetValue(int value)
{
   ui.rendererSpeedBox->setValue((double)value / 100.0);
}