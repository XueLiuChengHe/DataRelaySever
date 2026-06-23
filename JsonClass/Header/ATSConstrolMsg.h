#ifndef ATSCONTROLMSG_H
#define ATSCONTROLMSG_H

#include "ATSATPATOinfoJsonMsg.h"
#include "ATSMMIInfoJsonMsg.h"
#include "ATSSIMFaultInfo.h"

#include <QByteArray>
#include <QString>


class ATSControlMsg
{
public:
    ATSControlMsg();
    ~ATSControlMsg();

    // -------------------------
    // JSON 序列化 / 反序列化
    // -------------------------
    // bool fromJson(const QString &jsonStr);   ///< 从 JSON 字符串解析
    QByteArray toJson() const;               ///< 转成 JSON 字符串

    void setATPATOMsg(ATSATPATOInfoJsonMsg atsATPATOInfoJsonMsg); // 设置atpats相关的信息
    void setMMIMsg(ATSMMIInfoJsonMsg atsMMIMsg); // 设置mmi相关信息
    void setATSSIMFaultIngoMsg(ATSSIMFaultInfo atsSimFaultMsg);

public:
    // -------------------------
    // 消息状态
    // -------------------------
    bool send_status;             ///< 消息是否发送的状态

    // ----------------------------------------
    // ATP/ATO数据
    // ----------------------------------------
    int train_code;                     //列车编号
    // modes:模式
    int current_car_mode;               //当前列车驾驶模式  1-初始状态  2-ATO模式  3-CM(ATP监控模式)  4-RM限制模式  5EUM(ATP旁路)
    int current_operation_mode;         //当前运营等级  1-初始状态  2-"IXL" 联锁级  3-"BLOC" 点式 ATP    4CBTC" 连续式通信

    // door_control:车门控制
    bool A_doors_enable;                ///< A 侧门使能指令: true=激活, false=不激活
    bool A_door_open_cmd;               ///< A 侧门开指令: true=激活, false=不激活
    bool A_door_close_cmd;              ///< A 侧门关指令: true=激活, false=不激活
    bool B_doors_enable;                ///< B 侧门使能指令: true=激活, false=不激活
    bool B_door_open_cmd;               ///< B   侧门开指令: true=激活, false=不激活
    bool B_door_close_cmd;              ///< B 侧门关指令: true=激活, false=不激活
    // ATP_ATO_control:ATP/ATO 控制
    bool ATP_EB_cmd;                    ///< ATP 紧急制动指令: true=EB Release (缓解), false=EB Apply (施加)
    bool ATO_activate;                  ///< ATO 激活指令: true=激活, false=不激活
    bool ATO_available;                 ///< ATO 准备就绪: true=激活, false=不激活
    bool ATO_traction_cmd;              ///< ATO 牵引状态指令: true=激活, false=不激活
    bool ATO_brake_cmd;                 ///< ATO 制动状态指令: true=激活, false=不激活
    bool auto_change_ends_cmd;          ///< 自动折返指令: true=激活, false=不激活
    bool auto_changeend_activate;       ///< 自动折返激活: true=激活, false=不激活
    bool auto_changeend_available;      ///< 自动折返准备就绪: true=激活, false=不激活
    int  ATO_rec_speed;                 ///< ATO 推荐速度/牵引制动指令: 1% ~ 100% (值范围: 1 ~ 100)  ？？？怎么区分牵引还是制动

    // --------------------------------
    // MMI 信号屏信息
    // --------------------------------
    int train_code2;                    ///< 列车编号 (ATS虚拟车编号), eg: 01
    int brake_sign;                     ///< 制动标志: 1=初始, 2=制动请求, 3=紧急制动施加
    int train_run_speed;                ///< 列车当前速度: 0-100 km/h
    int ATP_recommended_speed;          ///< ATP 推荐速度: 0-100 km/h
    int EM_brake_trigger_speed;         ///< 紧急制动触发速度: 0-100 km/h
    int train_driving_state;            ///< 牵引制动状态: 1=惰行, 2=牵引, 3=制动
    int train_turn_back;                ///< 列车折返状态: 1=不在折返区, 2=可折返, 3=已确认折返
    int position_with_depot;            ///< 是否进入车辆段: 1=不显示, 2=进入, 3=在段内缓行
    bool next_station_skip;             ///< 下一站是否跳停: true=跳停, false=不跳停
    bool current_station_detain;        ///< 当前站台扣车: true=扣车, false=不扣车
    int train_station_stop;             ///< 停车状态: 1=不在站台, 2=未停准, 3=精准停车
    int dwell_time;                     ///< 剩余停站时间: 1=不显示, 2=剩余5秒（或其他数值也可扩展）
    int current_PSD_state;              ///< 屏蔽门状态: 1=未关闭, 2=关闭, 3=未知
    bool close_door_info;               ///< 提示关闭车门: true=提示, false=未提示
    bool departure_request;             ///< 发车请求: true=有请求, false=无请求
    char train_unit_number[10];              ///< 车次号 (具体内容待定)
    char destination_number[10];             ///< 目的地号 (具体内容待定)
    int train_real_number;
    int platform_stop_distance;                 ///< 站点停车点距离(0到999)其中越标多少米(-1到-999)没有进站-10000

    // --------------------------------
    // 列车故障信息
    // --------------------------------
    QString fault_type;     ///< 故障类型
    QString fault_ctrl;         ///<故障是否激活
    QString fault_param1;   ///<故障参数
    QString fault_param2;
    QString fault_param3;
};

#endif // ATSCONTROLMSG_H
