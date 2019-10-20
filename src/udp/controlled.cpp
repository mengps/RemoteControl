#include "controlled.h"
#include "connection.h"
#include "remoteevent.h"
#include "systemapi.h"
#include "socket.h"

#include <QBuffer>
#include <QGuiApplication>
#include <QScreen>
#include <QThread>

Controlled::Controlled(QObject *parent)
    : QTcpServer (parent)
{
    listen(QHostAddress::Any, 43801);
    m_screenSocket = new Socket;
    QThread *thread = new QThread;
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    m_screenSocket->moveToThread(thread);
    thread->start();
}

Controlled::~Controlled()
{

}

void Controlled::finish()
{
    if (m_connection)
        m_connection->disconnectFromHost();
}

void Controlled::processEvent(const RemoteEvent &ev)
{
    QRectF screenRect = qApp->primaryScreen()->geometry();
    QPointF localPos(ev.position().x() * screenRect.width(),
                     ev.position().y() * screenRect.height());

    switch (ev.type())
    {
    case RemoteEvent::EventType::Pressed:
        SystemApi::mousePress(localPos);
        break;
    case RemoteEvent::EventType::Released:
        SystemApi::mouseRelease(localPos);
        break;
    case RemoteEvent::EventType::Moved:
        SystemApi::mouseMove(localPos);
        break;
    case RemoteEvent::EventType::KeyInput:
        break;
    default:
        break;
    }
}

void Controlled::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);
    if (m_screenSocket) {
        QBuffer buffer;
        buffer.open(QIODevice::WriteOnly);
        QPixmap pixmap = SystemApi::grabScreen();
        pixmap.save(&buffer, "jpg", -1);
        QMetaObject::invokeMethod(m_screenSocket, "writeToSocket", Q_ARG(QByteArray, buffer.data()));
    }
}

void Controlled::incomingConnection(qintptr socketDescriptor)
{
    if (!m_connection) {
        m_connection = new Connection(this);
        connect(m_connection, &Connection::stateChanged, this, [this](QAbstractSocket::SocketState socketState) {
            switch (socketState)
            {
            case QAbstractSocket::ConnectedState:
                emit connected();
                break;
            default:
                break;
            }
        });
        connect(m_connection, &Connection::disconnected, this, [this]() {
            m_connection->deleteLater();
            m_connection = nullptr;
            QMetaObject::invokeMethod(m_screenSocket, "finish");
            killTimer(m_timerId);
            m_timerId = 0;
            emit disconnected();
        });
        connect(m_connection, &Connection::hasEventData, this, [this](const RemoteEvent &event) {
            processEvent(event);
        });
        m_connection->setSocketDescriptor(socketDescriptor);
        QMetaObject::invokeMethod(m_screenSocket, "setDestAddr",
                                  Q_ARG(QHostAddress, QHostAddress(m_connection->peerAddress())));

        if (!m_timerId)
            m_timerId = startTimer(std::chrono::milliseconds(40));
    }
}
