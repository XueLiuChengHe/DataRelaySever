#include <ATSJsonMsg.h>
#include <QtEndian>

ATSJsonMsg::ATSJsonMsg()
{
    memset(&ats_data, 0, sizeof(ATSData)); // 初始化
}

ATSJsonMsg::ATSJsonMsg(const ATSData &ats_data)
{
    // ----------------------------
    // 信息标识
    // ----------------------------
    this->ats_data.device_type = ats_data.device_type;
    this->ats_data.session_instance_id = ats_data.session_instance_id;
    this->ats_data.message_seqnum = ats_data.message_seqnum;
    this->ats_data.request_id = ats_data.request_id;

    // ----------------------------
    // 场景定义数据
    // ----------------------------
    this->ats_data.session_id = ats_data.session_id;
    this->ats_data.session_name = ats_data.session_name;
    this->ats_data.course_id = ats_data.course_id;
    this->ats_data.session_line_type = ats_data.session_line_type;
    // ----------------------------
    // 列车数组
    // ----------------------------
    for(int sign = 0; sign < int(sizeof(this->ats_data.train_data) / sizeof(TrainDataStruct)); sign++){
        this->ats_data.train_data[sign].vtrain_code = ats_data.train_data[sign].vtrain_code;
        this->ats_data.train_data[sign].track_code = ats_data.train_data[sign].track_code;
        this->ats_data.train_data[sign].offset_scale = ats_data.train_data[sign].offset_scale;
        this->ats_data.train_data[sign].direction = ats_data.train_data[sign].direction;
        this->ats_data.train_data[sign].sim_train_id  = ats_data.train_data[sign].sim_train_id;
        this->ats_data.train_data[sign].sim_train_cab = ats_data.train_data[sign].sim_train_cab;
        this->ats_data.train_data[sign].trainee_id = ats_data.train_data[sign].trainee_id;
    }
    // ----------------------------
    // 场景开始时间
    // ----------------------------
    this->ats_data.nYear  = ats_data.nYear;
    this->ats_data.nMonth = ats_data.nMonth;
    this->ats_data.nDay   = ats_data.nDay;
    this->ats_data.nHour  = ats_data.nHour;
    this->ats_data.nMin   = ats_data.nMin;
    this->ats_data.nSec   = ats_data.nSec;

    // ----------------------------
    // 场景状态控制
    // ----------------------------
    // control_type 1=场景开始, 2=运行, 3=暂停, 4=场景结束
    this->ats_data.control_type = ats_data.control_type;

    // ----------------------------
    // ATS虚拟时间
    // ----------------------------
    // 解析 ats_start_time
    this->ats_data.nYear  = ats_data.nYear;
    this->ats_data.nMonth = ats_data.nMonth;
    this->ats_data.nDay   = ats_data.nDay;
    this->ats_data.nHour  = ats_data.nHour;
    this->ats_data.nMin   = ats_data.nMin;
    this->ats_data.nSec   = ats_data.nSec;

    // 解析 ats_current_time
    this->ats_data.nYearNow  = ats_data.nYearNow;
    this->ats_data.nMonthNow = ats_data.nMonthNow;
    this->ats_data.nDayNow   = ats_data.nDayNow;
    this->ats_data.nHourNow  = ats_data.nHourNow;
    this->ats_data.nMinNow   = ats_data.nMinNow;
    this->ats_data.nSecNow   = ats_data.nSecNow;

    this->ats_data.ats_elapsed_time_s = ats_data.ats_elapsed_time_s;

    // ----------------------------
    // ATP/ATO 信息
    // ----------------------------
    this->ats_data.train_code           = ats_data.train_code;
    // 当前车模式
    this->ats_data.current_car_mode = ats_data.current_car_mode;

    // 当前运行模式
    this->ats_data.current_operation_mode = ats_data.current_operation_mode;

    // 车门控制
    this->ats_data.A_doors_enable      = ats_data.A_doors_enable;
    this->ats_data.A_door_open_cmd     = ats_data.A_door_open_cmd;
    this->ats_data.A_door_close_cmd    = ats_data.A_door_close_cmd;
    this->ats_data.B_doors_enable      = ats_data.B_doors_enable;
    this->ats_data.B_door_open_cmd     = ats_data.B_door_open_cmd;
    this->ats_data.B_door_close_cmd    = ats_data.B_door_close_cmd;

    // ATP/ATO 控制
    this->ats_data.ATP_EB_cmd               = ats_data.ATP_EB_cmd;
    this->ats_data.ATO_activate             = ats_data.ATO_activate;
    this->ats_data.ATO_available            = ats_data.ATO_available;
    this->ats_data.ATO_traction_cmd         = ats_data.ATO_traction_cmd;
    this->ats_data.ATO_brake_cmd            = ats_data.ATO_brake_cmd;
    this->ats_data.auto_change_ends_cmd     = ats_data.auto_change_ends_cmd;
    this->ats_data.auto_changeend_activate  = ats_data.auto_changeend_activate;
    this->ats_data.auto_changeend_available = ats_data.auto_changeend_available;
    this->ats_data.ATO_rec_speed            = ats_data.ATO_rec_speed;

    // ----------------------------
    // MMI相关信息映射
    // ----------------------------
    this->ats_data.train_code           = ats_data.train_code;
    this->ats_data.brake_sign           = ats_data.brake_sign;
    this->ats_data.train_run_speed      = ats_data.train_run_speed;
    this->ats_data.ATP_recommended_speed= ats_data.ATP_recommended_speed;
    this->ats_data.EM_brake_trigger_speed= ats_data.EM_brake_trigger_speed;

    // train_driving_state: 惰行/牵引/制动
    this->ats_data.train_driving_state = ats_data.train_driving_state;
    // train_turn_back: 折返状态
    this->ats_data.train_turn_back = ats_data.train_turn_back;
    // position_with_depot: 车辆段状态
    this->ats_data.position_with_depot = ats_data.position_with_depot;

    // ----------------------------
    // 站台与列车状态
    // ----------------------------
    this->ats_data.next_station_skip      = ats_data.next_station_skip;
    this->ats_data.current_station_detain = ats_data.current_station_detain;
    this->ats_data.train_station_stop = ats_data.train_station_stop;
    this->ats_data.dwell_time = ats_data.dwell_time;
    this->ats_data.current_PSD_state = ats_data.current_PSD_state;
    this->ats_data.close_door_info   = ats_data.close_door_info;
    this->ats_data.departure_request = ats_data.departure_request;
    for(int i = 0;i<10;i++){
        this->ats_data.train_unit_number[i] = ats_data.train_unit_number[i];
    }
    for(int i = 0;i<10;i++){
        this->ats_data.destination_number[i] = ats_data.destination_number[i];
    }
    this->ats_data.train_real_number = ats_data.train_real_number;
    this->ats_data.platform_stop_distance = ats_data.platform_stop_distance;

    // ----------------------------
    // 虚拟列车数组
    // ----------------------------
    int sign = 0;
    while(1){
        if(sign >= 10) break;
        if(ats_data.vtrainData[sign].vtrain_code == 0){
            break;
        }
        this->ats_data.vtrainData[sign].vtrain_code = ats_data.vtrainData[sign].vtrain_code;
        this->ats_data.vtrainData[sign].track_code= ats_data.vtrainData[sign].track_code;
        this->ats_data.vtrainData[sign].offset_scale = ats_data.vtrainData[sign].offset_scale;
        this->ats_data.vtrainData[sign].direction = ats_data.vtrainData[sign].direction;
        this->ats_data.vtrainData[sign].vtrain_train_speed_kmh = ats_data.vtrainData[sign].vtrain_train_speed_kmh;
        sign++;
    }

    // ----------------------------
    // 场景信息
    // ----------------------------
    this->ats_data.Line_session_id = ats_data.session_id;

    // ----------------------------
    // OCC 虚拟时间
    // ----------------------------
    this->ats_data.nOCC_Year  = ats_data.nOCC_Year;
    this->ats_data.nOCC_Month = ats_data.nOCC_Month;
    this->ats_data.nOCC_Day   = ats_data.nOCC_Day;
    this->ats_data.nOCC_Hour  = ats_data.nOCC_Hour;
    this->ats_data.nOCC_Min   = ats_data.nOCC_Min;
    this->ats_data.nOCC_Sec   = ats_data.nOCC_Sec;

    // ----------------------------
    // 道岔状态（turnout） 0=main, 1=branch
    // ----------------------------
    for (int i = 0; i < 10; i++) {
        this->ats_data.turnout[i] = ats_data.turnout[i];
    }

    // ----------------------------
    // 信号灯状态（signal） 0=none, 1=R, 2=G, 3=Y, 5=W, 8=B, 24=RY
    // ----------------------------
    for (int i = 0; i < 100; i++) {
        this->ats_data.signal[i] = ats_data.signal[i];
    }

    // ----------------------------
    // 屏蔽门状态（PSD） 0=close, 1=open
    // ----------------------------
    for (int i = 0; i < 2; i++) {
        this->ats_data.nPSD[i] = ats_data.nPSD[i];
    }

    // ----------------------------
    // 站台倒计时（platform_countdown_timer）
    // 示例中暂不处理，若需要可扩展
    // ----------------------------
    for (int i = 0; i < 6; ++i){
        this->ats_data.platform_countdown_timer[i] = ats_data.platform_countdown_timer[i];
    }

    for(int i = 0;i<10;i++){
        this->ats_data.ats_sim_fault[i] = ats_data.ats_sim_fault[i];
    }

    this->ats_data.fault_param1 = ats_data.fault_param1;
    this->ats_data.fault_param2 = ats_data.fault_param2;
    this->ats_data.fault_param3 = ats_data.fault_param3;
}

