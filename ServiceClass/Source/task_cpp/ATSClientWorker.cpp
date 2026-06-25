#include <task_h/ATSClientWorker.h>

#include <QAbstractSocket>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QThreadPool>
#include <SimRunningLogMsg.h>

#include <ReplyJsonMsg.h>
#include <SimAtsEventInfoMsg.h>

#include <task_h/DataParseTask.h>


ATSClientWorker::ATSClientWorker(
    const QUrl &ats_url,
    QMap<int,SimulatorSession *> trainIDMapAllSimulatorSession,
    QMap<QString,int> * trunoutCSVMap,
    ATSControlSession * atsControlSession,
    QMap<int,QList<int>> * trainMapTrainID,
    QString simulator_log_path,
    QString ats_log_path,
    QObject *parent
    ) :
    QObject(parent),
    trainIDMapAllSimulatorSession(trainIDMapAllSimulatorSession),
    trainMapTrainID(trainMapTrainID),
    atsControlSession(atsControlSession),
    trunoutCSVMap(trunoutCSVMap),
    countTurn(0),
    countSignal(0),
    ats_url(ats_url),
    ats_websocket(nullptr),
    revSimLogThread(nullptr),
    revSimLogWorker(nullptr),
    logPort(9980),
    shangJiShuGongState(false), // 初始化当前模拟器无电事件发送补丁
    firstDoorOpenFlag(false),
    leaveDriverFlag(0),
    vtrainCode("0"),
    simLogManager(nullptr),
    atsLogManager(nullptr)
{
    // 注意构造函数会被主线程调用
    memset(this->sendSimEventFlag,0,sizeof(bool)*20); // 初始化当前模拟器事件消息发送标记
    // 创建日志对象
    this->simLogManager = new LogManager(simulator_log_path,1LL*1024*3,this);
    this->atsLogManager = new LogManager(ats_log_path,1LL*1024*3,this);
}

ATSClientWorker::~ATSClientWorker()
{
    this->ats_websocket->close();
    this->ats_websocket->deleteLater();

    this->revSimLogThread->quit();
    this->revSimLogThread->wait();
    this->revSimLogThread->deleteLater();
    this->revSimLogWorker->deleteLater();
}

