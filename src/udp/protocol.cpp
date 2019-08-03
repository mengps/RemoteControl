#include "protocol.h"
#include "remoteevent.h"

QDataStream &operator>>(QDataStream &in, ScreenDataBlock &block)
{
    in >> block.blockSize
       >> block.blockIndex
       >> block.blockNum
       >> block.data;

    return in;
}

QDataStream &operator<<(QDataStream &out, const ScreenDataBlock &block)
{
    out << block.blockSize
        << block.blockIndex
        << block.blockNum
        << block.data;

    return out;
}

QDataStream &operator>>(QDataStream &in, RemoteEvent &block)
{
    qint32 type;
    QPointF position;
    in >> type >> position;
    block.setType(RemoteEvent::EventType(type));
    block.setPosition(position);

    return in;
}

QDataStream &operator<<(QDataStream &out, const RemoteEvent &block)
{
    out << qint32(block.type())
        << block.position();

    return out;
}

QDebug operator<<(QDebug debug, const ScreenDataBlock &block)
{
    debug << "[blockSize]: "  << block.blockSize   << endl
          << "[blockIndex]: " << block.blockIndex  << endl
          << "[blockNum]: "   << block.blockNum    << endl
          << "[data size]: "  << block.data.size() << endl;

    return debug;
}
