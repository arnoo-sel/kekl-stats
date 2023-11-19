#pragma once

#include <QCoroAsyncGenerator>

#include "../../auth.hpp"

namespace qttm::live::leaderboards
{

QCoro::AsyncGenerator<QJsonDocument> top(Authorization& auth, const QString mapUid);
QCoro::Task<QJsonDocument>
top_page(Authorization& auth, const QString mapUid, int offset, int length);
QCoro::Task<QJsonDocument> medals(Authorization& auth, const QString mapUid);
QCoro::Task<QJsonDocument> surround(Authorization& auth, const QString mapUid, int score);
QCoro::Task<QJsonDocument> trophies(Authorization& auth, const QStringList accountIds);

} // namespace qttm::live::leaderboards
