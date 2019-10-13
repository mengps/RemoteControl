#include "connection.h"
#include "protocol.h"
#include "remoteevent.h"

Connection::Connection(QObject *parent)
    : QTcpSocket (parent)
{
    connect(this, &QTcpSocket::readyRead, this, [this]() {
        m_recvData += readAll();
        processRecvData();
    });
}

void Connection::writeToSocket(const RemoteEvent &event)
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_12);
    out << event;
    write(data);
    flush();
}

void Connection::processRecvData()
{
    RemoteEvent event;
    QDataStream in(&m_recvData, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Qt_5_12);
    in >> event;

    if (!event.isEmpty()) {
        //position会在前面加4字节
        m_recvData.remove(0, sizeof (event.type()) + sizeof (event.position()) + 4);
        emit hasEventData(event);
    }

    if (m_recvData.size() > 0)
        processRecvData();
}
