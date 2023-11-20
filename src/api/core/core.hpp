#pragma once

#include "../../auth.hpp"

namespace qttm::core
{

QCoro::Task<QJsonDocument> getRequest(Authorization& auth, const QString absolutePathReference);

QCoro::Task<QJsonDocument> postRequest(
    Authorization& auth,
    const QString  absolutePathReference,
    QByteArray     body);

} // namespace qttm::core
