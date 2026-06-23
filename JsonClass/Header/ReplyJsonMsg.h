#ifndef REPLYJSONMSG_H
#define REPLYJSONMSG_H

#include <QString>
#include <StructClsss.h>


class ReplyJsonMsg{
public:
    ReplyJsonMsg();
    ReplyJsonMsg(const ReplyJsonMsg &reply_josn_msg);
    ~ReplyJsonMsg();

public:
    bool fromJson(const QString &jsonStr);
    QByteArray toJson();

public:
    bool send_statius;                    ///< 发送状态
    int msg_max;                          ///< 消息缓存的最大数量

    // -------------------------
    // 消息头信息(Header)
    // -------------------------
    int device_type;                  ///< 设备类型
    QString session_instance_id;          ///< 会话实例ID
    QString packet_type;                  ///< 消息类型
    int message_seqnum;                   ///< 消息序号
    QString request_id;                   ///< 请求ID

    // -------------------------
    // payload
    // -------------------------
    QString result_code = "200";                      ///< 回答编号（200）
    QString result_desc = "ok";                     ///< 回复描述（true:"ok"）

};

#endif // REPLYJSONMSG_H
