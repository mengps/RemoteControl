#include "networkapi.h"
#include <QNetworkInterface>

NetworkApi::NetworkApi(QObject *parent)
    : QObject (parent)
{

}

bool NetworkApi::isLocalAddress(const QHostAddress &address)
{
#ifndef Q_OS_ANDROID    //安卓需要一些东西
    auto interfaces = QNetworkInterface::allInterfaces();
    QList<QNetworkAddressEntry> entry;
    for (auto interface : interfaces) {
        if (interface.flags() & (QNetworkInterface::IsUp | QNetworkInterface::IsRunning)) {
            entry = interface.addressEntries();
            QHostAddress ip = entry.at(1).ip();
            if (ip.toIPv4Address() == address.toIPv4Address()) //应对IPv6->IPv4
                return true;
            entry.clear();
        }
    }
#endif
    return false;
}

QString NetworkApi::getLocalIpAddress()
{
    QString localIp = "0.0.0.0";
#ifndef Q_OS_ANDROID
    auto interfaces = QNetworkInterface::allInterfaces();
    QList<QNetworkAddressEntry> entries;
    for (auto interface : interfaces) {
        if (interface.flags() & QNetworkInterface::IsLoopBack)  //过滤环回地址
            continue;

        if (interface.flags() & (QNetworkInterface::IsUp | QNetworkInterface::IsRunning)) {
            entries = interface.addressEntries();
            for (auto entry : entries) {
                if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol) {
                    if (interface.name().indexOf("wireless") != -1) {
                        localIp = entry.ip().toString();
                        qDebug() << interface.humanReadableName() << interface.name() << " 无线网IP: " << localIp;
                    } else if (interface.name().indexOf("ethernet") != -1) {
                        //以太网ip暂时不用
                        qDebug() << interface.humanReadableName() << interface.name() << " 以太网IP: " << entry.ip().toString();
                    }
                }
            }
            entries.clear();
        }
    }
#endif
    return localIp;
}
