#ifndef ATSATPATOINFOJSONMSG_H
#define ATSATPATOINFOJSONMSG_H

#include <QString>
#include <StructClsss.h>



class ATSATPATOInfoJsonMsg{
public:
    ATSATPATOInfoJsonMsg();
    ATSATPATOInfoJsonMsg(const ATSATPATOInfoJsonMsg &other); ///< 对象复制
    ~ATSATPATOInfoJsonMsg();

    // -------------------------
    // JSON 序列化 / 反序列化
    // -------------------------
    bool fromJson(const QString &jsonStr);   ///< 从 JSON 字符串解析
    QByteArray toJson() const;               ///< 转成 JSON 字符串

    ATSData toATSData();                    // 将数据转化为ATSData结构体

    void fromATPATOMsg(const ATSATPATOInfoJsonMsg &other);

public:
    // -------------------------
    // 消息状态
    // -------------------------
    bool send_status;             ///< 消息是否发送的状态

    // -------------------------
    // 消息头信息
    // -------------------------
    int device_type;          ///< 设备类型
    QString session_instance_id;  ///< 会话实例 ID
    QString packet_type;          ///< 消息类型
    int message_seqnum;           ///< 消息序号
    QString request_id;           ///< 请求 ID

    // -------------------------
    // payload 内容
    // -------------------------
    QString train_code;           ///< 列车号

    // ---- modes ----
    QString current_car_mode;     ///< 当前车模式
    QString current_operation_mode; ///< 当前运行模式
    // QString ats_sim_fault; ///< ATS发出列车故障

    // ---- door_control ----
    bool A_doors_enable;
    bool A_door_open_cmd;
    bool A_door_close_cmd;
    bool B_doors_enable;
    bool B_door_open_cmd;
    bool B_door_close_cmd;

    // ---- ATP_ATO_control ----
    bool ATP_EB_cmd;
    bool ATO_activate;
    bool ATO_available;
    bool ATO_traction_cmd;
    bool ATO_brake_cmd;
    bool auto_change_ends_cmd;
    bool auto_changeend_activate;
    bool auto_changeend_available;
    int  ATO_rec_speed;
};

#endif // ATSATPATOINFOJSONMSG_H
