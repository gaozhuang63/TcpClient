#include "tcpsocket.h"
#include <QtConcurrent/QtConcurrent>
#include <QHostAddress>
#include <QDebug>
#include <QMessageBox>

TcpSocket::TcpSocket(qintptr socketDescriptor, QObject *parent) : //构造函数在主线程执行，lambda在子线程
    QTcpSocket(parent),socketID(socketDescriptor)
{
    TotalBytes = 0;
    bytesReceived = 0;
//    fileNameSize = 0;
    m_MessageType = NONE;

	sendLoadSize = 4*1024;
    sendTotalBytes = 0;
    sendBytesWritten = 0;
    sendBytesToWrite = 0;

    this->setSocketDescriptor(socketDescriptor);
    connect(this,&TcpSocket::readyRead,this,&TcpSocket::readDataSlot);
    dis = connect(this,&TcpSocket::disconnected,
        [&](){
            qDebug() << "disconnect" ;
            emit sockDisConnect(socketID,this->peerAddress().toString(),this->peerPort(),QThread::currentThread());//发送断开连接的用户信息
            this->deleteLater();
        });
	connect(this,SIGNAL(bytesWritten(qint64)),this,SLOT(OnSendFileDataSlot(qint64)));

    connect(&watcher,&QFutureWatcher<QByteArray>::finished,this,&TcpSocket::startNext);
    connect(&watcher,&QFutureWatcher<QByteArray>::canceled,this,&TcpSocket::startNext);
	
    qDebug() << "new connect" ;
}

TcpSocket::~TcpSocket()
{
}


void TcpSocket::sentData(const QByteArray &arrData, const int id)
{
    if(id == socketID)
    {
        write(arrData);
    }
}

void TcpSocket::disConTcpSlot(int i)
{
    if (i == socketID)
    {
        this->disconnectFromHost();
    }
    else if (i == -1) //-1为全部断开
    {
        disconnect(dis); //先断开连接的信号槽，防止二次析构
        this->disconnectFromHost();
        this->deleteLater();
    }
}

void TcpSocket::readDataSlot()
{
/*    QByteArray RecvData = this->readAll();                                                                            //注释头
    auto SendData  = handleData(RecvData,this->peerAddress().toString(),this->peerPort());

    qDebug() << "RecvData: " << RecvData;
    this->write(SendData);
    emit readDataSig(socketID,this->peerAddress().toString(),this->peerPort(),RecvData);                            */  //注释尾









    QDataStream in(this);
    in.setVersion(QDataStream::Qt_5_0);

//    float useTime = time.elapsed();

    if (bytesReceived <= sizeof(qint64))
    {
        in >> m_MessageType;
        bytesReceived += sizeof(qint64);
		if(this->bytesAvailable() <= 0)
		{
			return;
		}

		bytesReceived += this->bytesAvailable();
        switch(m_MessageType)
        {
            case Message:
            {
				char buf[8196];
				int nSize = this->bytesAvailable();
				QString strTmp;
				int sizeHead = 0;
                in >> TotalBytes >> sizeHead;
				//nSize = this->bytesAvailable();
                inBlock.append(this->readAll());
				//this->read(buf,nSize);
				qDebug() << "messageLen:" << inBlock.size() << "messageData:" << QString(inBlock);
//                bytesReceived += sizeof(qint64);
				nSize = this->bytesAvailable();
                break;
            }
            case FileName:
            {
                int sizeHead = 0;
				qint64 headSize = 0;

				char buf[256];
				in >> TotalBytes >> headSize >> sizeHead;
				int nSize = this->bytesAvailable();

				memset(buf,0,256);
				this->read(buf,headSize);
				fileName = QString::fromStdString(buf);

                //in >> TotalBytes >> fileName;
				//fileName = QString::fromStdString(fileName);
                qDebug() << "fileName:" << fileName;
//                bytesReceived += sizeof(qint64);
//                bytesReceived += fileName.size();
                localFile = new QFile(fileName);
                if(!localFile->open(QIODevice::WriteOnly | QIODevice::Append))
                {
//                    QMessageBox::warning(this,QString("exe"),QString("not read %1:\n%2.").arg(fileName).arg(localFile->errorString()));
                    qDebug() << "fileName:" << fileName << "Open False!";
                    return;
                }

				if(this->bytesAvailable() > 0)
				{
					inBlock = this->readAll();
					localFile->write(inBlock);
					inBlock.resize(0);
				}

				QString strMess;
				strMess += "Start Recv File: ";
				strMess += fileName;
				emit readDataSig(socketID,this->peerAddress().toString(),this->peerPort(),strMess.toUtf8());

                break;
            }
        }

		//return;
    }

    if (bytesReceived < TotalBytes && this->bytesAvailable() > 0)
    {
        bytesReceived += this->bytesAvailable();
        switch(m_MessageType)
        {
            case Message:
            {
                inBlock.append(this->readAll());
                qDebug() << "messageLen:" << inBlock.size() << "messageData:" << QString(inBlock);
                break;
            }
            case FileName:
            {
                inBlock = this->readAll();
                localFile->write(inBlock);
                inBlock.resize(0);
                break;
            }
        }
    }

//    double speed = bytesReceived / useTime;
//    ui->tcpClientStatusLabel->setText(tr("已接收 %1MB (%2MB/s) "
//                                         "\n共%3MB 已用时:%4秒\n估计剩余时间：%5秒")
//                                      .arg(bytesReceived / (1024*1024))
//                                      .arg(speed*1000/(1024*1024),0,'f',2)
//                                      .arg(TotalBytes / (1024 * 1024))
//                                      .arg(useTime/1000,0,'f',0)
//                                      .arg(TotalBytes/speed/1000 - useTime/1000,0,'f',0));

    if(bytesReceived >= TotalBytes)
    {
        switch(m_MessageType)
        {
            case Message:
            {
				emit readDataSig(socketID,this->peerAddress().toString(),this->peerPort(),inBlock);
				if(QString(inBlock).compare("file") == 0)
				{
                    startTransferFile("E:\\qt_tcp_cs_file\\ServerManager\\1111.txt");                                                //服务端向客户端发送文件位置 D:\\TestServerFile.txt
					TotalBytes = 0;
					bytesReceived = 0;
					inBlock.resize(0);
					return;
				}
                inBlock.resize(0);

				QString strMessage;
				strMessage += "The Data Recv Ok!";
				SendMessage(strMessage);
                break;
            }
            case FileName:
            {
				QString strMess;
				strMess += "File: ";
				strMess += fileName;
				strMess += "Received!";
				emit readDataSig(socketID,this->peerAddress().toString(),this->peerPort(),strMess.toUtf8());
                localFile->close();
                delete localFile;

				QString strMessage;
				strMessage += "File:";
				strMessage += fileName;
				strMessage += "Recv Ok!";
				SendMessage(strMessage);
                break;
            }
        }

		TotalBytes = 0;
		bytesReceived = 0;
		m_MessageType = NONE;

//        tcpClient->close();
//        ui->tcpClientStatusLabel->setText(tr("接收文件 %1 完毕")
//                                          .arg(fileName));
    }










//    if (!watcher.isRunning())//放到异步线程中处理。
//    {
//        watcher.setFuture(QtConcurrent::run(this,&TcpSocket::handleData,datas.dequeue(),this->peerAddress().toString(),this->peerPort()));
//    }
}

