#pragma once

#include <QCoroTask>

#include "../../auth.hpp"

namespace qttm::live::maps
{

QCoro::Task<QJsonDocument> info_multiple(Authorization& auth, const QStringList mapUids);

QCoro::Task<QJsonDocument> info(Authorization& auth, const QString mapUid);

} // namespace qttm::live::maps
