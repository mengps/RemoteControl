#ifndef CONNECTION_H
#define CONNECTION_H

#include <QTcpSocket>

class RemoteEvent;
class Connection : public QTcpSocket
{
    Q_OBJECT

public:
    Connection(QObject *parent = nullptr);

    Q_INVOKABLE void abort() { QTcpSocket::abort(); }
    Q_INVOKABLE void writeToSocket(const RemoteEvent &event);

signals:
    void hasEventData(const RemoteEvent &event);

private:
    void processRecvData();

    QByteArray m_recvData;
};

#endif // CONNECTION_H
