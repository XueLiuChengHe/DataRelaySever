#ifndef ATSSTATECONTROLJSONMSG_H
#define ATSSTATECONTROLJSONMSG_H

#include <QString>
#include <StructClsss.h>




class ATSStateControlJsonMsg{
public:
    ATSStateControlJsonMsg();
    ATSStateControlJsonMsg(const ATSStateControlJsonMsg &other); ///< 对象复制
    ~ATSStateControlJsonMsg();


    // -------------------------
    // JSON 序列化 / 反序列化
    // -------------------------
    bool fromJson(const QString &jsonStr);  // 从 JSON 字符串解析
    QByteArray toJson() const;             // 转成 JSON 字符串

    ATSData toATSData();            // 将数据转化为ATSData结构体

public:

    // -------------------------
    // 消息状态
    // -------------------------
    bool send_statius;             ///< 消息是否发送的状态

    // -------------------------
    // 消息头信息
    // -------------------------
    int device_type;           ///< 设备类型
    QString session_instance_id;   ///< 会话实例 ID
    QString packet_type;           ///< 消息类型
    int message_seqnum;            ///< 消息序号
    QString request_id;            ///< 请求 ID

    // -------------------------
    // payload 内容
    // -------------------------
    QString control_type;          ///< 控制类型 (run / pause / stop)
};

#endif // ATSSTATECONTROLJSONMSG_H
