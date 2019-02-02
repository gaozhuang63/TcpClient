#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include "mainwindow.h"
#include <QtCore>
#include <QtNetwork>


namespace Ui {
class login;
}



class login : public QWidget
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = nullptr);
    ~login();

    MainWindow * Main;
    //基本信息
    const QString localmachineName();//获取cpu 信息
    const QString& cpuType();//获取cpu 信息
    const QString dCard();//显卡信息
    const QString& memory();//内存信息
    const QString& osVersion();//操作系统信息
    const QString& screen();//屏幕分辨率信息
    const QString& disk();//硬盘信息
    const QString ip();//Ip信息
    QString message();

    //其他信息
    //获取硬盘号
    const QString diskId();
    //获取MAC地址
    const QString mac();
    //公网ip
    const QString publicIp();

public slots:
    QString receive(QString);

private slots:
    void on_pushButton_clicked();
    void readDataSlot();//接收数据

signals:
    void rec();

private:
    Ui::login *ui;    
    QTcpSocket * tcpClient;
    QString ipAdd;
    QString portd;
    QStringList list;
    qint64 m_MessageType;
    qint64 TotalBytes;
    qint64 bytesReceived;
    QByteArray inBlock;
    QString configure ;

    float m_freeMem;
    float m_totalMem;
    double m_maxFreeDisk;//GB
    QSize m_maxScreenSize;

    //检查成功结果描述
    QString m_cpuDescribe;
    QString m_memDescribe;
    QString m_osDescirbe;
    QString m_screenDescribe;
    QString m_diskDescribe;
    QString m_officeDecsribe;

};

#endif // LOGIN_H
