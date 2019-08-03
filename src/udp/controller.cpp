#include "controller.h"
#include "connection.h"
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
    m_socket->bind(QHostAddress::Any, 43800);
    QThread *thread = new QThread;
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    connect(m_socket, &Socket::hasScreenData, this, [this](const QByteArray &screenData)
    {
        QPixmap pixmap;
        pixmap.loadFromData(screenData);
        m_provider->setPixmap(pixmap);
        emit needUpdate();
    });
    m_socket->moveToThread(thread);
    thread->start();

    m_connection = new Connection(this);
    connect(m_connection, &Connection::connected, this, &Controller::connected);
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
    m_connection->abort();
    m_connection->connectToHost(address, 43801);
    QMetaObject::invokeMethod(m_socket, "setDestAddr", Q_ARG(QHostAddress, QHostAddress(address)));
}

void Controller::sendRemoteEvent(RemoteEvent::EventType type, const QPointF &position)
{
    RemoteEvent event(type, position);
    m_connection->writeToSocket(event);
}