ATSJsonMsg::~ATSJsonMsg()
{

}

void ATSJsonMsg::setSessionDefinitionData(const ATSData &ats_data)
{
    // ----------------------------
    // 信息标识
    // ----------------------------
    this->ats_data.device_type = ats_data.device_type;
    this->ats_data.session_instance_id = ats_data.session_instance_id;
    this->ats_data.message_seqnum = ats_data.message_seqnum;
    this->ats_data.request_id = ats_data.request_id;

    // ----------------------------
    // 场景定义数据
    // ----------------------------
    this->ats_data.session_id = ats_data.session_id;
    this->ats_data.session_name = ats_data.session_name;
    this->ats_data.course_id = ats_data.course_id;
    this->ats_data.session_line_type = ats_data.session_line_type;
    // ----------------------------
    // 列车数组
    // ----------------------------
    for(int sign = 0; sign < int(sizeof(this->ats_data.train_data) / sizeof(TrainDataStruct)); sign++){
        this->ats_data.train_data[sign].vtrain_code = ats_data.train_data[sign].vtrain_code;
        this->ats_data.train_data[sign].track_code = ats_data.train_data[sign].track_code;
        this->ats_data.train_data[sign].offset_scale = ats_data.train_data[sign].offset_scale;
        this->ats_data.train_data[sign].direction = ats_data.train_data[sign].direction;
        this->ats_data.train_data[sign].sim_train_id  = ats_data.train_data[sign].sim_train_id;
        this->ats_data.train_data[sign].sim_train_cab = ats_data.train_data[sign].sim_train_cab;
        this->ats_data.train_data[sign].trainee_id = ats_data.train_data[sign].trainee_id;
    }
    // ----------------------------
    // 场景开始时间
    // ----------------------------
    this->ats_data.nYear  = ats_data.nYear;
    this->ats_data.nMonth = ats_data.nMonth;
    this->ats_data.nDay   = ats_data.nDay;
    this->ats_data.nHour  = ats_data.nHour;
    this->ats_data.nMin   = ats_data.nMin;
    this->ats_data.nSec   = ats_data.nSec;
}

