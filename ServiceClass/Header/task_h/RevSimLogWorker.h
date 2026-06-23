#ifndef REVSIMLOGWORKER_H
#define REVSIMLOGWORKER_H

#include <HttpSever.h>
#include <QObject>
#include <QUdpSocket>

// ***********************************************************************************************************************************************************************
// 接收模拟器数据的线程任务对象
//  》信号
//      》接收到渔仁端发送的模拟器数据之后发出的信号：void datagramReceived(const QByteArray &data,const QString &sender, quint16 senderPort);
//  》槽函数
//      》接收模拟器数据任务类启动模拟器数据接收：void start();  // 线程启动后调用
//      》接收模拟器数据任务类循环接收数据：void processPendingDatagrams();
//  》成员
//      》用于接收数据的通讯套接字对象：QUdpSocket* socket = nullptr;
//      》用于接收数据的套接字监听的端口号：quint16 listenPort;
// ***********************************************************************************************************************************************************************
class RevSimLogWorker : public QObject {
    Q_OBJECT
public:
    explicit RevSimLogWorker(unsigned short Port, QObject* parent = nullptr);
    ~RevSimLogWorker();

signals:
    void simLogPaurseFinished(const QString simLogMsg);

public slots:
    void working();

private:
    HttpSever * httpFileSever;    ///< 用于接收日志文件的服务器对象
    unsigned short Port;
};

#endif // REVSIMLOGWORKER_H
