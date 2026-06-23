#ifndef DATAPARSETASK_H
#define DATAPARSETASK_H

#include <QMutex>
#include <QObject>
#include <QQueue>
#include <QTimer>
#include <QRunnable>

#include <ReplyJsonMsg.h>
#include <SimulatorJsonMsg.h>

// ***********************************************************************************************************************************************************************
// 数据解析的线程池任务
//  》信号
//      》发送数据解析完成的信号 ：void parseDataFinish(int lineNo,const SimulatorJsonMsg &simulator_json_msg);
//  》成员
//      》收到渔仁的数据 ：data
// ***********************************************************************************************************************************************************************
class DataParseTask : public QObject, public QRunnable{
    Q_OBJECT
public:
    explicit DataParseTask(const QByteArray &datagram,QObject * parent = nullptr);
    ~DataParseTask();

public:
    void run();  // 任务函数

signals:
    void parseDataFinish(int lineNo,const SimulatorJsonMsg &simulatorMsg);                                 // 数据解析完成信号

private:
    QByteArray data;
};

#endif // DATAPARSETASK_H
