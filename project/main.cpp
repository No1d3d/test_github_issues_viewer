#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDir>
#include <QDebug>
#include "IssuesModel.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qDebug() << "Cache directory:" << QDir::current().absolutePath() + "/offline_cache";

    QQmlApplicationEngine engine;

    IssuesModel model;
    engine.rootContext()->setContextProperty("issuesModel", &model);

    engine.load(QUrl("qrc:/qml/Main.qml"));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}