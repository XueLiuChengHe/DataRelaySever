#include <task_h/LogShowWorker.h>
#include <QDebug>
#include <QThread>


LogShowWorker::LogShowWorker(QObject *parent):QObject(parent)
{
}

LogShowWorker::~LogShowWorker()
{

}

void LogShowWorker::doWorking(const QString &log_data)
{
    qInfo() <<QThread::currentThread() <<log_data;
}
