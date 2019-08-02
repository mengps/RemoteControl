#ifndef REMOTEEVENT_H
#define REMOTEEVENT_H

#include <QByteArray>
#include <QPointF>

class RemoteEvent
{
public:
    enum EventType
    {
        Click = 1,
        DoubleClick,
        KeyInput
    };

    RemoteEvent() { }
    RemoteEvent(EventType type, const QPointF &position);

    EventType type() const { return m_type; }
    QPointF position() const { return m_position; }

private:
    EventType m_type;
    QPointF m_position;
};

#endif // REMOTEEVENT_H