void ATSClientWorker::working()
{
    // 创建本地客户端与ats服务器通讯套接字
    if(!this->ats_websocket){
        this->ats_websocket = WebSocket::connectToUrl(this->ats_url);
        // this->ats_websocket = new QWebSocket();
    }

    // 建立模拟器数据接收到之后发送到ATS的信号槽连接
    for(auto simulatorSessionSign : this->trainIDMapAllSimulatorSession.values()){
        connect(simulatorSessionSign,&SimulatorSession::simulatorDataRevFinished,this,[this](QDateTime rev_time, const SimulatorJsonMsg &simulatorJsonMsg,const QString &trainCode){
            if(simulatorJsonMsg.session_instance_id != "" && trainCode != "0" && simulatorJsonMsg.track_code != "-1"){
                bool sendFlag = false;
                for(auto trainID : this->vtrainCodeMapTrainID.value(trainCode.toInt())){
                    sendFlag = sendFlag ^ (this->trainIDMapAllSimulatorSession.value(trainID)->getSimulatorMsg()->TC1_key_state ^ this->trainIDMapAllSimulatorSession.value(trainID)->getSimulatorMsg()->TC2_key_state);
                }
                if(sendFlag || this->sessionIDMapVtrainCode.size() > 1){ // 使用异或来判断
                    if(this->ats_websocket->isValid()){
                        // 发送模拟器状态消息
                        this->ats_websocket->sendTextMessage(QString::fromUtf8(simulatorJsonMsg.toJson()));

                        // ================== 为了课程5打的补丁==================
                        // int x = simulatorJsonMsg.firstDoorOpen ^ this->firstDoorOpenFlag;
                        // 作异或处理最为合适
                        if(simulatorJsonMsg.firstDoorOpen ^ this->firstDoorOpenFlag){
                            SimAtsEventInfoMsg simWuDianOpenDoor;
                            simWuDianOpenDoor.session_instance_id = simulatorJsonMsg.session_instance_id;
                            simWuDianOpenDoor.event_param1 = simulatorJsonMsg.train_code;
                            simWuDianOpenDoor.event_type = "FirstDoorOpen";
                            if(simulatorJsonMsg.firstDoorOpen){
                                simWuDianOpenDoor.event_ctrl = "enable";
                                simWuDianOpenDoor.event_param2 = "区间无电第一个门打开";
                                // simWuDianOpenDoor.event_param2 = "夹物车门开状态";
                            }else{
                                simWuDianOpenDoor.event_ctrl = "disable";
                                simWuDianOpenDoor.event_param2 = "区间无电第一个门关闭";
                                // simWuDianOpenDoor.event_param2 = "夹物车门关状态";
                            }
                            this->ats_websocket->sendTextMessage(QString::fromUtf8(simWuDianOpenDoor.toJson()));
                            qInfo()<<"------------------------------------------------Simulator"<<trainCode<<"Event"<<simWuDianOpenDoor.event_type<<"已发送到ATS---------------------------------------------------------";
                            this->firstDoorOpenFlag = simulatorJsonMsg.firstDoorOpen;
                        }
                        // ================== 为了课程5打的补丁==================

                        // ================== 补丁==================
                        if(simulatorJsonMsg.leaveSiJiShi == 1 || simulatorJsonMsg.leaveSiJiShi == 2){
                            SimAtsEventInfoMsg simLeaveDriver;
                            simLeaveDriver.session_instance_id = simulatorJsonMsg.session_instance_id;
                            simLeaveDriver.event_param1 = simulatorJsonMsg.train_code;
                            simLeaveDriver.event_type = "LeaveDriverCab";
                            if(simulatorJsonMsg.leaveSiJiShi == 1 && this->leaveDriverFlag != simulatorJsonMsg.leaveSiJiShi){
                                this->leaveDriverFlag = simulatorJsonMsg.leaveSiJiShi;
                                simLeaveDriver.event_ctrl = "enable";
                                simLeaveDriver.event_param2 = "去客室";
                                this->ats_websocket->sendTextMessage(QString::fromUtf8(simLeaveDriver.toJson()));
                                qInfo()<<"------------------------------------------------Simulator"<<trainCode<<"Event"<<simLeaveDriver.event_type<<"已发送到ATS---------------------------------------------------------";
                            }
                            if(simulatorJsonMsg.leaveSiJiShi == 2 && this->leaveDriverFlag != simulatorJsonMsg.leaveSiJiShi){
                                this->leaveDriverFlag = simulatorJsonMsg.leaveSiJiShi;
                                simLeaveDriver.event_ctrl = "disable";
                                simLeaveDriver.event_param2 = "返回司机室";
                                this->ats_websocket->sendTextMessage(QString::fromUtf8(simLeaveDriver.toJson()));
                                qInfo()<<"------------------------------------------------Simulator"<<trainCode<<"Event"<<simLeaveDriver.event_type<<"已发送到ATS---------------------------------------------------------";
                            }
                        }else if(simulatorJsonMsg.leaveSiJiShi == 0){
                            this->leaveDriverFlag = 0;
                        }
                        // ================== 补丁==================

                        // 发送模拟器事件消息
                        for(int i = 0; i<20; i++){
                            SimAtsEventInfoMsg simAtsEventInfoMsg;
                            simAtsEventInfoMsg.session_instance_id = simulatorJsonMsg.session_instance_id;
                            simAtsEventInfoMsg.event_param1 = simulatorJsonMsg.train_code;

                            if(i == 0){
                                simAtsEventInfoMsg.event_type = "ATO";
                                simAtsEventInfoMsg.event_param2 = "ATO故障";
                            }
                            if(i == 1){
                                simAtsEventInfoMsg.event_type = "ATP";
                                simAtsEventInfoMsg.event_param2 = "ATP故障";
                            }
                            if(i == 2){
                                simAtsEventInfoMsg.event_type = "CMD23";
                                simAtsEventInfoMsg.event_param2 = "列车无电";
                            }
                            if(i == 3){
                                simAtsEventInfoMsg.event_type = "CMD24";
                                simAtsEventInfoMsg.event_param2 = "电线脱落";
                            }
                            if(i == 4){
                                simAtsEventInfoMsg.event_type = "MT10";
                                simAtsEventInfoMsg.event_param2 = "被迫停车";
                            }
                            if(i == 5){
                                simAtsEventInfoMsg.event_type = "CMD26";
                                simAtsEventInfoMsg.event_param2 = "操作联挂";
                            }
                            if(i == 6){
                                simAtsEventInfoMsg.event_type = "TrainCollision";
                                simAtsEventInfoMsg.event_param2 = "列车撞击";
                            }
                            if(i == 7){
                                simAtsEventInfoMsg.event_type = "TrackForeign";
                                simAtsEventInfoMsg.event_param2 = "轨道异物";
                            }
                            if(i == 8){
                                simAtsEventInfoMsg.event_type = "TunnelCollapse";
                                simAtsEventInfoMsg.event_param2 = "隧道坍塌故障";
                            }
                            if(i == 9){
                                simAtsEventInfoMsg.event_type = "GetToolBag";
                                simAtsEventInfoMsg.event_param2 = "司机灭火工具包:灭火器,防毒面具,反光衣";
                                simAtsEventInfoMsg.event_param3 = "司机拿取灭火工具包,包内物品为:灭火器,防毒面具,反光衣";
                            }

                            // 成都
                            // if(i == 6){
                            //     simAtsEventInfoMsg.event_type = "TrainCollision";
                            //     simAtsEventInfoMsg.event_param2 = "列车撞击";
                            // }
                            // if(i == 7){
                            //     simAtsEventInfoMsg.event_type = "CMD29";
                            //     simAtsEventInfoMsg.event_param2 = "车门解锁状态";
                            // }
                            // if(i == 8){
                            //     simAtsEventInfoMsg.event_type = "CMD03";
                            //     simAtsEventInfoMsg.event_param2 = "整列车门状态";
                            // }
                            // if(i == 9){
                            //     simAtsEventInfoMsg.event_type = "DoorIsolation";
                            //     simAtsEventInfoMsg.event_param2 = "司机操作故障车门隔离";
                            // }

                            if(simulatorJsonMsg.event_type_map[i] == 1){
                                simAtsEventInfoMsg.event_ctrl = "enable";
                            }else{
                                simAtsEventInfoMsg.event_ctrl = "disable";
                            }

                            if((simAtsEventInfoMsg.event_ctrl == "enable" && !this->sendSimEventFlag[i])){
                                qInfo()<<"------------------------------------------------Simulator"<<trainCode<<"Event"<<simAtsEventInfoMsg.event_type<<"已发送到ATS---------------------------------------------------------";
                                this->ats_websocket->sendTextMessage(QString::fromUtf8(simAtsEventInfoMsg.toJson()));
                                this->sendSimEventFlag[i] = true;
                            }

                            // ================== 为了课程5打的补丁==================
                            if(!this->shangJiShuGongState && simulatorJsonMsg.UpSingleGong && this->sendSimEventFlag[2]){
                                SimAtsEventInfoMsg simAtsWuDianEventInfoMsg;
                                simAtsWuDianEventInfoMsg.session_instance_id = simulatorJsonMsg.session_instance_id;
                                simAtsWuDianEventInfoMsg.event_param1 = simulatorJsonMsg.train_code;
                                simAtsWuDianEventInfoMsg.event_type = "CMD23";
                                simAtsWuDianEventInfoMsg.event_param2 = "列车无电";
                                simAtsWuDianEventInfoMsg.event_ctrl = "enable";
                                qInfo()<<"------------------------------------------------Simulator"<<trainCode<<"Event"<<simAtsEventInfoMsg.event_param2<<"double触发已发送到ATS---------------------------------------------------------";
                                this->ats_websocket->sendTextMessage(QString::fromUtf8(simAtsWuDianEventInfoMsg.toJson()));
                                this->shangJiShuGongState = true; // 补丁变量
                            }
                            // ================== 为了课程5打的补丁==================
                        }
                        // ------------------------
                        // 日志保存
                        // ------------------------
                        qint64 log_size =  this->saveLogSimulatorDate(QString::number(simulatorJsonMsg.sim_train_id), QString::fromUtf8(simulatorJsonMsg.toJson())+"已发送", rev_time.toString("yyyy-MM-dd HH:mm:ss:zzz"));
                    }else{
                        qInfo()<<"------------------------------------------------模拟器数"<<trainCode<<"据未发送到ATS---------------------------------------------------------\n";
                    }
                }else{

                }
            }else{

            }
        },Qt::QueuedConnection);
    }

    //  用于传输模拟器日志
    this->revSimLogThread = new QThread();
    this->revSimLogWorker = new RevSimLogWorker(this->logPort);
    this->revSimLogWorker->moveToThread(this->revSimLogThread);

    connect(this->revSimLogThread,&QThread::started,this->revSimLogWorker,&RevSimLogWorker::working);
    connect(this->revSimLogWorker,&RevSimLogWorker::simLogPaurseFinished,this,[this](const QString simLogMsg){
        if(this->trainIDMapAllSimulatorSession[3] != nullptr){
            if(this->trainIDMapAllSimulatorSession[3]->getSessionInstranceID() != "0"){
                this->simRunningLogMsg.session_instance_id = this->trainIDMapAllSimulatorSession[3]->getSessionInstranceID();
            }
        }
        this->simRunningLogMsg.log_message = simLogMsg;
        qInfo()<<this->simRunningLogMsg.log_message;

        this->ats_websocket->sendTextMessage(QString::fromUtf8(simRunningLogMsg.toJson()));
        qInfo()<<"------------------------------------------------日志数据已发送到ATS---------------------------------------------------------";
    },Qt::QueuedConnection);

    // ==========================================
    // 检测客户端与ATS系统的连接的信号槽连接
    // ==========================================
    connect(ats_websocket, &WebSocket::connected, this, [](){
        qInfo() <<"ATS系统连接成功"<<QThread::currentThread();
    },Qt::QueuedConnection);
    // ==========================================
    // 检测客户端与ATS系统断开连接的信号槽连接
    // ==========================================
    connect(ats_websocket, &WebSocket::disconnected, this,[this](){
        qInfo() <<"ATS系统连接已断开"<<ats_websocket->errorString();
        QThread::currentThread()->msleep(500); // 检测到连接断开睡眠500毫秒然后重新连接
        this->ats_websocket->connectToServer(this->ats_url); // 当连接断开的时候重新连接
        // this->ats_websocket->open(this->ats_url);
    },Qt::DirectConnection);

    // ==========================================
    // 客户端接收ATS系统消息的信号槽连接
    // ==========================================
    connect(ats_websocket, &WebSocket::textMessageReceived, this,[this](const QString &msg){
        QJsonDocument doc = QJsonDocument::fromJson(msg.toUtf8());
        QJsonObject obj = doc.object();
        QString packetType = obj.value("packet_type").toString();

        // ---------------------------------------
        // Session_Definition 消息
        // ---------------------------------------
        // TODO: 处理 Session_Definition
        if (packetType == "Session_Definition") {
            ATSSessionDefinitionJsonMsg atsSessionDefMsg;
            atsSessionDefMsg.fromJson(msg);
            if(!this->sessionIDMapTrainID.contains(atsSessionDefMsg.session_instance_id)){
                ReplyJsonMsg reply_json;
                reply_json.request_id = atsSessionDefMsg.request_id;
                reply_json.device_type = atsSessionDefMsg.device_type;
                reply_json.packet_type = atsSessionDefMsg.packet_type;
                reply_json.message_seqnum = atsSessionDefMsg.message_seqnum;
                reply_json.session_instance_id = atsSessionDefMsg.session_instance_id;

                if(this->ats_websocket->isValid()){
                    this->ats_websocket->sendTextMessage(QString::fromUtf8(reply_json.toJson()));
                    qInfo()<<"------------------------------------------------回复数据已发送到ATS---------------------------------------------------------";
                }else{
                    qInfo()<<"------------------------------------------------回复数据未发送到ATS---------------------------------------------------------\n";
                }

                QString log_create_time = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss:zzz");
                ATSData ats_data = atsSessionDefMsg.toATSData();
                this->sessionIDMapTrainID.insert(atsSessionDefMsg.session_instance_id,QList<int>());
                this->sessionIDMapVtrainCode.insert(atsSessionDefMsg.session_instance_id, QList<int>());
                for(int i = 0; i<atsSessionDefMsg.session_train_data.size();i++){
                    this->trainIDMapAllSimulatorSession[ats_data.train_data[i].sim_train_id]->setSessionStatus(true);
                    this->trainIDMapAllSimulatorSession[ats_data.train_data[i].sim_train_id]->getAtsMsg()->setSessionDefinitionData(ats_data);
                    this->trainIDMapAllSimulatorSession[ats_data.train_data[i].sim_train_id]->setSessionInstranceID(atsSessionDefMsg.session_instance_id);
                    this->trainIDMapAllSimulatorSession[ats_data.train_data[i].sim_train_id]->setVtrainCode(ats_data.train_data[i].vtrain_code);

                    if(atsSessionDefMsg.session_train_data.size() > 1){
                        this->sessionIDMapTrainID[atsSessionDefMsg.session_instance_id].append(ats_data.train_data[i].sim_train_id);
                        this->sessionIDMapTrainID[atsSessionDefMsg.session_instance_id].append(ats_data.train_data[i].sim_train_id+1);

                        if(this->vtrainCodeMapTrainID.contains(ats_data.train_data[i].vtrain_code)){
                            this->vtrainCodeMapTrainID[ats_data.train_data[i].vtrain_code].append(ats_data.train_data[i].sim_train_id);
                        }else{
                            this->vtrainCodeMapTrainID.insert(ats_data.train_data[i].vtrain_code, QList<int>());
                            this->vtrainCodeMapTrainID[ats_data.train_data[i].vtrain_code].append(ats_data.train_data[i].sim_train_id);
                        }
                    }
                    this->sessionIDMapVtrainCode[atsSessionDefMsg.session_instance_id].append(ats_data.train_data[i].vtrain_code);
                }

                // 为了应付当前整车模式
                if(atsSessionDefMsg.session_train_data.size() == 1){
                    for(auto i : this->trainMapTrainID->value(1)){
                        this->sessionIDMapTrainID[atsSessionDefMsg.session_instance_id].append(this->trainIDMapAllSimulatorSession[i]->getSimTrainID());
                        if(!(this->vtrainCodeMapTrainID.contains(atsSessionDefMsg.session_train_data[0].vtrain_code.toInt()))){
                            this->vtrainCodeMapTrainID.insert(atsSessionDefMsg.session_train_data[0].vtrain_code.toInt(), QList<int>());
                            this->vtrainCodeMapTrainID[atsSessionDefMsg.session_train_data[0].vtrain_code.toInt()].append(this->trainIDMapAllSimulatorSession[i]->getSimTrainID());
                        }else{
                            this->vtrainCodeMapTrainID[atsSessionDefMsg.session_train_data[0].vtrain_code.toInt()].append(this->trainIDMapAllSimulatorSession[i]->getSimTrainID());
                        }
                    }
                }
                // ------------------------
                // 日志保存
                // ------------------------
                qint64 log_size = this->saveLogATSDate(QString::fromUtf8(atsSessionDefMsg.toJson())+"已发送",log_create_time);
            }

            // ---------------------------------------
            // State_Control 消息
            // ---------------------------------------
            // TODO: 处理 State_Control
        }else if (packetType == "State_Control") {
            ATSStateControlJsonMsg atsStateControlMsg;
            atsStateControlMsg.fromJson(msg);

            ReplyJsonMsg reply_json;
            reply_json.request_id = atsStateControlMsg.request_id;
            reply_json.device_type = atsStateControlMsg.device_type;
            reply_json.packet_type = atsStateControlMsg.packet_type;
            reply_json.message_seqnum = atsStateControlMsg.message_seqnum;
            reply_json.session_instance_id = atsStateControlMsg.session_instance_id;

            if(this->ats_websocket->isValid()){
                this->ats_websocket->sendTextMessage(QString::fromUtf8(reply_json.toJson()));
                qInfo()<<"------------------------------------------------回复数据已发送到ATS---------------------------------------------------------";
            }else{
                qInfo()<<"------------------------------------------------回复数据未发送到ATS---------------------------------------------------------\n";
            }

            QString log_create_time = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss:zzz");
            ATSData ats_data = atsStateControlMsg.toATSData();
            if(this->sessionIDMapVtrainCode.contains(atsStateControlMsg.session_instance_id)){
                if(atsStateControlMsg.control_type == "session_start"){
                    for(auto trainID : this->sessionIDMapTrainID[atsStateControlMsg.session_instance_id]){
                        this->trainIDMapAllSimulatorSession[trainID]->setSessionStatus(true);
                        this->trainIDMapAllSimulatorSession[trainID]->getAtsMsg()->setStateControlData(ats_data);
                    }
                }
                if(atsStateControlMsg.control_type == "session_end"){
                    memset(this->sendSimEventFlag,0,sizeof(bool)*20);
                    this->shangJiShuGongState = false;
                    this->leaveDriverFlag = 0;
                    this->vtrainCode = "0";

                    for(auto trainID : this->sessionIDMapTrainID[atsStateControlMsg.session_instance_id]){
                        this->trainIDMapAllSimulatorSession[trainID]->setSessionStatus(false);
                        this->trainIDMapAllSimulatorSession[trainID]->setSessionInstranceID("");
                        this->trainIDMapAllSimulatorSession[trainID]->setVtrainCode(0);
                        this->trainIDMapAllSimulatorSession[trainID]->getAtsMsg()->setStateControlData(ats_data);
                    }
                    this->sessionIDMapTrainID.remove(atsStateControlMsg.session_instance_id);
                    for(auto vtrainCode : this->sessionIDMapVtrainCode[atsStateControlMsg.session_instance_id]){
                        this->vtrainCodeMapTrainID.remove(vtrainCode);
                    }
                    this->sessionIDMapVtrainCode.remove(atsStateControlMsg.session_instance_id); // 注意先后顺序这个只能写在最后面
                }
            }else{
                qInfo() <<"当前场景不存在";
            }
            // ------------------------
            // 日志保存
            // ------------------------
            qint64 log_size = this->saveLogATSDate(QString::fromUtf8(atsStateControlMsg.toJson())+"已发送",log_create_time);

            // ---------------------------------------
            // ATS_Virtual_Time 消息
            // ---------------------------------------
            // TODO: 处理 ATS_Virtual_Time
        } else if (packetType == "ATS_Virtual_Time") {
            ATSVirtualTimeJsonMsg atsVirtualMsg;
            atsVirtualMsg.fromJson(msg);
            QString log_create_time = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss:zzz");
            ATSData ats_data = atsVirtualMsg.toATSData();
            if(this->sessionIDMapVtrainCode.contains(atsVirtualMsg.session_instance_id)){
                for(auto trainID : this->sessionIDMapTrainID.value(atsVirtualMsg.session_instance_id)){
                    this->trainIDMapAllSimulatorSession.value(trainID)->getAtsMsg()->setVirtualTimeData(ats_data);
                }
            }else{
                qInfo() <<"当前场景不存在";
            }
            // ------------------------
            // 日志保存
            // ------------------------
            qint64 log_size = this->saveLogATSDate(QString::fromUtf8(atsVirtualMsg.toJson())+"已发送",log_create_time);

            // ---------------------------------------
            // ATP_ATO_Info 消息
            // ---------------------------------------
            // TODO: 处理 ATP_ATO_Info
        }else if (packetType == "ATP_ATO_Info") {
            ATSATPATOInfoJsonMsg atsAtpAtoMsg;
            atsAtpAtoMsg.fromJson(msg);

            QString log_create_time = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss:zzz");
            ATSData ats_data = atsAtpAtoMsg.toATSData();
            if(this->sessionIDMapVtrainCode.contains(atsAtpAtoMsg.session_instance_id)){
                for(auto trainID : this->vtrainCodeMapTrainID.value(atsAtpAtoMsg.train_code.toInt())){
                    this->trainIDMapAllSimulatorSession.value(trainID)->getAtsMsg()->setAtpAtoInfoData(ats_data);
                    this->trainIDMapAllSimulatorSession.value(trainID)->getAtsMsg()->ats_atp_ato_info_json_msg.fromATPATOMsg(atsAtpAtoMsg); // 用来显示
                }
                this->atsControlSession->getAtsMsg()->atsControlMsg.setATPATOMsg(atsAtpAtoMsg); // 用来给国旗发送
            }else{
                qInfo() <<"当前场景不存在";
            }
            // ------------------------
            // 日志保存
            // ------------------------
            qint64 log_size = this->saveLogATSDate(QString::fromUtf8(atsAtpAtoMsg.toJson()),log_create_time);

            // ---------------------------------------
            // MMI_Info 消息
            // ---------------------------------------
            // TODO: 处理 MMI_Info
        }else if (packetType == "MMI_Info") {
            ATSMMIInfoJsonMsg atsMMIMsg;
            atsMMIMsg.fromJson(msg);

            QString log_create_time = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss:zzz");
            ATSData ats_data = atsMMIMsg.toATSData();
            if(this->sessionIDMapVtrainCode.contains(atsMMIMsg.session_instance_id)){
                for(auto trainID : this->vtrainCodeMapTrainID.value(atsMMIMsg.train_code.toInt())){
                    this->trainIDMapAllSimulatorSession.value(trainID)->getAtsMsg()->setMmiInfoData(ats_data);
                    this->trainIDMapAllSimulatorSession.value(trainID)->getAtsMsg()->ats_mmi_info_json_msg.fromMMIMsg(atsMMIMsg); // 用来显示
                }
                this->atsControlSession->getAtsMsg()->atsControlMsg.setMMIMsg(atsMMIMsg);
            }else{
                qInfo() <<"当前场景不存在";
            }
            // ------------------------
            // 日志保存
            // ------------------------
            qint64 log_size = this->saveLogATSDate(QString::fromUtf8(atsMMIMsg.toJson()),log_create_time);

            // ---------------------------------------
            // Automatic_Train_Info 消息
            // ---------------------------------------
            // TODO: 处理 Automatic_Train_Info
        }else if (packetType == "Automatic_Train_Info") {
            ATSAutomaticTrainInfoJsonMsg atsAutomaticTrainMsg;
            atsAutomaticTrainMsg.fromJson(msg);

            QString log_create_time = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss:zzz");
            ATSData ats_data = atsAutomaticTrainMsg.toATSData(this->vtrainCode);
            if(this->sessionIDMapVtrainCode.contains(atsAutomaticTrainMsg.session_instance_id)){
                // // 可以在这里做一个处理
                for(auto trainID : this->sessionIDMapTrainID[atsAutomaticTrainMsg.session_instance_id]){
                    // qInfo()<<"Rev  VTrainInfo:"<<atsAutomaticTrainMsg.automatic_train_info[0].vtrain_code;
                    this->trainIDMapAllSimulatorSession[trainID]->getAtsMsg()->setAutomaticInfoData(ats_data);
                }
            }else{
                qInfo() <<"当前场景不存在";
            }
            // ------------------------
            // 日志保存
            // ------------------------
            qint64 log_size = this->saveLogATSDate(QString::fromUtf8(atsAutomaticTrainMsg.toJson())+"已发送",log_create_time);

            // ---------------------------------------
            // Line_Device_Data 消息
            // ---------------------------------------
            // TODO: 处理 Line_Device_Data
        }else if (packetType == "Line_Device_Data") {
            ATSLineDeviceDataJsonMsg atsLineDeviceDataMsg;
            atsLineDeviceDataMsg.fromJson(msg);

            this->parseLineDeviceDataTask = nullptr;
            this->parseLineDeviceDataTask = new ParseLineDeviceDataTask(atsLineDeviceDataMsg,this->trunoutCSVMap);
            // ================================================
            // LineDeviceData数据从ATS传过来的时候发出的信号槽的连接
            // ================================================
            connect(this->parseLineDeviceDataTask,&ParseLineDeviceDataTask::lineDeviceDataParseFinish,this,[this,atsLineDeviceDataMsg](ATSData ats_data){
                QString log_create_time = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss:zzz");
                if(this->sessionIDMapVtrainCode.contains(atsLineDeviceDataMsg.session_instance_id)){
                    for(auto trainID : this->sessionIDMapTrainID.value(atsLineDeviceDataMsg.session_instance_id)){
                        if(this->lineDataMsgFlag.session_instance_id == "000"){
                            this->lineDataMsgFlag = atsLineDeviceDataMsg;
                            this->trainIDMapAllSimulatorSession.value(trainID)->getAtsMsg()->setlineDeviceData(ats_data);
                            this->trainIDMapAllSimulatorSession.value(trainID)->getAtsMsg()->ats_line_device_data_json_msg.fromLineDataMsg(atsLineDeviceDataMsg); // 用来显示
                            continue;
                        }

                        this->countTurn = 0; // 在此处置零是为了适配国钊(2026/4/27 AM:11:43 添加)
                        // 设置此次与上次道岔变化数据
                        for(auto sign : lineDataMsgFlag.turnout.keys()){
                            if(this->lineDataMsgFlag.turnout.value(sign) != atsLineDeviceDataMsg.turnout.value(sign)){

                                // 在此处置零是为了适配国钊(2026/4/27 AM:11:43 添加)
                                memset(this->trainIDMapAllSimulatorSession.value(trainID)->getAtsMsg()->ats_data.changeTurnout,0,50); // 置0
                                memset(this->trainIDMapAllSimulatorSession.value(trainID)->getAtsMsg()->ats_data.changeTurnoutID,0,50); // 置0

                                ats_data.changeTurnoutID[this->countTurn] = sign.toInt();
                                ats_data.changeTurnout[this->countTurn] = atsLineDeviceDataMsg.turnout.value(sign) == "branch"? 1:0;
                                this->trainIDMapAllSimulatorSession.value(trainID)->getAtsMsg()->setChangeTurnoutData(ats_data);

                                qDebug()<<"=========================道岔"<<ats_data.changeTurnoutID[this->countTurn]<<"变为"<<ats_data.changeTurnout[this->countTurn]<<"==============================";
                                if(this->countTurn <= 49){
                                    this->countTurn++;
                                }else{
                                    this->countTurn = 0;
                                    memset(this->trainIDMapAllSimulatorSession.value(trainID)->getAtsMsg()->ats_data.changeTurnout,0,50); // 置0
                                    memset(this->trainIDMapAllSimulatorSession.value(trainID)->getAtsMsg()->ats_data.changeTurnoutID,0,50); // 置0
                                }
                            }
                        }

                        this->countSignal = 0; // 在此处置零是为了适配国钊(2026/4/27 AM:11:43 添加)
                        // 设置此次与上次信号机变化数据
                        for(auto sign : lineDataMsgFlag.signal.keys()){
                            if(this->lineDataMsgFlag.signal.value(sign) != atsLineDeviceDataMsg.signal.value(sign)){

                                // 在此处置零是为了适配国钊(2026/4/27 AM:11:43 添加)
                                memset(this->trainIDMapAllSimulatorSession.value(trainID)->getAtsMsg()->ats_data.changeSignal,0,50); // 置0
                                memset(this->trainIDMapAllSimulatorSession.value(trainID)->getAtsMsg()->ats_data.changeSignalID,0,50); // 置0

                                ats_data.changeSignalID[this->countSignal] = sign.toInt();
                                if(atsLineDeviceDataMsg.signal.value(sign) == "none"){
                                    ats_data.changeSignal[this->countSignal] = 0;
                                }
                                if(atsLineDeviceDataMsg.signal.value(sign) == "R"){
                                    ats_data.changeSignal[this->countSignal] = 1;
                                }
                                if(atsLineDeviceDataMsg.signal.value(sign) == "G"){
                                    ats_data.changeSignal[this->countSignal] = 2;
                                }
                                if(atsLineDeviceDataMsg.signal.value(sign) == "Y"){
                                    ats_data.changeSignal[this->countSignal] = 3;
                                }
                                if(atsLineDeviceDataMsg.signal.value(sign) == "W"){
                                    ats_data.changeSignal[this->countSignal] = 5;
                                }
                                if(atsLineDeviceDataMsg.signal.value(sign) == "B"){
                                    ats_data.changeSignal[this->countSignal] = 8;
                                }
                                if(atsLineDeviceDataMsg.signal.value(sign) == "RY"){
                                    ats_data.changeSignal[this->countSignal] = 24;
                                }
                                this->trainIDMapAllSimulatorSession.value(trainID)->getAtsMsg()->setChangeSignalData(ats_data);
                                if(this->countSignal <= 49){
                                    qDebug()<<"=========================信号灯"<<ats_data.changeSignalID[this->countSignal]<<"变为"<<ats_data.changeSignal[this->countSignal]<<"================================";
                                    this->countSignal++;
                                }else{
                                    this->countSignal = 0;
                                    memset(this->trainIDMapAllSimulatorSession.value(trainID)->getAtsMsg()->ats_data.changeSignal,0,50); // 置0
                                    memset(this->trainIDMapAllSimulatorSession.value(trainID)->getAtsMsg()->ats_data.changeSignalID,0,50); // 置0
                                }
                            }
                        }

                        // 数据处理完毕为下一轮处理做准备
                        this->lineDataMsgFlag = atsLineDeviceDataMsg;
                        this->trainIDMapAllSimulatorSession.value(trainID)->getAtsMsg()->setlineDeviceData(ats_data);

                        this->trainIDMapAllSimulatorSession.value(trainID)->getAtsMsg()->ats_line_device_data_json_msg.fromLineDataMsg(atsLineDeviceDataMsg); // 用来显示
                    }
                }else{
                    qInfo() <<"当前场景不存在";
                }
                // ------------------------
                // 日志保存
                // ------------------------
                qint64 log_size = this->saveLogATSDate(QString::fromUtf8(atsLineDeviceDataMsg.toJson()),log_create_time);
            },Qt::QueuedConnection);
            QThreadPool::globalInstance()->start(parseLineDeviceDataTask);

            // ---------------------------------------
            // ATS_SIM_Fault_Info 消息
            // ---------------------------------------
            // TODO: 处理 ATS_SIM_Fault_Info
        }else if(packetType == "ATS_SIM_Fault_Info"){
            ATSSIMFaultInfo atsSIMFaultInfoMsg;
            atsSIMFaultInfoMsg.fromJson(msg);
            QString log_create_time = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss:zzz");
            ATSData ats_data = atsSIMFaultInfoMsg.toATSData();
            if(this->sessionIDMapVtrainCode.contains(atsSIMFaultInfoMsg.session_instance_id)){
                for(auto trainID : this->sessionIDMapTrainID.value(atsSIMFaultInfoMsg.session_instance_id)){
                    this->trainIDMapAllSimulatorSession.value(trainID)->getAtsMsg()->setATSSIMFaultInfoData(ats_data);

                    // 救援联挂的补丁
                    if(atsSIMFaultInfoMsg.fault_type == "TrainLACP"){
                        this->vtrainCode = atsSIMFaultInfoMsg.fault_param1;
                    }

                    qInfo()<<"============================ 故障触发:"<<atsSIMFaultInfoMsg.fault_type<<"==================================";
                }
                this->atsControlSession->getAtsMsg()->atsControlMsg.setATSSIMFaultIngoMsg(atsSIMFaultInfoMsg);
            }else{
                qInfo() <<"当前场景不存在";
            }
            // ------------------------
            // 日志保存
            // ------------------------
            qint64 log_size = this->saveLogATSDate(QString::fromUtf8(atsSIMFaultInfoMsg.toJson())+"已发送",log_create_time);

            // ---------------------------------------
            // 未知 packet_type 消息
            // ---------------------------------------
            // TODO: 处理 未知 packet_type
        }else {
            qWarning() << "⚠️ 未知 packet_type:" << packetType<<":::::"<<doc;
        }
    },Qt::QueuedConnection);

    // ==========================================
    // 连接错误的信号槽连接
    // ==========================================
    connect(ats_websocket, qOverload<WebSocket::WebSocketError>(&WebSocket::error),this,[this](WebSocket::WebSocketError error){
        qDebug() << "WebSocket 连接出错：" << error << this->ats_websocket->errorString();
        QThread::currentThread()->msleep(500);
        this->ats_websocket->connectToServer(this->ats_url);
    },Qt::DirectConnection);
    // connect(ats_websocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),this,[this](QAbstractSocket::SocketError error){
    //     qDebug() << "WebSocket 连接出错：" << error << this->ats_websocket->errorString();
    //     QThread::currentThread()->msleep(500);
    //     this->ats_websocket->open(this->ats_url);
    //     // emit this->connectFail();
    // },Qt::DirectConnection);

    this->ats_websocket->connectToServer(this->ats_url);
    this->revSimLogThread->start();
    // this->ats_websocket->open(this->ats_url);
}

