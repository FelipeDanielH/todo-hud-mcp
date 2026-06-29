#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "app/TaskController.h"
#include "app/TaskModel.h"

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("Focus HUD"));
    app.setApplicationVersion(QStringLiteral("0.1.0"));

    TaskModel model;
    TaskController controller(&model);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QStringLiteral("taskController"), &controller);

    const QUrl url(QStringLiteral(SOURCE_DIR) + QStringLiteral("/src/qml/Main.qml"));
    engine.load(url);

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
