#pragma once

#include "../../auth.hpp"

namespace qttm::core::accounts
{

QCoro::Task<QJsonDocument> club_tags(Authorization& auth, const QStringList accountIds);
QCoro::Task<QJsonDocument> zones(Authorization& auth, const QStringList accountIds);

} // namespace qttm::core::accounts
