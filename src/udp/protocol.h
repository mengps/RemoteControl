#ifndef PROTOCOL_UDP_H
#define PROTOCOL_UDP_H

#include <QDebug>
#include <QtGlobal>
#include <QDataStream>

struct ScreenDataBlock
{
    qint32 blockSize;   //总大小
    qint32 blockIndex;  //索引
    qint32 blockNum;    //被分成的块数
    QByteArray data;    //块数据

    ScreenDataBlock()
        : blockSize(0), blockIndex(0)
        , blockNum(0), data(QByteArray()) { }

    int size() const
    {
        return int(sizeof (blockIndex)) +
               int(sizeof (blockSize)) +
               int(sizeof (blockNum)) +
               data.size() + 4;
    }
};

class RemoteEvent;

extern QDataStream &operator>>(QDataStream &in, ScreenDataBlock &block);
extern QDataStream &operator<<(QDataStream &out, const ScreenDataBlock &block);
extern QDataStream &operator>>(QDataStream &in, RemoteEvent &event);
extern QDataStream &operator<<(QDataStream &out, const RemoteEvent &event);
extern QDebug operator<<(QDebug debug, const ScreenDataBlock &block);

#endif // PROTOCOL_UDP_H