void ATSJsonMsg::setStateControlData(const ATSData &ats_data)
{
    // ----------------------------
    // 消息头
    // ----------------------------
    this->ats_data.message_seqnum = ats_data.message_seqnum;
    this->ats_data.request_id = ats_data.request_id;

    // ----------------------------
    // 场景状态控制
    // ----------------------------
    // control_type 1=场景开始, 2=运行, 3=暂停, 4=场景结束
    this->ats_data.control_type = ats_data.control_type;
}

void ATSJsonMsg::setVirtualTimeData(const ATSData &ats_data)
{
    // ----------------------------
    // 消息头
    // ----------------------------
    this->ats_data.message_seqnum      = ats_data.message_seqnum;
    this->ats_data.request_id          = ats_data.request_id;

    // ----------------------------
    // ATS虚拟时间
    // ----------------------------
    // 解析 ats_start_time
    this->ats_data.nYear  = ats_data.nYear;
    this->ats_data.nMonth = ats_data.nMonth;
    this->ats_data.nDay   = ats_data.nDay;
    this->ats_data.nHour  = ats_data.nHour;
    this->ats_data.nMin   = ats_data.nMin;
    this->ats_data.nSec   = ats_data.nSec;

    // 解析 ats_current_time
    this->ats_data.nYearNow  = ats_data.nYearNow;
    this->ats_data.nMonthNow = ats_data.nMonthNow;
    this->ats_data.nDayNow   = ats_data.nDayNow;
    this->ats_data.nHourNow  = ats_data.nHourNow;
    this->ats_data.nMinNow   = ats_data.nMinNow;
    this->ats_data.nSecNow   = ats_data.nSecNow;

    this->ats_data.ats_elapsed_time_s = ats_data.ats_elapsed_time_s;
}

