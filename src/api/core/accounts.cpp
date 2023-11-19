#include "accounts.hpp"

#include <print>

#include <QJsonDocument>

#include "../../qttm.hpp"

namespace qttm::core::accounts
{

QCoro::Task<QJsonDocument> club_tags(Authorization& auth, const QStringList accountIds)
{
    auto accountIdsConcat = accountIds.join(',');

    std::println("Fetching club tags for accounts: [{}]...", accountIdsConcat);

    auto json = co_await getRequest(auth, "/accounts/clubTags/?accountIdList=" + accountIdsConcat);

    std::println("  Found club tags");

    co_return json;
}

QCoro::Task<QJsonDocument> zones(Authorization& auth, const QStringList accountIds)
{
    auto accountIdsConcat = accountIds.join(',');

    std::println("Fetching zones for accounts: [{}]...", accountIdsConcat);

    auto json = co_await getRequest(auth, "/accounts/zones/?accountIdList=" + accountIdsConcat);

    std::println("  Found zones");

    co_return json;
}

} // namespace qttm::core::accounts
