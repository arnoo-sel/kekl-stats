#pragma once

#include <QCoroAsyncGenerator>

#include "../../auth.hpp"

namespace qttm::live::leaderboards
{

QCoro::AsyncGenerator<QJsonDocument> top(
    Authorization& auth,
    const QString  mapUid,
    const QString  groupUid = "Personal_Best");

QCoro::Task<QJsonDocument> top_page(
    Authorization& auth,
    const QString  mapUid,
    int            offset,
    int            length,
    const QString  groupUid = "Personal_Best");

QCoro::Task<QJsonDocument> medals(
    Authorization& auth,
    const QString  mapUid,
    const QString  groupUid = "Personal_Best");

QCoro::Task<QJsonDocument> surround(
    Authorization& auth,
    const QString  mapUid,
    int            score,
    const QString  groupUid = "Personal_Best");

QCoro::Task<QJsonDocument> trophies(Authorization& auth, const QStringList accountIds);

} // namespace qttm::live::leaderboards
