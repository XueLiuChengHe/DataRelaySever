#include "logmanager.h"

LogManager::LogManager(const QString &logPath, qint64 maxFileSize, QObject *parent)
    : QObject(parent), m_logPath(logPath), m_maxFileSize(maxFileSize), m_running(true)
{
    m_thread = QThread::create([this]{ this->processQueue(); });
    m_thread->start();
}

LogManager::~LogManager() {
    m_running = false;
    m_waitCondition.wakeOne();
    m_thread->quit();
    m_thread->wait();
    delete m_thread;
}

void LogManager::appendLog(const QString &fileNamePrefix, const QString &logLine) {
    QMutexLocker locker(&m_mutex);
    m_queue.enqueue(qMakePair(fileNamePrefix, logLine));
    m_waitCondition.wakeOne();
}

void LogManager::processQueue() {
    while (m_running) {
        QPair<QString, QString> logItem;
        {
            QMutexLocker locker(&m_mutex);
            if (m_queue.isEmpty()) {
                m_waitCondition.wait(&m_mutex);
                if (!m_running) break;
            }
            if (!m_queue.isEmpty())
                logItem = m_queue.dequeue();
        }
        if (!logItem.second.isEmpty()) {
            writeLogToFile(logItem.first, logItem.second);
        }
    }

    // 退出前写剩余日志
    while (!m_queue.isEmpty()) {
        QPair<QString, QString> logItem = m_queue.dequeue();
        writeLogToFile(logItem.first, logItem.second);
    }
}

void LogManager::writeLogToFile(const QString &prefix, const QString &line) {
    QString fileName = QDir(m_logPath).filePath(
                QString("%1_%2.log")
                .arg(prefix)
                .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd"))
                );

    QFile file(fileName);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) return;

    if (file.size() > m_maxFileSize) {
        QTextStream in(&file);
        QStringList lines;
        while (!in.atEnd()) lines.append(in.readLine());
        int removeCount = lines.size();
        lines.erase(lines.begin(), lines.begin() + removeCount);

        file.resize(0);
        QTextStream out(&file);
        out.setEncoding(QStringConverter::Utf8);
        for (const QString &l : lines) out << l << "\n";
    }

    file.seek(file.size());
    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << line << "\n";
    file.flush();
    file.close();
}
