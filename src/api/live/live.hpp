#pragma once

#include "../../auth.hpp"

namespace qttm::live
{

QCoro::Task<QJsonDocument> getRequest(Authorization& auth, const QString absolutePathReference);

QCoro::Task<QJsonDocument> postRequest(
    Authorization& auth,
    const QString  absolutePathReference,
    QByteArray     body);

} // namespace qttm::live
