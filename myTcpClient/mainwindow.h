#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore>
#include <QtNetwork>

namespace Ui {
class MainWindow;
}

//=====枚举标志信息类型，分别为消息、文件==============
enum MessageType{Message,FileName,NONE};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void ReadError(QAbstractSocket::SocketError);

    void on_pushSent_clicked();
    void on_pushConnect_clicked();
    void on_timeBut_clicked();

private slots:
    void on_SendFileBtn_clicked();
	void OnSendFileDataSlot(qint64 numBytes);
	void readDataSlot();//接收数据

private:
    void startTransferFile(QString strFileName);
	void openFile();//打开文件

private:
    Ui::MainWindow * ui;
    QTcpSocket * tcpClient;
    QTimer tm;
    QStringList list;
    qint64 m_MessageType;

    //发送文件参数
    QFile *sendFile;//要发送的文件
    qint64 sendTotalBytes;//数据总大小
    qint64 sendBytesWritten;//已经发送数据大小
    qint64 sendBytesToWrite;//剩余数据大小
    qint64 sendLoadSize;//每次发送数据大小
//    QString fileName;//保存文件路径
    QByteArray outBlock;//数据缓冲区，即存放每次要发送的数据


	//接收文件参数
    qint64 TotalBytes;
    qint64 bytesReceived;
    //qint64 bytesToReceive;
    QString fileName;
    QFile *localFile;
    QByteArray inBlock;
};

#endif // MAINWINDOW_H
