#ifndef LOGSHOWWORKER_H
#define LOGSHOWWORKER_H

#include <QObject>
#include <QRunnable>


class LogShowWorker : public QObject{
    Q_OBJECT
public:
    explicit LogShowWorker(QObject * parent = nullptr);
    ~LogShowWorker();

public slots:
    void doWorking(const QString &log_data);

};

#endif // LOGSHOWWORKER_H
