#include <task_h/DataParseTask.h>
#include <QDebug>
#include <QThread>
#include <ReplyJsonMsg.h>


DataParseTask::DataParseTask(const QByteArray &datagram,QObject *parent) : QObject(parent)
  ,data(datagram)
{
    setAutoDelete(true);
}

DataParseTask::~DataParseTask(){
}

/**任务函数
 * @brief DataParseTask::run
 */
void DataParseTask::run()
{
    if (this->data.size() != sizeof(SimulatorData)) {
        qDebug() << "协议数据格式错误!!!!"<<this->data.size()<<sizeof (SimulatorData);
        return;
    }
    SimulatorData simulator_data; // 将 QByteArray 转成结构体
    memcpy(&simulator_data, data.constData(), sizeof(SimulatorData));
    // qInfo()<<QThread::currentThread()<<"正在解析模拟器状态数据";

    SimulatorJsonMsg msg(simulator_data); // 转换为 JSON 消息
    if(!msg.fromJson(QString::fromUtf8(msg.toJson()))){
        // SimulatorJsonMsg msg_error;
       qDebug() << "协议数据格式错误!!!!";
        // emit this->parseDataFinish(simulator_data.lineNo,msg_error);
    }else{ // 发信号给主线程处理
       // qDebug() << "协议数据处理成功-----";
        emit this->parseDataFinish(simulator_data.lineNo,msg);
    }
}