void ATSJsonMsg::setAtpAtoInfoData(const ATSData &ats_data)
{
    this->ats_data.message_seqnum      = ats_data.message_seqnum;
    this->ats_data.request_id          = ats_data.request_id;

    // ----------------------------
    // ATP/ATO 信息
    // ----------------------------
    this->ats_data.train_code           = ats_data.train_code;
    // 当前车模式
    this->ats_data.current_car_mode = ats_data.current_car_mode;

    // 当前运行模式
    this->ats_data.current_operation_mode = ats_data.current_operation_mode;

    // 车门控制
    this->ats_data.A_doors_enable      = ats_data.A_doors_enable;
    this->ats_data.A_door_open_cmd     = ats_data.A_door_open_cmd;
    this->ats_data.A_door_close_cmd    = ats_data.A_door_close_cmd;
    this->ats_data.B_doors_enable      = ats_data.B_doors_enable;
    this->ats_data.B_door_open_cmd     = ats_data.B_door_open_cmd;
    this->ats_data.B_door_close_cmd    = ats_data.B_door_close_cmd;

    // ATP/ATO 控制
    this->ats_data.ATP_EB_cmd               = ats_data.ATP_EB_cmd;
    this->ats_data.ATO_activate             = ats_data.ATO_activate;
    this->ats_data.ATO_available            = ats_data.ATO_available;
    this->ats_data.ATO_traction_cmd         = ats_data.ATO_traction_cmd;
    this->ats_data.ATO_brake_cmd            = ats_data.ATO_brake_cmd;
    this->ats_data.auto_change_ends_cmd     = ats_data.auto_change_ends_cmd;
    this->ats_data.auto_changeend_activate  = ats_data.auto_changeend_activate;
    this->ats_data.auto_changeend_available = ats_data.auto_changeend_available;
    this->ats_data.ATO_rec_speed            = ats_data.ATO_rec_speed;
}

void ATSJsonMsg::setMmiInfoData(const ATSData &ats_data)
{
    this->ats_data.message_seqnum      = ats_data.message_seqnum;
    this->ats_data.request_id          = ats_data.request_id;

    // ----------------------------
    // MMI相关信息映射
    // ----------------------------
    this->ats_data.train_code           = ats_data.train_code;
    this->ats_data.brake_sign           = ats_data.brake_sign;
    this->ats_data.train_run_speed      = ats_data.train_run_speed;
    this->ats_data.ATP_recommended_speed= ats_data.ATP_recommended_speed;
    this->ats_data.EM_brake_trigger_speed= ats_data.EM_brake_trigger_speed;

    // train_driving_state: 惰行/牵引/制动
    this->ats_data.train_driving_state = ats_data.train_driving_state;
    // train_turn_back: 折返状态
    this->ats_data.train_turn_back = ats_data.train_turn_back;
    // position_with_depot: 车辆段状态
    this->ats_data.position_with_depot = ats_data.position_with_depot;

    // ----------------------------
    // 站台与列车状态
    // ----------------------------
    this->ats_data.next_station_skip      = ats_data.next_station_skip;
    this->ats_data.current_station_detain = ats_data.current_station_detain;
    this->ats_data.train_station_stop = ats_data.train_station_stop;
    this->ats_data.dwell_time = ats_data.dwell_time;
    this->ats_data.current_PSD_state = ats_data.current_PSD_state;
    this->ats_data.close_door_info   = ats_data.close_door_info;
    this->ats_data.departure_request = ats_data.departure_request;
    for(int i = 0;i<10;i++){
        this->ats_data.train_unit_number[i] = ats_data.train_unit_number[i];
    }
    for(int i = 0;i<10;i++){
        this->ats_data.destination_number[i] = ats_data.destination_number[i];
    }
    this->ats_data.train_real_number = ats_data.train_real_number;
    this->ats_data.platform_stop_distance = ats_data.platform_stop_distance;
}

