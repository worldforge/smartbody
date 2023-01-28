/********************************************************************************
** Form generated from reading UI file 'ResourceDialog.ui'
**
** Created: Tue May 29 15:01:03 2012
**      by: Qt User Interface Compiler version 4.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RESOURCEDIALOG_H
#define UI_RESOURCEDIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QScrollArea>
#include <QtGui/QTreeWidget>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ResourceDialog
{
public:
    QHBoxLayout *horizontalLayout;
    QTreeWidget *resourceTree;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QPushButton *refreshButton;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *ResourceDialog)
    {
        if (ResourceDialog->objectName().isEmpty())
            ResourceDialog->setObjectName(QString::fromUtf8("ResourceDialog"));
        ResourceDialog->resize(785, 535);
        horizontalLayout = new QHBoxLayout(ResourceDialog);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        resourceTree = new QTreeWidget(ResourceDialog);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QString::fromUtf8("1"));
        resourceTree->setHeaderItem(__qtreewidgetitem);
        resourceTree->setObjectName(QString::fromUtf8("resourceTree"));

        horizontalLayout->addWidget(resourceTree);

        scrollArea = new QScrollArea(ResourceDialog);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 375, 515));
        refreshButton = new QPushButton(scrollAreaWidgetContents);
        refreshButton->setObjectName(QString::fromUtf8("refreshButton"));
        refreshButton->setGeometry(QRect(10, 10, 75, 23));
        scrollArea->setWidget(scrollAreaWidgetContents);

        horizontalLayout->addWidget(scrollArea);

        buttonBox = new QDialogButtonBox(ResourceDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Vertical);
        buttonBox->setStandardButtons(QDialogButtonBox::NoButton);

        horizontalLayout->addWidget(buttonBox);


        retranslateUi(ResourceDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), ResourceDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), ResourceDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(ResourceDialog);
    } // setupUi

    void retranslateUi(QDialog *ResourceDialog)
    {
        ResourceDialog->setWindowTitle(QApplication::translate("ResourceDialog", "Resource", 0, QApplication::UnicodeUTF8));
        refreshButton->setText(QApplication::translate("ResourceDialog", "Refresh", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class ResourceDialog: public Ui_ResourceDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RESOURCEDIALOG_H
