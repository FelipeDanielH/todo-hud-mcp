#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QVariant>
#include <QDebug>
#include <QEvent>
#include <QObject>
#include <QProcessEnvironment>
#include <QQuickWindow>
#include <QTimer>
#include <QUrl>
#include "infrastructure/ApiTaskRepository.h"
#include "infrastructure/InMemoryTaskRepository.h"
#include "infrastructure/WebSocketClient.h"
#include "application/TaskService.h"
#include "presentation/TaskListModel.h"
#include "presentation/FocusTimerController.h"
#include "presentation/AppController.h"

#ifdef Q_OS_WIN
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

static QUrl deriveWsUrl(const QString& apiUrl)
{
    QUrl url(apiUrl);
    url.setScheme(url.scheme() == QStringLiteral("https") ? QStringLiteral("wss") : QStringLiteral("ws"));
    url.setPath(QStringLiteral("/ws/tasks"));
    url.setQuery(QString());
    url.setFragment(QString());
    return url;
}

class AlwaysOnTopEnforcer : public QObject {
public:
    explicit AlwaysOnTopEnforcer(QWindow* window)
        : QObject(window)
        , m_window(window)
    {
        m_window->installEventFilter(this);
        enforce();
    }

protected:
    bool eventFilter(QObject* watched, QEvent* event) override
    {
        if (watched != m_window)
            return QObject::eventFilter(watched, event);

        switch (event->type()) {
        case QEvent::Show:
        case QEvent::WindowActivate:
        case QEvent::WindowDeactivate:
        case QEvent::FocusOut:
        case QEvent::WindowStateChange:
            scheduleEnforce();
            break;
        default:
            break;
        }

        return QObject::eventFilter(watched, event);
    }

private:
    void scheduleEnforce()
    {
        QTimer::singleShot(0, this, [this] { enforce(); });
        QTimer::singleShot(75, this, [this] { enforce(); });
        QTimer::singleShot(200, this, [this] { enforce(); });
    }

    void enforce()
    {
        if (!m_window)
            return;

        m_window->setFlag(Qt::FramelessWindowHint, true);
        m_window->setFlag(Qt::WindowStaysOnTopHint, true);
        m_window->raise();

#ifdef Q_OS_WIN
        const HWND hwnd = reinterpret_cast<HWND>(m_window->winId());
        if (!hwnd)
            return;

        SetWindowPos(hwnd,
                     HWND_TOPMOST,
                     0,
                     0,
                     0,
                     0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE | SWP_SHOWWINDOW);
#endif
    }

    QWindow* m_window = nullptr;
};

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
    if (online) {
        const QUrl wsUrl = deriveWsUrl(apiUrl);
        eventsClient = new WebSocketClient(wsUrl, &app);
    }

    AppController appController(taskService, taskListModel, focusTimer, online, eventsClient);

    QQmlApplicationEngine engine;
    engine.setInitialProperties({
        {QStringLiteral("app"), QVariant::fromValue(&appController)},
    });
    engine.loadFromModule(QStringLiteral("FocusHUD"), QStringLiteral("Main"));

    if (engine.rootObjects().isEmpty())
        return -1;

    if (auto* window = qobject_cast<QQuickWindow*>(engine.rootObjects().first())) {
        new AlwaysOnTopEnforcer(window);
    }

    return app.exec();
}
