#include "ATSSessionDefinitionJsonMsg.h"

#include <QDateTime>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QDebug>


ATSSessionDefinitionJsonMsg::ATSSessionDefinitionJsonMsg()
{
    // 默认值初始化
    device_type = 0;
    session_instance_id = "000";
    packet_type = "0";
    message_seqnum = 0;
    request_id = "0";

    session_id = "000";
    session_name = "0";
    course_id = "0";
    session_start_time = "0";
    session_line_type = "0";
    send_statius = false;
}

ATSSessionDefinitionJsonMsg::ATSSessionDefinitionJsonMsg(const ATSSessionDefinitionJsonMsg &other)
{
    device_type = other.device_type;
    session_instance_id = other.session_instance_id;
    packet_type = other.packet_type;
    message_seqnum = other.message_seqnum;
    request_id = other.request_id;

    session_id = other.session_id;
    session_name = other.session_name;
    course_id = other.course_id;
    session_start_time = other.session_start_time;
    session_line_type = other.session_line_type;

    session_train_data = other.session_train_data;
    send_statius = other.send_statius;
}

ATSSessionDefinitionJsonMsg::~ATSSessionDefinitionJsonMsg()
{

}

/**
 * SessionDefinition消息的反序列化函数
 * @brief ATSSessionDefinitionJsonMsg::fromJson
 * @param jsonStr
 * @return
 */
bool ATSSessionDefinitionJsonMsg::fromJson(const QString &jsonStr)
{
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8(), &err);
    if (err.error != QJsonParseError::NoError || !doc.isObject()) return false;

    QJsonObject root = doc.object();
    device_type = root.value("device_type").toInt();
    session_instance_id = root.value("session_instance_id").toString();
    packet_type = root.value("packet_type").toString();
    message_seqnum = root.value("message_seqnum").toInt();
    request_id = root.value("request_id").toString();

    QJsonObject payload = root.value("payload").toObject();
    session_id = payload.value("session_id").toString();
    session_name = payload.value("session_name").toString();
    course_id = payload.value("course_id").toString();
    session_start_time = payload.value("session_start_time").toString();
    session_line_type = payload.value("session_line_type").toString();

    session_train_data.clear();
    QJsonArray trainArray = payload.value("session_train_data").toArray();
    for (const QJsonValue &v : trainArray) {
        QJsonObject t = v.toObject();
        TrainData train;
        train.vtrain_code = t.value("vtrain_code").toString();
        train.track_code = t.value("track_code").toString();
        train.offset_scale = t.value("offset_scale").toInt();
        train.direction = t.value("direction").toString();
//        qDebug() <<train.direction;
        train.sim_train_id = t.value("sim_train_id").toString();
        train.sim_train_cab = t.value("sim_train_cab").toString();
        train.trainee_id = t.value("trainee_id").toString();
        session_train_data.append(train);
    }

    return true;
}

/**
 * SessionDefinition消息的序列化函数
 * @brief ATSSessionDefinitionJsonMsg::toJson
 * @return
 */
QByteArray ATSSessionDefinitionJsonMsg::toJson() const
{
    QJsonObject payloadObj;
    payloadObj["session_id"] = session_id;
    payloadObj["session_name"] = session_name;
    payloadObj["course_id"] = course_id;
    payloadObj["session_start_time"] = session_start_time;
    payloadObj["session_line_type"] = session_line_type;

    QJsonArray trainArray;
    for (const auto &train : session_train_data) {
        QJsonObject trainObj;
        trainObj["vtrain_code"] = train.vtrain_code;
        trainObj["track_code"] = train.track_code;
        trainObj["offset_scale"] = train.offset_scale;
        trainObj["direction"] = train.direction;
        trainObj["sim_train_id"] = train.sim_train_id;
        trainObj["sim_train_cab"] = train.sim_train_cab;
        trainObj["trainee_id"] = train.trainee_id;
        trainArray.append(trainObj);
    }
    payloadObj["session_train_data"] = trainArray;

    QJsonObject root;
    root["device_type"] = device_type;
    root["session_instance_id"] = session_instance_id;
    root["packet_type"] = packet_type;
    root["message_seqnum"] = message_seqnum;
    root["request_id"] = request_id;
    root["payload"] = payloadObj;

    QJsonDocument doc(root);
    return doc.toJson(QJsonDocument::Indented);
}

