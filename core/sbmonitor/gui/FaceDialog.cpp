#include "FaceDialog.h"
#include "vhcl.h"
#include "vhmsg-tt.h"
#include <algorithm>

#define SPIN_BOX "spinBox"

// callbacks
bool GetVisemeNames(void* caller, NetRequest* req);
bool GetAUNumbers(void* caller, NetRequest* req);
bool GetAUSide(void* caller, NetRequest* req);


FaceDialog::FaceDialog(SBDebuggerClient* client, QWidget* parent ) : QDialog(parent)
{
   m_BlockSending = false;
   m_client = client;
   m_pScene = m_client->GetScene();
   ui.setupUi(this);
   QVBoxLayout *scrollViewLayout = new QVBoxLayout;
   ui.scrollArea->setLayout(scrollViewLayout);
   ui.scrollArea->widget()->setLayout(scrollViewLayout);

   ui.characterNameBox->clear();
   for (unsigned int i = 0; i < m_pScene->m_characters.size(); i++)
   {
      ui.characterNameBox->addItem(m_pScene->m_characters[i].m_name.c_str());
   }

   connect(ui.resetButton, SIGNAL(pressed()), this, SLOT(Reset()));
   connect(ui.characterNameBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(CharacterSelectionChanged(const QString&)));

   if (m_pScene->m_characters.size() >= 1)
   {
      CharacterSelectionChanged(m_pScene->m_characters[0].m_name.c_str());
   }

   timer.start(10, this);
}

FaceDialog::~FaceDialog()
{
   
}

void FaceDialog::AddFacialExpression(const string& name, double weight)
{
   weight = std::max<double>(0, weight);
   weight = std::min<double>(1, weight);

   QLabel* expressionName = new QLabel(name.c_str(), this);

   // create slider
   QSlider* weightSlider = new QSlider(this);
   weightSlider->setOrientation(Qt::Horizontal);
   weightSlider->setMaximum(100);
   weightSlider->setMinimum(0);
   weightSlider->setSingleStep(1);
   weightSlider->setValue(weight * 100);
   weightSlider->setObjectName(name.c_str());
   m_Sliders.push_back(weightSlider);

   // create spin box
   QDoubleSpinBox* weightDisplay = new QDoubleSpinBox(this);
   weightDisplay->setEnabled(false);
   weightDisplay->setMaximum(1.0f);
   weightDisplay->setMinimum(0);
   weightDisplay->setValue(weight);
   weightDisplay->setObjectName((name + SPIN_BOX).c_str());

   // create the frame that holds everything
   QFrame* frame = new QFrame(this);
   QHBoxLayout *frameLayout = new QHBoxLayout;
   frameLayout->addWidget(expressionName);
   frameLayout->addWidget(weightSlider);
   frameLayout->addWidget(weightDisplay);
   frame->setLayout(frameLayout);

   // add the frame to the scroll view
   ui.scrollArea->widget()->layout()->addWidget(frame);

   // setup gui messages
   connect(weightSlider, SIGNAL(valueChanged(int)), this, SLOT(SliderValueChanged(int)));


   // save for later deletion
   m_scrollListChildren.push_back(expressionName);
   m_scrollListChildren.push_back(weightSlider);
   m_scrollListChildren.push_back(weightDisplay);
   m_scrollListChildren.push_back(frame);
}

void FaceDialog::Reset()
{
   for (unsigned int i = 0; i < m_Sliders.size(); i++)
   {
      m_Sliders[i]->setValue(0);
   }
}

void FaceDialog::CharacterSelectionChanged(const QString& selection)
{
   // get AU's
   string commandAU = vhcl::Format("scene.getCharacter(\'%s\').getFaceDefinition().getAUNumbers()", selection.toStdString().c_str());
   m_client->SendSBMCommand(NetRequest::Get_Viseme_Names, "int-array", commandAU, GetAUNumbers, this);

   // get visemes
   string commandViseme = vhcl::Format("scene.getCharacter(\'%s\').getFaceDefinition().getVisemeNames()", selection.toStdString().c_str());
   m_client->SendSBMCommand(NetRequest::Get_Viseme_Names, "string-array", commandViseme, GetVisemeNames, this);

   for (unsigned int i = 0; i < m_scrollListChildren.size(); i++)
   {
      delete m_scrollListChildren[i];
   }
   m_scrollListChildren.clear(); 
   m_Sliders.clear();
   m_AUNums.clear();
}

