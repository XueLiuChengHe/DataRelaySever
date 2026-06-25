#include "ATSSIMFaultInfo.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>


ATSSIMFaultInfo::ATSSIMFaultInfo()
    : send_status(false)
    , device_type(0)
    , session_instance_id("")
    , packet_type("")
    , message_seqnum(0)
    , request_id("")
    , fault_type("")
    , fault_ctrl("")
    , fault_param1("")
    , fault_param2("")
    , fault_param3("")
{

}

ATSSIMFaultInfo::ATSSIMFaultInfo(const ATSSIMFaultInfo &other)
{
    this->send_status          = other.send_status;
    this->device_type          = other.device_type;
    this->session_instance_id  = other.session_instance_id;
    this->packet_type          = other.packet_type;
    this->message_seqnum       = other.message_seqnum;
    this->request_id           = other.request_id;

    this->fault_type          = other.fault_type;
    this->fault_ctrl            = other.fault_ctrl;
    this->fault_param1     = other.fault_param1;
    this->fault_param2     = other.fault_param2;
    this->fault_param3     = other.fault_param3;
}

ATSSIMFaultInfo::~ATSSIMFaultInfo()
{

}

bool ATSSIMFaultInfo::fromJson(const QString &jsonStr)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8(), &error);
    if (error.error != QJsonParseError::NoError || !doc.isObject()) {
        return false;
    }

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

        this->fault_type          = payload.value("fault_type").toString();
        this->fault_ctrl            = payload.value("fault_ctrl").toString();
        this->fault_param1     = payload.value("fault_param1").toString();
        this->fault_param2     = payload.value("fault_param2").toString();
        this->fault_param3     = payload.value("fault_param3").toString();
    }

    return true;
}

QByteArray ATSSIMFaultInfo::toJson() const
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
    payload["fault_type"]            = this->fault_type;
    payload["fault_ctrl"]            = this->fault_ctrl;
    payload["fault_param1"]       = this->fault_param1;
    payload["fault_param2"] = this->fault_param2;
    payload["fault_param3"]= this->fault_param3;

    obj["payload"] = payload;

    QJsonDocument doc(obj);
    return doc.toJson(QJsonDocument::Indented);
}

ATSData ATSSIMFaultInfo::toATSData()
{
    ATSData data;
    memset(&data, 0, sizeof(ATSData)); // 初始化所有字段

    // ----------------------------
    // 消息头
    // ----------------------------
    data.device_type = device_type;

    data.session_instance_id = session_instance_id.toInt();
    data.packet_type         = 6; // ATP/ATO & MMI 信息对应的类型，可按实际修改
    data.message_seqnum      = message_seqnum;
    data.request_id          = request_id.toInt();

    // ----------------------------
    // ATSSIMFault相关信息映射
    // ----------------------------
    if(this->fault_type == "ATOFault"){
        if(this->fault_ctrl == "enable"){
            data.ats_sim_fault[0] = 1;
        }else{
            data.ats_sim_fault[0] = 0;
        }
    }else if(this->fault_type == "ATPFault"){
        if(this->fault_ctrl == "enable"){
            data.ats_sim_fault[1] = 1;
        }else{
            data.ats_sim_fault[1] = 0;
        }
    }else if(this->fault_type == "TrainLACP"){
        if(this->fault_ctrl == "enable"){
            data.ats_sim_fault[2] = 1;
            data.fault_param1 = this->fault_param1.toInt();
            data.fault_param2 = this->fault_param2.toInt();
        }else{
            data.ats_sim_fault[2] = 0;
            data.fault_param1 = 0;
            data.fault_param2 = 0;
        }
    }else if(this->fault_type == "TrackForeign"){
        if(this->fault_ctrl == "disable"){
            data.ats_sim_fault[3] = 1;
            data.fault_param1 = this->fault_param1.toInt();
        }else{
            data.ats_sim_fault[3] = 0;
            data.fault_param1 = 0;
        }
    }else if(this->fault_type == "FireFault"){
        if(this->fault_ctrl == "enable"){
            data.ats_sim_fault[4] = 1;
        }else{
            data.ats_sim_fault[4] = 0;
            data.fault_param1 = 0;
        }
    }else if(this->fault_type == "trainHitPerson"){
        if(this->fault_ctrl == "disable"){
            data.ats_sim_fault[5] = 1;
        }else{
            data.ats_sim_fault[5] = 0;
        }
    }

    // 成都
    // else if(this->fault_type == "PAU"){
    //     if(this->fault_ctrl == "enable"){
    //         data.ats_sim_fault[6] = 1;
    //     }else{
    //         data.ats_sim_fault[6] = 0;
    //     }
    // }

    // data.fault_param1 = this->fault_param1.toInt();
    // data.fault_param2 = this->fault_param2.toInt();
    // data.fault_param3 = this->fault_param3.toInt();

    return data;
}

void ATSSIMFaultInfo::fromATSSIMFaultInfo(const ATSSIMFaultInfo &other)
{
    this->send_status          = other.send_status;
    this->device_type          = other.device_type;
    this->session_instance_id  = other.session_instance_id;
    this->packet_type          = other.packet_type;
    this->message_seqnum       = other.message_seqnum;
    this->request_id           = other.request_id;

    this->fault_type          = other.fault_type;
    this->fault_ctrl            = other.fault_ctrl;
    this->fault_param1     = other.fault_param1;
    this->fault_param2     = other.fault_param2;
    this->fault_param3     = other.fault_param3;
}