// ========================================================
// getterAndsetter
// ========================================================
WebSocket *ATSClientWorker::getAts_websocket() const
{
    return ats_websocket;
}

void ATSClientWorker::setAts_websocket(WebSocket *newAts_websocket)
{
    ats_websocket = newAts_websocket;
}

QUrl ATSClientWorker::getAts_url() const
{
    return ats_url;
}

void ATSClientWorker::setAts_url(const QUrl &value)
{
    ats_url = value;
}


/**
 * 保存日志数据到指定路径的文件
 * @brief SimulatorServerWidget::saveLogDate
 * @param msg
 * @return
 */
qint64 ATSClientWorker::saveLogSimulatorDate(const QString &sim_train_id,const QString &msg,const QString &log_msg_coming_date)
{
    if (msg.isEmpty()){
        return -1;
    }
    QString logLine = QString("%1-%2:%3").arg(sim_train_id,log_msg_coming_date,msg);
    QString filePrefix = QString("SimLog_%1").arg(sim_train_id);
    simLogManager->appendLog(filePrefix, logLine);
    return 0;
}

/**
 * 保存日志文件到指定路径
 * @brief SimulatorServerWidget::saveLogDate
 * @param msg
 * @param log_msg_coming_date
 * @return
 */
qint64 ATSClientWorker::saveLogATSDate(const QString &msg, const QString &log_msg_coming_date)
{
    if (msg.isEmpty()){
        return -1;
    }
    QString logLine = QString("%1:%2").arg(log_msg_coming_date,msg);
    atsLogManager->appendLog("ATSLog", logLine);
    return 0;
}