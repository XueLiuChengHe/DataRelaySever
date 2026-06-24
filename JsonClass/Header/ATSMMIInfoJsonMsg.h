#ifndef ATSMMIINFOJSONMSG_H
#define ATSMMIINFOJSONMSG_H

#include <QString>
#include <StructClsss.h>




class ATSMMIInfoJsonMsg{
public:
    ATSMMIInfoJsonMsg();
    ATSMMIInfoJsonMsg(const ATSMMIInfoJsonMsg &other); ///< 对象复制
    ~ATSMMIInfoJsonMsg();

    // -------------------------
    // JSON 序列化 / 反序列化
    // -------------------------
    bool fromJson(const QString &jsonStr);   ///< 从 JSON 字符串解析
    QByteArray toJson() const;               ///< 转成 JSON 字符串

    ATSData toATSData();            // 将数据转化为ATSData结构体

    void fromMMIMsg(const ATSMMIInfoJsonMsg &other);

public:
    // -------------------------
    // 消息状态
    // -------------------------
    bool send_status = false;             ///< 消息是否发送的状态

    // -------------------------
    // 消息头信息
    // -------------------------
    int device_type;             ///< 设备类型
    QString session_instance_id;     ///< 会话实例 ID
    QString packet_type;             ///< 消息类型
    int message_seqnum;              ///< 消息序号
    QString request_id;              ///< 请求 ID

    // -------------------------
    // payload 内容
    // -------------------------
    QString train_code;              ///< 列车编号
    QString brake_sign;              ///< 制动信号
    int train_run_speed;             ///< 列车当前速度
    int ATP_recommended_speed;       ///< ATP 推荐速度
    int EM_brake_trigger_speed;      ///< 紧急制动触发速度
    QString train_driving_state;     ///< 列车运行状态
    QString train_turn_back;         ///< 列车折返信息
    QString position_with_depot;     ///< 与车辆段位置关系

    bool next_station_skip;       ///< 是否跳过下一站
    bool current_station_detain;  ///< 是否滞留当前站
    QString train_station_stop;           ///< 停靠车站
    QString dwell_time;              ///< 停站时间
    QString current_PSD_state;       ///< 屏蔽门状态
    bool close_door_info;         ///< 关门信息
    bool departure_request;       ///< 发车请求

    QString train_unit_number;       ///< 车组号
    QString destination_number;      ///< 终点站编号
    QString train_real_number;          ///< 车次号

    int platform_stop_distance;         ///< 站点停车点距离(0到999)其中越标多少米(-1到-999)没有进站-10000
};

#endif // ATSMMIINFOJSONMSG_H
