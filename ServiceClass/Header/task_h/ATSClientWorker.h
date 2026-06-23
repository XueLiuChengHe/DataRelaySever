#ifndef ATSCLIENTTHREAD_H
#define ATSCLIENTTHREAD_H

#include "ParseLineDeviceDataTask.h"
#include "RevSimLogWorker.h"

#include <QThread>
#include <QTimer>
#include <SimRunningLogMsg.h>
#include <WebSocket.h>
#include <logmanager.h>

#include <ATSATPATOinfoJsonMsg.h>
#include <ATSAutomaticTrainInfoJsonMsg.h>
#include <ATSLineDeviceDataJsonMsg.h>
#include <ATSMMIInfoJsonMsg.h>
#include <ATSSessionDefinitionJsonMsg.h>
#include <ATSStateControlJsonMsg.h>
#include <ATSVirtualTimeJsonMsg.h>
#include <QtWebSockets/QWebSocket>
#include <sessions_h/ATSControlSession.h>
#include <sessions_h/SimulatorSession.h>

// ***********************************************************************************************************************************************************************
// 与ATS系统通讯的线程

// ***********************************************************************************************************************************************************************
class ATSClientWorker : public QObject{
    Q_OBJECT
public:
    explicit ATSClientWorker(
        const QUrl &ats_url,
        QMap<int,SimulatorSession *> trainIDMapAllSimulatorSession,
        QMap<QString,int> * trunoutCSVMap,
        ATSControlSession * atsControlSession,
        QMap<int,QList<int>> * trainMapTrainID,
        QString simulator_log_path,
        QString ats_log_path,
        QObject *parent = nullptr
    );
    ~ATSClientWorker();

    WebSocket *getAts_websocket() const;
    void setAts_websocket(WebSocket *newAts_websocket);

    QUrl getAts_url() const;
    void setAts_url(const QUrl &value);

    qint64 saveLogSimulatorDate(const QString &sim_train_id,const QString &msg,const QString &log_msg_coming_date);       // 日志保存函数
    qint64 saveLogATSDate(const QString &msg, const QString &log_msg_coming_date);                                                           // 日志保存函数

signals:
    void sendSimulatorMsg(qintptr socketDescriptor,const QString &simulatorJsonMsg,const QString &trainCode);
    void sendReply(qintptr socketDescriptor, const QString &replyMsg,const QString &trainCode);

public slots:
    void working();

public:
    QMap<int,SimulatorSession *> trainIDMapAllSimulatorSession; // 所有模拟器信息
    QMap<int,QList<int>> * trainMapTrainID;
    ATSControlSession * atsControlSession;

    QMap<QString,int> * trunoutCSVMap;    // ATS和我方道岔数据映射

    QMap<QString,QList<int>> sessionIDMapTrainID; // 记录当前场景中的模拟器编号
    QMap<int,QList<int>> vtrainCodeMapTrainID; // 记录当前列车编号绑定的模拟器
    QMap<QString,QList<int>> sessionIDMapVtrainCode; // 记录当前场景中有几个列车

    ATSLineDeviceDataJsonMsg lineDataMsgFlag; // 用于比对当前的到的线路数据与上一次的差异
    int countTurn; // 变化道岔的下标索引变量
    int countSignal; // 变化信号机的下标索引变量

private:
    WebSocket * ats_websocket;
    QUrl ats_url;

    ParseLineDeviceDataTask * parseLineDeviceDataTask;

    QThread * revSimLogThread;   ///< 收取模拟器日志转发的线程
    RevSimLogWorker * revSimLogWorker;          ///< 收取模拟器日志的任务类
    unsigned short logPort;

    SimRunningLog simRunningLogMsg;  ///< 接收的日志消息

    // ============================ 区间无电的补丁变量============================
    bool sendSimEventFlag[20];       ///< 最多20个故障 0未触发故障、1触发故障 .  20个故障的顺序为1 ATO   2 ATP(下标从0开始)   3列车无电故障  4电线脱落事件  5被迫停车(联挂)   6VTM操作联挂    7列车撞击 8轨道异物   9隧道坍塌故障  10拿取灭火工具包
                                                            //成都  7列车撞击   8车门解锁状态    9整列车门状态    10司机操作故障车门隔离
    bool shangJiShuGongState;
    bool firstDoorOpenFlag;
    int leaveDriverFlag;
    QString vtrainCode;                         ///< 救援联挂打的补丁
    // ============================ 区间无电的补丁变量============================

    LogManager * simLogManager;
    LogManager * atsLogManager;
};

#endif // ATSCLIENTTHREAD_H
