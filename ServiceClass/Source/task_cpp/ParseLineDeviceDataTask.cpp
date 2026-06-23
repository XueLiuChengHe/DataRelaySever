#include <task_h/ParseLineDeviceDataTask.h>
#include <QDebug>
#include <QThread>


ParseLineDeviceDataTask::ParseLineDeviceDataTask(const ATSLineDeviceDataJsonMsg &atsLineDeviceDataMsg,const QMap<QString, int> *csv_turnout_map,QObject *parent)
    : QObject(parent),
      atsLineDeviceDataMsg(atsLineDeviceDataMsg),
      csv_turnout_map(csv_turnout_map)
{
    setAutoDelete(true);
}

ParseLineDeviceDataTask::~ParseLineDeviceDataTask()
{

}

void ParseLineDeviceDataTask::run()
{
    ATSData ats_data = atsLineDeviceDataMsg.toATSData(csv_turnout_map);
    // qInfo()<<QThread::currentThread()<<"正在解析线路数据";
    emit this->lineDeviceDataParseFinish(ats_data);
}

void ParseLineDeviceDataTask::doWorking(const ATSLineDeviceDataJsonMsg &atsLineDeviceDataMsg)
{
    ATSData ats_data = const_cast<ATSLineDeviceDataJsonMsg&>(atsLineDeviceDataMsg).toATSData(csv_turnout_map);
    emit this->lineDeviceDataParseFinish(ats_data);
}