void FaceDialog::SliderValueChanged(int val)
{
   QSlider* sender = dynamic_cast<QSlider*>(QObject::sender());
   if (!sender)
      return;

   QString visemeName = sender->objectName();

   QDoubleSpinBox* spinBox = ui.scrollArea->findChild<QDoubleSpinBox*>(sender->objectName() + SPIN_BOX);
   if (!spinBox)
      return;

   double weight = (double)val / sender->maximum();
   spinBox->setValue(weight);

   if (!m_BlockSending)
   {
      // inform smartbody
      string msg = vhcl::Format("sbm char %s viseme %s %.2f",
         ui.characterNameBox->currentText().toStdString().c_str(), visemeName.toStdString().c_str(), weight); 
      vhmsg::ttu_notify1(msg.c_str());
   }
   
}

void FaceDialog::Update()
{
   if (!ui.interactiveModeCheckBox->isChecked())
      return;

   // we're setting the sliders based off values from smartbody  
   m_BlockSending = true;
   Character* selectedChar = m_pScene->FindCharacter(ui.characterNameBox->currentText().toStdString());
   if (selectedChar)
   {
      QString visemeName;
      Joint* visemeJoint = NULL;
      for (unsigned int i = 0; i < m_Sliders.size(); i++)
      {
         // find the au channel in the character's skeleton
         visemeName = m_Sliders[i]->objectName();
         visemeJoint = selectedChar->FindJoint(visemeName.toStdString());
         if (visemeJoint)
         {
            m_Sliders[i]->setValue(visemeJoint->pos.x * m_Sliders[i]->maximum());
         }
      }
   }
   m_BlockSending = false;
}

void FaceDialog::timerEvent(QTimerEvent * event)
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

bool GetVisemeNames(void* caller, NetRequest* req)
{
   FaceDialog* dlg = req->getCaller<FaceDialog*>();
   
   vector<string> visemeNames = req->Args();
   for (unsigned int i = 0; i < visemeNames.size(); i++)
   {
      dlg->AddFacialExpression(visemeNames[i].c_str(), 0);
   }

   return true;
}

bool GetAUNumbers(void* caller, NetRequest* req)
{
   FaceDialog* dlg = req->getCaller<FaceDialog*>();
   
   // save au's for later use
   dlg->m_AUNums = req->Args();
   for (unsigned int i = 0; i < dlg->m_AUNums.size(); i++)
   {
      // i need to know the side that the au is on, so this command is sent
      string commandAUSide = vhcl::Format("scene.getCharacter(\'%s\').getFaceDefinition().getAUSide(%s)",
         dlg->ui.characterNameBox->currentText().toStdString().c_str(), dlg->m_AUNums[i].c_str());
      dlg->m_client->SendSBMCommand(NetRequest::Get_AU_Side, "string", commandAUSide, GetAUSide, dlg); 

      dlg->m_AUNums[i].insert(0, "au_");
   }

   return true;
}

bool GetAUSide(void* caller, NetRequest* req)
{
   FaceDialog* dlg = req->getCaller<FaceDialog*>();
   if (dlg->m_AUNums.size() == 0)
   {
      return false;
   }

   string side = req->ArgsAsString();
   std::transform(side.begin(), side.end(), side.begin(), ::tolower);
   string auName = dlg->m_AUNums[0];
   dlg->m_AUNums.erase(dlg->m_AUNums.begin());

   if (side == "leftright")
   {
      dlg->AddFacialExpression(auName + "_left", 0);
      dlg->AddFacialExpression(auName + "_right", 0);
   }
   else if (side == "both")
   {
      dlg->AddFacialExpression(auName, 0);
   }
   else
   {
      side.insert(side.begin(), '_');
      dlg->AddFacialExpression(auName + side, 0);
   }

   return true;
}

