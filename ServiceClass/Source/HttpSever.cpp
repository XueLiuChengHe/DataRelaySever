#include "HttpSever.h"

#include <QDir>
#include <QThread>

HttpSever::HttpSever(unsigned short Port,QObject *parent)
    : QTcpServer(parent)
    , connectionState(nullptr)
    , revSocket(nullptr)
    , Port(Port)
{
    if (this->isListening()) {
        this->close();
        qInfo()<<QStringLiteral("服务器已停止。");
        return;
    }

    if (!this->listen(QHostAddress::AnyIPv4, this->Port)) {
        qInfo()<<QStringLiteral("监听失败: %1").arg(this->errorString());
        return;
    }

    qInfo()<<QStringLiteral("服务器已启动，监听地址: http://127.0.0.1:%1/upload").arg(this->Port)<<QThread::currentThread();

    QObject::connect(this,&HttpSever::newConnectionComing,this,[this](qintptr descriptor){
        if(this->revSocket != nullptr){
            QObject::disconnect(this->revSocket, nullptr, this, nullptr);  // 先断开所有信号，防止 disconnected 打到新 socket
            this->revSocket->disconnectFromHost();
            this->revSocket->deleteLater();
            this->revSocket = nullptr;
        }
        this->revSocket = new QTcpSocket(this);
        this->revSocket->setSocketDescriptor(descriptor);

        if(this->connectionState != nullptr){
            delete this->connectionState;
            this->connectionState = nullptr;
        }
        this->connectionState = new ConnectionState();

        connect(revSocket, &QTcpSocket::readyRead, this, &HttpSever::handleSocketReadyRead);
        connect(revSocket, &QTcpSocket::disconnected, this, &HttpSever::handleSocketDisconnected);
    });
}

HttpSever::~HttpSever()
{
    if(this->revSocket){  // 先判空
        if(this->revSocket->isOpen() && this->revSocket->isValid()){
            this->revSocket->disconnectFromHost();
            this->revSocket->close();
        }
        this->revSocket->deleteLater();
        this->revSocket = nullptr;
    }

    delete this->connectionState;
    this->connectionState = nullptr;
}

/**
 * 当有新连接接入的时候
 * @brief HttpSever::incomingConnection
 * @param handle
 */
void HttpSever::incomingConnection(qintptr handle)
{
    emit this->newConnectionComing(handle);
}

/**
 * 处理请求
 * @brief HttpSever::processRequest
 */
void HttpSever::processRequest()
{
    // connectionState 理论上不会为 null，但保险起见加一下
    if(!this->connectionState || !this->revSocket) return;
    const QByteArray payload = this->connectionState->buffer.left(this->connectionState->contentLength);

    if (this->connectionState->method.compare(QStringLiteral("POST"), Qt::CaseInsensitive) != 0) {
        qInfo()<<QStringLiteral("拒绝请求: 仅支持 POST。");

        sendHttpResponse(405, "Method Not Allowed", "Only POST is supported");

        this->revSocket->disconnectFromHost();
        return;
    }

    QString errorMessage;

    qInfo() << QStringLiteral("接收成功: %1 bytes").arg(payload.size());
    // 直接解析成字符串
    QString csvString = QString::fromUtf8(payload);
    emit this->simLogRev(csvString);

    const QByteArray responseBody = QStringLiteral("接收成功: %1 bytes").arg(payload.size()).toUtf8();
    sendHttpResponse(200, "OK", responseBody);
    this->revSocket->disconnectFromHost();
}

/**
 * 发送Http响应消息
 * @brief HttpSever::sendHttpResponse
 * @param statusCode
 * @param statusText
 * @param body
 */
void HttpSever::sendHttpResponse(int statusCode, const QByteArray &statusText, const QByteArray &body) const
{
    if(!this->revSocket || !this->revSocket->isValid()){
        qWarning() << "sendHttpResponse: socket 无效";
        return;
    }

    QByteArray response;
    response += "HTTP/1.1 " + QByteArray::number(statusCode) + ' ' + statusText + "\r\n";
    response += "Content-Type: text/plain; charset=utf-8\r\n";
    response += "Content-Length: " + QByteArray::number(body.size()) + "\r\n";
    response += "Connection: close\r\n\r\n";
    response += body;

    this->revSocket->write(response);
    this->revSocket->flush();
}

// ===================================================================================================
//                                                                                                      槽                     函                   数
// ===================================================================================================
/**
 * 当有消息需要读取
 * @brief HttpSever::handleSocketReadyRead
 */
void HttpSever::handleSocketReadyRead()
{
    this->connectionState->buffer.append(this->revSocket->readAll());

    if (!this->connectionState->headerParsed) {
        const int headerEndIndex = this->connectionState->buffer.indexOf("\r\n\r\n");
        if (headerEndIndex < 0) {
            return;
        }

        const QByteArray headerBytes = this->connectionState->buffer.left(headerEndIndex);
        const QList<QByteArray> lines = headerBytes.split('\n');
        if (lines.isEmpty()) {
            sendHttpResponse(400, "Bad Request", "Missing request line");
            this->revSocket->disconnectFromHost();
            return;
        }

        const QList<QByteArray> requestLineParts = lines.first().trimmed().split(' ');
        if (requestLineParts.size() < 2) {
            sendHttpResponse(400, "Bad Request", "Invalid request line");
            this->revSocket->disconnectFromHost();
            return;
        }

        this->connectionState->method = QString::fromLatin1(requestLineParts.at(0));
        this->connectionState->path = QString::fromLatin1(requestLineParts.at(1));

        for (int index = 1; index < lines.size(); ++index) {
            const QByteArray line = lines.at(index).trimmed();
            const int colonIndex = line.indexOf(':');
            if (colonIndex <= 0) {
                continue;
            }

            const QByteArray name = line.left(colonIndex).trimmed().toLower();
            const QByteArray value = line.mid(colonIndex + 1).trimmed();
            this->connectionState->headers.insert(QString::fromLatin1(name), value);
        }

        this->connectionState->contentLength = this->connectionState->headers.value(QStringLiteral("content-length")).toLongLong();
        this->connectionState->buffer.remove(0, headerEndIndex + 4);
        this->connectionState->headerParsed = true;

        if (this->connectionState->contentLength < 0) {
            sendHttpResponse(411, "Length Required", "Content-Length is required");
            this->revSocket->disconnectFromHost();
            return;
        }
    }

    if (this->connectionState->headerParsed && this->connectionState->buffer.size() >= this->connectionState->contentLength) {
        processRequest();
    }
}

/**
 * 当对方断开连接的时候
 * @brief HttpSever::handleSocketDisconnected
 */
void HttpSever::handleSocketDisconnected()
{
    if(this->revSocket != nullptr){
        this->revSocket->deleteLater();
        this->revSocket = nullptr;
    }
}
