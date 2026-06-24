#ifndef ATSAUTOMATICTRAININFOJSONMSG_H
#define ATSAUTOMATICTRAININFOJSONMSG_H

#include "SessionTrainData.h"

#include <QList>
#include <QVector>
#include <StructClsss.h>

class ATSAutomaticTrainInfoJsonMsg{
public:
    ATSAutomaticTrainInfoJsonMsg();
    ATSAutomaticTrainInfoJsonMsg(const ATSAutomaticTrainInfoJsonMsg &other); ///< 对象复制
    ~ATSAutomaticTrainInfoJsonMsg();

    // -------------------------
    // JSON 序列化 / 反序列化
    // -------------------------
    bool fromJson(const QString &jsonStr);   ///< 从 JSON 字符串解析
    QByteArray toJson() const;               ///< 转成 JSON 字符串

    ATSData toATSData(QString vtrainCode);            // 将数据转化为ATSData结构体

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
    QList<TrainData> automatic_train_info;
};

#endif // ATSAUTOMATICTRAININFOJSONMSG_H
