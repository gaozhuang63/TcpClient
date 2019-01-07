#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    m_MessageType = NONE;

	TotalBytes = 0;
    bytesReceived = 0;


    sendLoadSize = 4*1024;
    sendTotalBytes = 0;
    sendBytesWritten = 0;
    sendBytesToWrite = 0;

    ui->setupUi(this);
    //初始化TCP客户端
    tcpClient = new QTcpSocket(this);                                       //实例化tcpClient
    ui->pushSent->setEnabled(false);
    this->ui->timeBut->setEnabled(false);
    tcpClient->abort();                                                     //取消原有连接
    //connect(tcpClient,&QTcpSocket::readyRead,
    //        [&](){this->ui->textEdit->append(tr("%1 Server Say：%2").arg(QTime::currentTime().toString("hh:mm:ss.zzz")).arg(QString(this->tcpClient->readAll())));});
    
    connect(tcpClient,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(ReadError(QAbstractSocket::SocketError)));   //初始化错误信息

    connect(&tm,&QTimer::timeout,[&](){
            int i = qrand() % 6;
            this->ui->textEdit->append(tr("%1 Timer Sent: %2").arg(QTime::currentTime().toString("hh:mm:ss.zzz")).arg(list.at(i)));
            tcpClient->write(list.at(i).toUtf8());
    });
    connect(tcpClient,&QTcpSocket::disconnected,[](){qDebug()<< "123333" ;});
    connect(tcpClient,SIGNAL(bytesWritten(qint64)),this,SLOT(OnSendFileDataSlot(qint64)));
    connect(tcpClient,&QTcpSocket::readyRead,this,&MainWindow::readDataSlot);                                            //初始化TCP

    list << "我是谁?" << "渡世白玉" << "hello" << "哈哈哈哈哈" << "你是坏蛋!" <<  "测试一下下了" << "不知道写什么" ;
    QTime time;
    time= QTime::currentTime();
    qsrand(time.msec()+time.second()*1000);
    this->ui->txtIp->setText("127.0.0.1");
    this->ui->txtPort->setText("6666");
}

MainWindow::~MainWindow()
{
    delete ui;
    delete tcpClient;
}

void MainWindow::on_pushConnect_clicked()
{
    qDebug() << "点击连接：" ;
    if ("连接" == this->ui->pushConnect->text())
    {
        QString ipAdd(this->ui->txtIp->text()), portd(this->ui->txtPort->text());
        if (ipAdd.isEmpty() || portd.isEmpty())
        {
            this->ui->textEdit->append("请输入IP和端口!");
            return;
        }
        tcpClient->connectToHost(ipAdd,portd.toInt());   //建立连接
        if (tcpClient->waitForConnected(1000))           //等待TCP连接成功函数;连接成功则进入if{}
        {
            ui->pushConnect->setText("断开");
            ui->textEdit->append("连接服务器成功");
            ui->pushSent->setEnabled(true);
            this->ui->txtIp->setEnabled(false);
            this->ui->txtPort->setEnabled(false);
            this->ui->timeBut->setEnabled(true);
        }
    }
    else
    {
        tcpClient->disconnectFromHost();                        //断开TCP连接的函数
        if (tcpClient->state() == QAbstractSocket::UnconnectedState || tcpClient->waitForDisconnected(1000) )       //已断开连接 则进入if{}
        {
            ui->pushConnect->setText("连接");
            ui->textEdit->append("断开服务器");
            ui->pushSent->setEnabled(false);
            this->ui->txtIp->setEnabled(true);
            this->ui->txtPort->setEnabled(true);
            tm.stop();
            this->ui->timeBut->setEnabled(false);
            this->ui->lineEdit->setEnabled(true);
            this->ui->timeBut->setText("启动定时");
        }
    }
}

