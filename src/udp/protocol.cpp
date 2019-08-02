#include "protocol.h"

QDataStream &operator>>(QDataStream &in, DataBlock &block)
{
    in >> block.blockType
       >> block.blockSize
       >> block.blockIndex
       >> block.blockNum
       >> block.data;

    return in;
}

QDataStream &operator<<(QDataStream &out, DataBlock &block)
{
    out << block.blockType
        << block.blockSize
        << block.blockIndex
        << block.blockNum
        << block.data;

    return out;
}

QDebug operator<<(QDebug debug, const DataBlock &block)
{
    debug << "[blockType]: "  << block.blockType   << endl
          << "[blockSize]: "  << block.blockSize   << endl
          << "[blockIndex]: " << block.blockIndex  << endl
          << "[blockNum]: "   << block.blockNum    << endl
          << "[data size]: "  << block.data.size() << endl;

    return debug;
}