/**将数据转化为ATS结构体
 * @brief ATSSessionDefinitionJsonMsg::toATSData
 * @return
 */
ATSData ATSSessionDefinitionJsonMsg::toATSData()
{
    ATSData data;
    memset(&data, 0, sizeof(ATSData)); // 初始化

    // ----------------------------
    // 信息标识
    // ----------------------------
    data.device_type = device_type;

    data.session_instance_id = session_instance_id.toInt();
    data.packet_type = 1; // Session_Definition
    data.message_seqnum = message_seqnum;
    data.request_id = request_id.toInt();

    // ----------------------------
    // 场景定义数据
    // ----------------------------
    data.session_id = session_id.toInt();
    data.session_name = (session_name == "列车故障演练场景") ? 1 : 0;
    if(course_id == "M-KC-001"){
        data.course_id = 1;
    }
    if(course_id == "M-KC-002"){
        data.course_id = 2;
    }
    if(course_id == "M-KC-003"){
        data.course_id = 3;
    }
    if(course_id == "M-KC-004"){
        data.course_id = 4;
    }
    if(course_id == "M-KC-005"){
        data.course_id = 5;
    }
    if(course_id == "M-KC-006"){
        data.course_id = 6;
    }
    if(course_id == "M-KC-007"){
        data.course_id = 7;
    }
    if(course_id == "M-KC-008"){
        data.course_id = 8;
    }
    if(course_id == "M-KC-009"){
        data.course_id = 9;
    }
    if(course_id == "M-KC-010"){
        data.course_id = 10;
    }

//    if (session_line_type == "Line1") data.session_line_type = 1;
//    else if (session_line_type == "Line5") data.session_line_type = 5;
//    else if (session_line_type == "Line10") data.session_line_type = 10;
//    else if (session_line_type == "Line12") data.session_line_type = 12;
//    else data.session_line_type = 0;
    data.session_line_type = 5;

    // ----------------------------
    // 列车数组
    // ----------------------------
    if (!session_train_data.isEmpty()) {
        int sign = 0;
        for(TrainData train : this->session_train_data){
            if(sign >= this->session_train_data.size()){
                break;
            }
            data.train_data[sign].vtrain_code   = train.vtrain_code.toInt();
            data.train_data[sign].track_code    = train.track_code.toInt();
            data.train_data[sign].offset_scale  = train.offset_scale;
            data.train_data[sign].direction     = (train.direction == "up") ? 0 : 1;
            qDebug() <<data.train_data[sign].direction;
            data.train_data[sign].sim_train_id  = train.sim_train_id.toInt();
            data.train_data[sign].sim_train_cab = (train.sim_train_cab == "TC1") ? 1 : (train.sim_train_cab == "TC2") ? 2 : 3;
            data.train_data[sign].trainee_id    = train.trainee_id.toInt();
            sign++;
        }
    }

    // ----------------------------
    // 场景开始时间
    // ----------------------------
    if (!session_start_time.isEmpty()) {
        QDateTime dt = QDateTime::fromString(session_start_time, "yyyy-MM-dd HH:mm:ss");
        if (dt.isValid()) {
            QDate date = dt.date();
            QTime time = dt.time();
            data.nYear  = date.year();
            data.nMonth = date.month();
            data.nDay   = date.day();
            data.nHour  = time.hour();
            data.nMin   = time.minute();
            data.nSec   = time.second();
        }
    }
    return data;
}
