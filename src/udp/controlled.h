#ifndef CONTROLLED_H
#define CONTROLLED_H

#include <QObject>
#include <QTcpServer>

class Socket;
class RemoteEvent;
class Connection;
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
    int m_timerId = 0;
    Socket *m_screenSocket = nullptr;
    Connection *m_connection = nullptr;
};

#endif // CONTROLLED_H
