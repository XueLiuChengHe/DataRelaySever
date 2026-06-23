#include <ATSLineDeviceDataJsonMsg.h>

#include <QDateTime>
#include <QJsonObject>
#include <QJsonParseError>


ATSLineDeviceDataJsonMsg::ATSLineDeviceDataJsonMsg() :
    send_status(false)
  , device_type(0)
  , session_instance_id("000")
  , packet_type("0")
  , message_seqnum(0)
  , request_id("0")
  , session_id("000")
  , occ_virtual_time("0")
{

}

ATSLineDeviceDataJsonMsg::ATSLineDeviceDataJsonMsg(const ATSLineDeviceDataJsonMsg &other)
{
    send_status         = other.send_status;
    device_type         = other.device_type;
    session_instance_id = other.session_instance_id;
    packet_type         = other.packet_type;
    message_seqnum      = other.message_seqnum;
    request_id          = other.request_id;

    session_id          = other.session_id;
    occ_virtual_time    = other.occ_virtual_time;

    turnout             = other.turnout;
    signal              = other.signal;
    PSD                 = other.PSD;
    platform_countdown_timer = other.platform_countdown_timer;
}

ATSLineDeviceDataJsonMsg::~ATSLineDeviceDataJsonMsg()
{

}

/**
 * LineDeviceData消息的反序列化函数
 * @brief ATSLineDeviceDataJsonMsg::fromJson
 * @param jsonStr
 * @return
 */
bool ATSLineDeviceDataJsonMsg::fromJson(const QString &jsonStr)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8(), &error);
    if (error.error != QJsonParseError::NoError || !doc.isObject())
        return false;

    QJsonObject obj = doc.object();

    // 消息头
    device_type         = obj.value("device_type").toInt();
    session_instance_id = obj.value("session_instance_id").toString();
    packet_type         = obj.value("packet_type").toString();
    message_seqnum      = obj.value("message_seqnum").toInt();
    request_id          = obj.value("request_id").toString();

    // payload
    if (obj.contains("payload") && obj["payload"].isObject()) {
        QJsonObject payload = obj["payload"].toObject();
        session_id       = payload.value("session_id").toString();
        occ_virtual_time = payload.value("occ_virtual_time").toString();

        auto parseMap = [](const QJsonObject &obj, QMap<QString, QString> &map) {
            map.clear();
            for (const QString &key : obj.keys()) {
                map[key] = obj.value(key).toString();
            }
        };

        if (payload.contains("turnout") && payload["turnout"].isObject())
            parseMap(payload["turnout"].toObject(), turnout);

        if (payload.contains("signal") && payload["signal"].isObject())
            parseMap(payload["signal"].toObject(), signal);

        if (payload.contains("PSD") && payload["PSD"].isObject())
            parseMap(payload["PSD"].toObject(), PSD);

        if (payload.contains("platform_countdown_timer") && payload["platform_countdown_timer"].isObject())
            parseMap(payload["platform_countdown_timer"].toObject(), platform_countdown_timer);
    }

    return true;
}

/**
 * LineDeviceData消息的序列化函数
 * @brief ATSLineDeviceDataJsonMsg::toJson
 * @return
 */
QByteArray ATSLineDeviceDataJsonMsg::toJson() const
{
    QJsonObject obj;

    // 消息头
    obj["device_type"]         = device_type;
    obj["session_instance_id"] = session_instance_id;
    obj["packet_type"]         = packet_type;
    obj["message_seqnum"]      = message_seqnum;
    obj["request_id"]          = request_id;

    // payload
    QJsonObject payload;
    payload["session_id"]       = session_id;
    payload["occ_virtual_time"] = occ_virtual_time;

    auto mapToObj = [](const QMap<QString, QString> &map) -> QJsonObject {
        QJsonObject obj;
        for (auto it = map.constBegin(); it != map.constEnd(); ++it)
            obj[it.key()] = it.value();
        return obj;
    };

    payload["turnout"]               = mapToObj(turnout);
    payload["signal"]                = mapToObj(signal);
    payload["PSD"]                   = mapToObj(PSD);
    payload["platform_countdown_timer"] = mapToObj(platform_countdown_timer);

    obj["payload"] = payload;

    QJsonDocument doc(obj);
    return doc.toJson(QJsonDocument::Indented);
}

/**转化为ATSData结构体
 * @brief ATSLineDeviceDataJsonMsg::toATSData
 * @return
 */
