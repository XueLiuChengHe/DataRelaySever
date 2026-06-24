#include <task_h/SendATSDataWorker.h>

#include <ATSConstrolMsg.h>
#include <QThread>


SendATSDataWorker::SendATSDataWorker(unsigned short listen_port,QObject *parent) :
    QObject(parent),
    socket(nullptr)
{

}

SendATSDataWorker::~SendATSDataWorker()
{
    this->socket->close();
    this->socket->deleteLater();
}

void SendATSDataWorker::sendDatagram(const ATSJsonMsg * ats_json_data, const QString &ip, quint16 port)
{
    if(!this->socket){
        this->socket = new QUdpSocket();
        qInfo() << QThread::currentThread() <<": 创建发送ATS信息的套接字,IP"<<ip<<"端口"<<port;
    }

    qint64 sent = -1;
    if(port == 7431){
        this->data = QByteArray::fromRawData(reinterpret_cast<const char*>(&ats_json_data->ats_data), sizeof(ATSData));
        sent = this->socket->writeDatagram(this->data, QHostAddress(ip), port);
    }
    if(port == 61004){
        this->data = ats_json_data->atsControlMsg.toJson();
        sent = this->socket->writeDatagram(this->data, QHostAddress(ip), port);
    }

    if (sent == -1) {
        // 立刻失败（网卡没插、UDP 套接字未绑定、参数非法……）
        qDebug() << "send error:" << socket->errorString();
    } else if (sent != data.size()) {
        // 这种情况理论上不会出现，一旦出现说明系统层只“吞”了一部分数据
        qDebug() << "incomplete send" << sent << "/" << data.size();
    }else{
        // qDebug()<<"发送成功";
    }
}
