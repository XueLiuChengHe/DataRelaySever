#ifndef ATSJSONMSG_H
#define ATSJSONMSG_H

#include "ATSATPATOinfoJsonMsg.h"
#include "ATSAutomaticTrainInfoJsonMsg.h"
#include "ATSConstrolMsg.h"
#include "ATSLineDeviceDataJsonMsg.h"
#include "ATSMMIInfoJsonMsg.h"
#include "ATSSessionDefinitionJsonMsg.h"
#include "ATSStateControlJsonMsg.h"
#include "ATSVirtualTimeJsonMsg.h"

#include <QString>
#include <StructClsss.h>


class ATSJsonMsg{
public:
    ATSJsonMsg();
    ATSJsonMsg(const ATSData &ats_data);
    ~ATSJsonMsg();

public:
    void setSessionDefinitionData(const ATSData &ats_data);
    void setStateControlData(const ATSData &ats_data);
    void setVirtualTimeData(const ATSData &ats_data);
    void setAtpAtoInfoData(const ATSData &ats_data);
    void setMmiInfoData(const ATSData &ats_data);
    void setAutomaticInfoData(const ATSData &ats_data);
    void setlineDeviceData(const ATSData &ats_data);
    void setATSSIMFaultInfoData(const ATSData &ats_data);

    void setChangeTurnoutData(const ATSData &ats_data); // 设置此次与上次的道岔变化数据
    void setChangeSignalData(const ATSData &ats_data); // 设置此次与上次的信号机变化数据

public:
    int sim_train_id;

    ATSSessionDefinitionJsonMsg ats_session_definition_json_msg;
    ATSStateControlJsonMsg ats_state_control_json_msg;
    ATSVirtualTimeJsonMsg ats_vrtual_time_json_msg;

    ATSATPATOInfoJsonMsg ats_atp_ato_info_json_msg; // 用于前端界面包的显示
    ATSMMIInfoJsonMsg ats_mmi_info_json_msg;
    ATSControlMsg atsControlMsg; // 用于向国旗传输ATP控制消息

    ATSAutomaticTrainInfoJsonMsg ats_automatic_train_info_json_msg;
    ATSLineDeviceDataJsonMsg ats_line_device_data_json_msg;
    ATSSIMFaultInfo ats_sim_fault_info_json_msg;

    ATSData ats_data; // 用于向主仿真发送相关ats的消息
};

#endif // ATSJSONMSG_H
