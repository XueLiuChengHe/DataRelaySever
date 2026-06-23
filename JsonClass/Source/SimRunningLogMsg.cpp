#include "SimRunningLogMsg.h"

#include <QJsonObject>

SimRunningLog::SimRunningLog()
{
    // Header
    device_type = 2;
    session_instance_id = "0"; // 可用 track_code 或其他唯一标识
    packet_type = "SIM_Running_log";
    message_seqnum = 0;
    request_id = "";
}

SimRunningLog::SimRunningLog(const SimRunningLog &other)
{
    // Header
    device_type = 2;
    session_instance_id = "0"; // 可用 track_code 或其他唯一标识
    packet_type = "SIM_Running_log";
    message_seqnum = 0;
    request_id = "";

    this->log_message = other.log_message;
}

SimRunningLog::~SimRunningLog()
{

}

/**
 * 消息的反序列化
 * @brief SimRunningLog::toJson
 * @return
 */
QByteArray SimRunningLog::toJson() const
{
    // -------------------
    // payload 部分
    // -------------------
    QJsonObject payloadObj;
    payloadObj["log_message"] = this->log_message;

    // -------------------
    // 根对象（包含 Header）
    // -------------------
    QJsonObject rootObj;
    rootObj["device_type"] = device_type;
    rootObj["session_instance_id"] = session_instance_id;
    rootObj["packet_type"] = packet_type;
    rootObj["message_seqnum"] = message_seqnum;
    rootObj["request_id"] = request_id;
    rootObj["payload"] = payloadObj;

    QJsonDocument doc(rootObj);
    return doc.toJson(QJsonDocument::Indented); // 返回 QByteArray
}
