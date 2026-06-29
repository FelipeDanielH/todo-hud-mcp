#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QVariant>
#include <QDebug>
#include <QProcessEnvironment>
#include "infrastructure/ApiTaskRepository.h"
#include "infrastructure/InMemoryTaskRepository.h"
#if FOCUSHUD_HAS_QT_WEBSOCKETS
#include <QUrl>
#include "infrastructure/WebSocketClient.h"
#endif
#include "application/TaskService.h"
#include "presentation/TaskListModel.h"
#include "presentation/FocusTimerController.h"
#include "presentation/AppController.h"

#if FOCUSHUD_HAS_QT_WEBSOCKETS
static QUrl deriveWsUrl(const QString& apiUrl)
{
    QUrl url(apiUrl);
    url.setScheme(url.scheme() == QStringLiteral("https") ? QStringLiteral("wss") : QStringLiteral("ws"));
    url.setPath(QStringLiteral("/ws/tasks"));
    return url;
}
#endif

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

    TaskEventsClient* eventsClient = nullptr;
#if FOCUSHUD_HAS_QT_WEBSOCKETS
    if (online) {
        const QUrl wsUrl = deriveWsUrl(apiUrl);
        eventsClient = new WebSocketClient(wsUrl, &app);
    }
#else
    if (online) {
        qDebug().noquote() << QStringLiteral("[FocusHUD] Qt WebSockets module not available — live sync disabled, manual refresh remains available");
    }
#endif

    AppController appController(taskService, taskListModel, focusTimer, online, eventsClient);

    QQmlApplicationEngine engine;
    engine.setInitialProperties({
        {QStringLiteral("app"), QVariant::fromValue(&appController)},
    });
    engine.loadFromModule(QStringLiteral("FocusHUD"), QStringLiteral("Main"));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
