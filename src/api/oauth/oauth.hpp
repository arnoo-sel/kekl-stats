#pragma once

#include "../../auth.hpp"

namespace qttm::oauth
{

QCoro::Task<QJsonDocument> getRequest(Authorization& auth, const QString absolutePathReference);
QCoro::Task<QJsonDocument>
postRequest(Authorization& auth, const QString absolutePathReference, QByteArray body);

} // namespace qttm::oauth
