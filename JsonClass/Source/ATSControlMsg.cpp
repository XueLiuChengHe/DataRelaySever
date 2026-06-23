#include "ATSConstrolMsg.h"

#include <QJsonObject>

ATSControlMsg::ATSControlMsg()
    : train_code(0),
    current_car_mode(0),
    current_operation_mode(0),

    A_doors_enable(false),
    A_door_open_cmd(false),
    A_door_close_cmd(false),
    B_doors_enable(false),
    B_door_open_cmd(false),
    B_door_close_cmd(false),

    ATP_EB_cmd(false),
    ATO_activate(false),
    ATO_available(false),
    ATO_traction_cmd(false),
    ATO_brake_cmd(false),
    auto_change_ends_cmd(false),
    auto_changeend_activate(false),
    auto_changeend_available(false),
    ATO_rec_speed(0),

    train_code2(0),
    brake_sign(0),
    train_run_speed(0),
    ATP_recommended_speed(0),
    EM_brake_trigger_speed(0),
    train_driving_state(0),
    train_turn_back(0),
    position_with_depot(0),

    next_station_skip(false),
    current_station_detain(false),
    train_station_stop(0),
    dwell_time(0),
    current_PSD_state(0),
    close_door_info(false),
    departure_request(false),

    train_unit_number(""),
    destination_number(""),
    train_real_number(0),
    fault_type("0"),
    fault_ctrl("0"),
    fault_param1("0"),
    fault_param2("0"),
    fault_param3("0")
{

}

ATSControlMsg::~ATSControlMsg()
{
}

// bool ATSControlMsg::fromJson(const QString &jsonStr)
// {

// }

QByteArray ATSControlMsg::toJson() const{
    QJsonObject json;

    // ATOATP
    json["train_code"] = train_code;
    json["current_car_mode"] = current_car_mode;
    json["current_operation_mode"] = current_operation_mode;

    json["A_doors_enable"] = A_doors_enable;
    json["A_door_open_cmd"] = A_door_open_cmd;
    json["A_door_close_cmd"] = A_door_close_cmd;
    json["B_doors_enable"] = B_doors_enable;
    json["B_door_open_cmd"] = B_door_open_cmd;
    json["B_door_close_cmd"] = B_door_close_cmd;

    json["ATP_EB_cmd"] = ATP_EB_cmd;
    json["ATO_activate"] = ATO_activate;
    json["ATO_available"] = ATO_available;
    json["ATO_traction_cmd"] = ATO_traction_cmd;
    json["ATO_brake_cmd"] = ATO_brake_cmd;
    json["auto_change_ends_cmd"] = auto_change_ends_cmd;
    json["auto_changeend_activate"] = auto_changeend_activate;
    json["auto_changeend_available"] = auto_changeend_available;
    json["ATO_rec_speed"] = ATO_rec_speed;

    // MMI
    json["train_code2"] = train_code2;
    json["brake_sign"] = brake_sign;
    json["train_run_speed"] = train_run_speed;
    json["ATP_recommended_speed"] = ATP_recommended_speed;
    json["EM_brake_trigger_speed"] = EM_brake_trigger_speed;
    json["train_driving_state"] = train_driving_state;
    json["train_turn_back"] = train_turn_back;
    json["position_with_depot"] = position_with_depot;

    json["next_station_skip"] = next_station_skip;
    json["current_station_detain"] = current_station_detain;
    json["train_station_stop"] = train_station_stop;
    json["dwell_time"] = dwell_time;
    json["current_PSD_state"] = current_PSD_state;
    json["close_door_info"] = close_door_info;
    json["departure_request"] = departure_request;

    json["train_unit_number"] = train_unit_number;
    json["destination_number"] = destination_number;
    json["train_real_number"] = train_real_number;
    json["platform_stop_distance"] = platform_stop_distance;

    json["fault_type"] = fault_type;
    json["fault_ctrl"] = fault_ctrl;
    json["fault_param1"] = fault_param1;
    json["fault_param2"] = fault_param2;
    json["fault_param3"] = fault_param3;

    return QJsonDocument(json).toJson(QJsonDocument::Indented);
}


void ATSControlMsg::setATPATOMsg(ATSATPATOInfoJsonMsg atsATPATOInfoJsonMsg)
{
    ATSData data = atsATPATOInfoJsonMsg.toATSData();

    // ----------------------------
    // ATP/ATO 信息
    // ----------------------------
    this->train_code   = data.train_code;
    // 当前车模式
    this->current_car_mode = data.current_car_mode;

    // 当前运行模式
    this->current_operation_mode = data.current_operation_mode;

    // 车门控制
    this->A_doors_enable      = data.A_doors_enable;
    this->A_door_open_cmd     = data.A_door_open_cmd;
    this->A_door_close_cmd    = data.A_door_close_cmd;
    this->B_doors_enable      = data.B_doors_enable;
    this->B_door_open_cmd     = data.B_door_open_cmd;
    this->B_door_close_cmd    = data.B_door_close_cmd;

    // ATP/ATO 控制
    this->ATP_EB_cmd               = data.ATP_EB_cmd;
    this->ATO_activate             = data.ATO_activate;
    this->ATO_available            = data.ATO_available;
    this->ATO_traction_cmd         = data.ATO_traction_cmd;
    this->ATO_brake_cmd            = data.ATO_brake_cmd;
    this->auto_change_ends_cmd     = data.auto_change_ends_cmd;
    this->auto_changeend_activate  = data.auto_changeend_activate;
    this->auto_changeend_available = data.auto_changeend_available;
    this->ATO_rec_speed            = data.ATO_rec_speed;
}

void ATSControlMsg::setMMIMsg(ATSMMIInfoJsonMsg atsMMIMsg)
{
    ATSData data =  atsMMIMsg.toATSData();
    // ---- 基本信息 ----
    this->train_code2 = data.train_code;

    // ---- 运行信息 ----
    this->brake_sign = data.brake_sign;
    this->train_run_speed = data.train_run_speed;
    this->ATP_recommended_speed = data.ATP_recommended_speed;
    this->EM_brake_trigger_speed = data.EM_brake_trigger_speed;

    this->train_driving_state = data.train_driving_state;
    this->train_turn_back = data.train_turn_back;
    this->position_with_depot = data.position_with_depot;

    // ---- 站点控制 ----
    this->next_station_skip = data.next_station_skip;
    this->current_station_detain = data.current_station_detain;

    this->train_station_stop = data.train_station_stop;
    this->dwell_time = data.dwell_time;
    this->current_PSD_state = data.current_PSD_state;

    this->close_door_info = data.close_door_info;
    this->departure_request = data.departure_request;

    // ---- 编号信息 ----
    for(int i = 0;i<10;i++){
        this->train_unit_number[i] = data.train_unit_number[i];
    }
    for(int i = 0;i<10;i++){
        this->destination_number[i] = data.destination_number[i];
    }
    this->train_real_number = data.train_real_number;

    this->platform_stop_distance = data.platform_stop_distance;
}

void ATSControlMsg::setATSSIMFaultIngoMsg(ATSSIMFaultInfo atsSimFaultMsg)
{
    this->fault_type = atsSimFaultMsg.fault_type;
    this->fault_ctrl = atsSimFaultMsg.fault_ctrl;
    this->fault_param1 = atsSimFaultMsg.fault_param1;
    this->fault_param2 = atsSimFaultMsg.fault_param2;
    this->fault_param3 = atsSimFaultMsg.fault_param3;
}