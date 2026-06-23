#ifndef SIMULATORSESSION_H
#define SIMULATORSESSION_H

#include <QObject>
#include <QThread>
#include <QTimer>

#include <ATSJsonMsg.h>
#include <SimulatorJsonMsg.h>

#include <task_h/RevSimulatorDataWorker.h>
#include <task_h/SendATSDataWorker.h>

// ***************************************************************************************
// 各个线路对应的基础场景
//  》场景基础数据
//      定义该模拟器的线路编号 lineType
//      定义模拟器 simTrainID
//      定义模拟器IP  IP
//      定义模拟器Port listenPort sendPort
// ===================================================
//  》场景定义数据
//      定义对应演练场景的ID: QString sessionInstranceID;
//      定义对应演练场景的开始状态: bool sessionStatus;
//      定义该模拟器绑定的演练场景中的虚拟列车编号: int vtrainCode;
// ===================================================
//  》场景数据发送到对应场景定义的模拟器的线程和任务
//      》定义ATS系统发送的数据 sended_ats_json_msg
//      》 定义接收模拟器数据的线程 rev_simulator_data_thread
//      》定义接收模拟器数据的任务类对象 rev_simulator_data_worker
//      》定义发送ATS系统数据的计时器 send_ats_data_timer
//      》 定义发送ATS系统数据的线程 send_ats_data_thread
//      》定义发送ATS数据的任务类对象 send_ats_data_worker
// ***************************************************************************************

class SimulatorSession : public QObject{
    Q_OBJECT
public:
    explicit SimulatorSession(QObject * parent = nullptr);
    ~SimulatorSession();

public:
    void startSession();

    QString getIP() const;
    void setIP(const QString &newIP);
    unsigned short getListenPort() const;
    void setListenPort(unsigned short newListenPort);
    unsigned short getSendPort() const;
    void setSendPort(unsigned short newSendPort);
    QString getSessionInstranceID() const;
    void setSessionInstranceID(const QString &newSessionInstranceID);
    QString getLineType() const;
    void setLineType(QString newLineType);
    int getSimTrainID() const;
    void setSimTrainID(int newSimTrainID);
    bool getSessionStatus() const;
    void setSessionStatus(bool newSessionStatus);
    int getVtrainCode() const;
    void setVtrainCode(int newVtrainCode);
    SimulatorJsonMsg *getSimulatorMsg() const;
    void setSimulatorMsg(SimulatorJsonMsg *newSimulatorMsg);
    ATSJsonMsg *getAtsMsg() const;
    void setAtsMsg(ATSJsonMsg *newAtsMsg);

signals:
    void sendATSData(const ATSJsonMsg * ats_json_data, const QString &ip, quint16 port);
    void simulatorDataRevFinished(QDateTime rev_time, const SimulatorJsonMsg &simulatorJsonMsg,const QString &trainCode);
    void sendSimulatorAndATSMsg(const ATSJsonMsg * ats_json_data, const SimulatorJsonMsg * simulator_json_data);

private:
    QString IP;
    unsigned short listenPort;
    unsigned short sendPort;

    QString sessionInstranceID;
    QString  lineType;
    int simTrainID;
    bool sessionStatus;
    int vtrainCode;

    SimulatorJsonMsg * simulatorMsg;
    ATSJsonMsg * atsMsg;

    QTimer * atsMsgSendedTimer;
    QTimer * atsMsgAndSimulatorMsgShowTimer;

    QThread * sendAtsMsgThread;
    QThread * revSimulatorThread;
    RevSimulatorDataWorker * revSimulatorWorker;
    SendATSDataWorker * sendAtsMsgWorker;
};

#endif // SIMULATORSESSION_H
