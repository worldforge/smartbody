#include "vhcl.h"

using vhcl::Vector4;
#include "DataViewerDialog.h"

//callbacks
bool GetStringList(void* caller, NetRequest* req);

DataViewerDialog::DataViewerDialog(SBDebuggerClient* client, QWidget* parent) : QDialog(parent)
{
   m_client = client;
   m_pScene = m_client->GetScene();
   ui.setupUi(this);
   m_pGraphWidget = new GLGraphWidget(ui.renderSize->geometry(), m_pScene, this);

   m_AccumTime = 0;
   m_DrawIntervals = 10;
   timer.start((int)m_DrawIntervals, this);
   ui.showRotaionAsBox->addItem("Quaternion");
   //ui.showRotaionAsBox->addItem("Euler Angle");
   //ui.showRotaionAsBox->addItem("Axis-Angle");
   //ui.showRotaionAsBox->addItem("Quaternion Velocity");
   //ui.showRotaionAsBox->addItem("Euler Velocity");
   //ui.showRotaionAsBox->addItem("Axis-Angle Velocity");

   connect(ui.addChannelButton, SIGNAL(pressed()), this, SLOT(AddSelectedChannels()));
   connect(ui.removeChannelButton, SIGNAL(pressed()), this, SLOT(RemoveSelectedChannels()));
   connect(ui.showRotaionAsBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(ChangedRotationDisplay(const QString&)));
   connect(ui.refreshButton, SIGNAL(pressed()), this, SLOT(Refresh()));
   connect(ui.motionBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(ChangedSelectedMotion(const QString&)));
   connect(ui.characterNameBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(ChangedSelectedCharacter(const QString&)));

   Refresh();
}

DataViewerDialog::~DataViewerDialog()
{
   delete m_pGraphWidget;
}

void DataViewerDialog::Refresh()
{
   ui.characterNameBox->clear();
   for (unsigned int i = 0; i < m_pScene->m_characters.size(); i++)
   {
      ui.characterNameBox->addItem(m_pScene->m_characters[i].m_name.c_str());
   }

   m_client->SendSBMCommand(NetRequest::Get_Motion_Names, "string-array", "scene.getMotionNames()", GetStringList, this);
}

void DataViewerDialog::AddSelectedChannels()
{
   QList<QListWidgetItem*> selectedItems = ui.channelListBox->selectedItems();
   for (int i = 0; i < selectedItems.length(); i++)
   {
      ui.monitoredListBox->addItem(selectedItems[i]->text());
      delete ui.channelListBox->takeItem(ui.channelListBox->row(selectedItems[i]));
   }
}

void DataViewerDialog::RemoveSelectedChannels()
{
   QList<QListWidgetItem*> selectedItems = ui.monitoredListBox->selectedItems();
   for (int i = 0; i < selectedItems.length(); i++)
   {
      if (selectedItems[i]->text().contains("quat", Qt::CaseInsensitive))
      {
         // get rid of all 4 lines
         m_pGraphWidget->RemoveLineGraph(selectedItems[i]->text().toStdString() + "x");
         m_pGraphWidget->RemoveLineGraph(selectedItems[i]->text().toStdString() + "y");
         m_pGraphWidget->RemoveLineGraph(selectedItems[i]->text().toStdString() + "z");
         m_pGraphWidget->RemoveLineGraph(selectedItems[i]->text().toStdString() + "w");
      }
      else
      {
         m_pGraphWidget->RemoveLineGraph(selectedItems[i]->text().toStdString());
      }

      ui.channelListBox->addItem(selectedItems[i]->text());
      delete ui.monitoredListBox->takeItem(ui.monitoredListBox->row(selectedItems[i]));
   }

   if (ui.monitoredListBox->selectedItems().length() == 0)
   {
      m_AccumTime = 0;
   }
}

void DataViewerDialog::ChangedRotationDisplay(const QString& text)
{
   ui.wCheckBox->setEnabled(text == "Quaternion");
}

void DataViewerDialog::ChangedSelectedCharacter(const QString& text)
{
   string command = vhcl::Format("scene.getCharacter(\'%s\').getControllerNames()", text.toStdString().c_str());
   m_client->SendSBMCommand(NetRequest::Get_Controller_Names, "string-array", command, GetStringList, this);
}

