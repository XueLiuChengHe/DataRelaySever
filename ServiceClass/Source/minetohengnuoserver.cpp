#include "minetohengnuoserver.h"

#include <QJsonParseError>
#include <QSettings>

MineToHengNuoServer::MineToHengNuoServer(const QString &simulator_log_path,const QString &ats_log_path,const QString &simulator_config_path,const QString &turn_config_path,QObject *parent) : QUdpSocket(parent)
{
    this->initSimulatorSessions(simulator_log_path,ats_log_path,simulator_config_path,turn_config_path);
    this->initDataRelayWidget();
}

MineToHengNuoServer::~MineToHengNuoServer()
{
    this->ats_client_thread->quit();
    this->ats_client_thread->wait();
    this->ats_client_thread->deleteLater();
    this->ats_client_worker->deleteLater();
    this->turnout_state_csv_map->clear();
    delete this->turnout_state_csv_map;
    delete this->trainMapTrainID;

    qDeleteAll(this->trainIDMapSimulatorSession);
    this->trainIDMapSimulatorSession.clear();
    this->atsControlSession->deleteLater();

    this->dataRelayWidget->deleteLater();
}

// =================================================================================================
// 工具函数
// =================================================================================================
/**
 * 加载配置文件
 * @brief MineToHengNuoServer::loadConfig
 */
void MineToHengNuoServer::loadConfig(const QString &config_path)
{
    QSettings settings(config_path, QSettings::IniFormat);
    QStringList groups = settings.childGroups();

    int trainCountFlag = 2; // 记录整车模拟器数量
    this->trainMapTrainID = new QMap<int,QList<int>>();

    for (const QString &group : groups) { // 遍历线路分组
        if (group.startsWith("Session")){
            settings.beginGroup(group); // 开始
            // 遍历该线路下所有 Simulator_X_Ip
            SimulatorSession * simulator_session = new SimulatorSession(); // 对应组的模拟器
            int lineNo;
            QString ip;
            unsigned short listen_port;
            unsigned short send_port;
            int sim_train_id;

            // 初始化对应模拟器的场景对象
            lineNo = settings.value("LineNo").toInt();
            send_port = settings.value("SendPort").toUInt();
            ip = settings.value("Simulator_IP").toString();
            listen_port = settings.value("ListenPort").toUInt();
            sim_train_id = settings.value("Simulator_ID").toInt(); // 获取对用ip的模拟器号
            if (!ip.isEmpty() && listen_port > 0) {
                simulator_session->setListenPort(listen_port);
                simulator_session->setSendPort(send_port);
                simulator_session->setLineType(QString::number(lineNo));
                simulator_session->setSimTrainID(sim_train_id);
                simulator_session->setIP(ip);
                this->trainIDMapSimulatorSession.insert(sim_train_id,simulator_session);
                simulator_session = nullptr;
                qDebug() << "加载模拟器:" << sim_train_id<< "线路号:" << lineNo << "IP:" << ip <<"SendPort"<<send_port<< "ListenPort:" << listen_port;
            }

            if((trainCountFlag+1)%2 != 0){
                this->trainMapTrainID->insert(trainCountFlag/2,QList<int>());
                (*this->trainMapTrainID)[trainCountFlag/2].append(sim_train_id);
            }else{
                (*this->trainMapTrainID)[trainCountFlag/2].append(sim_train_id);
            }
            trainCountFlag++;

            settings.endGroup(); // 结束
        }
        if(group.startsWith("ATS")){
            settings.beginGroup(group); // 开始
            this->atsControlSession = new ATSControlSession();
            this->atsControlSession->setIP(settings.value("ATSControl_IP").toString());
            this->atsControlSession->setSendPort(settings.value("SendPort").toUInt());
            this->ats_ws_url = settings.value("ATS_WS_URL").toString();
            qDebug() << "加载ATSControl:" << "ATS_WS_URL:" << this->ats_ws_url;
            settings.endGroup(); // 结束
        }
    }

    this->ats_client_thread = new QThread();
    this->ats_client_worker = new ATSClientWorker(
        QUrl(this->ats_ws_url),
        this->trainIDMapSimulatorSession,
        this->turnout_state_csv_map,
        this->atsControlSession, // 发给国旗的场景
        this->trainMapTrainID, // 整车映射
        this->simulator_log_path,
        this->ats_log_path
    );
    this->ats_client_worker->moveToThread(this->ats_client_thread);
    connect(this->ats_client_thread,&QThread::started,this->ats_client_worker,&ATSClientWorker::working,Qt::QueuedConnection);

    this->ats_client_thread->start();
    for(auto sessionSign : this->trainIDMapSimulatorSession.values()){
        sessionSign->startSession();
    }
    this->atsControlSession->startSession();
}

/**
 * 加载道岔配置文件
 * @brief MineToHengNuoServer::loadTurnoutConfig
 * @param turnout_config_path
 */
void MineToHengNuoServer::loadTurnoutConfig(const QString &turnout_config_path)
{
    // 打开 JSON 文件
    QFile file(turnout_config_path);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "无法打开文件读取：" << file.errorString();
        return;
    }

    // 读取文件内容
    QByteArray fileData = file.readAll();
    file.close();

    // 解析 JSON 数据
    QJsonDocument doc = QJsonDocument::fromJson(fileData);
    if (!doc.isObject()) {
        qWarning() << "无效的 JSON 格式。";
        return;
    }

    QJsonObject jsonObj = doc.object();

    for (auto it = jsonObj.begin(); it != jsonObj.end(); ++it) {
        this->turnout_state_csv_map->insert(it.key(),it.value().toInt());
    }
    qInfo() <<"道岔配置文件加载完毕";
}

/**
 * 初始化服务器
 * @brief MineToHengNuoServer::initSimulatorSessions
 * @param simulator_log_path
 * @param ats_log_path
 * @param simulator_config_path
 */
void MineToHengNuoServer::initSimulatorSessions(const QString &simulator_log_path,const QString &ats_log_path,const QString &simulator_config_path,const QString &turn_config_path)
{
    this->simulator_log_path = simulator_log_path;
    this->ats_log_path = ats_log_path;
    this->simulator_config_path = simulator_config_path;

    this->turnout_config_path = turn_config_path;
    this->turnout_state_csv_map = new QMap<QString,int>();
    this->loadTurnoutConfig(this->turnout_config_path);

    this->loadConfig(this->simulator_config_path);
}

/**
 * @brief MineToHengNuoServer::initDataRelayWidget
 */
void MineToHengNuoServer::initDataRelayWidget()
{
    this->dataRelayWidget = new DataRelayWidget(this->trainIDMapSimulatorSession);
    this->dataRelayWidget->show();
}
