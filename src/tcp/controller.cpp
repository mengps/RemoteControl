#include "controller.h"
#include "imageprovider.h"
#include "socket.h"

#include <QGuiApplication>
#include <QHostAddress>
#include <QThread>

Controller::Controller(QObject *parent)
    : QObject(parent)
{
    m_provider = new ImageProvider;

    m_socket = new Socket;
    QThread *thread = new QThread;
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    connect(m_socket, &Socket::connected, this, &Controller::connected);
    connect(m_socket, &Socket::disconnected, this, &Controller::disconnected);
    connect(m_socket, &Socket::hasScreenData, this, [this](const QByteArray &screenData)
    {
        QPixmap pixmap;
        pixmap.loadFromData(screenData);
        m_provider->setPixmap(pixmap);
        emit needUpdate();
    });
    m_socket->moveToThread(thread);
    thread->start();
}

void Controller::finish()
{
     QMetaObject::invokeMethod(m_socket, "abort");
}

void Controller::mouseClicked(const QPointF &position)
{
    sendRemoteEvent(RemoteEvent::Click, position);
}

void Controller::mouseDBClicked(const QPointF &position)
{
    sendRemoteEvent(RemoteEvent::DoubleClick, position);
}

void Controller::requestNewConnection(const QString &address)
{
    QHostAddress hostAddress(address);
    if (!hostAddress.isNull() && !hostAddress.isLoopback())
    {
        QMetaObject::invokeMethod(m_socket, "abort");
        QMetaObject::invokeMethod(m_socket, "connectTo", Q_ARG(QHostAddress, hostAddress), Q_ARG(quint16, 43800));
    }
}

void Controller::sendRemoteEvent(RemoteEvent::EventType type, const QPointF &position)
{
    RemoteEvent event(type, position);
    QMetaObject::invokeMethod(m_socket, "writeToSocket", Q_ARG(RemoteEvent, event));
}
