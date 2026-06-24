#ifndef SIMATSEVENTINFOMSG_H
#define SIMATSEVENTINFOMSG_H

#include "ATSSIMFaultInfo.h"

#include <QString>
#include <StructClsss.h>

class SimAtsEventInfoMsg{
public:
    SimAtsEventInfoMsg();
    SimAtsEventInfoMsg(const SimAtsEventInfoMsg &other); ///< 对象复制
    ~SimAtsEventInfoMsg();

    // -------------------------
    // JSON 序列化 / 反序列化
    // -------------------------
    QByteArray toJson() const;               ///< 转成 JSON 字符串

    void fromATSSimFaultInfo(const ATSSIMFaultInfo atsSimFaultInfo);

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
    QString event_type;
    QString event_ctrl;
    QString event_param1;
    QString event_param2;
    QString event_param3;
};

#endif // SIMATSEVENTINFOMSG_H
