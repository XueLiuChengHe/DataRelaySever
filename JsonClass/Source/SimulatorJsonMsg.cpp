#include <SimulatorJsonMsg.h>
#include <QJsonObject>
#include <QJsonParseError>
#include <QDebug>
#include <QMetaType>
Q_DECLARE_METATYPE(SimulatorJsonMsg)

SimulatorJsonMsg::SimulatorJsonMsg() :
      send_statius(false),
      device_type(2),
      session_instance_id("000"),
      packet_type("Sim_Train_Info"),
      message_seqnum(0),
      request_id("0"),
      train_code("0"),
      direction("0"),
      train_speed_km_per_h(0.0),
      TC1_activated(false),
      TC2_activated(false),
      active_cab_forward_state(false),
      active_cab_reverse_state(false),
      train_coasting(false),
      train_braking(false),
      train_traction(false),
      ATO_release(false),
      traction_enabled(false),
      TC1_key_state(false),
      TC2_key_state(false),
      EB_applied(false),
      train_mode_set("none"),
      TC1_ATP_cutout(false),
      TC2_ATP_cutout(false),
      TC1_ATO_start_PB(false),
      TC2_ATO_start_PB(false),
      TC1_auto_change_end_PB(false),
      TC2_auto_change_end_PB(false),
      TC1_door_mode("0"),
      TC2_door_mode("0"),
      A_doors_close_state(false),
      A_door_open_request(false),
      A_door_close_request(false),
      B_doors_close_state(false),
      B_door_open_request(false),
      B_door_close_request(false),
     UpSingleGong(false),
     firstDoorOpen(false),
    leaveSiJiShi(0)
{
    // 这里可以做额外初始化，比如连接信号槽


}
SimulatorJsonMsg::SimulatorJsonMsg(const SimulatorData &data)
{
    // 消息状态
    send_statius = false;

    // Header
    device_type = 2;
    session_instance_id = " "; // 可用 track_code 或其他唯一标识
    packet_type = "Sim_Train_Info";
    message_seqnum = 0;
    request_id = "";

    // 列车基本信息
    train_code = QString::number(data.train_code);
    direction = (data.direction == 0) ? "up" : "down";
    train_speed_km_per_h = data.train_speed_km_per_h;
    track_code = QString::number(data.track_code);
    landmark = QString::number(data.landmark);
    offset_scale = data.offset_scale;

    // 驾驶室信息
    TC1_activated = data.TC1_activated;
    TC2_activated = data.TC2_activated;
    active_cab_forward_state = data.active_cab_forward_state;
    active_cab_reverse_state = data.active_cab_reverse_state;
    train_coasting = data.rain_coasting;
    train_braking = data.train_braking;
    train_traction = data.train_traction;
    ATO_release = data.ATO_release;
    traction_enabled = data.traction_enabled;
    TC1_key_state = data.TC1_key_state;
    TC2_key_state = data.TC2_key_state;
    EB_applied = data.EB_applied;
    if(data.train_mode_set == 0) train_mode_set = "none";
    if(data.train_mode_set == 1) train_mode_set = "up";
    if(data.train_mode_set == 2) train_mode_set = "down";
    if(data.train_mode_set == 3) train_mode_set = "confirm";

    // ATO 信息
    TC1_ATP_cutout = data.TC1_ATP_cutout;
    TC2_ATP_cutout = data.TC2_ATP_cutout;
    TC1_ATO_start_PB = data.TC1_ATO_start_PB;
    TC2_ATO_start_PB = data.TC2_ATO_start_PB;
    TC1_auto_change_end_PB = data.TC1_auto_change_end_PB;
    TC2_auto_change_end_PB = data.TC2_auto_change_end_PB;
    TC1_door_mode = QString::number(data.TC1_door_mode);
    TC2_door_mode = QString::number(data.TC2_door_mode);

    for(int i = 0; i<20; i++){
        this->event_type_map[i] = data.sim_ats_event[i];
    }

    // 门控制
    A_doors_close_state = data.A_doors_close_state;
    A_door_open_request = data.A_door_open_request;
    A_door_close_request = data.A_door_close_request;
    B_doors_close_state = data.B_doors_close_state;
    B_door_open_request = data.B_door_open_request;
    B_door_close_request = data.B_door_close_request;

    this->UpSingleGong = data.UpSingleGong;
    this->firstDoorOpen = data.firstDoorOpen;
    this->leaveSiJiShi = data.leaveSiJiShi;
}