void ATSJsonMsg::setAutomaticInfoData(const ATSData &ats_data)
{
    // ----------------------------
    // 消息头
    // ----------------------------
    this->ats_data.message_seqnum = ats_data.message_seqnum;
    this->ats_data.request_id  = ats_data.request_id;

    // ----------------------------
    // 虚拟列车数组
    // ----------------------------
    int sign = 0;
    while(1){
        if(sign >= 10) break;
        if(ats_data.vtrainData[sign].vtrain_code == 0){
            break;
        }
        this->ats_data.vtrainData[sign].vtrain_code = ats_data.vtrainData[sign].vtrain_code;
        this->ats_data.vtrainData[sign].track_code= ats_data.vtrainData[sign].track_code;
        this->ats_data.vtrainData[sign].offset_scale = ats_data.vtrainData[sign].offset_scale;
        this->ats_data.vtrainData[sign].direction = ats_data.vtrainData[sign].direction;
        this->ats_data.vtrainData[sign].vtrain_train_speed_kmh = ats_data.vtrainData[sign].vtrain_train_speed_kmh;
        sign++;
    }
}

void ATSJsonMsg::setlineDeviceData(const ATSData &ats_data)
{
    this->ats_data.message_seqnum      = ats_data.message_seqnum;
    this->ats_data.request_id          = ats_data.request_id;

    // ----------------------------
    // 场景信息
    // ----------------------------
    this->ats_data.Line_session_id = ats_data.session_id;

    // ----------------------------
    // OCC 虚拟时间
    // ----------------------------
    this->ats_data.nOCC_Year  = ats_data.nOCC_Year;
    this->ats_data.nOCC_Month = ats_data.nOCC_Month;
    this->ats_data.nOCC_Day   = ats_data.nOCC_Day;
    this->ats_data.nOCC_Hour  = ats_data.nOCC_Hour;
    this->ats_data.nOCC_Min   = ats_data.nOCC_Min;
    this->ats_data.nOCC_Sec   = ats_data.nOCC_Sec;

    // ----------------------------
    // 道岔状态（turnout） 0=main, 1=branch
    // ----------------------------
    for (int i = 0; i < 10; i++) {
        this->ats_data.turnout[i] = ats_data.turnout[i];
    }

    // ----------------------------
    // 信号灯状态（signal） 0=none, 1=R, 2=G, 3=Y, 5=W, 8=B, 24=RY
    // ----------------------------
    for (int i = 0; i < 100; i++) {
        this->ats_data.signal[i] = ats_data.signal[i];
    }

    // ----------------------------
    // 屏蔽门状态（PSD） 0=close, 1=open
    // ----------------------------
    for (int i = 0; i < 2; i++) {
        this->ats_data.nPSD[i] = ats_data.nPSD[i];
    }

    // ----------------------------
    // 站台倒计时（platform_countdown_timer）
    // 示例中暂不处理，若需要可扩展
    // ----------------------------
    for (int i = 0; i < 6; ++i){
        this->ats_data.platform_countdown_timer[i] = ats_data.platform_countdown_timer[i];
    }
}

void ATSJsonMsg::setATSSIMFaultInfoData(const ATSData &ats_data)
{
    // ----------------------------
    // ATSSIMFault相关信息映射
    // ----------------------------
    for(int i = 0;i<10;i++){
        this->ats_data.ats_sim_fault[i] = ats_data.ats_sim_fault[i];
    }

    this->ats_data.fault_param1 = ats_data.fault_param1;
    this->ats_data.fault_param2 = ats_data.fault_param2;
    this->ats_data.fault_param3 = ats_data.fault_param3;
}

void ATSJsonMsg::setChangeTurnoutData(const ATSData &ats_data)
{
    for(int i = 0; i<50;i++){
        this->ats_data.changeTurnout[i] = ats_data.changeTurnout[i];
    }

    for(int i = 0; i<50;i++){
        this->ats_data.changeTurnoutID[i] = ats_data.changeTurnoutID[i];
    }
}

void ATSJsonMsg::setChangeSignalData(const ATSData &ats_data)
{
    for(int i = 0; i<50;i++){
        this->ats_data.changeSignal[i] = ats_data.changeSignal[i];
    }

    for(int i = 0; i<50;i++){
        this->ats_data.changeSignalID[i] = ats_data.changeSignalID[i];
    }
}
