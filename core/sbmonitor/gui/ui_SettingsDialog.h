/********************************************************************************
** Form generated from reading UI file 'SettingsDialog.ui'
**
** Created: Tue May 29 15:01:03 2012
**      by: Qt User Interface Compiler version 4.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETTINGSDIALOG_H
#define UI_SETTINGSDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QSlider>
#include <QtGui/QTabWidget>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SettingsDialog
{
public:
    QDialogButtonBox *buttonBox;
    QTabWidget *tabWidget;
    QWidget *general_tab;
    QComboBox *unitsBox;
    QLabel *label;
    QSlider *rendererSpeedSlider;
    QLabel *rendererSpeedLabel;
    QDoubleSpinBox *rendererSpeedBox;
    QWidget *camera_tab;
    QComboBox *cameraControlBox;
    QLabel *cameraControlLabel;
    QLabel *cameraMovementSpeedLabel;
    QDoubleSpinBox *cameraMovementSpeedBox;
    QLabel *cameraRotationSpeedLabel;
    QDoubleSpinBox *cameraRotationSpeedBox;

    void setupUi(QDialog *SettingsDialog)
    {
        if (SettingsDialog->objectName().isEmpty())
            SettingsDialog->setObjectName(QString::fromUtf8("SettingsDialog"));
        SettingsDialog->resize(466, 260);
        buttonBox = new QDialogButtonBox(SettingsDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(380, 30, 81, 51));
        buttonBox->setOrientation(Qt::Vertical);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        tabWidget = new QTabWidget(SettingsDialog);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setGeometry(QRect(10, 10, 361, 231));
        general_tab = new QWidget();
        general_tab->setObjectName(QString::fromUtf8("general_tab"));
        unitsBox = new QComboBox(general_tab);
        unitsBox->setObjectName(QString::fromUtf8("unitsBox"));
        unitsBox->setGeometry(QRect(10, 30, 61, 22));
        label = new QLabel(general_tab);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 10, 61, 16));
        rendererSpeedSlider = new QSlider(general_tab);
        rendererSpeedSlider->setObjectName(QString::fromUtf8("rendererSpeedSlider"));
        rendererSpeedSlider->setGeometry(QRect(10, 90, 121, 19));
        rendererSpeedSlider->setMaximum(100);
        rendererSpeedSlider->setValue(100);
        rendererSpeedSlider->setOrientation(Qt::Horizontal);
        rendererSpeedLabel = new QLabel(general_tab);
        rendererSpeedLabel->setObjectName(QString::fromUtf8("rendererSpeedLabel"));
        rendererSpeedLabel->setGeometry(QRect(10, 70, 81, 16));
        rendererSpeedBox = new QDoubleSpinBox(general_tab);
        rendererSpeedBox->setObjectName(QString::fromUtf8("rendererSpeedBox"));
        rendererSpeedBox->setEnabled(false);
        rendererSpeedBox->setGeometry(QRect(140, 90, 62, 22));
        rendererSpeedBox->setValue(1);
        tabWidget->addTab(general_tab, QString());
        camera_tab = new QWidget();
        camera_tab->setObjectName(QString::fromUtf8("camera_tab"));
        cameraControlBox = new QComboBox(camera_tab);
        cameraControlBox->setObjectName(QString::fromUtf8("cameraControlBox"));
        cameraControlBox->setGeometry(QRect(10, 30, 101, 22));
        cameraControlLabel = new QLabel(camera_tab);
        cameraControlLabel->setObjectName(QString::fromUtf8("cameraControlLabel"));
        cameraControlLabel->setGeometry(QRect(10, 10, 81, 16));
        cameraMovementSpeedLabel = new QLabel(camera_tab);
        cameraMovementSpeedLabel->setObjectName(QString::fromUtf8("cameraMovementSpeedLabel"));
        cameraMovementSpeedLabel->setGeometry(QRect(10, 70, 91, 16));
        cameraMovementSpeedBox = new QDoubleSpinBox(camera_tab);
        cameraMovementSpeedBox->setObjectName(QString::fromUtf8("cameraMovementSpeedBox"));
        cameraMovementSpeedBox->setGeometry(QRect(10, 90, 62, 22));
        cameraMovementSpeedBox->setSingleStep(0.1);
        cameraMovementSpeedBox->setValue(0.05);
        cameraRotationSpeedLabel = new QLabel(camera_tab);
        cameraRotationSpeedLabel->setObjectName(QString::fromUtf8("cameraRotationSpeedLabel"));
        cameraRotationSpeedLabel->setGeometry(QRect(10, 120, 81, 16));
        cameraRotationSpeedBox = new QDoubleSpinBox(camera_tab);
        cameraRotationSpeedBox->setObjectName(QString::fromUtf8("cameraRotationSpeedBox"));
        cameraRotationSpeedBox->setGeometry(QRect(10, 140, 62, 22));
        cameraRotationSpeedBox->setValue(0.05);
        tabWidget->addTab(camera_tab, QString());

        retranslateUi(SettingsDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), SettingsDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), SettingsDialog, SLOT(reject()));

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(SettingsDialog);
    } // setupUi

    void retranslateUi(QDialog *SettingsDialog)
    {
        SettingsDialog->setWindowTitle(QApplication::translate("SettingsDialog", "Settings", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("SettingsDialog", "Units", 0, QApplication::UnicodeUTF8));
        rendererSpeedLabel->setText(QApplication::translate("SettingsDialog", "Renderer Speed", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(general_tab), QApplication::translate("SettingsDialog", "General", 0, QApplication::UnicodeUTF8));
        cameraControlLabel->setText(QApplication::translate("SettingsDialog", "Camera Control", 0, QApplication::UnicodeUTF8));
        cameraMovementSpeedLabel->setText(QApplication::translate("SettingsDialog", "Movement Speed", 0, QApplication::UnicodeUTF8));
        cameraMovementSpeedBox->setPrefix(QString());
        cameraRotationSpeedLabel->setText(QApplication::translate("SettingsDialog", "Rotation Speed", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(camera_tab), QApplication::translate("SettingsDialog", "Camera", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class SettingsDialog: public Ui_SettingsDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETTINGSDIALOG_H
