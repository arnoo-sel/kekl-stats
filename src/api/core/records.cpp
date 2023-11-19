#include "records.hpp"

#include <print>

#include <QJsonDocument>

#include "../../qttm.hpp"

namespace qttm::core::records
{

QCoro::Task<QJsonDocument>
map_records(Authorization& auth, const QStringList accountIds, const QStringList mapIds)
{
    auto accountIdsConcat = accountIds.join(',');
    auto mapIdsConcat     = mapIds.join(',');

    std::println("Fetching records for accounts: [{}] on maps [{}]...",
        accountIdsConcat,
        mapIdsConcat);

    auto json = co_await getRequest(auth,
        "/mapRecords/?accountIdList=" + accountIdsConcat + "&mapIdList=" + mapIdsConcat);

    std::println("  Found records");

    co_return json;
}

QCoro::Task<QJsonDocument> map_record(Authorization& auth, const QString mapRecordId)
{
    std::println("Fetching record {}...", mapRecordId);

    auto json = co_await getRequest(auth, "/mapRecords/" + mapRecordId);

    std::println("  Found record");

    co_return json;
}

} // namespace qttm::core::records
