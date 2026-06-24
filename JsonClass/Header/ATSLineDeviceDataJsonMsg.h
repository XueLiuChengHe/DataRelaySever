#ifndef ATSLINEDEVICEDATAJSONMSG_H
#define ATSLINEDEVICEDATAJSONMSG_H

#include <QMap>
#include <StructClsss.h>


class ATSLineDeviceDataJsonMsg{
public:
    ATSLineDeviceDataJsonMsg();
    ATSLineDeviceDataJsonMsg(const ATSLineDeviceDataJsonMsg &other); ///< 对象复制
    ~ATSLineDeviceDataJsonMsg();

    // -------------------------
    // JSON 序列化 / 反序列化
    // -------------------------
    bool fromJson(const QString &jsonStr);
    QByteArray toJson() const;

    ATSData toATSData(const QMap<QString,int> * turn_config_map);            // 将数据转化为ATSData结构体

    void fromLineDataMsg(const ATSLineDeviceDataJsonMsg &other);

public:
    // -------------------------
    // 消息状态
    // -------------------------
    bool send_status = false;

    // -------------------------
    // 消息头信息
    // -------------------------
    int device_type;
    QString session_instance_id;
    QString packet_type;
    int message_seqnum;
    QString request_id;

    // -------------------------
    // payload 内容
    // -------------------------
    QString session_id;
    QString occ_virtual_time;

    QMap<QString, QString> turnout;                   ///< 道岔状态
    QMap<QString, QString> signal;                   ///< 信号状态
    QMap<QString, QString> PSD;                      ///< 屏蔽门状态
    QMap<QString, QString> platform_countdown_timer; ///< 站台倒计时

};

#endif // ATSLINEDEVICEDATAJSONMSG_H
