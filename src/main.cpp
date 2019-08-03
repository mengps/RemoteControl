#include "controlled.h"
#include "controller.h"
#include "imageprovider.h"
#include "protocol.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

#ifdef USE_TCP
    qRegisterMetaType<DataBlock>("DataBlock");
#endif
    qRegisterMetaType<QHostAddress>("QHostAddress");
    qRegisterMetaType<RemoteEvent>("RemoteEvent");

    Controlled *controlled = new Controlled;
    Controller *controller = new Controller;

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("controlled", controlled);
    engine.rootContext()->setContextProperty("controller", controller);
    engine.addImageProvider(QLatin1String("screen"), controller->getImageProvider());
    engine.load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
