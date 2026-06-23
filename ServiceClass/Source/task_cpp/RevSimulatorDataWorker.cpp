#include <task_h/RevSimulatorDataWorker.h>

#include <QDateTime>
#include <QThread>
#include <StructClsss.h>



RevSimulatorDataWorker::RevSimulatorDataWorker(quint16 listen_port, QObject *parent) : QObject(parent),
    listenPort(listen_port)
{

}

RevSimulatorDataWorker::~RevSimulatorDataWorker()
{
    this->socket->close();
    this->socket->deleteLater();
}

void RevSimulatorDataWorker::start()
{
    socket = new QUdpSocket();
    if (!socket->bind(QHostAddress::AnyIPv4, listenPort,QUdpSocket::ReuseAddressHint)) {
        qCritical() << "UdpWorker: UDP 绑定失败, 端口:" << listenPort<< " 错误:" << socket->errorString();
        return;
    }
    connect(socket, &QUdpSocket::readyRead,this, &RevSimulatorDataWorker::processPendingDatagrams);
    qDebug() << "UdpWorker: 启动成功, 监听端口:" << listenPort<< " 所在线程:" << QThread::currentThread();
}

/**
 * 接受数据槽函数
 * @brief RevSimulatorDataWorker::processPendingDatagrams
 */
void RevSimulatorDataWorker::processPendingDatagrams()
{
    while (this->socket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(this->socket->pendingDatagramSize());

        QHostAddress sender;
        quint16 senderPort;
        this->socket->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);
       // qDebug() << "收到来自" << sender.toString() << ":" << senderPort << "的数据:" << datagram.size(); // 这里可以发信号将数据传到主显示界面做显示
        QString sender_str = sender.toString(); // 判断ip的格式是ipv6还是ipv4
        if (sender.toString().startsWith("::ffff:")) {
            sender_str = sender.toString().mid(7); // 去掉 ::ffff: 前缀
        }

        SimulatorData simulator_data;
        memcpy(&simulator_data, datagram.constData(), sizeof(SimulatorData));
        emit this->datagramReceived(datagram,sender_str,senderPort);        // 发出收到数据的信号
    }
}

quint16 RevSimulatorDataWorker::getListenPort() const
{
    return listenPort;
}

void RevSimulatorDataWorker::setListenPort(const quint16 &value)
{
    // 需要重新绑定端口
    this->socket->close(); // 解除原来的绑定
    if (this->socket->bind(QHostAddress::Any, value)) {
        qDebug() << "成功绑定新端口 :"+QString::number(value);
    } else {
        qDebug() << "绑定新端口失败";
    }
    listenPort = value;
}
