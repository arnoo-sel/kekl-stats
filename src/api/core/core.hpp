#pragma once

#include "../../auth.hpp"

namespace qttm::core
{

QCoro::Task<QJsonDocument> getRequest(Authorization& auth, const QString absolutePathReference);

} // namespace qttm::core
