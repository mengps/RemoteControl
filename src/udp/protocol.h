#ifndef PROTOCOL_UDP_H
#define PROTOCOL_UDP_H

#include <QDebug>
#include <QtGlobal>
#include <QDataStream>

#define INFO_TYPE   0x01
#define EVENT_TYPE  0x02
#define SCREEN_TYPE 0x03

struct DataBlock
{
    qint8  blockType;   //类型
    qint32 blockSize;   //总大小
    qint32 blockIndex;  //索引
    qint32 blockNum;    //被分成的块数
    QByteArray data;    //块数据

    DataBlock()
        : blockType(0), blockSize(0), blockIndex(0)
        , blockNum(0), data(QByteArray()) { }

    int size() const
    {
        return int(sizeof (blockType)) +
               int(sizeof (blockType)) +
               int(sizeof (blockIndex)) +
               int(sizeof (blockSize)) +
               int(sizeof (blockNum)) +
               data.size() + 4;
    }
};

extern QDataStream &operator>>(QDataStream &in, DataBlock &block);
extern QDataStream &operator<<(QDataStream &out, DataBlock &block);
extern QDebug operator<<(QDebug debug, const DataBlock &block);

#endif // PROTOCOL_UDP_H
