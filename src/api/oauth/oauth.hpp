#pragma once

#include "../../auth.hpp"

namespace qttm::oauth
{

QCoro::Task<QJsonDocument> getRequest(Authorization& auth, const QString absolutePathReference);

} // namespace qttm::oauth