SimulatorJsonMsg::SimulatorJsonMsg(const SimulatorJsonMsg &other)
{
    this->send_statius = other.send_statius;
    this->sim_train_id = other.sim_train_id;
    // -------------------
    // Header
    // -------------------
    device_type = other.device_type;
    session_instance_id = other.session_instance_id;
    packet_type = other.packet_type;
    message_seqnum = other.message_seqnum;
    request_id = other.request_id;

    // -------------------
    // Payload: 列车基本信息
    // -------------------
    train_code = other.train_code;
    direction = other.direction;
    train_speed_km_per_h = other.train_speed_km_per_h;
    track_code = other.track_code;
    landmark = other.landmark;
    offset_scale = other.offset_scale;

    // -------------------
    // Payload: 驾驶室信息
    // -------------------
    TC1_activated = other.TC1_activated;
    TC2_activated = other.TC2_activated;
    active_cab_forward_state = other.active_cab_forward_state;
    active_cab_reverse_state = other.active_cab_reverse_state;
    train_coasting = other.train_coasting;
    train_braking = other.train_braking;
    train_traction = other.train_traction;
    ATO_release = other.ATO_release;
    traction_enabled = other.traction_enabled;
    TC1_key_state = other.TC1_key_state;
    TC2_key_state = other.TC2_key_state;
    EB_applied = other.EB_applied;
    train_mode_set = other.train_mode_set;

    // -------------------
    // Payload: ATO信息
    // -------------------
    TC1_ATP_cutout = other.TC1_ATP_cutout;
    TC2_ATP_cutout = other.TC2_ATP_cutout;
    TC1_ATO_start_PB = other.TC1_ATO_start_PB;
    TC2_ATO_start_PB = other.TC2_ATO_start_PB;
    TC1_auto_change_end_PB = other.TC1_auto_change_end_PB;
    TC2_auto_change_end_PB = other.TC2_auto_change_end_PB;
    TC1_door_mode = other.TC1_door_mode;
    TC2_door_mode = other.TC2_door_mode;
    for(int i = 0; i<20; i++){
        this->event_type_map[i] = other.event_type_map[i];
    }

    // -------------------
    // Payload: 门控制信息
    // -------------------
    A_doors_close_state = other.A_doors_close_state;
    A_door_open_request = other.A_door_open_request;
    A_door_close_request = other.A_door_close_request;
    B_doors_close_state = other.B_doors_close_state;
    B_door_open_request = other.B_door_open_request;
    B_door_close_request = other.B_door_close_request;

    this->UpSingleGong = other.UpSingleGong;
    this->firstDoorOpen = other.firstDoorOpen;
    this->leaveSiJiShi = other.leaveSiJiShi;
}

SimulatorJsonMsg::~SimulatorJsonMsg()
{

}

/**
 * 解析JSON并存入类成员
 * @brief SimulatorJsonMsg::fromJson
 * @return
 */
