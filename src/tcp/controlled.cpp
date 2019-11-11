#include "controlled.h"
#include "remoteevent.h"
#include "systemapi.h"
#include "socket.h"

#include <QBuffer>
#include <QGuiApplication>
#include <QScreen>
#include <QTime>
#include <QThread>

Controlled::Controlled(QObject *parent)
    : QTcpServer (parent)
{
    listen(QHostAddress::Any, 43800);
}

Controlled::~Controlled()
{

}

void Controlled::finish()
{
    if (m_controlled)
        QMetaObject::invokeMethod(m_controlled, "abort");
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
    if (m_controlled) {
        QBuffer buffer;
        buffer.open(QIODevice::WriteOnly);
        QTime time = QTime::currentTime();
        QPixmap pixmap = SystemApi::grabScreen();
        qDebug() << time.msecsTo(QTime::currentTime());
        pixmap.save(&buffer, "jpg", -1);
        BlockHeader header = { SCREEN_TYPE, qint32(buffer.size()) };
        DataBlock data = { header, buffer.data() };
        QMetaObject::invokeMethod(m_controlled, "writeToSocket", Q_ARG(DataBlock, data));
    }
}

void Controlled::incomingConnection(qintptr socketDescriptor)
{
    if (!m_controlled) {
        QThread *thread = new QThread;
        connect(thread, &QThread::finished, thread, &QThread::deleteLater);
        m_controlled = new Socket;
        connect(m_controlled, &Socket::stateChanged, this, [this](QAbstractSocket::SocketState socketState) {
            switch (socketState)
            {
            case QAbstractSocket::ConnectedState:
                emit connected();
                break;
            default:
                break;
            }
        });
        connect(m_controlled, &Socket::disconnected, this, [this]() {
            Socket *socket = m_controlled;
            m_controlled = nullptr;
            socket->deleteLater();
            killTimer(m_timerId);
            m_timerId = 0;
            emit disconnected();
        });
        connect(m_controlled, &Socket::hasEventData, this, [this](const RemoteEvent &event) {
            processEvent(event);
        });
        m_controlled->setSocketDescriptor(socketDescriptor);
        m_controlled->moveToThread(thread);
        thread->start();

        if (!m_timerId)
            m_timerId = startTimer(std::chrono::milliseconds(30));
    }
}
