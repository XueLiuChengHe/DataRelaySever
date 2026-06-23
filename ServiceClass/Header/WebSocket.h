#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include <QByteArray>
#include <QUrl>
#include <QAbstractSocket>
#include <QSslError>
#include <QCryptographicHash>
#include <QTextStream>
#include <QElapsedTimer>

/**
 * @brief A WebSocket implementation compatible with QWebSocket interface,
 *        but exposing the underlying socket file descriptor.
 *
 * Supports both server mode (accept incoming connection) and
 * client mode (connect to a remote server).
 *
 * Usage (server side):
 *   void MyServer::incomingConnection(qintptr fd) {
 *       QThread *thread = new QThread();
 *       WebSocket *ws = new WebSocket(fd);
 *       ws->moveToThread(thread);
 *       connect(thread, &QThread::started, ws, &WebSocket::start);
 *       connect(ws, &WebSocket::disconnected, thread, &QThread::quit);
 *       connect(thread, &QThread::finished, ws, &QObject::deleteLater);
 *       connect(thread, &QThread::finished, thread, &QObject::deleteLater);
 *       thread->start();
 *   }
 *
 * Usage (client side, same thread):
 *   WebSocket *ws = WebSocket::connectToUrl(QUrl("ws://host:port/path"), parent);
 *   connect(ws, &WebSocket::connected, ...);
 *
 * Usage (client side, cross-thread):
 *   QThread *thread = new QThread();
 *   WebSocket *ws = new WebSocket(-1);   // -1 = client mode placeholder
 *   ws->moveToThread(thread);
 *   connect(thread, &QThread::started, ws, [ws]() {
 *       ws->connectToServer(QUrl("ws://host:port/path"));
 *   });
 *   connect(ws, &WebSocket::disconnected, thread, &QThread::quit);
 *   connect(thread, &QThread::finished, ws, &QObject::deleteLater);
 *   connect(thread, &QThread::finished, thread, &QObject::deleteLater);
 *   thread->start();
 */
class WebSocket : public QObject
{
    Q_OBJECT

public:
    // ── 错误类型，对齐 QAbstractSocket::SocketError ──────────────────────────
    enum WebSocketError {
        NoError,
        ConnectionRefusedError,
        HandshakeFailedError,
        ProtocolError,
        DatagramTooLargeError,
        SslHandshakeFailedError,
        UnknownError
    };
    Q_ENUM(WebSocketError)

    // ── 关闭状态码，对齐 QWebSocketProtocol::CloseCode ───────────────────────
    enum CloseCode {
        CloseCodeNormal                = 1000,
        CloseCodeGoingAway             = 1001,
        CloseCodeProtocolError         = 1002,
        CloseCodeDatatypeNotSupported  = 1003,
        CloseCodeReserved1004          = 1004,
        CloseCodeMissingStatusCode     = 1005,
        CloseCodeAbnormalDisconnection = 1006,
        CloseCodeWrongDatatype         = 1007,
        CloseCodePolicyViolated        = 1008,
        CloseCodeTooMuchData           = 1009,
        CloseCodeMissingExtension      = 1010,
        CloseCodeBadOperation          = 1011,
        CloseCodeTlsHandshakeFailed    = 1015
    };
    Q_ENUM(CloseCode)

public:
    /**
     * @param descriptor  服务端模式传入 fd；客户端模式传 -1
     */
    explicit WebSocket(qintptr descriptor, QObject *parent = nullptr);
    ~WebSocket();

    // ── 生命周期（服务端模式） ────────────────────────────────────────────────
    /** 在目标线程中调用，初始化 QTcpSocket 并开始握手（服务端模式） */
    void start();

    // ── 生命周期（客户端模式） ────────────────────────────────────────────────
    /** 工厂方法：在当前线程创建并立即发起连接 */
    static WebSocket *connectToUrl(const QUrl &url, QObject *parent = nullptr);

    /** 在目标线程中调用，主动连接到服务端（客户端模式） */
    void connectToServer(const QUrl &url);

    // ── QWebSocket 兼容接口 ──────────────────────────────────────────────────
    qint64 sendTextMessage(const QString &message);
    qint64 sendBinaryMessage(const QByteArray &data);

    void close(CloseCode closeCode = CloseCodeNormal,
               const QString &reason = QString());
    void ping(const QByteArray &payload = QByteArray());
    void abort();

    bool isValid() const;