bool SimulatorJsonMsg::fromJson(const QString &jsonStr)
{
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8(), &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qWarning() << "JSON解析错误:" << parseError.errorString();
        return false;
    }
    if (!doc.isObject()) {
        qWarning() << "JSON不是对象格式";
        return false;
    }

    QJsonObject rootObj = doc.object();
    QJsonObject payloadObj = rootObj.value("payload").toObject(); // 解析列车基本信息
    QJsonObject trainInfoObj = payloadObj.value("train_basic_info").toObject();

    // -------------------
    // 解析 Header 部分
    // -------------------
    device_type = rootObj.value("device_type").toInt();
    session_instance_id = rootObj.value("session_instance_id").toString();
    packet_type = rootObj.value("packet_type").toString();
    message_seqnum = rootObj.value("message_seqnum").toInt();
    request_id = rootObj.value("request_id").toString();  // 注意 JSON key 不要多余空格

    train_code = trainInfoObj.value("train_code").toString();
    direction = trainInfoObj.value("direction").toString();
    train_speed_km_per_h = trainInfoObj.value("train_speed_km_per_h").toDouble();
    track_code = trainInfoObj.value("track_code").toString();
    landmark = trainInfoObj.value("landmark").toString();
    offset_scale = trainInfoObj.value("offset_scale").toInt();

    // 驾驶室信息
    QJsonObject cabObj = payloadObj.value("cab_control_info").toObject();
    TC1_activated = cabObj.value("TC1_activated").toBool();
    TC2_activated = cabObj.value("TC2_activated").toBool();
    active_cab_forward_state = cabObj.value("active_cab_forward_state").toBool();
    active_cab_reverse_state = cabObj.value("active_cab_reverse_state").toBool();
    train_coasting = cabObj.value("train_coasting").toBool();
    train_braking = cabObj.value("train_braking").toBool();
    train_traction = cabObj.value("train_traction").toBool();
    ATO_release = cabObj.value("ATO_release").toBool();
    traction_enabled = cabObj.value("traction_enabled").toBool();
    TC1_key_state = cabObj.value("TC1_key_state").toBool();
    TC2_key_state = cabObj.value("TC2_key_state").toBool();
    EB_applied = cabObj.value("EB_applied").toBool();
    train_mode_set = cabObj.value("train_mode_set").toString();

    // ATO信息
    QJsonObject atoObj = payloadObj.value("ATO_info").toObject();
    TC1_ATP_cutout = atoObj.value("TC1_ATP_cutout").toBool();
    TC2_ATP_cutout = atoObj.value("TC2_ATP_cutout").toBool();
    TC1_ATO_start_PB = atoObj.value("TC1_ATO_start_PB").toBool();
    TC2_ATO_start_PB = atoObj.value("TC2_ATO_start_PB").toBool();
    TC1_auto_change_end_PB = atoObj.value("TC1_auto_change_end_PB").toBool();
    TC2_auto_change_end_PB = atoObj.value("TC2_auto_change_end_PB").toBool();
    TC1_door_mode = atoObj.value("TC1_door_mode").toString();
    TC2_door_mode = atoObj.value("TC2_door_mode").toString();
    // sim_ats_fault = atoObj.value("sim_ats_fault").toString();

    // 门控制
    QJsonObject doorObj = payloadObj.value("door_control").toObject();
    A_doors_close_state = doorObj.value("A_doors_close_state").toBool();
    A_door_open_request = doorObj.value("A_door_open_request").toBool();
    A_door_close_request = doorObj.value("A_door_close_request").toBool();
    B_doors_close_state = doorObj.value("B_doors_close_state").toBool();
    B_door_open_request = doorObj.value("B_door_open_request").toBool();
    B_door_close_request = doorObj.value("B_door_close_request").toBool();

    return true;
}

/**
 * 消息的反序列化
 * @brief SimulatorJsonMsg::toJson
 * @return
 */
