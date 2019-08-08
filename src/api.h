#ifndef API_H
#define API_H

#include <QObject>

class QHostAddress;
class Api : public QObject
{
    Q_OBJECT

public:
    Api();
    static bool isLocalAddress(const QHostAddress &address);
    Q_INVOKABLE QString getLocalIpAddress();
};

#endif // API_H
