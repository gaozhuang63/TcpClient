#include "login.h"
#include "ui_login.h"
#include <QDesktopWidget>
#include <QMessageBox>
#include <QFileDialog>

login::login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::login)
{
    ui->setupUi(this);
    tcpClient = new QTcpSocket(this);                                       //实例化tcpClient
    tcpClient->abort();                                                     //取消原有连接


    ipAdd="192.168.3.89";
    portd="6666";

    tcpClient->connectToHost(ipAdd,portd.toInt());   //建立连接
    connect(tcpClient,&QTcpSocket::readyRead,this,&login::readDataSlot);                                            //初始化TCP
//    if (tcpClient->waitForConnected(1000))           //等待TCP连接成功函数;连接成功则进入if{}
//    {
//        ui->pushConnect->setText("断开");
//        ui->textEdit->append("连接服务器成功");
//        ui->pushSent->setEnabled(true);
//        this->ui->txtIp->setEnabled(false);
//        this->ui->txtPort->setEnabled(false);
//        this->ui->timeBut->setEnabled(true);
//    }


}

login::~login()
{
    delete ui;
}








void login::on_pushButton_clicked()
{

    qDebug() << "登陆，验证中..." ;

//    tcpClient->write(data.toUtf8());

    QByteArray outBlock;                                         //用于暂存我们要发送的数据
    QDataStream sendOut(&outBlock,QIODevice::WriteOnly);         //使用数据流写入数据
    sendOut.setVersion(QDataStream::Qt_5_0);                     //设置数据流的版本，客户端和服务器端使用的版本要相同

    m_MessageType = Login;
    //sendOut << m_MessageType;

    QString strData = this->ui->lineEdit->text();
    QString strData1 = this->ui->lineEdit_2->text();
    QString data="#"+strData+"#"+strData1;

    if (strData.isEmpty())
    {
        return ;
    }
    if(strData=="" )
    {
        QMessageBox::warning(0,tr("警告"),tr("发送内容不为空"),QMessageBox::Ok);
        return;
    }



    sendOut << quint64(0) << quint64(0);                                                  //标志类型和发送数据总长
    sendOut << data.toUtf8() ;                                    //写入账号密码
    sendOut.device()->seek(0);                                                            //将读写操作指向从头开始
    sendOut << m_MessageType << quint64(outBlock.size()-sizeof(qint64)*2);                 //写入标志和总长


    qDebug() << "data.length(): " << outBlock.length() << "SendData:" << outBlock;

    tcpClient->write(outBlock);                                                           //发送给服务端




    if(this->ui->lineEdit->text().trimmed() == tr("test") &&
            this->ui->lineEdit_2->text().trimmed()== tr("test123"))  //去除lineEdit内的用户名和密码进行校验
    {
        //登陆成功
        Main = new MainWindow(this);
        Main->show();
        this->hide();

    }
    else
    {
//        //用户输入存在错误
//        Dlg_1 = new dlgwait_1(this);
//        Dlg_1->show();
//        Dlg_1->move((wid_x - Dlg_1->width())/2,(wid_y - Dlg_1->height())/2);
//        QTimer::singleShot(3000, this, SLOT(slotHidefailed()));  // 这里是一个3秒定时器， 且只执行一次。

//        //QMessageBox::warning(this,tr("waring"),tr("your passward is wrong"),QMessageBox::Yes);

    }
}

void login::readDataSlot()
{
    QDataStream in(tcpClient);
    in.setVersion(QDataStream::Qt_5_0);

    if (bytesReceived <= sizeof(qint64))
    {
        in >> m_MessageType;
        bytesReceived += sizeof(qint64);
        if(tcpClient->bytesAvailable() <= 0)
        {
            return;
        }

        bytesReceived += tcpClient->bytesAvailable();
        switch(m_MessageType)
        {
            case Login:
            {
                char buf[8196];
                int nSize = tcpClient->bytesAvailable();
                QString strTmp;
                int sizeHead = 0;
                in >> TotalBytes >> sizeHead;
                //nSize = this->bytesAvailable();
                inBlock.append(tcpClient->readAll());
                //this->read(buf,nSize);
                qDebug() << "messageLen:" << inBlock.size() << "messageData:" << QString(inBlock);
//                bytesReceived += sizeof(qint64);
                //nSize = tcpClient->bytesAvailable();
                break;
            }

        }
    }

}