void DataViewerDialog::ChangedSelectedMotion(const QString& text)
{
   string command = vhcl::Format("scene.getMotion(\'%s\').getChannels()", text.toStdString().c_str());
   m_client->SendSBMCommand(NetRequest::Get_Channel_Names, "string-array", command, GetStringList, this);
}

void DataViewerDialog::Update()
{
   if (ui.monitoredListBox->count() == 0 || ui.characterNameBox->count() == 0)
      return; 

   // find the character that is selected
   Character* pSelectedCharacter = m_pScene->FindCharacter(ui.characterNameBox->currentText().toStdString());
   if (!pSelectedCharacter)
      return;
   
   // go through the monitored joints and update the graph with the new data
   for (int i = 0; i < ui.monitoredListBox->count(); i++)
   {
      QListWidgetItem* item = ui.monitoredListBox->item(i);
      QString channelName = item->text();
      QString suffix = "";
      Vector4 value; // positions use only the x value, rotations use all 4 values
      int suffixIndex = channelName.lastIndexOf("_");
      
      if (suffixIndex != -1)
      {
         suffix = channelName.right(suffixIndex);
         channelName = channelName.remove(suffixIndex, channelName.length() - suffixIndex);
      }
      
      Joint* joint = pSelectedCharacter->FindJoint(channelName.toStdString());
      const float ZPos = -9.9f;

      if (joint)
      {
         if (suffix.contains("x", Qt::CaseInsensitive))
            m_pGraphWidget->AddLineGraphPoint(item->text().toStdString(), Vector3(m_AccumTime, joint->pos.x, ZPos), Vector3(1, 0, 1), ui.numFramesBox->value());
         else if (suffix.contains("y", Qt::CaseInsensitive))
            m_pGraphWidget->AddLineGraphPoint(item->text().toStdString(), Vector3(m_AccumTime, joint->pos.y, ZPos), Vector3(1, 0, 1), ui.numFramesBox->value());
         else if (suffix.contains("z", Qt::CaseInsensitive))
            m_pGraphWidget->AddLineGraphPoint(item->text().toStdString(), Vector3(m_AccumTime, joint->pos.z, ZPos), Vector3(1, 0, 1), ui.numFramesBox->value());
         else if (suffix.contains("quat", Qt::CaseInsensitive))
         {
            m_pGraphWidget->AddLineGraphPoint(item->text().toStdString() + "x", Vector3(m_AccumTime, joint->rot.x, ZPos), Vector3(1, 0, 0), ui.numFramesBox->value());
            m_pGraphWidget->AddLineGraphPoint(item->text().toStdString() + "y", Vector3(m_AccumTime, joint->rot.y, ZPos), Vector3(0, 1, 0), ui.numFramesBox->value());
            m_pGraphWidget->AddLineGraphPoint(item->text().toStdString() + "z", Vector3(m_AccumTime, joint->rot.z, ZPos), Vector3(0, 0, 1), ui.numFramesBox->value());
            m_pGraphWidget->AddLineGraphPoint(item->text().toStdString() + "w", Vector3(m_AccumTime, joint->rot.w, ZPos), Vector3(1, 0, 1), ui.numFramesBox->value());
         }

         
         m_AccumTime += m_DrawIntervals / 1000.0f;
      }
   }
}

void DataViewerDialog::timerEvent(QTimerEvent * event)
{
   if (event->timerId() == timer.timerId())
   {
      Update();
   }
   else
   {
      QWidget::timerEvent(event);
   }
}


bool GetStringList(void* caller, NetRequest* req)
{
   DataViewerDialog* dlg = req->getCaller<DataViewerDialog*>();

   vector<string> args = req->Args();
   QStringList names;
   for (unsigned int i = 0; i < args.size(); i++)
   {
      names.append(args[i].c_str());
   }

   switch (req->Rid())
   {
   case NetRequest::Get_Motion_Names:
      dlg->ui.motionBox->clear();
      dlg->ui.motionBox->addItems(names);
      break;

   case NetRequest::Get_Channel_Names:
      dlg->ui.channelListBox->clear();
      //for (int i = 0; i < names.length(); i++)
      //{
      //   if (names[i].contains("Quat"))
      //   {
      //      names[i].append(" (4)");
      //   }
      //   else
      //   {
      //      names[i].append(" (1)");
      //   }
      //}
      dlg->ui.channelListBox->addItems(names);
      break;

   case NetRequest::Get_Controller_Names:
      dlg->ui.controllerBox->clear();
      dlg->ui.controllerBox->addItems(names);
      break;
   }

   return true;
}
