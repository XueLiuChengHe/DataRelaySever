#include <ATSMMIInfoJsonMsg.h>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QDebug>

ATSMMIInfoJsonMsg::ATSMMIInfoJsonMsg() :
    send_status(false)
  , device_type(0)
  , session_instance_id("000")
  , packet_type("0")
  , message_seqnum(0)
  , request_id("0")
  , train_code("0")
  , brake_sign("0")
  , train_run_speed(0)
  , ATP_recommended_speed(0)
  , EM_brake_trigger_speed(0)
  , train_driving_state("0")
  , train_turn_back("0")
  , position_with_depot("0")
  , next_station_skip(false)
  , current_station_detain(false)
  , train_station_stop("0")
  , dwell_time("0")
  , current_PSD_state("0")
  , close_door_info(false)
  , departure_request(false)
  , train_unit_number("0")
  , destination_number("0")
  , train_real_number("0")
  , platform_stop_distance(0)
{

}

ATSMMIInfoJsonMsg::ATSMMIInfoJsonMsg(const ATSMMIInfoJsonMsg &other)
{
    this->send_status          = other.send_status;
    this->device_type          = other.device_type;
    this->session_instance_id  = other.session_instance_id;
    this->packet_type          = other.packet_type;
    this->message_seqnum       = other.message_seqnum;
    this->request_id           = other.request_id;

    this->train_code           = other.train_code;
    this->brake_sign           = other.brake_sign;
    this->train_run_speed      = other.train_run_speed;
    this->ATP_recommended_speed= other.ATP_recommended_speed;
    this->EM_brake_trigger_speed= other.EM_brake_trigger_speed;
    this->train_driving_state  = other.train_driving_state;
    this->train_turn_back      = other.train_turn_back;
    this->position_with_depot  = other.position_with_depot;

    this->next_station_skip    = other.next_station_skip;
    this->current_station_detain = other.current_station_detain;
    this->train_station_stop   = other.train_station_stop;
    this->dwell_time           = other.dwell_time;
    this->current_PSD_state    = other.current_PSD_state;
    this->close_door_info      = other.close_door_info;
    this->departure_request    = other.departure_request;

    this->train_unit_number    = other.train_unit_number;
    this->destination_number   = other.destination_number;
    this->train_real_number = other.train_real_number;
    this->platform_stop_distance = other.platform_stop_distance;
}

ATSMMIInfoJsonMsg::~ATSMMIInfoJsonMsg()
{

}

/**
 * MMIInfo消息的反序列化函数
 * @brief ATSMMIInfoJsonMsg::fromJson
 * @param jsonStr
 * @return
 */
bool ATSMMIInfoJsonMsg::fromJson(const QString &jsonStr)
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

        this->train_code            = payload.value("train_code").toString();
        this->brake_sign            = payload.value("brake_sign").toString();
        this->train_run_speed       = payload.value("train_run_speed").toInt();
        this->ATP_recommended_speed = payload.value("ATP_recommended_speed").toInt();
        this->EM_brake_trigger_speed= payload.value("EM_brake_trigger_speed").toInt();
        this->train_driving_state   = payload.value("train_driving_state").toString();
        this->train_turn_back       = payload.value("train_turn_back").toString();
        this->position_with_depot   = payload.value("position_with_depot").toString();

        this->next_station_skip     = payload.value("next_station_skip").toBool();
        this->current_station_detain= payload.value("current_station_detain").toBool();
        this->train_station_stop    = payload.value("train_station_stop").toString();
        this->dwell_time            = payload.value("dwell_time").toString();
        this->current_PSD_state     = payload.value("current_PSD_state").toString();
        this->close_door_info       = payload.value("close_door_info").toBool();
        this->departure_request     = payload.value("departure_request").toBool();

        this->train_unit_number     = payload.value("train_unit_number").toString();
        this->destination_number    = payload.value("destination_number").toString();
        this->train_real_number    = payload.value("train_real_number").toString();
        this->platform_stop_distance = payload.value("platform_stop_distance").toInt();
    }

    return true;
}

/**
 * MMIInfo消息的序列化函数
 * @brief ATSMMIInfoJsonMsg::toJson
 * @return
 */
QByteArray ATSMMIInfoJsonMsg::toJson() const
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
    payload["train_code"]            = this->train_code;
    payload["brake_sign"]            = this->brake_sign;
    payload["train_run_speed"]       = this->train_run_speed;
    payload["ATP_recommended_speed"] = this->ATP_recommended_speed;
    payload["EM_brake_trigger_speed"]= this->EM_brake_trigger_speed;
    payload["train_driving_state"]   = this->train_driving_state;
    payload["train_turn_back"]       = this->train_turn_back;
    payload["position_with_depot"]   = this->position_with_depot;

    payload["next_station_skip"]     = this->next_station_skip;
    payload["current_station_detain"]= this->current_station_detain;
    payload["train_station_stop"]    = this->train_station_stop;
    payload["dwell_time"]            = this->dwell_time;
    payload["current_PSD_state"]     = this->current_PSD_state;
    payload["close_door_info"]       = this->close_door_info;
    payload["departure_request"]     = this->departure_request;

    payload["train_unit_number"]     = this->train_unit_number;
    payload["destination_number"]    = this->destination_number;
    payload["train_real_number"]    = this->train_real_number;
    payload["platform_stop_distance"]    = this->platform_stop_distance;

    obj["payload"] = payload;

    QJsonDocument doc(obj);
    return doc.toJson(QJsonDocument::Indented);
}

