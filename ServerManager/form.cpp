#include "form.h"
#include "ui_form.h"
#include <QApplication>

Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);


    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");   //数据库驱动类型为SQL Server
    db.setDatabaseName("QTDSN");


    if(!db.open())

    {

    //错误处理

    }
    else
    {
        qDebug()<<"database open success!";
    }

    static QSqlQueryModel *model = new QSqlQueryModel(ui->tableView);


    model->setQuery(QString("select * from Information"));

    model->setHeaderData(0,Qt::Horizontal,QObject::tr("Name"));

    model->setHeaderData(1,Qt::Horizontal,QObject::tr("charge"));

    model->setHeaderData(2,Qt::Horizontal,QObject::tr("IP_Address"));

    model->setHeaderData(3,Qt::Horizontal,QObject::tr("login"));

    model->setHeaderData(4,Qt::Horizontal,QObject::tr("ID"));

    model->setHeaderData(5,Qt::Horizontal,QObject::tr("Password"));

    ui->tableView->setModel(model);

   // db.close();


}

Form::~Form()
{
    delete ui;
}
