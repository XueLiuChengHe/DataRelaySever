#include <QCoreApplication>
#include <QDateTime>
#include <SimulatorJsonMsg.h>
#include <QAbstractSocket>
#include <ATSSessionDefinitionJsonMsg.h>
#include <ATSStateControlJsonMsg.h>
#include <ATSVirtualTimeJsonMsg.h>
#include <ATSATPATOinfoJsonMsg.h>
#include <ATSMMIInfoJsonMsg.h>
#include <ATSAutomaticTrainInfoJsonMsg.h>
#include <ATSLineDeviceDataJsonMsg.h>

#ifdef Q_OS_WIN
#include <windows.h>
#include <stdio.h>
#include <MineToHengNuoServer.h>
#include <QApplication>
#endif

void myMessageHandler(QtMsgType type, const QMessageLogContext &, const QString &msg);

int main(int argc, char *argv[])
{
    qInstallMessageHandler(myMessageHandler);
    // QCoreApplication a(argc, argv);
    QApplication a(argc, argv);

    // 注册元类型
    qRegisterMetaType<SimulatorJsonMsg>("SimulatorJsonMsg");
    qRegisterMetaType<QAbstractSocket::SocketError>("QAbstractSocket::SocketError");
    qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");
    qRegisterMetaType<ATSSessionDefinitionJsonMsg>("ATSSessionDefinitionJsonMsg");
    qRegisterMetaType<ATSStateControlJsonMsg>("ATSStateControlJsonMsg");
    qRegisterMetaType<ATSVirtualTimeJsonMsg>("ATSVirtualTimeJsonMsg");
    qRegisterMetaType<ATSATPATOInfoJsonMsg>("ATSATPATOInfoJsonMsg");
    qRegisterMetaType<ATSMMIInfoJsonMsg>("ATSMMIInfoJsonMsg");
    qRegisterMetaType<ATSAutomaticTrainInfoJsonMsg>("ATSAutomaticTrainInfoJsonMsg");
    qRegisterMetaType<ATSLineDeviceDataJsonMsg>("ATSLineDeviceDataJsonMsg");
    qRegisterMetaType<ATSSIMFaultInfo>("ATSSIMFaultInfo");
    qRegisterMetaType<ATSData>("ATSData");

    QString baseDir = QCoreApplication::applicationDirPath();
    QDir dir(baseDir);
    QString ats_log_path = dir.filePath("log/ats_log");
    QString simulator_log_path = dir.filePath("log/simulator_log");
    QString simulator_config_path = dir.filePath("config/SimulatorServerConfig.ini");
    QString turn_config_path = dir.filePath("config/Turnoutconfig.json");

    MineToHengNuoServer mainToHengNuoSever(
        simulator_log_path,
        ats_log_path,
        simulator_config_path,
        turn_config_path
    );

    // return QCoreApplication::exec();
    return a.exec();
}


/**
 * 日志样式显示
 * @brief myMessageHandler
 * @param type
 * @param msg
 */
void myMessageHandler(QtMsgType type, const QMessageLogContext &, const QString &msg)
{
    QString prefix;
    switch (type) {
    case QtDebugMsg:    prefix = "DEBUG"; break;
    case QtInfoMsg:     prefix = "INFO "; break;
    case QtWarningMsg:  prefix = "WARN "; break;
    case QtCriticalMsg: prefix = "ERROR"; break;
    case QtFatalMsg:    prefix = "FATAL"; break;
    }

    fprintf(stdout, "[%s] [%s] %s\n",
            QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz").toUtf8().constData(),
            prefix.toUtf8().constData(),
            msg.toUtf8().constData());
    fflush(stdout);
}