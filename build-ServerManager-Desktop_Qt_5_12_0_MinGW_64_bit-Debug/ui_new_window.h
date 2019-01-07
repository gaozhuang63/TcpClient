/********************************************************************************
** Form generated from reading UI file 'new_window.ui'
**
** Created by: Qt User Interface Compiler version 5.12.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NEW_WINDOW_H
#define UI_NEW_WINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTableView>

QT_BEGIN_NAMESPACE

class Ui_new_window
{
public:
    QGridLayout *gridLayout;
    QTableView *tableView;

    void setupUi(QDialog *new_window)
    {
        if (new_window->objectName().isEmpty())
            new_window->setObjectName(QString::fromUtf8("new_window"));
        new_window->resize(722, 450);
        gridLayout = new QGridLayout(new_window);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        tableView = new QTableView(new_window);
        tableView->setObjectName(QString::fromUtf8("tableView"));

        gridLayout->addWidget(tableView, 0, 0, 1, 1);


        retranslateUi(new_window);

        QMetaObject::connectSlotsByName(new_window);
    } // setupUi

    void retranslateUi(QDialog *new_window)
    {
        new_window->setWindowTitle(QApplication::translate("new_window", "Dialog", nullptr));
    } // retranslateUi

};

namespace Ui {
    class new_window: public Ui_new_window {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NEW_WINDOW_H
