#include <ReplyJsonMsg.h>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>


ReplyJsonMsg::ReplyJsonMsg() :
    send_statius(false)       // 默认消息未发送
    , msg_max(500)                // 消息缓存的最大数量
    , device_type(0)         // 默认设备类型
    , session_instance_id("000")  // 默认空
    , packet_type("0") // 默认消息类型
    , message_seqnum(0)        // 默认序号
    , request_id("0")           // 默认空
    , result_code("200")          // 回复码
    , result_desc("ok")       // 回复描述
{

}

ReplyJsonMsg::ReplyJsonMsg(const ReplyJsonMsg &reply_josn_msg)
{
    // -------------------
    // Header
    // -------------------
    device_type = reply_josn_msg.device_type;
    session_instance_id = reply_josn_msg.session_instance_id;
    packet_type = reply_josn_msg.packet_type;
    message_seqnum = reply_josn_msg.message_seqnum;
    request_id = reply_josn_msg.request_id;

    this->result_code = reply_josn_msg.result_code;
    this->result_desc = reply_josn_msg.result_desc;
}

ReplyJsonMsg::~ReplyJsonMsg()
{

}

bool ReplyJsonMsg::fromJson(const QString &jsonStr)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8(), &error);
    if (error.error != QJsonParseError::NoError || !doc.isObject()) {
        return false;
    }

    QJsonObject obj = doc.object();

    // 消息头
    this->device_type         = obj.value("device_type").toInt();
    this->session_instance_id = obj.value("session_instance_id").toString();
    this->packet_type         = obj.value("packet_type").toString();
    this->message_seqnum      = obj.value("message_seqnum").toInt();
    this->request_id          = obj.value("request_id").toString();

    // payload
    if (obj.contains("payload") && obj["payload"].isObject()) {
        QJsonObject payload = obj["payload"].toObject();
        this->result_code = payload.value("result_code").toString();
        this->result_desc = payload.value("result_desc").toString();
    }
    return true;
}

QByteArray ReplyJsonMsg::toJson()
{
    QJsonObject obj;

    // 消息头
    obj["device_type"]         = this->device_type;
    obj["session_instance_id"] = this->session_instance_id;
    obj["packet_type"]         = this->packet_type;
    obj["message_seqnum"]      = this->message_seqnum;
    obj["request_id"]          = this->request_id;

    // payload
    QJsonObject payload;
    payload["result_code"]     = this->result_code;
    payload["result_desc"]   = this->result_desc;

    obj["payload"] = payload;

    QJsonDocument doc(obj);
    return doc.toJson(QJsonDocument::Indented);
}
