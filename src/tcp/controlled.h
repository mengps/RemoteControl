#ifndef CONTROLLED_H
#define CONTROLLED_H

#include "protocol.h"
#include <QObject>
#include <QTcpServer>

class Socket;
class RemoteEvent;
class Controlled : public QTcpServer
{
    Q_OBJECT

public:
    explicit Controlled(QObject *parent = nullptr);
    ~Controlled();

    Q_INVOKABLE void finish();

signals:
    void connected();
    void disconnected();

public slots:
    void processEvent(const RemoteEvent &ev);

protected:
    void timerEvent(QTimerEvent *event);
    void incomingConnection(qintptr socketDescriptor);

private:
    Socket *m_controlled = nullptr;
    int m_timerId = 0;
};

#endif // CONTROLLED_H
