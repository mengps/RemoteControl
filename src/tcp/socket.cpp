#include "remoteevent.h"
#include "socket.h"

Socket::Socket(QObject *parent)
    : QTcpSocket (parent)
{
    connect(this, &QTcpSocket::readyRead, this, [this]() {
        m_recvData += readAll();
        processRecvBlock();
    });
}

Socket::~Socket()
{

}

void Socket::abort()
{
    QTcpSocket::abort();
    m_recvData.clear();
    m_recvHeader.clear();
}

void Socket::writeToSocket(DataBlock block)
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_12);
    out << block;
    write(data);
}

void Socket::writeToSocket(const RemoteEvent &event)
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_12);
    out << qint32(event.type()) << event.position();
    BlockHeader header = { EVENT_TYPE, data.size() };
    DataBlock block = { header, data };
    out.device()->seek(0);
    out << block;
    write(data);
    flush();
}

void Socket::processRecvBlock()
{
    if (m_recvHeader.isEmpty() && m_recvData.size() > 0) {
        BlockHeader header;
        QDataStream in(&m_recvData, QIODevice::ReadOnly);
        in.setVersion(QDataStream::Qt_5_12);
        in >> header;

        if (header.isEmpty())
            return;

        m_recvHeader = header;
        m_recvData.remove(0, header.size());      
    }

    if (m_recvData.size() < m_recvHeader.dataSize)
        return;

    DataBlock block;
    block.header = m_recvHeader;
    block.data = m_recvData.left(m_recvHeader.dataSize);
    m_recvData = m_recvData.mid(m_recvHeader.dataSize);
    m_recvHeader.clear();

    if (block.header.type == SCREEN_TYPE) {
        emit hasScreenData(block.data);
    } else if (block.header.type == EVENT_TYPE) {
        qint32 type;
        QPointF position;
        QDataStream in(block.data);
        in.setVersion(QDataStream::Qt_5_12);
        in >> type >> position;
        RemoteEvent event(RemoteEvent::EventType(type), position);
        emit hasEventData(event);
    }

    if (m_recvData.size() > 0)  //如果还有则继续处理
        processRecvBlock();
}


