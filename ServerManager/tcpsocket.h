#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <QTcpSocket>
#include <QQueue>
#include <QFutureWatcher>
#include <QByteArray>
#include <QFile>
#include <QTime>

//=====枚举标志信息类型，分别为登陆、消息、文件==============
enum MessageType{Login,Message,FileName,NONE};

class TcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit TcpSocket(qintptr socketDescriptor, QObject *parent = 0);
    ~TcpSocket();
    QByteArray handleData(QByteArray data,const QString & ip, qint16 port);//用来处理数据的函数

signals:
    void readDataSig(const int,const QString &,const quint16,const QByteArray &);
    void sockDisConnect(const int ,const QString &,const quint16, QThread *);//NOTE:断开连接的用户信息，此信号必须发出！线程管理类根据信号计数的

public slots:
    void sentData(const QByteArray & ,const int);//发送信号的槽
    void disConTcpSlot(int i);
	void OnSendFileDataSlot(qint64);

protected slots:
    void readDataSlot();//接收数据
    void startNext();//处理下一个

private:
	void startTransferFile(QString strFileName);
	void SendMessage(QString strMessage);

protected:
    QFutureWatcher<QByteArray> watcher;
    QQueue<QByteArray> datas;

private:
    qintptr socketID;
    QMetaObject::Connection dis;

    qint64 m_MessageType;

	//接收文件参数
    qint64 TotalBytes;
    qint64 bytesReceived;
    qint64 bytesToReceive;
    QString fileName;
    QFile *localFile;
    QByteArray inBlock;
    QString log_pas;

	//发送文件参数
    QFile *sendFile;//要发送的文件
    qint64 sendTotalBytes;//数据总大小
    qint64 sendBytesWritten;//已经发送数据大小
    qint64 sendBytesToWrite;//剩余数据大小
    qint64 sendLoadSize;//每次发送数据大小
    QByteArray outBlock;//数据缓冲区，即存放每次要发送的数据
};

#endif // TCPSOCKET_H
