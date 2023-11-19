#pragma once

#include <QCoroAsyncGenerator>

#include "../../auth.hpp"

namespace qttm::live::leaderboards
{

QCoro::AsyncGenerator<QJsonDocument> top(Authorization& auth, const QString mapUid);
QCoro::Task<QJsonDocument>
top_page(Authorization& auth, const QString mapUid, int offset, int length);

} // namespace qttm::live::leaderboards
