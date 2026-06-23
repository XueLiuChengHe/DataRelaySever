#ifndef MINETOHENGNUOSERVER_H
#define MINETOHENGNUOSERVER_H

#include "logmanager.h"

#include <DataRelayWidget.h>
#include <QUdpSocket>

#include <sessions_h/SimulatorSession.h>
#include <task_h/ATSClientWorker.h>

#include <sessions_h/ATSControlSession.h>
#include <sessions_h/SimulatorSession.h>

// ***************************************************************************************
//  》定义
// ===================================================
// ***************************************************************************************
class MineToHengNuoServer : public QUdpSocket
{
    Q_OBJECT
public:
    MineToHengNuoServer(const QString &simulator_log_path,const QString &ats_log_path,const QString &simulator_config_path,const QString &turnout_config_path,QObject * parent = nullptr);
    ~MineToHengNuoServer();

    void loadConfig(const QString &config_path);                                                // 加载配置文件
    void loadTurnoutConfig(const QString &turnout_config_path);                     // 加载道岔配置文件
    void initSimulatorSessions(const QString &simulator_log_path,const QString &ats_log_path,const QString &simulator_config_path,const QString &turn_config_path);  // 初始化线路模拟器集合
    void initDataRelayWidget(); // 初始化展示窗口

signals:
    void simulatorDataRevFinished(const QWebSocket * ats_websocket,const QString &simulator_json_msg);

public:
    QString simulator_log_path;
    QString ats_log_path;
    QString simulator_config_path;
    QString turnout_config_path;
    QString ats_ws_url;           // ATS WebSocket 服务端地址

private:
    QMap<int,SimulatorSession*> trainIDMapSimulatorSession; // 所有模拟器信息
    QMap<int,QList<int>> * trainMapTrainID; /// 每个整车包含的列车ID

    // 到时候可以加一个映射数据 sessionIDMapTrain: QMap<QString,int>, 其中的int就是整车的编号从1开始的

    ATSControlSession * atsControlSession; // 发给国旗的

    QMap<QString,int> * turnout_state_csv_map;    // ATS和我方道岔数据映射

    QThread * ats_client_thread;
    ATSClientWorker * ats_client_worker; // 与信号系统通讯的客户端

    DataRelayWidget * dataRelayWidget; // 消息状态展示界面
};

#endif // MINETOHENGNUOSERVER_H