void MainWindow::on_pushSent_clicked()
{
    qDebug() << "点击发送：" ;
    QString strData = this->ui->txtData->toPlainText();//->text();
    if (strData.isEmpty())
    {
        return ;
    }
//    tcpClient->write(data.toUtf8());
    ui->textEdit->append(tr("Say:%1").arg(strData));



    QByteArray outBlock;
    QDataStream sendOut(&outBlock,QIODevice::WriteOnly);
    sendOut.setVersion(QDataStream::Qt_5_0);

    m_MessageType = Message;
    //sendOut << m_MessageType;

    if(strData=="")
    {
        QMessageBox::warning(0,tr("警告"),tr("发送内容不为空"),QMessageBox::Ok);
        return;
    }

	int nn = outBlock.size();
	nn = outBlock.length();
	
	sendOut << qint64(0) << qint64(0) << strData.toUtf8();
	//sendOut << qint64(0) << qint64(0);

	nn = strData.toUtf8().size();
	nn = strData.toUtf8().length();

	nn = outBlock.size();
	nn = outBlock.length();

	qint64 totalBytes = outBlock.size();

	sendOut.device()->seek(0);
    sendOut << m_MessageType << totalBytes;

	nn = outBlock.size();
	nn = outBlock.length();

	//qint64 totalSize = strData.size()+sizeof(qint64)*2;

    //sendOut << totalSize << strData;

    qDebug() << "data.length(): " << outBlock.length() << "SendData:" << outBlock;

	nn = outBlock.size();
	nn = outBlock.length();

    tcpClient->write(outBlock);
}

void MainWindow::ReadError(QAbstractSocket::SocketError)        //这段函数的作用是：当错误发生时，首先断开TCP连接，再用QMessageBox提示出errorString，即错误原因。
{
    tcpClient->disconnectFromHost();
    ui->pushConnect->setText("连接");
    ui->textEdit->append(tr("连接出错：%1").arg(tcpClient->errorString()));
    ui->pushSent->setEnabled(false);
    this->ui->txtIp->setEnabled(true);
    this->ui->txtPort->setEnabled(true);
    tm.stop();
    this->ui->timeBut->setEnabled(false);
    this->ui->lineEdit->setEnabled(true);
    this->ui->timeBut->setText("启动定时");
}

void MainWindow::on_timeBut_clicked()
{
    if (this->ui->lineEdit->text().isEmpty())
    {
        this->ui->textEdit->append("请输入时间：");
        return;
    }
    if ("启动定时" == this->ui->timeBut->text())
    {
        int h = this->ui->lineEdit->text().toInt();
        h = h*1000;
        tm.start(h);
        this->ui->lineEdit->setEnabled(false);
        this->ui->timeBut->setText("停止定时");
    }
    else
    {
        tm.stop();
        this->ui->timeBut->setText("启动定时");
        this->ui->lineEdit->setEnabled(true);
    }
}

void MainWindow::on_SendFileBtn_clicked()
{
	QString fileName = QFileDialog::getOpenFileName(this);
    //startTransferFile(QStringLiteral("C:\\Users\\Administrator\\Desktop\\新建文本文档.txt"));
	startTransferFile(fileName);
}

void MainWindow::startTransferFile(QString strFileName)//实现文件大小等信息的发送
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
	//currentFileName = "test.txt";
    qDebug() << currentFileName;

    m_MessageType = FileName;

    //依次写入类型、总大小信息、文件名信息大小空间，文件名
    sendOut << qint64(0) << qint64(0) << qint64(0) << currentFileName.toUtf8();

    //这里总大小是文件名大小等信息和实际文件大小的总和
    sendTotalBytes += outBlock.size();

	qDebug() << "FileSize:" << sendTotalBytes;

    //返回outBlock开始，用实际信息代替两个qint64空间
    sendOut.device()->seek(0);
//    sendOut << sendTotalBytes << qint64(outBlock.size()-sizeof(qint64)*2);
    //sendOut << m_MessageType << sendTotalBytes;
	sendOut << m_MessageType << sendTotalBytes << qint64(outBlock.size()-sizeof(qint64)*3);

    //发送数据，并计算发送完数据后剩余数据的大小
    sendBytesToWrite = sendTotalBytes - tcpClient->write(outBlock);

//    ui->clientStatusLabel->setText(tr("已连接"));
    outBlock.resize(0);
}

