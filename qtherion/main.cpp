#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "qtherion.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);

    QTherion qtherion;

    QQmlApplicationEngine engine;
    qmlRegisterSingletonInstance("QTherion", 1, 0, "QTherion", &qtherion);
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
