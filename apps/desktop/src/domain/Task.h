#pragma once
#include <QString>

struct Task {
    int id = 0;
    QString apiId;
    QString title;
    bool completed = false;
    QString status = QStringLiteral("pending");
    QString phaseId;
    QString phaseName;
    QString completedAt;
    QString archivedAt;
    int sortOrder = 0;
};
