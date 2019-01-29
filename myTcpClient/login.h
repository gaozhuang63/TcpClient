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





private slots:
    void on_pushButton_clicked();
    void readDataSlot();//接收数据

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

};

#endif // LOGIN_H
