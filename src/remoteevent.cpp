#include "protocol.h"
#include "remoteevent.h"

RemoteEvent::RemoteEvent(RemoteEvent::EventType type, const QPointF &position)
    : m_type(type)
    , m_position(position)
{

}
