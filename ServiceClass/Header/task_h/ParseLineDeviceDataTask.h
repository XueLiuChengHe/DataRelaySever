#ifndef PARSELINEDEVICEDATAWORKER_H
#define PARSELINEDEVICEDATAWORKER_H

#include <QObject>
#include <QRunnable>

#include <ATSLineDeviceDataJsonMsg.h>

// ***********************************************************************************************************************************************************************
//  》信号
//      》线路设备信息解析完毕信号：void lineDeviceDataParseFinish(const ATSLineDeviceDataJsonMsg ats_line_device_data_json_msg,ATSData ats_data);
//  》槽函数
//      》解析线路设备信息的槽函数：void doWorking(const ATSLineDeviceDataJsonMsg &ats_line_device_data_json_msg);
//  》成员
//      》保存道岔映射数据的成员变量：const QMap<QString,int> * csv_turnout_map;
// ***********************************************************************************************************************************************************************
class ParseLineDeviceDataTask : public QObject,public QRunnable{
    Q_OBJECT
public:
    explicit ParseLineDeviceDataTask(const ATSLineDeviceDataJsonMsg &atsLineDeviceDataMsg,const QMap<QString,int> * csv_turnout_map,QObject * parent = nullptr);
    ~ParseLineDeviceDataTask();

public:
    void run();

signals:
    void lineDeviceDataParseFinish(ATSData ats_data);

public slots:
    void doWorking(const ATSLineDeviceDataJsonMsg &atsLineDeviceDataMsg);


private:
    ATSLineDeviceDataJsonMsg atsLineDeviceDataMsg;
    const QMap<QString,int> * csv_turnout_map;

};

#endif // PARSELINEDEVICEDATAWORKER_H
