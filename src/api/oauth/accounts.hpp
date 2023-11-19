#pragma once

#include "../../auth.hpp"

namespace qttm::oauth::accounts
{

QCoro::Task<QJsonDocument> display_names(Authorization& auth, QStringList authorIds);

} // namespace qttm::oauth::accounts