void MainWindow::OnSendFileDataSlot(qint64 numBytes)
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
        sendBytesToWrite -= (int)tcpClient->write(outBlock);
        outBlock.resize(0);//清空发送缓冲区
    }
    else
    {
        //sendFile->close();//如果没有发送任何数据，则关闭文件
    }

    //更新进度条
//    ui->clientProgressBar->setMaximum(totalBytes);
//    ui->clientProgressBar->setValue(sendBytesWritten);

    if(sendBytesWritten >= sendTotalBytes)//发送完毕
    {
//        ui->clientStatusLabel->setText(tr("发送文件%1成功").arg(fileName));
        sendFile->close();

        sendTotalBytes = 0;
        sendBytesWritten = 0;
        sendBytesToWrite = 0;
    }

//    QThread::sleep(1);////////////////
}

void MainWindow::openFile()//打开文件
{
    //fileName = QFileDialog::getOpenFileName(this);
    //if(!fileName.isEmpty())
    //{
    //}
}

void MainWindow::readDataSlot()
{
 //   QDataStream in(tcpClient);
 //   in.setVersion(QDataStream::Qt_5_0);


	//qint64 a,b;
	//int d;
	//in >> a >> b >> d;

	//if(tcpClient->bytesAvailable() <= 0)
	//{
	//	return;
	//}

	//this->ui->textEdit->append(tr("%1 Server Say: %2").arg(QTime::currentTime().toString("hh:mm:ss")).arg(QString(this->tcpClient->readAll())));

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
            case Message:
            {
				char buf[8196];
				int nSize = tcpClient->bytesAvailable();
				QString strTmp;
				int sizeHead = 0;
                in >> TotalBytes >> sizeHead;
				//nSize = this->bytesAvailable();
                inBlock.append(tcpClient->readAll());
				//this->read(buf,nSize);
				//qDebug() << "messageLen:" << inBlock.size() << "messageData:" << QString(inBlock);
//                bytesReceived += sizeof(qint64);
				//nSize = tcpClient->bytesAvailable();
                break;
            }
            case FileName:
            {
                int sizeHead = 0;
				qint64 headSize = 0;

				char buf[256];
				in >> TotalBytes >> headSize >> sizeHead;
				int nSize = tcpClient->bytesAvailable();

				memset(buf,0,256);
				tcpClient->read(buf,headSize);
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

				if(tcpClient->bytesAvailable() > 0)
				{
					inBlock = tcpClient->readAll();
					localFile->write(inBlock);
					inBlock.resize(0);
				}

				//QString strMess;
				//strMess += "Start Recv File: ";
				//strMess += fileName;
				//emit readDataSig(socketID,this->peerAddress().toString(),this->peerPort(),strMess.toUtf8());

                break;
            }
        }
    }

    if (bytesReceived < TotalBytes && tcpClient->bytesAvailable() > 0)
    {
        bytesReceived += tcpClient->bytesAvailable();
        switch(m_MessageType)
        {
            case Message:
            {
                inBlock.append(tcpClient->readAll());
                qDebug() << "messageLen:" << inBlock.size() << "messageData:" << QString(inBlock);
				break;
            }
            case FileName:
            {
                inBlock = tcpClient->readAll();
                localFile->write(inBlock);
                inBlock.resize(0);
                break;
            }
        }
    }

    if(bytesReceived >= TotalBytes)
    {
        switch(m_MessageType)
        {
            case Message:
            {
				//emit readDataSig(socketID,this->peerAddress().toString(),this->peerPort(),inBlock);
				this->ui->textEdit->append(tr("%1 Server Say: %2").arg(QTime::currentTime().toString("hh:mm:ss")).arg(QString(inBlock)));
                
				//if(QString(inBlock).compare("file") == 0)
				//{
				//	startTransferFile("D:\\TestServerFile.txt");
				//}
                inBlock.resize(0);
                break;
            }
            case FileName:
            {
				QString strMess;
				strMess += "File: ";
				strMess += fileName;
				strMess += "Received!";
				//emit readDataSig(socketID,this->peerAddress().toString(),this->peerPort(),strMess.toUtf8());
                localFile->close();
                delete localFile;
                break;
            }
        }

		TotalBytes = 0;
		bytesReceived = 0;
		m_MessageType = NONE;
    }

}
