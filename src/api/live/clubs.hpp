#pragma once

#include <QCoroAsyncGenerator>

#include "../../auth.hpp"

namespace qttm::live::clubs
{

QCoro::AsyncGenerator<QJsonDocument> activities(Authorization& auth, int clubId);
QCoro::Task<QJsonDocument> activities_page(Authorization& auth, int clubId, int offset, int length);
QCoro::Task<QJsonDocument> campaign(Authorization& auth, int clubId, int campaignId);

} // namespace qttm::live::clubs
