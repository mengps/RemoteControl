#include "controller.h"
#include "imageprovider.h"
#include "socket.h"

#include <QGuiApplication>
#include <QHostAddress>
#include <QTcpSocket>
#include <QThread>

Controller::Controller(QObject *parent)
    : QObject(parent)
{
    m_provider = new ImageProvider;

    m_socket = new Socket;
#ifndef USE_TCP   //UDP
    m_socket->bind(QHostAddress::Any, 43800);
#endif
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
#ifdef USE_TCP
    QMetaObject::invokeMethod(m_socket, "abort");
    QMetaObject::invokeMethod(m_socket, "connectTo", Q_ARG(QString, address), Q_ARG(quint16, 43800));
#else
    QMetaObject::invokeMethod(m_socket, "setDestAddr", Q_ARG(QHostAddress, QHostAddress(address)));
#endif
}

void Controller::sendRemoteEvent(RemoteEvent::EventType type, const QPointF &position)
{
    RemoteEvent event(type, position);
    QMetaObject::invokeMethod(m_socket, "writeToSocket", Q_ARG(RemoteEvent, event));
}
