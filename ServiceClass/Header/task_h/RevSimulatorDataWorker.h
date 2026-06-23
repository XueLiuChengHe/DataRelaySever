#ifndef REVSIMULATORDATAWORKER_H
#define REVSIMULATORDATAWORKER_H

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
class RevSimulatorDataWorker : public QObject {
    Q_OBJECT
public:
    explicit RevSimulatorDataWorker(quint16 port, QObject* parent = nullptr);
    ~RevSimulatorDataWorker();

    quint16 getListenPort() const;
    void setListenPort(const quint16 &value);

signals:
    void datagramReceived(const QByteArray &data,const QString &senderIP, quint16 senderPort);

public slots:
    void start();  // 线程启动后调用
    void processPendingDatagrams();

private:
    QUdpSocket* socket = nullptr;
    quint16 listenPort;
};

#endif // REVSIMULATORDATAWORKER_H
