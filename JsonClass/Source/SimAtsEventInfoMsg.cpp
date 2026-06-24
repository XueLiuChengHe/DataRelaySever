#include "SimAtsEventInfoMsg.h"

#include <QJsonDocument>
#include <QJsonObject>

SimAtsEventInfoMsg::SimAtsEventInfoMsg()
    : send_status(false)
    , device_type(0)
    , session_instance_id("0")
    , packet_type("SIM_ATS_Event_Info")
    , message_seqnum(0)
    , request_id("0")
    , event_type("0")
    , event_ctrl("0")
    , event_param1("0")
    , event_param2("0")
    , event_param3("0")
{

}

SimAtsEventInfoMsg::SimAtsEventInfoMsg(const SimAtsEventInfoMsg &other)
{
    this->send_status          = other.send_status;
    this->device_type          = other.device_type;
    this->session_instance_id  = other.session_instance_id;
    this->packet_type          = other.packet_type;
    this->message_seqnum       = other.message_seqnum;
    this->request_id           = other.request_id;

    this->event_type          = other.event_type;
    this->event_ctrl            = other.event_ctrl;
    this->event_param1     = other.event_param1;
    this->event_param2     = other.event_param2;
    this->event_param3     = other.event_param3;
}

SimAtsEventInfoMsg::~SimAtsEventInfoMsg()
{

}

QByteArray SimAtsEventInfoMsg::toJson() const
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
    payload["event_type"]            = this->event_type;
    payload["event_ctrl"]            = this->event_ctrl;
    payload["event_param1"]       = this->event_param1;
    payload["event_param2"] = this->event_param2;
    payload["event_param3"]= this->event_param3;

    obj["payload"] = payload;

    QJsonDocument doc(obj);
    return doc.toJson(QJsonDocument::Indented);
}

void SimAtsEventInfoMsg::fromATSSimFaultInfo(const ATSSIMFaultInfo atsSimFaultInfo)
{
    this->send_status          = atsSimFaultInfo.send_status;
    this->device_type          = atsSimFaultInfo.device_type;
    this->session_instance_id  = atsSimFaultInfo.session_instance_id;
    // this->packet_type          = atsSimFaultInfo.packet_type;
    this->message_seqnum       = atsSimFaultInfo.message_seqnum;
    this->request_id           = atsSimFaultInfo.request_id;

    this->event_type          = atsSimFaultInfo.fault_type;
    this->event_ctrl            = atsSimFaultInfo.fault_ctrl;
    this->event_param1     = atsSimFaultInfo.fault_param1;
    this->event_param2     = atsSimFaultInfo.fault_param2;
    this->event_param3     = atsSimFaultInfo.fault_param3;
}
