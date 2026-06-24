#ifndef HTTPSEVER_H
#define HTTPSEVER_H

#include <QByteArray>
#include <QHash>
#include <QString>
#include <QTcpServer>
#include <QTcpSocket>

struct ConnectionState {
    bool headerParsed = false;
    qint64 contentLength = -1;
    QByteArray buffer;
    QString method;
    QString path;
    QHash<QString, QByteArray> headers;
};

class HttpSever : public QTcpServer
{
    Q_OBJECT

public:
    HttpSever(unsigned short Port,QObject *parent = nullptr);
    ~HttpSever();

    // QTcpServer interface
protected:
    void incomingConnection(qintptr handle) override;

private:
    void processRequest();
    void sendHttpResponse(int statusCode, const QByteArray &statusText, const QByteArray &body) const;

signals:
    void newConnectionComing(qintptr descriptor);
    void simLogRev(const QString simLogMsg);

private slots:
    void handleSocketReadyRead();
    void handleSocketDisconnected();

private:
    ConnectionState * connectionState;
    QTcpSocket * revSocket;

    unsigned short Port;
};

#endif // HTTPSEVER_H