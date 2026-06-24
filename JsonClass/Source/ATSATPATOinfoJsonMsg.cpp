#include <ATSATPATOinfoJsonMsg.h>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

ATSATPATOInfoJsonMsg::ATSATPATOInfoJsonMsg() :
      send_status(false)
    , device_type(0)         // 默认设备类型
    , session_instance_id("000")  // 默认空
    , packet_type("0") // 默认消息类型
    , message_seqnum(0)        // 默认序号
    , request_id("0")           // 默认空
    , train_code("0")
    , A_doors_enable(false)
    , A_door_open_cmd(false)
    , A_door_close_cmd(false)
    , B_doors_enable(false)
    , B_door_open_cmd(false)
    , B_door_close_cmd(false)
    , ATP_EB_cmd(false)
    , ATO_activate(false)
    , ATO_available(false)
    , ATO_traction_cmd(false)
    , ATO_brake_cmd(false)
    , auto_change_ends_cmd(false)
    , auto_changeend_activate(false)
    , auto_changeend_available(false)
    , ATO_rec_speed(0)
{

}

ATSATPATOInfoJsonMsg::ATSATPATOInfoJsonMsg(const ATSATPATOInfoJsonMsg &other)
{
    this->send_status          = other.send_status;
    this->device_type          = other.device_type;
    this->session_instance_id  = other.session_instance_id;
    this->packet_type          = other.packet_type;
    this->message_seqnum       = other.message_seqnum;
    this->request_id           = other.request_id;

    this->train_code           = other.train_code;

    this->current_car_mode     = other.current_car_mode;
    this->current_operation_mode = other.current_operation_mode;
    // this->ats_sim_fault = other.ats_sim_fault;

    this->A_doors_enable       = other.A_doors_enable;
    this->A_door_open_cmd      = other.A_door_open_cmd;
    this->A_door_close_cmd     = other.A_door_close_cmd;
    this->B_doors_enable       = other.B_doors_enable;
    this->B_door_open_cmd      = other.B_door_open_cmd;
    this->B_door_close_cmd     = other.B_door_close_cmd;

    this->ATP_EB_cmd           = other.ATP_EB_cmd;
    this->ATO_activate         = other.ATO_activate;
    this->ATO_available        = other.ATO_available;
    this->ATO_traction_cmd     = other.ATO_traction_cmd;
    this->ATO_brake_cmd        = other.ATO_brake_cmd;
    this->auto_change_ends_cmd = other.auto_change_ends_cmd;
    this->auto_changeend_activate = other.auto_changeend_activate;
    this->auto_changeend_available = other.auto_changeend_available;
    this->ATO_rec_speed        = other.ATO_rec_speed;
}

ATSATPATOInfoJsonMsg::~ATSATPATOInfoJsonMsg()
{

}

/**
 * ATP/ATOInfo消息的反序列化函数
 * @brief ATSATPATOInfoJsonMsg::fromJson
 * @param jsonStr
 * @return
 */
bool ATSATPATOInfoJsonMsg::fromJson(const QString &jsonStr)
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

        this->train_code = payload.value("train_code").toString();

        if (payload.contains("modes")) {
            QJsonObject modes = payload["modes"].toObject();
            this->current_car_mode       = modes.value("current_car_mode").toString();
            this->current_operation_mode = modes.value("current_operation_mode").toString();
            // this->ats_sim_fault =modes.value("ats_sim_fault").toString();
        }

        if (payload.contains("door_control")) {
            QJsonObject doors = payload["door_control"].toObject();
            this->A_doors_enable   = doors.value("A_doors_enable").toBool();
            this->A_door_open_cmd  = doors.value("A_door_open_cmd").toBool();
            this->A_door_close_cmd = doors.value("A_door_close_cmd").toBool();
            this->B_doors_enable   = doors.value("B_doors_enable").toBool();
            this->B_door_open_cmd  = doors.value("B_door_open_cmd").toBool();
            this->B_door_close_cmd = doors.value("B_door_close_cmd").toBool();
        }

        if (payload.contains("ATP_ATO_control")) {
            QJsonObject ato = payload["ATP_ATO_control"].toObject();
            this->ATP_EB_cmd             = ato.value("ATP_EB_cmd").toBool();
            this->ATO_activate           = ato.value("ATO_activate").toBool();
            this->ATO_available          = ato.value("ATO_available").toBool();
            this->ATO_traction_cmd       = ato.value("ATO_traction_cmd").toBool();
            this->ATO_brake_cmd          = ato.value("ATO_brake_cmd").toBool();
            this->auto_change_ends_cmd   = ato.value("auto_change_ends_cmd").toBool();
            this->auto_changeend_activate= ato.value("auto_changeend_activate").toBool();
            this->auto_changeend_available= ato.value("auto_changeend_available").toBool();
            this->ATO_rec_speed          = ato.value("ATO_rec_speed").toInt();
        }
    }

    return true;
}

/**
 * ATP/ATOInfo消息的序列化函数
 * @brief ATSATPATOInfoJsonMsg::toJson
 * @return
 */
