#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "scanengine.h"
#include "scannode.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    qmlRegisterType<ScanNode>("ScanNode", 1, 0, "ScanNode");
    qmlRegisterType<ScanEngine>("ScanEngine", 1, 0, "ScanEngine");

    QQmlApplicationEngine engine;
    QQmlContext *context = engine.rootContext();

    ScanEngine model;
    context->setContextProperty(QStringLiteral("scanEngine"), &model);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
