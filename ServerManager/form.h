#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include <QApplication>
#include <QSql>
#include "qsql.h"
#include <QDialog>
#include <QDebug>
#include <QSqlQueryModel>
#include <QSqlError>
#include <QSqlDatabase>
#include <QSqlQuery>

namespace Ui {
class Form;
}

class Form : public QWidget
{
    Q_OBJECT

public:
    explicit Form(QWidget *parent = nullptr);
    ~Form();


private:
    Ui::Form *ui;
};

#endif // FORM_H
