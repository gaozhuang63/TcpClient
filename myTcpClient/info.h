#ifndef INFO_H
#define INFO_H

#include <QWidget>



namespace Ui {
class Info;
}

class Info : public QWidget
{
    Q_OBJECT

public:
    explicit Info(QWidget *parent = nullptr);
    ~Info();

public:
    //基本信息
    const QString localmachineName();//获取cpu 信息
    const QString& cpuType();//获取cpu 信息
    const QString dCard();//显卡信息
    const QString& memory();//内存信息
    const QString& osVersion();//操作系统信息
    const QString& screen();//屏幕分辨率信息
    const QString& disk();//硬盘信息
    const QString ip();//Ip信息


    //其他信息
    //获取硬盘号
    const QString diskId();
    //获取MAC地址
    const QString mac();
    //公网ip
    const QString publicIp();

public slots:
    QString message();

signals:
    void send(QString);

private:
    QString getHtml(QString url);

private:
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

private:
    Ui::Info *ui;
};

#endif // INFO_H
