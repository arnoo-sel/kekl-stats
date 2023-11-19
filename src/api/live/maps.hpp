#pragma once

#include <QCoroTask>

#include "../../auth.hpp"

namespace qttm::live::maps
{

QCoro::Task<QJsonDocument> info_multiple(Authorization& auth, const QStringList mapUids);

} // namespace qttm::live::maps
