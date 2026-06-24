#include "WebSocket.h"

#include <QPointer>
#include <QElapsedTimer>
#include <QHostAddress>
#include <QRandomGenerator>

// ─────────────────────────────────────────────────────────────────────────────
// RFC 6455 常量
// ─────────────────────────────────────────────────────────────────────────────
static const QByteArray WS_MAGIC_KEY = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

static constexpr quint8 FIN_BIT    = 0x80;
static constexpr quint8 MASK_BIT   = 0x80;

// Opcode
static constexpr quint8 OP_CONTINUATION = 0x0;
static constexpr quint8 OP_TEXT         = 0x1;
static constexpr quint8 OP_BINARY       = 0x2;
static constexpr quint8 OP_CLOSE        = 0x8;
static constexpr quint8 OP_PING         = 0x9;
static constexpr quint8 OP_PONG         = 0xA;

// ─────────────────────────────────────────────────────────────────────────────
// 构造 / 析构
// ─────────────────────────────────────────────────────────────────────────────
WebSocket::WebSocket(qintptr descriptor, QObject *parent)
    : QObject(parent)
    , m_socketDescriptor(descriptor)
{
}

WebSocket::~WebSocket()
{
    // socket 已在 onDisconnected 中 deleteLater，此处防御性处理
    if (m_socket) {
        m_socket->disconnect(this);
        m_socket->abort();
        m_socket->deleteLater();
        m_socket = nullptr;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// 生命周期：服务端模式
// ─────────────────────────────────────────────────────────────────────────────
void WebSocket::start()
{
    m_socket = new QTcpSocket(this);

    if (!m_socket->setSocketDescriptor(m_socketDescriptor)) {
        setError(UnknownError, QStringLiteral("Failed to set socket descriptor: %1")
                     .arg(m_socket->errorString()));
        m_socket->deleteLater();
        m_socket = nullptr;
        return;
    }

    connect(m_socket, &QTcpSocket::readyRead,    this, &WebSocket::onReadyReadForHandshake);
    connect(m_socket, &QTcpSocket::disconnected, this, &WebSocket::onDisconnected);
    connect(m_socket, &QTcpSocket::bytesWritten, this, &WebSocket::onBytesWritten);
    connect(m_socket, &QTcpSocket::stateChanged, this, &WebSocket::stateChanged);
}

// ─────────────────────────────────────────────────────────────────────────────
// 生命周期：客户端模式 - 工厂方法
// ─────────────────────────────────────────────────────────────────────────────
WebSocket *WebSocket::connectToUrl(const QUrl &url, QObject *parent)
{
    WebSocket *ws = new WebSocket(-1, parent);
    // ws->connectToServer(url);
    return ws;
}

// ─────────────────────────────────────────────────────────────────────────────
// 生命周期：客户端模式 - 在目标线程中调用
// ─────────────────────────────────────────────────────────────────────────────
void WebSocket::connectToServer(const QUrl &url)
{
    m_isClient  = true;
    m_serverUrl = url;

    m_socket = new QTcpSocket(this);

    connect(m_socket, &QTcpSocket::connected,    this, &WebSocket::onConnectedForHandshake);
    connect(m_socket, &QTcpSocket::disconnected, this, &WebSocket::onDisconnected);
    connect(m_socket, &QTcpSocket::bytesWritten, this, &WebSocket::onBytesWritten);
    connect(m_socket, &QTcpSocket::stateChanged, this, &WebSocket::stateChanged);
    connect(m_socket, &QAbstractSocket::errorOccurred, this,
            [this](QAbstractSocket::SocketError) {
                setError(UnknownError, m_socket->errorString());
            });

    quint16 port = static_cast<quint16>(url.port(80));
    m_socket->connectToHost(url.host(), port);
}

// ─────────────────────────────────────────────────────────────────────────────
// 客户端握手第一步：TCP 连接成功，发送 HTTP Upgrade 请求
// ─────────────────────────────────────────────────────────────────────────────
void WebSocket::onConnectedForHandshake()
{
    // 生成随机 16 字节，Base64 编码作为 Sec-WebSocket-Key
    QByteArray rawKey(16, 0);
    for (int i = 0; i < 16; ++i)
        rawKey[i] = static_cast<char>(QRandomGenerator::global()->bounded(256));
    m_clientKey = rawKey.toBase64();

    QString path = m_serverUrl.path();
    if (path.isEmpty()) path = QStringLiteral("/");
    if (!m_serverUrl.query().isEmpty())
        path += QLatin1Char('?') + m_serverUrl.query();

    QByteArray request;
    request += "GET " + path.toUtf8() + " HTTP/1.1\r\n";
    request += "Host: " + m_serverUrl.host().toUtf8();
    if (m_serverUrl.port() != -1)
        request += ":" + QByteArray::number(m_serverUrl.port());
    request += "\r\n";
    request += "Upgrade: websocket\r\n";
    request += "Connection: Upgrade\r\n";
    request += "Sec-WebSocket-Key: " + m_clientKey + "\r\n";
    request += "Sec-WebSocket-Version: 13\r\n";
    request += "\r\n";

    m_socket->write(request);
    m_socket->flush();

    // 等待服务端返回 101
    connect(m_socket, &QTcpSocket::readyRead,
            this, &WebSocket::onReadyReadForClientHandshake);
}

// ─────────────────────────────────────────────────────────────────────────────
// 客户端握手第二步：读取并验证服务端的 101 响应
// ─────────────────────────────────────────────────────────────────────────────
void WebSocket::onReadyReadForClientHandshake()
{
    m_handshakeBuffer += m_socket->readAll();

    if (!m_handshakeBuffer.contains("\r\n\r\n")) return; // 头还没收完

    // 必须是 101 Switching Protocols
    if (!m_handshakeBuffer.startsWith("HTTP/1.1 101")) {
        setError(HandshakeFailedError,
                 QStringLiteral("Server did not return 101, got: %1")
                     .arg(QString::fromUtf8(m_handshakeBuffer.left(64))));
        m_socket->disconnectFromHost();
        return;
    }

    // 计算期望的 Sec-WebSocket-Accept
    QByteArray expectedAccept = QCryptographicHash::hash(
                                    m_clientKey + WS_MAGIC_KEY,
                                    QCryptographicHash::Sha1
                                    ).toBase64();

    // 从响应头中找到并验证 Sec-WebSocket-Accept
    bool acceptOk = false;
    QByteArray headerPart = m_handshakeBuffer.left(
        m_handshakeBuffer.indexOf("\r\n\r\n") + 4);
    QTextStream stream(headerPart);
    QString line;
    while (!(line = stream.readLine()).isNull()) {
        if (line.isEmpty()) break;
        int colon = line.indexOf(':');
        if (colon < 0) continue;
        if (line.left(colon).trimmed().toLower() == QLatin1String("sec-websocket-accept")) {
            acceptOk = (line.mid(colon + 1).trimmed().toUtf8() == expectedAccept);
            break;
        }
    }

    if (!acceptOk) {
        setError(HandshakeFailedError,
                 QStringLiteral("Sec-WebSocket-Accept validation failed"));
        m_socket->disconnectFromHost();
        return;
    }

    disconnect(m_socket, &QTcpSocket::readyRead,
               this, &WebSocket::onReadyReadForClientHandshake);

    // 将头部之后多读的数据转给 wsBuffer
    int headerEnd = m_handshakeBuffer.indexOf("\r\n\r\n") + 4;
    QByteArray leftover = m_handshakeBuffer.mid(headerEnd);
    m_handshakeBuffer.clear();

    finalizeHandshake(leftover);
}

// ─────────────────────────────────────────────────────────────────────────────
// 握手完成后的公共收尾（服务端 / 客户端共用）
// ─────────────────────────────────────────────────────────────────────────────
void WebSocket::finalizeHandshake(const QByteArray &leftover)
{
    m_wsBuffer      = leftover;
    m_handshakeDone = true;

    connect(m_socket, &QTcpSocket::readyRead, this, &WebSocket::onReadyRead);

    emit connected();
    emit handshakeCompleted();

    if (!m_wsBuffer.isEmpty())
        onReadyRead();
}

// ─────────────────────────────────────────────────────────────────────────────
// QWebSocket 兼容接口
// ─────────────────────────────────────────────────────────────────────────────
qint64 WebSocket::sendTextMessage(const QString &message)
{
    if (!isValid()) return -1;
    // 客户端模式必须加掩码（RFC 6455 §5.1）
    return writeFrame(encodeFrame(message.toUtf8(), OP_TEXT, m_isClient));
}

qint64 WebSocket::sendBinaryMessage(const QByteArray &data)
{
    if (!isValid()) return -1;
    return writeFrame(encodeFrame(data, OP_BINARY, m_isClient));
}

void WebSocket::close(CloseCode closeCode, const QString &reason)
{
    if (!m_socket) return;

    if (!m_handshakeDone) {
        m_socket->abort();
        return;
    }

    if (m_closing) return;
    m_closing = true;

    // Close 帧：状态码(2字节) + reason(UTF-8，最多 123 字节)
    QByteArray payload;
    payload.append(static_cast<char>((closeCode >> 8) & 0xFF));
    payload.append(static_cast<char>(closeCode & 0xFF));
    if (!reason.isEmpty())
        payload.append(reason.toUtf8().left(123));

    writeFrame(encodeFrame(payload, OP_CLOSE, m_isClient));
    m_socket->flush();

    QPointer<WebSocket> guard(this);
    QTimer::singleShot(5000, this, [guard]() {
        if (guard && guard->m_socket && guard->m_closing)
            guard->m_socket->disconnectFromHost();
    });
}

void WebSocket::abort()
{
    if (m_socket)
        m_socket->abort();
}

void WebSocket::ping(const QByteArray &payload)
{
    if (!isValid()) return;
    QByteArray safePayload = payload.left(125);
    m_pingTimer2.restart();
    writeFrame(encodeFrame(safePayload, OP_PING, m_isClient));
}

bool WebSocket::isValid() const
{
    return m_socket
           && m_socket->state() == QAbstractSocket::ConnectedState
           && m_handshakeDone
           && !m_closing;
}

QString                      WebSocket::errorString() const { return m_errorString; }
WebSocket::WebSocketError    WebSocket::error()       const { return m_error;       }

QAbstractSocket::SocketState WebSocket::state() const
{
    return m_socket ? m_socket->state() : QAbstractSocket::UnconnectedState;
}

QHostAddress WebSocket::localAddress() const
{
    return m_socket ? m_socket->localAddress() : QHostAddress();
}

quint16 WebSocket::localPort() const
{
    return m_socket ? m_socket->localPort() : 0;
}

QHostAddress WebSocket::peerAddress() const
{
    return m_socket ? m_socket->peerAddress() : QHostAddress();
}

quint16 WebSocket::peerPort() const
{
    return m_socket ? m_socket->peerPort() : 0;
}

QString WebSocket::peerName() const
{
    return m_socket ? m_socket->peerName() : QString();
}

qint64 WebSocket::readBufferSize() const
{
    return m_socket ? m_socket->readBufferSize() : 0;
}

void WebSocket::setReadBufferSize(qint64 size)
{
    if (m_socket) m_socket->setReadBufferSize(size);
}

void    WebSocket::setMaxAllowedIncomingFrameSize(quint64 size)   { m_maxFrameSize   = size; }
quint64 WebSocket::maxAllowedIncomingFrameSize()  const           { return m_maxFrameSize;   }
void    WebSocket::setMaxAllowedIncomingMessageSize(quint64 size) { m_maxMessageSize = size; }
quint64 WebSocket::maxAllowedIncomingMessageSize() const          { return m_maxMessageSize; }

// ─────────────────────────────────────────────────────────────────────────────
// 扩展接口
// ─────────────────────────────────────────────────────────────────────────────
qintptr WebSocket::socketDescriptor() const
{
    return m_socket ? m_socket->socketDescriptor() : -1;
}

QTcpSocket *WebSocket::socket() const
{
    return m_socket;
}

// ─────────────────────────────────────────────────────────────────────────────
// 服务端握手
// ─────────────────────────────────────────────────────────────────────────────
void WebSocket::onReadyReadForHandshake()
{
    m_handshakeBuffer += m_socket->readAll();

    if (!m_handshakeBuffer.contains("\r\n\r\n")) return;

    QByteArray headerPart = m_handshakeBuffer.left(
        m_handshakeBuffer.indexOf("\r\n\r\n") + 4);

    QString wsKey;
    QString requestedProtocol;
    bool upgradeFound    = false;
    bool connectionFound = false;
    bool versionOk       = false;

    QTextStream stream(headerPart);
    QString line;
    bool firstLine = true;
    while (!(line = stream.readLine()).isNull()) {
        if (firstLine) { firstLine = false; continue; }
        if (line.isEmpty()) break;

        int colon = line.indexOf(':');
        if (colon < 0) continue;

        QString name  = line.left(colon).trimmed().toLower();
        QString value = line.mid(colon + 1).trimmed();

        if (name == QLatin1String("sec-websocket-key"))
            wsKey = value;
        else if (name == QLatin1String("upgrade") && value.toLower() == QLatin1String("websocket"))
            upgradeFound = true;
        else if (name == QLatin1String("connection") && value.toLower().contains(QLatin1String("upgrade")))
            connectionFound = true;
        else if (name == QLatin1String("sec-websocket-version") && value == QLatin1String("13"))
            versionOk = true;
        else if (name == QLatin1String("sec-websocket-protocol"))
            requestedProtocol = value.split(',').first().trimmed();
    }

    if (wsKey.isEmpty() || !upgradeFound || !connectionFound || !versionOk) {
        qWarning() << "[WebSocket] Handshake failed: invalid HTTP upgrade request";
        m_socket->write("HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\n\r\n");
        m_socket->flush();
        setError(HandshakeFailedError, QStringLiteral("Invalid WebSocket upgrade request"));
        m_socket->disconnectFromHost();
        return;
    }

    QByteArray acceptKey = QCryptographicHash::hash(
                               wsKey.toUtf8() + WS_MAGIC_KEY,
                               QCryptographicHash::Sha1
                               ).toBase64();

    QByteArray response;
    response += "HTTP/1.1 101 Switching Protocols\r\n";
    response += "Upgrade: websocket\r\n";
    response += "Connection: Upgrade\r\n";
    response += "Sec-WebSocket-Accept: " + acceptKey + "\r\n";
    if (!requestedProtocol.isEmpty())
        response += "Sec-WebSocket-Protocol: " + requestedProtocol.toUtf8() + "\r\n";
    response += "\r\n";

    m_socket->write(response);
    m_socket->flush();

    disconnect(m_socket, &QTcpSocket::readyRead, this, &WebSocket::onReadyReadForHandshake);

    int headerEnd = m_handshakeBuffer.indexOf("\r\n\r\n") + 4;
    QByteArray leftover = m_handshakeBuffer.mid(headerEnd);
    m_handshakeBuffer.clear();

    finalizeHandshake(leftover);
}

// ─────────────────────────────────────────────────────────────────────────────
// 帧读取
// ─────────────────────────────────────────────────────────────────────────────
void WebSocket::onReadyRead()
{
    m_wsBuffer += m_socket->readAll();

    while (true) {
        QByteArray rawFrame;
        int consumed = parseNextFrame(m_wsBuffer, rawFrame);

        if (consumed == 0) break;
        if (consumed < 0) {
            close(CloseCodeProtocolError, QStringLiteral("Frame parse error"));
            break;
        }

        m_wsBuffer.remove(0, consumed);
        processFrame(rawFrame);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// 帧解析：返回消耗的字节数，0=不完整，-1=错误
// ─────────────────────────────────────────────────────────────────────────────
int WebSocket::parseNextFrame(const QByteArray &buf, QByteArray &outFrame)
{
    if (buf.size() < 2) return 0;

    quint8 b1 = static_cast<quint8>(buf[0]);
    quint8 b2 = static_cast<quint8>(buf[1]);

    if (b1 & 0x70) {
        qWarning() << "[WebSocket] RSV bits set without negotiated extension";
        return -1;
    }

    bool    masked     = b2 & MASK_BIT;
    quint64 payloadLen = b2 & 0x7F;
    int     index      = 2;

    if (payloadLen == 126) {
        if (buf.size() < 4) return 0;
        payloadLen = (static_cast<quint8>(buf[2]) << 8)
                     |  static_cast<quint8>(buf[3]);
        index += 2;
    } else if (payloadLen == 127) {
        if (buf.size() < 10) return 0;
        payloadLen = 0;
        for (int i = 0; i < 8; ++i)
            payloadLen = (payloadLen << 8) | static_cast<quint8>(buf[2 + i]);
        index += 8;
    }

    if (payloadLen > m_maxFrameSize) {
        qWarning() << "[WebSocket] Frame too large:" << payloadLen;
        return -1;
    }

    int maskLen   = masked ? 4 : 0;
    int totalSize = index + maskLen + static_cast<int>(payloadLen);
    if (buf.size() < totalSize) return 0;

    outFrame = buf.left(totalSize);
    return totalSize;
}

// ─────────────────────────────────────────────────────────────────────────────
// 帧处理（含分片重组）
// ─────────────────────────────────────────────────────────────────────────────
void WebSocket::processFrame(const QByteArray &frame)
{
    if (frame.size() < 2) return;

    quint8 b1     = static_cast<quint8>(frame[0]);
    bool   fin    = (b1 & FIN_BIT) != 0;
    quint8 opcode = b1 & 0x0F;

    QByteArray payload = decodeFrame(frame);

    // ── 控制帧 ────────────────────────────────────────────────────────────────
    if (opcode & 0x08) {
        switch (opcode) {
        case OP_CLOSE: {
            CloseCode code   = CloseCodeNormal;
            QString   reason;
            if (payload.size() >= 2) {
                code   = static_cast<CloseCode>(
                    (static_cast<quint8>(payload[0]) << 8) | static_cast<quint8>(payload[1]));
                reason = QString::fromUtf8(payload.mid(2));
            }
            if (!m_closing)
                close(code, reason);
            else
                m_socket->disconnectFromHost();
            break;
        }
        case OP_PING:
            writeFrame(encodeFrame(payload, OP_PONG, m_isClient));
            break;
        case OP_PONG: {
            quint64 elapsed = m_pingTimer2.isValid()
            ? static_cast<quint64>(m_pingTimer2.elapsed()) : 0;
            emit pong(elapsed, payload);
            break;
        }
        default:
            qWarning() << "[WebSocket] Unknown control opcode:" << opcode;
            break;
        }
        return;
    }

    // ── 数据帧 ────────────────────────────────────────────────────────────────
    if (opcode == OP_CONTINUATION) {
        if (!m_fragmenting) {
            qWarning() << "[WebSocket] Unexpected continuation frame";
            close(CloseCodeProtocolError, QStringLiteral("Unexpected continuation frame"));
            return;
        }
        m_fragmentBuffer += payload;
        if (static_cast<quint64>(m_fragmentBuffer.size()) > m_maxMessageSize) {
            close(CloseCodeTooMuchData, QStringLiteral("Message too large"));
            return;
        }
        if (fin) {
            dispatchMessage(m_fragmentOpcode, m_fragmentBuffer, true);
            m_fragmentBuffer.clear();
            m_fragmenting = false;
        } else {
            dispatchMessage(m_fragmentOpcode, payload, false);
        }
    } else if (opcode == OP_TEXT || opcode == OP_BINARY) {
        if (!fin) {
            if (m_fragmenting) {
                qWarning() << "[WebSocket] New message started before previous one completed";
                close(CloseCodeProtocolError, QStringLiteral("Interleaved fragmented messages"));
                return;
            }
            m_fragmenting    = true;
            m_fragmentOpcode = opcode;
            m_fragmentBuffer = payload;
            dispatchMessage(opcode, payload, false);
        } else {
            dispatchMessage(opcode, payload, true);
        }
    } else {
        qWarning() << "[WebSocket] Unknown data opcode:" << opcode;
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// 消息分发
// ─────────────────────────────────────────────────────────────────────────────
void WebSocket::dispatchMessage(quint8 opcode, const QByteArray &payload, bool isLastFrame)
{
    if (opcode == OP_TEXT) {
        QString text = QString::fromUtf8(payload);
        emit textFrameReceived(text, isLastFrame);
        if (isLastFrame)
            emit textMessageReceived(text);
    } else if (opcode == OP_BINARY) {
        emit binaryFrameReceived(payload, isLastFrame);
        if (isLastFrame)
            emit binaryMessageReceived(payload);
    }
}

// ─────────────────────────────────────────────────────────────────────────────
// 帧解码（去掩码）
// ─────────────────────────────────────────────────────────────────────────────
QByteArray WebSocket::decodeFrame(const QByteArray &frame)
{
    if (frame.size() < 2) return {};

    quint8  b2         = static_cast<quint8>(frame[1]);
    bool    masked     = b2 & MASK_BIT;
    quint64 payloadLen = b2 & 0x7F;
    int     index      = 2;

    if (payloadLen == 126) {
        payloadLen = (static_cast<quint8>(frame[2]) << 8) | static_cast<quint8>(frame[3]);
        index += 2;
    } else if (payloadLen == 127) {
        payloadLen = 0;
        for (int i = 0; i < 8; ++i)
            payloadLen = (payloadLen << 8) | static_cast<quint8>(frame[2 + i]);
        index += 8;
    }

    QByteArray mask;
    if (masked) {
        mask = frame.mid(index, 4);
        index += 4;
    }

    QByteArray payload = frame.mid(index, static_cast<int>(payloadLen));

    if (masked) {
        for (int i = 0; i < payload.size(); ++i)
            payload[i] = static_cast<char>(
                static_cast<quint8>(payload[i]) ^ static_cast<quint8>(mask[i % 4]));
    }

    return payload;
}

// ─────────────────────────────────────────────────────────────────────────────
// 帧编码
// 服务端发送：mask = false（RFC 6455 §5.1）
// 客户端发送：mask = true（RFC 6455 §5.1 强制要求）
// ─────────────────────────────────────────────────────────────────────────────
QByteArray WebSocket::encodeFrame(const QByteArray &payload, quint8 opcode, bool mask)
{
    QByteArray frame;

    frame.append(static_cast<char>(FIN_BIT | opcode));

    quint8 maskBit = mask ? MASK_BIT : 0x00;
    int    size    = payload.size();

    if (size <= 125) {
        frame.append(static_cast<char>(maskBit | size));
    } else if (size <= 0xFFFF) {
        frame.append(static_cast<char>(maskBit | 126));
        frame.append(static_cast<char>((size >> 8) & 0xFF));
        frame.append(static_cast<char>( size       & 0xFF));
    } else {
        frame.append(static_cast<char>(maskBit | 127));
        for (int i = 7; i >= 0; --i)
            frame.append(static_cast<char>((size >> (8 * i)) & 0xFF));
    }

    if (mask) {
        QByteArray maskKey(4, 0);
        for (int i = 0; i < 4; ++i)
            maskKey[i] = static_cast<char>(QRandomGenerator::global()->bounded(256));
        frame.append(maskKey);
        QByteArray maskedPayload = payload;
        for (int i = 0; i < maskedPayload.size(); ++i)
            maskedPayload[i] = static_cast<char>(
                static_cast<quint8>(maskedPayload[i]) ^ static_cast<quint8>(maskKey[i % 4]));
        frame.append(maskedPayload);
    } else {
        frame.append(payload);
    }

    return frame;
}

// ─────────────────────────────────────────────────────────────────────────────
// 内部写入
// ─────────────────────────────────────────────────────────────────────────────
qint64 WebSocket::writeFrame(const QByteArray &frame)
{
    if (!m_socket) return -1;
    return m_socket->write(frame);
}

// ─────────────────────────────────────────────────────────────────────────────
// Slots
// ─────────────────────────────────────────────────────────────────────────────
void WebSocket::onBytesWritten(qint64 bytes)
{
    emit bytesWritten(bytes);
}

void WebSocket::sendPingTimeout()
{
    ping(QByteArray());
}

void WebSocket::onDisconnected()
{
    m_closing = false;

    if (m_pingTimer) {
        m_pingTimer->stop();
        m_pingTimer->deleteLater();
        m_pingTimer = nullptr;
    }

    m_wsBuffer.clear();
    m_handshakeBuffer.clear();
    m_fragmentBuffer.clear();
    m_fragmenting = false;

    if (m_socket) {
        m_socket->deleteLater();
        m_socket = nullptr;
    }

    m_handshakeDone = false;

    emit readChannelFinished();
    emit disconnected();
}

// ─────────────────────────────────────────────────────────────────────────────
// 内部错误处理
// ─────────────────────────────────────────────────────────────────────────────
void WebSocket::setError(WebSocketError err, const QString &errorString)
{
    m_error       = err;
    m_errorString = errorString;
    qWarning() << "[WebSocket] Error:" << errorString;
    emit error(err);
}