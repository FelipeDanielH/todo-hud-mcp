#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "infrastructure/InMemoryTaskRepository.h"
#include "application/TaskService.h"
#include "presentation/TaskListModel.h"
#include "presentation/FocusTimerController.h"
#include "presentation/AppController.h"

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("Focus HUD"));
    app.setApplicationVersion(QStringLiteral("0.1.0"));

    InMemoryTaskRepository repository;
    TaskService taskService(&repository);
    TaskListModel taskListModel(&taskService);
    FocusTimerController focusTimer;
    AppController appController(&taskService, &taskListModel, &focusTimer);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QStringLiteral("app"), &appController);
    engine.loadFromModule(QStringLiteral("FocusHUD"), QStringLiteral("Main"));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
