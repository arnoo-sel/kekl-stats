#include "maps.hpp"

#include <print>

#include "../../qttm.hpp"

namespace qttm::live::maps
{

QCoro::Task<QJsonDocument> info_multiple(Authorization& auth, const QStringList mapUids)
{
    std::println("Fetching maps info for {} maps...", mapUids.size());

    auto json =
        co_await getRequest(auth, "/api/token/map/get-multiple?mapUidList=" + mapUids.join(','));

    std::println("  Found maps info");

    co_return json;
}

QCoro::Task<QJsonDocument> info(Authorization& auth, const QString mapUid)
{
    std::println("Fetching map info for map {}...", mapUid);

    auto json = co_await getRequest(auth, "/api/token/map/" + mapUid);

    std::println("  Found map info");

    co_return json;
}

} // namespace qttm::live::maps
