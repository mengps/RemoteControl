#ifndef PROTOCOL_UDP_H
#define PROTOCOL_UDP_H

#include <QDebug>
#include <QtGlobal>
#include <QDataStream>

struct DataBlock
{
    qint32 blockSize;   //总大小
    qint32 blockIndex;  //索引
    qint32 blockNum;    //被分成的块数
    QByteArray data;    //块数据

    DataBlock()
        : blockSize(0), blockIndex(0)
        , blockNum(0), data(QByteArray()) { }

    int size() const {
        return int(sizeof (blockIndex)) +
               int(sizeof (blockSize)) +
               int(sizeof (blockNum)) +
               data.size() + 4;
    }
};

class RemoteEvent;

extern QDataStream &operator>>(QDataStream &in, DataBlock &block);
extern QDataStream &operator<<(QDataStream &out, const DataBlock &block);
extern QDataStream &operator>>(QDataStream &in, RemoteEvent &event);
extern QDataStream &operator<<(QDataStream &out, const RemoteEvent &event);
extern QDebug operator<<(QDebug debug, const DataBlock &block);

#endif // PROTOCOL_UDP_H
