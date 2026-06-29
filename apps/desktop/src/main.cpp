#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QVariant>
#include <QDebug>
#include <QProcessEnvironment>
#include <QUrl>
#include "infrastructure/ApiTaskRepository.h"
#include "infrastructure/InMemoryTaskRepository.h"
#include "infrastructure/WebSocketClient.h"
#include "application/TaskService.h"
#include "presentation/TaskListModel.h"
#include "presentation/FocusTimerController.h"
#include "presentation/AppController.h"

static QUrl deriveWsUrl(const QString& apiUrl)
{
    QUrl url(apiUrl);
    url.setScheme(url.scheme() == QStringLiteral("https") ? QStringLiteral("wss") : QStringLiteral("ws"));
    url.setPath(QStringLiteral("/ws/tasks"));
    return url;
}

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("Focus HUD"));
    app.setApplicationVersion(QStringLiteral("0.1.0"));
    QQuickStyle::setStyle(QStringLiteral("Basic"));

    const QString apiUrl = QProcessEnvironment::systemEnvironment()
        .value(QStringLiteral("FOCUS_HUD_API_URL"), QStringLiteral("http://localhost:3000"));

    qDebug().noquote() << QStringLiteral("[FocusHUD] FOCUS_HUD_API_URL = %1").arg(apiUrl);

    ApiTaskRepository apiRepo(apiUrl);
    bool online = apiRepo.isConnected();

    TaskRepository* repo = nullptr;
    InMemoryTaskRepository inMemoryRepo;

    if (online) {
        repo = &apiRepo;
        qDebug().noquote() << QStringLiteral("[FocusHUD] Connected to backend API — using ApiTaskRepository");
    } else {
        repo = &inMemoryRepo;
        qDebug().noquote() << QStringLiteral("[FocusHUD] ⚠ BACKEND NOT REACHABLE — falling back to InMemoryTaskRepository (mock/dev mode)");
        qDebug().noquote() << QStringLiteral("[FocusHUD] ⚠ Tasks will NOT persist. Start the backend at %1 and restart this app.").arg(apiUrl);
    }

    TaskService taskService(*repo);
    TaskListModel taskListModel(taskService);
    FocusTimerController focusTimer;

    WebSocketClient* wsClient = nullptr;
    if (online) {
        const QUrl wsUrl = deriveWsUrl(apiUrl);
        wsClient = new WebSocketClient(wsUrl, &app);
    }

    AppController appController(taskService, taskListModel, focusTimer, online, wsClient);

    QQmlApplicationEngine engine;
    engine.setInitialProperties({
        {QStringLiteral("app"), QVariant::fromValue(&appController)},
    });
    engine.loadFromModule(QStringLiteral("FocusHUD"), QStringLiteral("Main"));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
