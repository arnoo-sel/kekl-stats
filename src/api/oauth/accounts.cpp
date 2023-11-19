#include "accounts.hpp"

#include <print>

#include "../../qttm.hpp"

namespace qttm::oauth::accounts
{

QCoro::Task<QJsonDocument> display_names(Authorization& auth, QStringList authorIds)
{
    std::println("Fetching {} names...", authorIds.size());

    for (QString& authorId : authorIds)
    {
        authorId.prepend("accountId[]=");
    }

    auto json = co_await getRequest(auth, "/api/display-names?" + authorIds.join('&'));

    std::println("  Found names");

    co_return json;
}

} // namespace qttm::oauth::accounts