void TcpSocket::SendMessage(QString strMessage)
{
	QByteArray outBlock;
    QDataStream sendOut(&outBlock,QIODevice::WriteOnly);
    sendOut.setVersion(QDataStream::Qt_5_0);

    m_MessageType = Message;
    //sendOut << m_MessageType;

    if(strMessage=="")
    {
        QMessageBox::warning(0,tr("警告"),tr("发送内容不为空"),QMessageBox::Ok);
        return;
    }

	int nn = outBlock.size();
	nn = outBlock.length();
	
	sendOut << qint64(0) << qint64(0) << strMessage.toUtf8();

	qint64 totalBytes = outBlock.size();

	sendOut.device()->seek(0);
    sendOut << m_MessageType << totalBytes;

    qDebug() << "data.length(): " << outBlock.length() << "SendData:" << outBlock;

    this->write(outBlock);
}

QByteArray TcpSocket::handleData(QByteArray arrData, const QString &ip, qint16 port)
{
    QTime time;
    time.start();

    QElapsedTimer tm;
    tm.start();
    while(tm.elapsed() < 100)
    {}
    arrData = ip.toUtf8() + " " + QByteArray::number(port) + " " + arrData + " " + QTime::currentTime().toString().toUtf8();
    return arrData;
}

void TcpSocket::startNext()
{
    this->write(watcher.future().result());
    if (!datas.isEmpty())
    {
        watcher.setFuture(QtConcurrent::run(this,&TcpSocket::handleData,datas.dequeue(),this->peerAddress().toString(),this->peerPort()));
    }
}

void TcpSocket::startTransferFile(QString strFileName)//实现文件大小等信息的发送
{
	strFileName = strFileName.replace("\\","/");
    sendFile = new QFile(strFileName);
    if(!sendFile->open(QFile::ReadOnly))
    {
        qDebug() << "open file error";
        return;
    }

    sendTotalBytes = sendFile->size();//文件总大小

    QDataStream sendOut(&outBlock,QIODevice::WriteOnly);
    sendOut.setVersion(QDataStream::Qt_5_0);
    QString currentFileName = strFileName.right(strFileName.size() - strFileName.lastIndexOf('/')-1);
    qDebug() << currentFileName;

    m_MessageType = FileName;

    //依次写入类型、总大小信息、文件名信息大小空间，文件名
    sendOut << qint64(0) << qint64(0) << qint64(0) << currentFileName.toUtf8();

    //这里总大小是文件名大小等信息和实际文件大小的总和
    sendTotalBytes += outBlock.size();
	qDebug() << "ServerFileSize:" << sendTotalBytes;

    //返回outBlock开始，用实际信息代替两个qint64空间
    sendOut.device()->seek(0);
	sendOut << m_MessageType << sendTotalBytes << qint64(outBlock.size()-sizeof(qint64)*3);

    //发送数据，并计算发送完数据后剩余数据的大小
    sendBytesToWrite = sendTotalBytes - this->write(outBlock);

    outBlock.resize(0);
}

void TcpSocket::OnSendFileDataSlot(qint64 numBytes)
{
	if(m_MessageType != FileName)
	{
		return;
	}

    sendBytesWritten += (int)numBytes;//已经发送数据的大小

    if(sendBytesToWrite > 0)//如果已经发送了数据
    {
        //每次发送sendLoadSize大小的数据，这里设置为4KB，如果剩余的数据不足4KB，就发送剩余数据的大小
        outBlock = sendFile->read(sendLoadSize);
        //发送完一次数据后剩余数据的大小
        sendBytesToWrite -= (int)this->write(outBlock);
        outBlock.resize(0);//清空发送缓冲区
    }
    else
    {
        //sendFile->close();//如果没有发送任何数据，则关闭文件
    }

    if(sendBytesWritten >= sendTotalBytes)//发送完毕
    {
        sendFile->close();

        sendTotalBytes = 0;
        sendBytesWritten = 0;
        sendBytesToWrite = 0;
    }
}
