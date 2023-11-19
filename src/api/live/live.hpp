#pragma once

#include "../../auth.hpp"

namespace qttm::live
{

QCoro::Task<QJsonDocument> getRequest(Authorization& auth, const QString absolutePathReference);

} // namespace qttm::live