ATSData ATSLineDeviceDataJsonMsg::toATSData(const QMap<QString,int> * turn_config_map)
{
    ATSData data;
    memset(&data, 0, sizeof(ATSData)); // 初始化所有字段

    // ----------------------------
    // 消息头
    // ----------------------------
    data.device_type = device_type;

    data.session_instance_id = session_instance_id.toInt();
    data.packet_type         = 8; // Line_Device_Data
    data.message_seqnum      = message_seqnum;
    data.request_id          = request_id.toInt();

    // ----------------------------
    // 场景信息
    // ----------------------------
    data.Line_session_id = session_id.toInt();

    // ----------------------------
    // OCC 虚拟时间
    // ----------------------------
    if (!occ_virtual_time.isEmpty()) {
        QDateTime dt = QDateTime::fromString(occ_virtual_time, "yyyy-MM-dd HH:mm:ss");
        if (dt.isValid()) {
            data.nOCC_Year  = dt.date().year();
            data.nOCC_Month = dt.date().month();
            data.nOCC_Day   = dt.date().day();
            data.nOCC_Hour  = dt.time().hour();
            data.nOCC_Min   = dt.time().minute();
            data.nOCC_Sec   = dt.time().second();
        }
    }

    // ----------------------------
    // 道岔状态（turnout） 0=main, 1=branch
    // ----------------------------
    for (auto it = turnout.begin(); it != turnout.end(); ++it) {
        int arrayIndex =  turn_config_map->value(it.key())/ 32;
        int bitIndex = turn_config_map->value(it.key())%32;
        bool isReverse = false;
        if(it.value() == "branch") isReverse = true;
        if (isReverse)
            data.turnout[arrayIndex] |= (1 << (31-bitIndex));
    }
//    for (auto it = turnout.constBegin(); it != turnout.constEnd(); ++it) {
//        int idx = it.key().toInt() - 1; // 道岔编号从1开始
//        if (idx >= 0 && idx < 10) {
//            data.turnout[idx] = (it.value() == "branch") ? 1 : 0;
//        }
//    }

    // ----------------------------
    // 信号灯状态（signal） 0=none, 1=R, 2=G, 3=Y, 5=W, 8=B, 24=RY
    // ----------------------------
    int signalIndex = 0;
    for (auto it = signal.begin(); it != signal.end(); ++it, ++signalIndex) {
        int sign_num = 0;
        if(it.value() == "R") sign_num = 1;
        if(it.value() == "G") sign_num = 2;
        if(it.value() == "Y") sign_num = 3;
        if(it.value() == "W") sign_num = 5;
        if(it.value() == "B") sign_num = 8;
        if(it.value() == "RY") sign_num = 24;
        int arrayIndex = signalIndex / 4;
        int offset = (signalIndex % 4) * 8;
       data.signal[arrayIndex] |= ((sign_num & 0xFFFF) << (24-offset));
    }

    // ----------------------------
    // 屏蔽门状态（PSD） 0=close, 1=open
    // ----------------------------
    int psdIndex = 0;
    for (auto it = PSD.begin(); it != PSD.end(); ++it, ++psdIndex) {
        int arrayIndex = psdIndex / 32;
        int bitIndex = psdIndex % 32;
        bool isOpen = false;
        if(it.value() == "open") isOpen = true;
        if (isOpen)
            data.nPSD[arrayIndex] |= (1 << bitIndex);
    }

    // ----------------------------
    // 站台倒计时（platform_countdown_timer）
    // 示例中暂不处理，若需要可扩展
    // ----------------------------
    for (int i = 0; i < 6; ++i)
        data.platform_countdown_timer[i] = 0;

    for (int i = 0; i < 6; ++i) {
        QString key = QString::number(i + 1);
        if (!platform_countdown_timer.contains(key)) continue;
        QString val = platform_countdown_timer[key];
        if (val == "=") data.platform_countdown_timer[i] = 1;
        else if (val == "= =") data.platform_countdown_timer[i] = 2;
        else if (val == "-=-") data.platform_countdown_timer[i] = 3;
        else if (val == "H") data.platform_countdown_timer[i] = 4;
        else if (val == "---") data.platform_countdown_timer[i] = 5;
        else {
            bool ok = false;
            int num = val.toInt(&ok);
            data.platform_countdown_timer[i] = ok ? num : 0;
        }
    }

    return data;
}

void ATSLineDeviceDataJsonMsg::fromLineDataMsg(const ATSLineDeviceDataJsonMsg &other)
{
    send_status         = other.send_status;
    device_type         = other.device_type;
    session_instance_id = other.session_instance_id;
    packet_type         = other.packet_type;
    message_seqnum      = other.message_seqnum;
    request_id          = other.request_id;

    session_id          = other.session_id;
    occ_virtual_time    = other.occ_virtual_time;

    turnout             = other.turnout;
    signal              = other.signal;
    PSD                 = other.PSD;
    platform_countdown_timer = other.platform_countdown_timer;
}
