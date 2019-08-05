#ifndef API_H
#define API_H

#include <QObject>

class Api : public QObject
{
    Q_OBJECT

public:
    Api();

    Q_INVOKABLE QString getLocalIpAddress();
};

#endif // API_H
