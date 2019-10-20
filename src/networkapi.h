#ifndef NETWORKAPI_H
#define NETWORKAPI_H

#include <QObject>

class QHostAddress;
class NetworkApi : public QObject
{
    Q_OBJECT

public:
    NetworkApi(QObject *parent = nullptr);

    static bool isLocalAddress(const QHostAddress &address);
    Q_INVOKABLE QString getLocalIpAddress();
};

#endif // NETWORKAPI_H
