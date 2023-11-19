#pragma once

#include "../../auth.hpp"

namespace qttm::core::records
{

QCoro::Task<QJsonDocument>
map_records(Authorization& auth, const QStringList accountIds, const QStringList mapIds);

QCoro::Task<QJsonDocument> map_record(Authorization& auth, const QString mapRecordId);

} // namespace qttm::core::records
