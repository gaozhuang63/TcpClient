#include "info.h"
#include "ui_info.h"

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
#include "login.h"
#define GB (1024 * 1024 * 1024)
#define MB (1024 * 1024)
#define KB (1024)


Info::Info(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Info)
{
    ui->setupUi(this);
//    qDebug() << "计算机名称:\t" << localmachineName();
//    qDebug() << "Mac地址:\t" << mac();
//    qDebug() << "IP:\t" << ip();
//    qDebug() << "内存:\t" << memory();
//    qDebug() << "Cpu型号:\t" << cpuType();
//    qDebug() << "操作系统:\t" << osVersion();
//    qDebug() << "硬盘:\t" << disk();
//    qDebug() << "屏幕分辨率:\t" <<screen();



}

Info::~Info()
{
    delete ui;
}

QString Info::message()
{
    QString msg="#"+localmachineName()+"#"+mac()+"#"+ip()+"#"+memory()+"#"+cpuType()+"#"+osVersion()+"#"+disk()+"#"+screen();

    login log;
    connect(this,SIGNAL(send(QString)),&log,SLOT(receive(QString)));
    emit send(msg);
//    qDebug() << msg ;
//    return msg;
}



const QString Info::localmachineName()
{
    QString machineName = QHostInfo::localHostName();
    return machineName;
}

//IP地址
const QString Info::ip()
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



const QString Info::mac()
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

const QString& Info::cpuType()
{
    QSettings *CPU = new QSettings("HKEY_LOCAL_MACHINE\\HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0",QSettings::NativeFormat);
    m_cpuDescribe = CPU->value("ProcessorNameString").toString();
    delete CPU;

    return m_cpuDescribe;
}



#include <Windows.h>
const QString& Info::memory()
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

const QString& Info::osVersion()
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

const QString& Info::screen()
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

const QString& Info::disk()
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
