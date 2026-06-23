#ifndef LOGMANAGER_H
#define LOGMANAGER_H

#pragma once
#include <QObject>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
#include <QThread>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QDir>
#include <QPair>

class LogManager : public QObject {
    Q_OBJECT
public:
    explicit LogManager(const QString &logPath, qint64 maxFileSize = 1LL*1024*1024*1024, QObject *parent = nullptr);
    ~LogManager();

    void appendLog(const QString &fileNamePrefix, const QString &logLine);

private:
    void processQueue();
    void writeLogToFile(const QString &prefix, const QString &line);

private:
    QThread *m_thread;
    QQueue<QPair<QString, QString>> m_queue;
    QMutex m_mutex;
    QWaitCondition m_waitCondition;
    QString m_logPath;
    qint64 m_maxFileSize;
    bool m_running;
};

#endif // LOGMANAGER_H
