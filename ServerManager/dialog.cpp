#include "dialog.h"
#include "ui_dialog.h"
#include <QMouseEvent>
#include <QPropertyAnimation>
#include "tcpserver.h"
#include <QDebug>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

//    setWindowFlags(Qt::FramelessWindowHint);//无边框
//    setAttribute(Qt::WA_TranslucentBackground);//背景透明
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);//任务栏使程序最小化

    Init();
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::Init()
{
    TcpServer *ser = new TcpServer();
    connect(ser,&TcpServer::connectClientSig,this,&Dialog::OnConnectClientSlot);
    connect(ser,&TcpServer::readDataSig,this,&Dialog::OnReadDataSlot);
    connect(ser,&TcpServer::sockDisConnectSig,this,&Dialog::OnsockDisConnectSlot);
    ser->listen(QHostAddress::Any,6666);
    ui->messageBrowser->append("listen");
}

void Dialog::OnConnectClientSlot(const int , const QString &strIP ,const quint16 nPort)
{
    QString str;
    str += "IP:";
    str += strIP;
    str += "Port:";
    str += QString::number(nPort);
    qDebug() << str;
    ui->messageBrowser->append(str);
}

void Dialog::OnReadDataSlot(const int,const QString &strIP, quint16, const QByteArray &strData)
{
    QString str;
    str += "IP:";
    str += strIP;
    str += "Data:";
    str += strData;
    qDebug() << str;
    ui->messageBrowser->append(str);
}

void Dialog::OnsockDisConnectSlot(int ,QString strIP ,quint16)
{
    QString str;
    str += "IP:";
    str += strIP;
    str += "DisConnect!";
    qDebug() << str;
    ui->messageBrowser->append(str);
}

void Dialog::mousePressEvent(QMouseEvent *event)
{
    if(event->buttons() == Qt::LeftButton)
    {
        //记录窗口此时的坐标
        StartPos = event->globalPos() - this->frameGeometry().topLeft();
    }
}
//---------------------虚框------------------------------
void Dialog::mouseMoveEvent(QMouseEvent *event)
{
    //去表格虚框
    QPoint EndPos;
    if ( event->buttons() == Qt::LeftButton )
    {
        EndPos = event->globalPos() - StartPos;
        this->move(EndPos);
    }
}

void Dialog::on_closeButton_clicked()
{
    /* 慢慢消失的效果 */
    QPropertyAnimation *animation = new QPropertyAnimation(this,"windowOpacity");
    animation->setDuration(1000);
    animation->setStartValue(1);
    animation->setEndValue(0);
    animation->start();
    connect(animation,SIGNAL(finished()),this,SLOT(close()));//效果显示完后关闭
}

void Dialog::on_minButton_clicked()
{
    this->showMinimized();//最小化
}

void Dialog::on_pushButton_clicked()
{
    new_win = new Form;
    new_win->show();
}