/**转化为ATSData结构体
 * @brief ATSMMIInfoJsonMsg::toATSData
 * @return
 */
ATSData ATSMMIInfoJsonMsg::toATSData()
{
    ATSData data;
    memset(&data, 0, sizeof(ATSData)); // 初始化所有字段

    // ----------------------------
    // 消息头
    // ----------------------------
    data.device_type = device_type;

    data.session_instance_id = session_instance_id.toInt();
    data.packet_type         = 6; // ATP/ATO & MMI 信息对应的类型，可按实际修改
    data.message_seqnum      = message_seqnum;
    data.request_id          = request_id.toInt();

    // ----------------------------
    // ATP/ATO相关信息映射
    // ----------------------------
    data.train_code           = train_code.toInt();

    if      (brake_sign == "initial") data.brake_sign = 1;
    else if (brake_sign == "brake_request") data.brake_sign = 2;
    else if (brake_sign == "apply_EB") data.brake_sign = 3;
    else data.brake_sign = 0;

    data.train_run_speed      = train_run_speed;
    data.ATP_recommended_speed= ATP_recommended_speed;
    data.EM_brake_trigger_speed= EM_brake_trigger_speed;

    // train_driving_state: 惰行/牵引/制动
    if      (train_driving_state == "cruise") data.train_driving_state = 1;
    else if (train_driving_state == "traction") data.train_driving_state = 2;
    else if (train_driving_state == "brake") data.train_driving_state = 3;
    else data.train_driving_state = 0;

    // train_turn_back: 折返状态
    if      (train_turn_back == "none") data.train_turn_back = 1;
    else if (train_turn_back == "available")     data.train_turn_back = 2;
    else if (train_turn_back == "active") data.train_turn_back = 3;
    else data.train_turn_back = 0;

    // position_with_depot: 车辆段状态
    if      (position_with_depot == "none") data.position_with_depot = 1;
    else if (position_with_depot == "enter")   data.position_with_depot = 2;
    else if (position_with_depot == "in") data.position_with_depot = 3;
    else data.position_with_depot = 0;

    // ----------------------------
    // 站台与列车状态
    // ----------------------------
    data.next_station_skip      = next_station_skip ? 1 : 0;
    data.current_station_detain = current_station_detain ? 1 : 0;

    if(train_station_stop == "none") data.train_station_stop = 1;
    else if (train_station_stop == "not_docked")   data.train_station_stop = 2;
    else if (train_station_stop == "docked")   data.train_station_stop = 3;
    else data.train_station_stop = 0;

    if(dwell_time == "none") data.dwell_time = 1;
    else if (dwell_time == "5")   data.dwell_time = 2;
    else data.dwell_time = 0;

    if(current_PSD_state == "unclosed") data.current_PSD_state = 1;
    else if (current_PSD_state == "closed")   data.current_PSD_state = 2;
    else if (current_PSD_state == "unknow")   data.current_PSD_state = 3;
    else data.current_PSD_state = 0;

    data.close_door_info   = close_door_info ? 1 : 0;
    data.departure_request = departure_request ? 1 : 0;

    for(int i = 0;i<this->train_unit_number.size();i++){
        data.train_unit_number[i] = this->train_unit_number.toUtf8().data()[i];
    }
    for(int i = 0;i<this->destination_number.size();i++){
        data.destination_number[i] = this->destination_number.toUtf8().data()[i];
    }
    data.train_real_number = this->train_real_number.toInt();
    data.platform_stop_distance = this->platform_stop_distance;

    return data;
}

void ATSMMIInfoJsonMsg::fromMMIMsg(const ATSMMIInfoJsonMsg &other)
{
    this->send_status          = other.send_status;
    this->device_type          = other.device_type;
    this->session_instance_id  = other.session_instance_id;
    this->packet_type          = other.packet_type;
    this->message_seqnum       = other.message_seqnum;
    this->request_id           = other.request_id;

    this->train_code           = other.train_code;
    this->brake_sign           = other.brake_sign;
    this->train_run_speed      = other.train_run_speed;
    this->ATP_recommended_speed= other.ATP_recommended_speed;
    this->EM_brake_trigger_speed= other.EM_brake_trigger_speed;
    this->train_driving_state  = other.train_driving_state;
    this->train_turn_back      = other.train_turn_back;
    this->position_with_depot  = other.position_with_depot;

    this->next_station_skip    = other.next_station_skip;
    this->current_station_detain = other.current_station_detain;
    this->train_station_stop   = other.train_station_stop;
    this->dwell_time           = other.dwell_time;
    this->current_PSD_state    = other.current_PSD_state;
    this->close_door_info      = other.close_door_info;
    this->departure_request    = other.departure_request;

    this->train_unit_number    = other.train_unit_number;
    this->destination_number   = other.destination_number;
    this->train_real_number = other.train_real_number;
    this->platform_stop_distance = other.platform_stop_distance;
}
