#ifndef SENDATSDATAWORKER_H
#define SENDATSDATAWORKER_H

#include <QObject>
#include <QUdpSocket>
#include <StructClsss.h>

#include <ATSJsonMsg.h>

// ***********************************************************************************************************************************************************************
//  》槽函数
//      》用于向模拟器发送ATS数据的槽函数：void sendDatagram(const ATSJsonMsg * ats_json_data, const QString& ip, quint16 port);
//  》成员
//      》用于发送ATS数据的套接字对象：socket
//      》要发送的ATS数据存储对象：data
// ***********************************************************************************************************************************************************************
class SendATSDataWorker : public QObject{
    Q_OBJECT
public:
    explicit SendATSDataWorker(unsigned short listen_port,QObject * parent = nullptr);
    ~SendATSDataWorker();

public slots:
    void sendDatagram(const ATSJsonMsg * ats_json_data, const QString& ip, quint16 port);

private:
    QUdpSocket* socket = nullptr;
    QByteArray data;

};

#endif // SENDATSDATAWORKER_H
