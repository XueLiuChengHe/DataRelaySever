#ifndef SESSIONTRAINDATA_H
#define SESSIONTRAINDATA_H

#include <QJsonObject>


class TrainData
{
public:
    TrainData();
    TrainData(const TrainData &other);
    ~TrainData();

public:
    QString vtrain_code;    ///< 虚拟列车编号
    QString track_code;     ///< 道岔/轨道编号
    int offset_scale;       ///< 偏移量
    QString direction;      ///< 行驶方向
    QString sim_train_id;   ///< 仿真列车ID
    QString sim_train_cab;  ///< 仿真列车驾驶室
    QString trainee_id;     ///< 受训人员ID
    int train_speed_kmh;    ///< 列车行驶速度
};

#endif // SESSIONTRAINDATA_H
