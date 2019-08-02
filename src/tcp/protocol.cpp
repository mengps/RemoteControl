#include "protocol.h"

QDataStream& operator>>(QDataStream &in, BlockHeader &header)
{
    in >> header.type
       >> header.dataSize;

    return in;
}

QDataStream& operator<<(QDataStream &out, BlockHeader &header)
{
    out << header.type
        << header.dataSize;

    return out;
}

QDataStream& operator>>(QDataStream &in, DataBlock &block)
{
    in >> block.header
       >> block.data;

    return in;
}

QDataStream& operator<<(QDataStream &out, DataBlock &block)
{
    out << block.header
        << block.data;

    return out;
}

QDebug operator<<(QDebug debug, const BlockHeader &header)
{
    debug << "[type]: " << header.type << endl
          << "[data size]: " << header.dataSize << endl;

    return debug;
}

QDebug operator<<(QDebug debug, const DataBlock &block)
{
    debug << "[type]: " << block.header.type << endl
          << "[data size]: " << block.header.dataSize << endl;

    return debug;
}
