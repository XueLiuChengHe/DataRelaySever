#ifndef SIMRUNNINGLOG_H
#define SIMRUNNINGLOG_H

#include <QString>
#include <StructClsss.h>


class SimRunningLog{
public:
    SimRunningLog();
    SimRunningLog(const SimRunningLog &other); // 对象复制
    ~SimRunningLog();


    /* ------- 序列化与反序列化函数 ------- */
    QByteArray toJson() const; // 反序列化函数

public:

    // -------------------------
    // 消息状态
    // -------------------------
    bool send_statius;                    ///< 消息是否发送的状态

    // -------------------------
    // 消息头信息(Header)
    // -------------------------
    int device_type;                  ///< 设备类型
    QString session_instance_id;          ///< 会话实例ID
    QString packet_type;                  ///< 消息类型
    int message_seqnum;                   ///< 消息序号
    QString request_id;                   ///< 请求ID

    // -------------------------
    // 日志消息
    // -------------------------
    QString log_message;           ///< 驾驶室1是否激活
};
#endif // SIMRUNNINGLOG_H