    QString        errorString() const;
    WebSocketError error() const;

    QAbstractSocket::SocketState state() const;

    QHostAddress localAddress() const;
    quint16      localPort() const;
    QHostAddress peerAddress() const;
    quint16      peerPort() const;
    QString      peerName() const;

    qint64 readBufferSize() const;
    void   setReadBufferSize(qint64 size);

    void    setMaxAllowedIncomingFrameSize(quint64 maxAllowedIncomingFrameSize);
    quint64 maxAllowedIncomingFrameSize() const;

    void    setMaxAllowedIncomingMessageSize(quint64 maxAllowedIncomingMessageSize);
    quint64 maxAllowedIncomingMessageSize() const;

    // ── 扩展接口：暴露 fd ─────────────────────────────────────────────────────
    /** 返回底层 TCP socket 的文件描述符，-1 表示无效 */
    qintptr socketDescriptor() const;

    /** 直接访问底层 QTcpSocket（谨慎使用） */
    QTcpSocket *socket() const;

signals:
    // ── QWebSocket 兼容信号 ──────────────────────────────────────────────────
    void connected();
    void disconnected();
    void textMessageReceived(const QString &message);
    void binaryMessageReceived(const QByteArray &message);
    void textFrameReceived(const QString &frame, bool isLastFrame);
    void binaryFrameReceived(const QByteArray &frame, bool isLastFrame);
    void pong(quint64 elapsedTime, const QByteArray &payload);
    void bytesWritten(qint64 bytes);
    void readChannelFinished();
    void stateChanged(QAbstractSocket::SocketState state);
    void error(WebSocket::WebSocketError error);
    void sslErrors(const QList<QSslError> &errors); // 预留，暂未实现 TLS

    // ── 扩展信号 ──────────────────────────────────────────────────────────────
    /** 握手完成后触发，此时 socketDescriptor() 可用 */
    void handshakeCompleted();

private slots:
    // 服务端握手
    void onReadyReadForHandshake();
    // 客户端握手
    void onConnectedForHandshake();
    void onReadyReadForClientHandshake();
    // 公共
    void onReadyRead();
    void onDisconnected();
    void onBytesWritten(qint64 bytes);
    void sendPingTimeout();

private:
    // 帧解析
    int        parseNextFrame(const QByteArray &buffer, QByteArray &outFrame);
    void       processFrame(const QByteArray &frame);
    QByteArray decodeFrame(const QByteArray &frame);
    QByteArray encodeFrame(const QByteArray &payload, quint8 opcode, bool mask = false);

    // 帧分发（处理分片）
    void dispatchMessage(quint8 opcode, const QByteArray &payload, bool isLastFrame);

    // 内部发送
    qint64 writeFrame(const QByteArray &frame);

    void setError(WebSocketError err, const QString &errorString);

    // 公共握手完成后的收尾（切换到帧读取模式）
    void finalizeHandshake(const QByteArray &leftover);

private:
    // ── 网络 ──────────────────────────────────────────────────────────────────
    QTcpSocket   *m_socket            = nullptr;
    QTimer       *m_pingTimer         = nullptr;
    QElapsedTimer m_pingTimer2;                  // 用于计算 pong RTT
    qintptr       m_socketDescriptor;

    // ── 模式 ──────────────────────────────────────────────────────────────────
    bool          m_isClient          = false;
    QUrl          m_serverUrl;
    QByteArray    m_clientKey;                   // 客户端握手随机 key（Base64）

    // ── 状态 ──────────────────────────────────────────────────────────────────
    bool          m_handshakeDone     = false;
    bool          m_closing           = false;

    // ── 缓冲 ──────────────────────────────────────────────────────────────────
    QByteArray    m_handshakeBuffer;
    QByteArray    m_wsBuffer;

    // ── 分片帧重组 ─────────────────────────────────────────────────────────────
    bool          m_fragmenting       = false;
    quint8        m_fragmentOpcode    = 0x00;
    QByteArray    m_fragmentBuffer;

    // ── 流量控制 ──────────────────────────────────────────────────────────────
    quint64       m_maxFrameSize      = 32 * 1024 * 1024; // 32 MB
    quint64       m_maxMessageSize    = 32 * 1024 * 1024; // 32 MB

    // ── 错误 ──────────────────────────────────────────────────────────────────
    WebSocketError m_error            = NoError;
    QString        m_errorString;
};

#endif // WEBSOCKET_H
