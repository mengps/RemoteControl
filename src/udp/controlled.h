#ifndef CONTROLLED_H
#define CONTROLLED_H

#include "protocol.h"
#include <QObject>

class Socket;
class RemoteEvent;
class Controlled : public QObject
{
    Q_OBJECT

public:
    explicit Controlled(QObject *parent = nullptr);
    ~Controlled();

signals:
    void hasRemoteConnection();

public slots:
    void processEvent(const RemoteEvent &ev);

protected:
    void timerEvent(QTimerEvent *event);

private:
    QPixmap grabScreen();

    Socket *m_controlled = nullptr;
    int m_timerId = 0;
};

#endif // CONTROLLED_H
