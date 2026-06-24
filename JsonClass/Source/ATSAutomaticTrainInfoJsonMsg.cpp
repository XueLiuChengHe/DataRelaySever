#include <ATSAutomaticTrainInfoJsonMsg.h>

#include <QJsonArray>
#include <QJsonParseError>


ATSAutomaticTrainInfoJsonMsg::ATSAutomaticTrainInfoJsonMsg() :
    send_status(false)
  , device_type(0)
  , session_instance_id("000")
  , packet_type("0")
  , message_seqnum(0)
  , request_id("0")
  , session_id("0")
{

}

ATSAutomaticTrainInfoJsonMsg::ATSAutomaticTrainInfoJsonMsg(const ATSAutomaticTrainInfoJsonMsg &other)
{
    this->send_status         = other.send_status;
    this->device_type         = other.device_type;
    this->session_instance_id = other.session_instance_id;
    this->packet_type         = other.packet_type;
    this->message_seqnum      = other.message_seqnum;
    this->request_id          = other.request_id;

    this->session_id          = other.session_id;
    this->automatic_train_info = other.automatic_train_info;
}

ATSAutomaticTrainInfoJsonMsg::~ATSAutomaticTrainInfoJsonMsg()
{

}

/**
 * SimulatorTrainInfo消息的反序列化函数
 * @brief ATSAutomaticTrainInfoJsonMsg::fromJson
 * @param jsonStr
 * @return
 */
bool ATSAutomaticTrainInfoJsonMsg::fromJson(const QString &jsonStr)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8(), &error);
    if (error.error != QJsonParseError::NoError || !doc.isObject())
        return false;

    QJsonObject obj = doc.object();

    // 消息头
    this->device_type         = obj.value("device_type").toInt();
    this->session_instance_id = obj.value("session_instance_id").toString();
    this->packet_type         = obj.value("packet_type").toString();
    this->message_seqnum      = obj.value("message_seqnum").toInt();
    this->request_id          = obj.value("request_id").toString();

    // payload
    if (obj.contains("payload") && obj["payload"].isObject()) {
        QJsonObject payload = obj["payload"].toObject();
        this->session_id = payload.value("session_id").toString();

        this->automatic_train_info.clear();
        if (payload.contains("automatic_train_info") && payload["automatic_train_info"].isArray()) {
            QJsonArray arr = payload["automatic_train_info"].toArray();
            if(!arr.isEmpty()){
                for (const QJsonValue &val : arr) {
                    if (!val.isObject()) continue;
                    QJsonObject trainObj = val.toObject();
                    TrainData trainInfo;
                    trainInfo.vtrain_code     = trainObj.value("vtrain_code").toString();
                    trainInfo.track_code      = trainObj.value("track_code").toString();
                    trainInfo.offset_scale    = trainObj.value("offset_scale").toInt();
                    trainInfo.direction       = trainObj.value("direction").toString();
                    trainInfo.train_speed_kmh = trainObj.value("train_speed_kmh").toInt();

                    this->automatic_train_info.append(trainInfo);
                }
            }
        }
    }

    return true;
}

/**
 * SimulatorTrainInfo消息的序列化函数
 * @brief ATSAutomaticTrainInfoJsonMsg::toJson
 * @return
 */
QByteArray ATSAutomaticTrainInfoJsonMsg::toJson() const
{
    QJsonObject obj;

    // 消息头
    obj["device_type"]         = this->device_type;
    obj["session_instance_id"] = this->session_instance_id;
    obj["packet_type"]         = this->packet_type;
    obj["message_seqnum"]      = this->message_seqnum;
    obj["request_id"]          = this->request_id;

    // payload
    QJsonObject payload;
    payload["session_id"] = this->session_id;

    QJsonArray trainArr;
    for (const TrainData &trainInfo : automatic_train_info) {
        QJsonObject trainObj;
        trainObj["vtrain_code"]     = trainInfo.vtrain_code;
        trainObj["track_code"]      = trainInfo.track_code;
        trainObj["offset_scale"]    = trainInfo.offset_scale;
        trainObj["direction"]       = trainInfo.direction;
        trainObj["train_speed_kmh"] = trainInfo.train_speed_kmh;
        trainArr.append(trainObj);
    }
    payload["automatic_train_info"] = trainArr;

    obj["payload"] = payload;

    QJsonDocument doc(obj);
    return doc.toJson(QJsonDocument::Indented);
}

/**转化为ATSData结构体
 * @brief ATSAutomaticTrainInfoJsonMsg::toATSData
 * @return
 */
ATSData ATSAutomaticTrainInfoJsonMsg::toATSData(QString vtrainCode)
{
    ATSData data;
    memset(&data, 0, sizeof(ATSData)); // 初始化所有字段

    // ----------------------------
    // 消息头
    // ----------------------------
    data.device_type = device_type;

    data.session_instance_id = session_instance_id.toInt();
    data.packet_type         = 7; // Automatic_Train_Info
    data.message_seqnum      = message_seqnum;
    data.request_id          = request_id.toInt();

    // ----------------------------
    // 虚拟列车数组
    // ----------------------------
    if (!automatic_train_info.isEmpty() && vtrainCode != "0") {
        int sign = 0;
        for(TrainData train : this->automatic_train_info){
            if(sign >= this->automatic_train_info.size()){
                break;
            }
            // data.vtrainData[sign].vtrain_session_id = session_id.toInt();
            // data.vtrainData[sign].vtrain_code      = train.vtrain_code.toInt();
            // data.vtrainData[sign].track_code= train.track_code.toInt();
            // data.vtrainData[sign].offset_scale = train.offset_scale;
            // data.vtrainData[sign].direction = (train.direction == "up") ? 0 : 1;
            // data.vtrainData[sign].vtrain_train_speed_kmh = train.train_speed_kmh;
            if(train.vtrain_code == vtrainCode){
                // data.vtrainData[sign].vtrain_session_id = session_id.toInt();
                data.vtrainData[0].vtrain_code      = train.vtrain_code.toInt();
                data.vtrainData[0].track_code= train.track_code.toInt();
                data.vtrainData[0].offset_scale = train.offset_scale;
                data.vtrainData[0].direction = (train.direction == "up") ? 0 : 1;
                data.vtrainData[0].vtrain_train_speed_kmh = train.train_speed_kmh;
            }
            sign++;
        }
    }

    return data;
}
