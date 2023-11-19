#pragma once

#include <QJsonArray>
#include <QJsonDocument> // remove
#include <QJsonObject>

#include "../../auth.hpp"

namespace qttm::core::records
{

QCoro::Task<QJsonDocument>
map_records(Authorization& auth, const QStringList accountIds, const QStringList mapIds);

} // namespace qttm::core::records
