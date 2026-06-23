#include "sessions_h/ATSControlSession.h"

// #include <task_h/DataParseTask.h>

// #include <QThreadPool>

ATSControlSession::ATSControlSession(QObject * parent) : QObject(parent),
    // sessionInstranceID(""),
    // sessionStatus(false),
    // vtrainCode(0),
    // simulatorMsg(nullptr),
    atsMsg(nullptr),
    atsMsgSendedTimer(nullptr),
    sendAtsMsgThread(nullptr),
    sendAtsMsgWorker(nullptr)
    // revSimulatorThread(nullptr),
    // revSimulatorWorker(nullptr)
{

}

ATSControlSession::~ATSControlSession(){
    this->sendAtsMsgThread->quit();
    this->sendAtsMsgThread->wait();
    this->sendAtsMsgThread->deleteLater();
    this->sendAtsMsgWorker->deleteLater();
    this->atsMsgSendedTimer->stop();
    this->atsMsgSendedTimer->deleteLater();

    // this->revSimulatorThread->quit();
    // this->revSimulatorThread->wait();
    // this->revSimulatorThread->deleteLater();
    // this->revSimulatorWorker->deleteLater();

    delete this->atsMsg;
    // delete this->simulatorMsg;
    this->atsMsg = nullptr;
    // this->simulatorMsg = nullptr;
}

void ATSControlSession::startSession()
{
    this->atsMsg = new ATSJsonMsg();
    // this->simulatorMsg = new SimulatorJsonMsg();

    this->atsMsgSendedTimer = new QTimer(this);

    this->sendAtsMsgThread = new QThread();
    // this->revSimulatorThread = new QThread();

    this->sendAtsMsgWorker = new SendATSDataWorker(this->sendPort);
    // this->revSimulatorWorker = new RevSimulatorDataWorker(this->listenPort);

    this->sendAtsMsgWorker->moveToThread(this->sendAtsMsgThread);
    // this->revSimulatorWorker->moveToThread(this->revSimulatorThread);

    connect(this,&ATSControlSession::sendATSData,this->sendAtsMsgWorker,&SendATSDataWorker::sendDatagram,Qt::QueuedConnection);
    connect(this->atsMsgSendedTimer,&QTimer::timeout,this,[this](){
        emit this->sendATSData(this->atsMsg,this->IP,this->sendPort);
        // 处理场景结束之后赋值的问题
        // if(this->atsMsg->ats_data.control_type == 4 && !this->sessionStatus){
        //     this->sessionInstranceID = "";
        //     this->sessionStatus = false;
        //     this->vtrainCode = 0;
        //     memset(&this->atsMsg->ats_data, 0, sizeof(ATSData)); // 初始化
        //     this->atsMsg->ats_data.control_type = 4;
        // }
    });

    // // 连接数据接收信号触发拉姆达函数分析数据
    // connect(this->revSimulatorThread,&QThread::started,this->revSimulatorWorker,&RevSimulatorDataWorker::start);
    // connect(this->revSimulatorWorker,&RevSimulatorDataWorker::datagramReceived,this,[this](const QByteArray &datagram,const QString &senderIP, quint16 senderPort){
    // // 将数据交给线程池处理
    //     QDateTime rev_time = QDateTime::currentDateTime(); // 数据接收到的时间
    //     DataParseTask* task = new DataParseTask(datagram); // 创建数据分析任务类

    //     connect(task,&DataParseTask::parseDataFinish,this,[this,senderIP,rev_time](int lineNo,const SimulatorJsonMsg &simulatorMsg){
    //     // 连接数据分析完成信号与分析完成之后执行的视景容器填充操作（并发射对应的数据接收完毕信号，以便主线程向发送模拟器数据到ATS的线程做通知）
    //         SimulatorJsonMsg simulatorJsonMsg(simulatorMsg);
    //         simulatorJsonMsg.session_instance_id = this->sessionInstranceID; // 为模拟器发送的包设置场景ID
    //         // this->simulatorMsg->fromSimulatorJsonMsg(simulatorMsg);
    //         // simulatorJsonMsg.session_instance_id = this->sessionInstranceID;
    //         emit this->simulatorDataRevFinished(simulatorJsonMsg,simulatorJsonMsg.train_code);
    //     },Qt::QueuedConnection);
    //     if(this->IP == senderIP){
    //         QThreadPool::globalInstance()->start(task); // 在收到模拟器消息之后将数据解析的任务添加到线程池中
    //     }else{
    //         delete task;
    //     }
    // },Qt::QueuedConnection);
    this->sendAtsMsgThread->start();
    // this->revSimulatorThread->start();
    this->atsMsgSendedTimer->start(17);
}

QString ATSControlSession::getIP() const
{
    return IP;
}

void ATSControlSession::setIP(const QString &newIP)
{
    IP = newIP;
}

// unsigned short SimulatorSession::getListenPort() const
// {
//     return listenPort;
// }

// void SimulatorSession::setListenPort(unsigned short newListenPort)
// {
//     listenPort = newListenPort;
// }

unsigned short ATSControlSession::getSendPort() const
{
    return sendPort;
}

void ATSControlSession::setSendPort(unsigned short newSendPort)
{
    sendPort = newSendPort;
}

// QString SimulatorSession::getSessionInstranceID() const
// {
//     return sessionInstranceID;
// }

// void SimulatorSession::setSessionInstranceID(const QString &newSessionInstranceID)
// {
//     sessionInstranceID = newSessionInstranceID;
// }

// QString SimulatorSession::getLineType() const
// {
//     return lineType;
// }

// void SimulatorSession::setLineType(QString newLineType)
// {
//     lineType = newLineType;
// }

// int SimulatorSession::getSimTrainID() const
// {
//     return simTrainID;
// }

// void SimulatorSession::setSimTrainID(int newSimTrainID)
// {
//     simTrainID = newSimTrainID;
// }

// bool SimulatorSession::getSessionStatus() const
// {
//     return sessionStatus;
// }

// void SimulatorSession::setSessionStatus(bool newSessionStatus)
// {
//     sessionStatus = newSessionStatus;
// }

// int SimulatorSession::getVtrainCode() const
// {
//     return vtrainCode;
// }

// void SimulatorSession::setVtrainCode(int newVtrainCode)
// {
//     vtrainCode = newVtrainCode;
// }

// SimulatorJsonMsg *SimulatorSession::getSimulatorMsg() const
// {
//     return simulatorMsg;
// }

// void SimulatorSession::setSimulatorMsg(SimulatorJsonMsg *newSimulatorMsg)
// {
//     simulatorMsg = newSimulatorMsg;
// }

ATSJsonMsg *ATSControlSession::getAtsMsg() const
{
    return atsMsg;
}

void ATSControlSession::setAtsMsg(ATSJsonMsg *newAtsMsg)
{
    atsMsg = newAtsMsg;
}
