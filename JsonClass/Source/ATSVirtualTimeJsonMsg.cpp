#include <ATSVirtualTimeJsonMsg.h>

#include <QDateTime>
#include <QJsonObject>
#include <QJsonParseError>


ATSVirtualTimeJsonMsg::ATSVirtualTimeJsonMsg() :
      send_status(false)
    , device_type(0)         // 默认设备类型
    , session_instance_id("000")  // 默认空
    , packet_type("0") // 默认消息类型
    , message_seqnum(0)        // 默认序号
    , request_id("0")           // 默认空
    , ats_start_time("0")
    , ats_current_time("0")
    , ats_elapsed_time_s(0)
{

}

ATSVirtualTimeJsonMsg::ATSVirtualTimeJsonMsg(const ATSVirtualTimeJsonMsg &other)
{
    this->send_status        = other.send_status;
    this->device_type        = other.device_type;
    this->session_instance_id= other.session_instance_id;
    this->packet_type        = other.packet_type;
    this->message_seqnum     = other.message_seqnum;
    this->request_id         = other.request_id;

    this->ats_start_time     = other.ats_start_time;
    this->ats_current_time   = other.ats_current_time;
    this->ats_elapsed_time_s = other.ats_elapsed_time_s;
}

ATSVirtualTimeJsonMsg::~ATSVirtualTimeJsonMsg()
{

}

/**
 * VirtualTime消息的反序列化函数
 * @brief ATSVirtualTimeJsonMsg::fromJson
 * @param jsonStr
 * @return
 */
bool ATSVirtualTimeJsonMsg::fromJson(const QString &jsonStr)
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
        this->ats_start_time     = payload.value("ats_start_time").toString();
        this->ats_current_time   = payload.value("ats_current_time").toString();
        this->ats_elapsed_time_s = payload.value("ats_elapsed_time_s").toInt();
    }

    return true;
}

/**
 * VirtualTime消息的序列化函数
 * @brief ATSVirtualTimeJsonMsg::toJson
 * @return
 */
QByteArray ATSVirtualTimeJsonMsg::toJson() const
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
    payload["ats_start_time"]     = this->ats_start_time;
    payload["ats_current_time"]   = this->ats_current_time;
    payload["ats_elapsed_time_s"] = this->ats_elapsed_time_s;

    obj["payload"] = payload;

    QJsonDocument doc(obj);
    return doc.toJson(QJsonDocument::Indented);
}

/**转化为ATSData结构体
 * @brief ATSVirtualTimeJsonMsg::toATSData
 * @return
 */
ATSData ATSVirtualTimeJsonMsg::toATSData()
{
    ATSData data;
    memset(&data, 0, sizeof(ATSData)); // 初始化所有字段

    // ----------------------------
    // 消息头
    // ----------------------------
    data.device_type = device_type;

    data.session_instance_id = session_instance_id.toInt();
    data.packet_type         = 3; // ATS_Virtual_Time 对应 packet_type (可根据协议调整)
    data.message_seqnum      = message_seqnum;
    data.request_id          = request_id.toInt();

    // ----------------------------
    // ATS虚拟时间
    // ----------------------------
    // 解析 ats_start_time
    if (!ats_start_time.isEmpty()) {
        QDateTime dt = QDateTime::fromString(ats_start_time, "yyyy-MM-dd HH:mm:ss");
        if (dt.isValid()) {
            data.nYear  = dt.date().year();
            data.nMonth = dt.date().month();
            data.nDay   = dt.date().day();
            data.nHour  = dt.time().hour();
            data.nMin   = dt.time().minute();
            data.nSec   = dt.time().second();
        }
    }

    // 解析 ats_current_time
    if (!ats_current_time.isEmpty()) {
        QDateTime dtNow = QDateTime::fromString(ats_current_time, "yyyy-MM-dd HH:mm:ss");
        if (dtNow.isValid()) {
            data.nYearNow  = dtNow.date().year();
            data.nMonthNow = dtNow.date().month();
            data.nDayNow   = dtNow.date().day();
            data.nHourNow  = dtNow.time().hour();
            data.nMinNow   = dtNow.time().minute();
            data.nSecNow   = dtNow.time().second();
        }
    }

    data.ats_elapsed_time_s = ats_elapsed_time_s;

    return data;
}
