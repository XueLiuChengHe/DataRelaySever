#ifndef ATSSESSIONDEFINITIONJSONMSG_H
#define ATSSESSIONDEFINITIONJSONMSG_H

#include "SessionTrainData.h"

#include <QList>
#include <StructClsss.h>


class ATSSessionDefinitionJsonMsg{
public:
    ATSSessionDefinitionJsonMsg();
    ATSSessionDefinitionJsonMsg(const ATSSessionDefinitionJsonMsg &other); // 对象复制
    ~ATSSessionDefinitionJsonMsg();


    // 序列化 / 反序列化
    bool fromJson(const QString &jsonStr);
    QByteArray toJson() const;

    ATSData toATSData();            // 将数据转化为ATSData结构体

public:
    // -------------------------
    // 消息头信息(Header)
    // -------------------------
    int device_type;           ///< 设备类型
    QString session_instance_id;   ///< 会话实例ID
    QString packet_type;           ///< 消息类型
    int message_seqnum;        ///< 消息序号
    QString request_id;            ///< 请求ID

    // -------------------------
    // payload
    // -------------------------
    QString session_id;            ///< 会话ID
    QString session_name;          ///< 会话名称
    QString course_id;                  ///< 课程ID(2026/5/7 协议新增)
    QString session_start_time;    ///< 会话开始时间
    QString session_line_type;     ///< 线路类型

    // -------------------------
    // 列车数组
    // -------------------------
    QList<TrainData> session_train_data;

    // -------------------------
    // 消息状态
    // -------------------------
    bool send_statius;             ///< 消息是否发送的状态
};

#endif // ATSSESSIONDEFINITIONJSONMSG_H
