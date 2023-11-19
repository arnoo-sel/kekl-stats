#include "maps.hpp"

#include <print>

#include "../../qttm.hpp"

namespace qttm::live::maps
{

QCoro::Task<QJsonDocument> info_multiple(Authorization& auth, const QStringList mapUids)
{
    std::println("Fetching maps details for {} maps", mapUids.size());

    auto json =
        co_await getRequest(auth, "/api/token/map/get-multiple?mapUidList=" + mapUids.join(','));

    std::println("  Found campaign details");

    co_return json;
}

} // namespace qttm::live::maps