QByteArray SimulatorJsonMsg::toJson() const
{
    // -------------------
    // payload 部分
    // -------------------
    QJsonObject payloadObj;

    // train_basic_info
    QJsonObject trainInfoObj;
    trainInfoObj["train_code"] = train_code;
    trainInfoObj["direction"] = direction;
    trainInfoObj["train_speed_km_per_h"] = train_speed_km_per_h;
    trainInfoObj["track_code"] = track_code;
    trainInfoObj["landmark"] = landmark;
    trainInfoObj["offset_scale"] = offset_scale;
    payloadObj["train_basic_info"] = trainInfoObj;

    // cab_control_info
    QJsonObject cabObj;
    cabObj["TC1_activated"] = TC1_activated;
    cabObj["TC2_activated"] = TC2_activated;
    cabObj["active_cab_forward_state"] = active_cab_forward_state;
    cabObj["active_cab_reverse_state"] = active_cab_reverse_state;
    cabObj["train_coasting"] = train_coasting;
    cabObj["train_braking"] = train_braking;
    cabObj["train_traction"] = train_traction;
    cabObj["ATO_release"] = ATO_release;
    cabObj["traction_enabled"] = traction_enabled;
    cabObj["TC1_key_state"] = TC1_key_state;
    cabObj["TC2_key_state"] = TC2_key_state;
    cabObj["EB_applied"] = EB_applied;
    cabObj["train_mode_set"] = train_mode_set;
    payloadObj["cab_control_info"] = cabObj;

    // ATO_info
    QJsonObject atoObj;
    atoObj["TC1_ATP_cutout"] = TC1_ATP_cutout;
    atoObj["TC2_ATP_cutout"] = TC2_ATP_cutout;
    atoObj["TC1_ATO_start_PB"] = TC1_ATO_start_PB;
    atoObj["TC2_ATO_start_PB"] = TC2_ATO_start_PB;
    atoObj["TC1_auto_change_end_PB"] = TC1_auto_change_end_PB;
    atoObj["TC2_auto_change_end_PB"] = TC2_auto_change_end_PB;
    atoObj["TC1_door_mode"] = TC1_door_mode;
    atoObj["TC2_door_mode"] = TC2_door_mode;
    payloadObj["ATO_info"] = atoObj;

    // door_control
    QJsonObject doorObj;
    doorObj["A_doors_close_state"] = A_doors_close_state;
    doorObj["A_door_open_request"] = A_door_open_request;
    doorObj["A_door_close_request"] = A_door_close_request;
    doorObj["B_doors_close_state"] = B_doors_close_state;
    doorObj["B_door_open_request"] = B_door_open_request;
    doorObj["B_door_close_request"] = B_door_close_request;
    payloadObj["door_control"] = doorObj;

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

void SimulatorJsonMsg::fromSimulatorData(const SimulatorData &data)
{
    // 消息状态
    send_statius = false;

    // Header
    device_type = 2;
    session_instance_id = " "; // 可用 track_code 或其他唯一标识
    packet_type = "Sim_Train_Info";
    message_seqnum = 0;
    request_id = "";

    // 列车基本信息
    train_code = QString::number(data.train_code);
    direction = (data.direction == 0) ? "up" : "down";
    train_speed_km_per_h = data.train_speed_km_per_h;
    track_code = QString::number(data.track_code);
    landmark = QString::number(data.landmark);
    offset_scale = data.offset_scale;

    // 驾驶室信息
    TC1_activated = data.TC1_activated;
    TC2_activated = data.TC2_activated;
    active_cab_forward_state = data.active_cab_forward_state;
    active_cab_reverse_state = data.active_cab_reverse_state;
    train_coasting = data.rain_coasting;
    train_braking = data.train_braking;
    train_traction = data.train_traction;
    ATO_release = data.ATO_release;
    traction_enabled = data.traction_enabled;
    TC1_key_state = data.TC1_key_state;
    TC2_key_state = data.TC2_key_state;
    EB_applied = data.EB_applied;
    if(data.train_mode_set == 0) train_mode_set = "none";
    if(data.train_mode_set == 1) train_mode_set = "up";
    if(data.train_mode_set == 2) train_mode_set = "down";
    if(data.train_mode_set == 3) train_mode_set = "confirm";

    // ATO 信息
    TC1_ATP_cutout = data.TC1_ATP_cutout;
    TC2_ATP_cutout = data.TC2_ATP_cutout;
    TC1_ATO_start_PB = data.TC1_ATO_start_PB;
    TC2_ATO_start_PB = data.TC2_ATO_start_PB;
    TC1_auto_change_end_PB = data.TC1_auto_change_end_PB;
    TC2_auto_change_end_PB = data.TC2_auto_change_end_PB;
    TC1_door_mode = QString::number(data.TC1_door_mode);
    TC2_door_mode = QString::number(data.TC2_door_mode);

    for(int i = 0; i<20; i++){
        this->event_type_map[i] = data.sim_ats_event[i];
    }

    // 门控制
    A_doors_close_state = data.A_doors_close_state;
    A_door_open_request = data.A_door_open_request;
    A_door_close_request = data.A_door_close_request;
    B_doors_close_state = data.B_doors_close_state;
    B_door_open_request = data.B_door_open_request;
    B_door_close_request = data.B_door_close_request;

    this->UpSingleGong = data.UpSingleGong;
    this->firstDoorOpen = data.firstDoorOpen;
    this->leaveSiJiShi = data.leaveSiJiShi;
}

void SimulatorJsonMsg::fromSimulatorJsonMsg(const SimulatorJsonMsg &other)
{
    // -------------------
    // Header
    // -------------------
    device_type = other.device_type;
    session_instance_id = other.session_instance_id;
    packet_type = other.packet_type;
    message_seqnum = other.message_seqnum;
    request_id = other.request_id;

    // -------------------
    // Payload: 列车基本信息
    // -------------------
    train_code = other.train_code;
    direction = other.direction;
    train_speed_km_per_h = other.train_speed_km_per_h;
    track_code = other.track_code;
    landmark = other.landmark;
    offset_scale = other.offset_scale;

    // -------------------
    // Payload: 驾驶室信息
    // -------------------
    TC1_activated = other.TC1_activated;
    TC2_activated = other.TC2_activated;
    active_cab_forward_state = other.active_cab_forward_state;
    active_cab_reverse_state = other.active_cab_reverse_state;
    train_coasting = other.train_coasting;
    train_braking = other.train_braking;
    train_traction = other.train_traction;
    ATO_release = other.ATO_release;
    traction_enabled = other.traction_enabled;
    TC1_key_state = other.TC1_key_state;
    TC2_key_state = other.TC2_key_state;
    EB_applied = other.EB_applied;
    train_mode_set = other.train_mode_set;

    // -------------------
    // Payload: ATO信息
    // -------------------
    TC1_ATP_cutout = other.TC1_ATP_cutout;
    TC2_ATP_cutout = other.TC2_ATP_cutout;
    TC1_ATO_start_PB = other.TC1_ATO_start_PB;
    TC2_ATO_start_PB = other.TC2_ATO_start_PB;
    TC1_auto_change_end_PB = other.TC1_auto_change_end_PB;
    TC2_auto_change_end_PB = other.TC2_auto_change_end_PB;
    TC1_door_mode = other.TC1_door_mode;
    TC2_door_mode = other.TC2_door_mode;
    for(int i = 0; i<20; i++){
        this->event_type_map[i] = other.event_type_map[i];
    }

    // -------------------
    // Payload: 门控制信息
    // -------------------
    A_doors_close_state = other.A_doors_close_state;
    A_door_open_request = other.A_door_open_request;
    A_door_close_request = other.A_door_close_request;
    B_doors_close_state = other.B_doors_close_state;
    B_door_open_request = other.B_door_open_request;
    B_door_close_request = other.B_door_close_request;
    this->UpSingleGong = other.UpSingleGong;
    this-> firstDoorOpen = other.firstDoorOpen;
    this->leaveSiJiShi = other.leaveSiJiShi;
}

void SimulatorJsonMsg::fromSimulatorJsonMsg(const SimulatorJsonMsg *other)
{
    if (other == nullptr) return;
    fromSimulatorJsonMsg(*other);  // 直接复用引用版本
}