QByteArray ATSATPATOInfoJsonMsg::toJson() const
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
    payload["train_code"] = this->train_code;

    QJsonObject modes;
    modes["current_car_mode"]       = this->current_car_mode;
    modes["current_operation_mode"] = this->current_operation_mode;
    // modes["ats_sim_fault"] = this->ats_sim_fault;
    payload["modes"] = modes;

    QJsonObject doors;
    doors["A_doors_enable"]   = this->A_doors_enable;
    doors["A_door_open_cmd"]  = this->A_door_open_cmd;
    doors["A_door_close_cmd"] = this->A_door_close_cmd;
    doors["B_doors_enable"]   = this->B_doors_enable;
    doors["B_door_open_cmd"]  = this->B_door_open_cmd;
    doors["B_door_close_cmd"] = this->B_door_close_cmd;
    payload["door_control"]   = doors;

    QJsonObject ato;
    ato["ATP_EB_cmd"]              = this->ATP_EB_cmd;
    ato["ATO_activate"]            = this->ATO_activate;
    ato["ATO_available"]           = this->ATO_available;
    ato["ATO_traction_cmd"]        = this->ATO_traction_cmd;
    ato["ATO_brake_cmd"]           = this->ATO_brake_cmd;
    ato["auto_change_ends_cmd"]    = this->auto_change_ends_cmd;
    ato["auto_changeend_activate"] = this->auto_changeend_activate;
    ato["auto_changeend_available"]= this->auto_changeend_available;
    ato["ATO_rec_speed"]           = this->ATO_rec_speed;
    payload["ATP_ATO_control"]     = ato;

    obj["payload"] = payload;

    QJsonDocument doc(obj);
    return doc.toJson(QJsonDocument::Indented);
}

/**转化为ATSData结构体
 * @brief ATSATPATOInfoJsonMsg::toATSData
 * @return
 */
ATSData ATSATPATOInfoJsonMsg::toATSData()
{
    ATSData data;
    memset(&data, 0, sizeof(ATSData)); // 初始化所有字段

    // ----------------------------
    // 消息头
    // ----------------------------
    data.device_type = device_type;

    data.session_instance_id = session_instance_id.toInt();
    data.packet_type         = 5; // ATP/ATO 信息
    data.message_seqnum      = message_seqnum;
    data.request_id          = request_id.toInt();

    // ----------------------------
    // ATP/ATO 信息
    // ----------------------------
    data.train_code           = train_code.toInt();
    // 当前车模式
    if      (current_car_mode == "initial") data.current_car_mode = 1;
    else if (current_car_mode == "ATO")     data.current_car_mode = 2;
    else if (current_car_mode == "CM")      data.current_car_mode = 3;
    else if (current_car_mode == "RM")      data.current_car_mode = 4;
    else if (current_car_mode == "EUM")     data.current_car_mode = 5;
    else if (current_car_mode == "LACP")     data.current_car_mode = 6;
    else data.current_car_mode = 0;

    // 当前运行模式
    if      (current_operation_mode == "initial") data.current_operation_mode = 1;
    else if (current_operation_mode == "IXL")    data.current_operation_mode = 2;
    else if (current_operation_mode == "BLOC")   data.current_operation_mode = 3;
    else if (current_operation_mode == "CBTC")   data.current_operation_mode = 4;
    else if (current_operation_mode == "LACP")   data.current_operation_mode = 5;
    else data.current_operation_mode = 0;

    // 车门控制
    data.A_doors_enable      = A_doors_enable;
    data.A_door_open_cmd     = A_door_open_cmd;
    data.A_door_close_cmd    = A_door_close_cmd;
    data.B_doors_enable      = B_doors_enable;
    data.B_door_open_cmd     = B_door_open_cmd;
    data.B_door_close_cmd    = B_door_close_cmd;

    // ATP/ATO 控制
    data.ATP_EB_cmd               = ATP_EB_cmd;
    data.ATO_activate             = ATO_activate;
    data.ATO_available            = ATO_available;
    data.ATO_traction_cmd         = ATO_traction_cmd;
    data.ATO_brake_cmd            = ATO_brake_cmd;
    data.auto_change_ends_cmd     = auto_change_ends_cmd;
    data.auto_changeend_activate  = auto_changeend_activate;
    data.auto_changeend_available = auto_changeend_available;
    data.ATO_rec_speed            = ATO_rec_speed;

    return data;
}

void ATSATPATOInfoJsonMsg::fromATPATOMsg(const ATSATPATOInfoJsonMsg &other)
{
    this->send_status          = other.send_status;
    this->device_type          = other.device_type;
    this->session_instance_id  = other.session_instance_id;
    this->packet_type          = other.packet_type;
    this->message_seqnum       = other.message_seqnum;
    this->request_id           = other.request_id;

    this->train_code           = other.train_code;

    this->current_car_mode     = other.current_car_mode;
    this->current_operation_mode = other.current_operation_mode;
    // this->ats_sim_fault = other.ats_sim_fault;

    this->A_doors_enable       = other.A_doors_enable;
    this->A_door_open_cmd      = other.A_door_open_cmd;
    this->A_door_close_cmd     = other.A_door_close_cmd;
    this->B_doors_enable       = other.B_doors_enable;
    this->B_door_open_cmd      = other.B_door_open_cmd;
    this->B_door_close_cmd     = other.B_door_close_cmd;

    this->ATP_EB_cmd           = other.ATP_EB_cmd;
    this->ATO_activate         = other.ATO_activate;
    this->ATO_available        = other.ATO_available;
    this->ATO_traction_cmd     = other.ATO_traction_cmd;
    this->ATO_brake_cmd        = other.ATO_brake_cmd;
    this->auto_change_ends_cmd = other.auto_change_ends_cmd;
    this->auto_changeend_activate = other.auto_changeend_activate;
    this->auto_changeend_available = other.auto_changeend_available;
    this->ATO_rec_speed        = other.ATO_rec_speed;
}
