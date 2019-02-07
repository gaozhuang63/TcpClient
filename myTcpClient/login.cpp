#include "login.h"
#include "ui_login.h"
#include <QDesktopWidget>
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>
#include "info.h"

#include <synchapi.h>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSysInfo>
#include "QSettings"
#include "QDebug"
#include <QDesktopWidget>
#include <QFileInfoList>
#include <QDir>
#include <QLibrary>
#include <QTimer>
#include <QHostInfo>

#define GB (1024 * 1024 * 1024)
#define MB (1024 * 1024)
#define KB (1024)

login::login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::login)
{
    ui->setupUi(this);


    tcpClient = new QTcpSocket(this);                                       //实例化tcpClient
    tcpClient->abort();                                                     //取消原有连接
    ipAdd="192.168.3.89";
    portd="6666";

    //tcpClient->connectToHost(ipAdd,portd.toInt());   //建立连接
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

QString login::receive(QString a)
{

    configure = a ;
//    qDebug()<<configure ;
}



void login::on_pushButton_clicked()
{
    tcpClient->connectToHost(ipAdd,portd.toInt());   //建立连接
    qDebug() << "登陆，验证中..." ;
    qDebug() << message();
    configure  = message() ;



    QByteArray outBlock ="";                                         //用于暂存我们要发送的数据
    QString data="";
    QDataStream sendOut(&outBlock,QIODevice::WriteOnly);         //使用数据流写入数据
    sendOut.setVersion(QDataStream::Qt_5_0);                     //设置数据流的版本，客户端和服务器端使用的版本要相同

    m_MessageType = Login;
    //sendOut << m_MessageType;

    QString strData = this->ui->lineEdit->text();
    QString strData1 = this->ui->lineEdit_2->text();
    data="#"+strData+"#"+strData1+configure;

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


   // qDebug() << "data.length(): " << outBlock.length() << "SendData:" << outBlock;

    tcpClient->write(outBlock);                                                           //发送给服务端

    data ="";
    qDebug() <<"data:"<<outBlock ;


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
                inBlock.append(tcpClient->readAll());
                qDebug() << "messageLen:" << inBlock.size() << "messageData:" << QString(inBlock);
//                bytesReceived += sizeof(qint64);

                break;
            }

        }
    }

}



QString login::message()
{
    QString msg="#"+localmachineName()+"#"+mac()+"#"+ip()+"#"+memory()+"#"+cpuType()+"#"+osVersion()+"#"+disk()+"#"+screen();

    return msg;
//    login log;
//    connect(this,SIGNAL(send(QString)),&log,SLOT(receive(QString)));
//    emit send(msg);
//    qDebug() << msg ;
//    return msg;
}



const QString login::localmachineName()
{
    QString machineName = QHostInfo::localHostName();
    return machineName;
}

//IP地址
const QString login::ip()
{
    QString ip="";

    QList<QNetworkInterface> interFaceList = QNetworkInterface::allInterfaces();
    for(int i=0; i< interFaceList.size(); i++)
    {
        QNetworkInterface interface = interFaceList.at(i);
        if(interface.flags().testFlag(QNetworkInterface::IsRunning))
        {
            QList<QNetworkAddressEntry> entryList = interface.addressEntries();
            foreach(QNetworkAddressEntry entry, entryList)
            {
                if(QAbstractSocket::IPv4Protocol == entry.ip().protocol() &&
                   entry.ip() != QHostAddress::LocalHost && entry.ip().toString().startsWith("192.168."))
                {
                    ip = entry.ip().toString();
                    break;
                }
            }
        }
    }

    return ip;
}



const QString login::mac()
{
    QString strMac;

    QList<QNetworkInterface> netList = QNetworkInterface::allInterfaces();
    foreach(QNetworkInterface item, netList)
    {
        if((QNetworkInterface::IsUp & item.flags()) && (QNetworkInterface::IsRunning & item.flags()))
        {
            if(strMac.isEmpty() || strMac < item.hardwareAddress())
            {
                strMac = item.hardwareAddress();
            }
        }
    }
    return strMac;
}

