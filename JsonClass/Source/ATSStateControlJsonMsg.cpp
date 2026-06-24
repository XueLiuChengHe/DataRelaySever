#include <ATSStateControlJsonMsg.h>


#include <QJsonObject>
#include <QJsonParseError>

ATSStateControlJsonMsg::ATSStateControlJsonMsg() :
      send_statius(false)       // 默认消息未发送
    , device_type(0)         // 默认设备类型
    , session_instance_id("000")  // 默认空
    , packet_type("0") // 默认消息类型
    , message_seqnum(0)        // 默认序号
    , request_id("0")           // 默认空
    , control_type("0")      // 默认控制类型
{

}

ATSStateControlJsonMsg::ATSStateControlJsonMsg(const ATSStateControlJsonMsg &other)
{
    send_statius = other.send_statius;
    device_type = other.device_type;
    session_instance_id = other.session_instance_id;
    packet_type = other.packet_type;
    message_seqnum = other.message_seqnum;
    request_id = other.request_id;
    control_type = other.control_type;
}

ATSStateControlJsonMsg::~ATSStateControlJsonMsg()
{

}

/**
 * StateControl消息的反序列化函数
 * @brief ATSStateControlJsonMsg::fromJson
 * @param jsonStr
 * @return
 */
bool ATSStateControlJsonMsg::fromJson(const QString &jsonStr)
{
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8(), &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        return false;
    }

    if (!doc.isObject()) return false;
    QJsonObject obj = doc.object();

    device_type = obj.value("device_type").toInt();
    session_instance_id = obj.value("session_instance_id").toString();
    packet_type = obj.value("packet_type").toString();
    message_seqnum = obj.value("message_seqnum").toInt();
    request_id = obj.value("request_id").toString();

    if (obj.contains("payload") && obj.value("payload").isObject()) {
        QJsonObject payloadObj = obj.value("payload").toObject();
        control_type = payloadObj.value("control_type").toString();
    } else {
        return false;
    }
    return true;
}

/**
 * StateControl消息的序列化函数
 * @brief ATSStateControlJsonMsg::toJson
 * @return
 */
QByteArray ATSStateControlJsonMsg::toJson() const
{
    QJsonObject payloadObj;
    payloadObj["control_type"] = control_type;

    QJsonObject obj;
    obj["device_type"] = device_type;
    obj["session_instance_id"] = session_instance_id;
    obj["packet_type"] = packet_type;
    obj["message_seqnum"] = message_seqnum;
    obj["request_id"] = request_id;
    obj["payload"] = payloadObj;

    QJsonDocument doc(obj);
    return doc.toJson(QJsonDocument::Indented);
}

/**转化为ATSData结构体
 * @brief ATSStateControlJsonMsg::toATSData
 * @return
 */
ATSData ATSStateControlJsonMsg::toATSData()
{
    ATSData data;
    memset(&data, 0, sizeof(ATSData)); // 初始化所有字段

    // ----------------------------
    // 消息头
    // ----------------------------
    data.device_type = device_type;

    data.session_instance_id = session_instance_id.toInt();
    data.packet_type         = 2; // State_Control 对应 packet_type (可根据实际协议调整)
    data.message_seqnum      = message_seqnum;
    data.request_id          = request_id.toInt();

    // ----------------------------
    // 场景状态控制
    // ----------------------------
    // control_type 1=场景开始, 2=运行, 3=暂停, 4=场景结束
    if (control_type == "session_start")      data.control_type = 1;
    else if (control_type == "run")           data.control_type = 2;
    else if (control_type == "pause")         data.control_type = 3;
    else if (control_type == "session_end")   data.control_type = 4;
    else data.control_type = 0;

    return data;
}
