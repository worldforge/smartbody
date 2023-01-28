/********************************************************************************
** Form generated from reading UI file 'EntityCreatorDialog.ui'
**
** Created: Tue May 29 15:01:03 2012
**      by: Qt User Interface Compiler version 4.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ENTITYCREATORDIALOG_H
#define UI_ENTITYCREATORDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPlainTextEdit>

QT_BEGIN_NAMESPACE

class Ui_EntityCreator
{
public:
    QDialogButtonBox *buttonBox;
    QComboBox *entityTypeBox;
    QLabel *label;
    QPlainTextEdit *sbmNameBox;
    QLabel *label_2;
    QComboBox *skeletonBox;
    QLabel *label_3;
    QPlainTextEdit *objectNameBox;
    QLabel *label_4;

    void setupUi(QDialog *EntityCreator)
    {
        if (EntityCreator->objectName().isEmpty())
            EntityCreator->setObjectName(QString::fromUtf8("EntityCreator"));
        EntityCreator->resize(279, 155);
        buttonBox = new QDialogButtonBox(EntityCreator);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setGeometry(QRect(60, 120, 161, 31));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        entityTypeBox = new QComboBox(EntityCreator);
        entityTypeBox->setObjectName(QString::fromUtf8("entityTypeBox"));
        entityTypeBox->setGeometry(QRect(20, 30, 111, 22));
        label = new QLabel(EntityCreator);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 10, 61, 16));
        sbmNameBox = new QPlainTextEdit(EntityCreator);
        sbmNameBox->setObjectName(QString::fromUtf8("sbmNameBox"));
        sbmNameBox->setGeometry(QRect(20, 80, 111, 21));
        sbmNameBox->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        label_2 = new QLabel(EntityCreator);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(20, 60, 91, 16));
        skeletonBox = new QComboBox(EntityCreator);
        skeletonBox->setObjectName(QString::fromUtf8("skeletonBox"));
        skeletonBox->setGeometry(QRect(150, 30, 111, 22));
        label_3 = new QLabel(EntityCreator);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(150, 10, 61, 16));
        objectNameBox = new QPlainTextEdit(EntityCreator);
        objectNameBox->setObjectName(QString::fromUtf8("objectNameBox"));
        objectNameBox->setGeometry(QRect(150, 80, 111, 21));
        objectNameBox->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        label_4 = new QLabel(EntityCreator);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(150, 60, 91, 16));

        retranslateUi(EntityCreator);
        QObject::connect(buttonBox, SIGNAL(accepted()), EntityCreator, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), EntityCreator, SLOT(reject()));

        QMetaObject::connectSlotsByName(EntityCreator);
    } // setupUi

    void retranslateUi(QDialog *EntityCreator)
    {
        EntityCreator->setWindowTitle(QApplication::translate("EntityCreator", "Entity Creator", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("EntityCreator", "Entity Type", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("EntityCreator", "Smartbody Name", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("EntityCreator", "Skeleton", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("EntityCreator", "Object Name", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class EntityCreator: public Ui_EntityCreator {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ENTITYCREATORDIALOG_H