const QString& login::cpuType()
{
    QSettings *CPU = new QSettings("HKEY_LOCAL_MACHINE\\HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",QSettings::NativeFormat);
    m_cpuDescribe = CPU->value("ProcessorNameString").toString();
    delete CPU;

    return m_cpuDescribe;
}



#include <Windows.h>
const QString& login::memory()
{
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof (statex);
    GlobalMemoryStatusEx(&statex);
    m_totalMem = statex.ullTotalPhys  * 1.0/ GB;
    m_freeMem = statex.ullAvailPhys * 1.0 / GB;

    m_memDescribe = QString("可用 %1 GB / 共 %2 GB" ).
            arg(QString::asprintf("%.2f", m_freeMem)).arg(QString::asprintf("%.2f", m_totalMem));

    return m_memDescribe;
}

const QString& login::osVersion()
{
    typedef BOOL (WINAPI *LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
    LPFN_ISWOW64PROCESS fnIsWow64Process;
    BOOL bIsWow64 = FALSE;
    fnIsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress( GetModuleHandle(TEXT("kernel32")),"IsWow64Process");
    if (NULL != fnIsWow64Process)
    {
        fnIsWow64Process(GetCurrentProcess(),&bIsWow64);
    }
    QString sysBit = "unknown";
    if(bIsWow64)
        sysBit = "64位";
    else
        sysBit = "32位";

    m_osDescirbe = QSysInfo::prettyProductName() + " " + sysBit;
    return m_osDescirbe;
}

const QString& login::screen()
{
    m_screenDescribe = "";
    QList<QSize> screenSizeList;
    QList <int> screenCountList;

    for(int i=0; i<QApplication::desktop()->screenCount(); i++)
    {
        QRect screenRect = QApplication::desktop()->screenGeometry(i);
        QSize size(screenRect.width(), screenRect.height());

        bool bExist = false;
        for(int j=0; j<screenSizeList.length(); j++)
        {
            QSize existSize = screenSizeList.at(j);
            if(size == existSize)
            {
                screenCountList[j]++;
                bExist = true;
                break;
            }
        }

        if(!bExist)
        {
            screenSizeList.append(size);
            screenCountList.append(1);
        }
    }

    m_maxScreenSize = screenSizeList.at(0);
    for(int i=0; i<screenSizeList.length(); i++)
    {
        int width = screenSizeList.at(i).width();
        int height = screenSizeList.at(i).height();

        if(width > m_maxScreenSize.width() && height > m_maxScreenSize.height())
            m_maxScreenSize = screenSizeList.at(i);

        m_screenDescribe += QString ("(%1像素 x %2像素) x %3个").arg(width).arg(height).arg(screenCountList.at(i));
        if(i!= screenSizeList.length()-1)
            m_screenDescribe += "、 ";
    }

    return m_screenDescribe;
}

const QString& login::disk()
{
    m_diskDescribe = "";
    QFileInfoList list = QDir::drives();
    foreach (QFileInfo dir, list)
    {
        QString dirName = dir.absolutePath();
        dirName.remove("/");
        LPCWSTR lpcwstrDriver = (LPCWSTR)dirName.utf16();
        ULARGE_INTEGER liFreeBytesAvailable, liTotalBytes, liTotalFreeBytes;
        if(GetDiskFreeSpaceEx(lpcwstrDriver, &liFreeBytesAvailable, &liTotalBytes, &liTotalFreeBytes) )
        {
            QString free = QString::number((double) liTotalFreeBytes.QuadPart / GB, 'f', 1);
            free += "G";
            QString all = QString::number((double) liTotalBytes.QuadPart / GB, 'f', 1);
            all += "G";

            QString str = QString("%1 %2/%3       ").arg(dirName, free, all);
            m_diskDescribe += str;

            double freeMem = (double) liTotalFreeBytes.QuadPart / GB;

            if(freeMem > m_maxFreeDisk)
                m_maxFreeDisk = freeMem;
        }
    }

    return m_diskDescribe;
}
