#include "leaderboards.hpp"

#include <print>

#include "../../qttm.hpp"

namespace qttm::live::leaderboards
{

QCoro::AsyncGenerator<QJsonDocument> top(Authorization& auth, const QString mapUid)
{
    static const int pageLength = 100;
    int              offset     = 0;

    do
    {
        QJsonDocument json  = co_await top_page(auth, mapUid, offset, pageLength);
        int           count = json["tops"].toArray().first()["top"].toArray().size();
        co_yield json;
        offset += pageLength;
        if (count < pageLength)
        {
            break;
        }
    } while (true);
}

QCoro::Task<QJsonDocument>
top_page(Authorization& auth, const QString mapUid, int offset, int length)
{
    std::println("Fetching leaderboard positions [{}, {}] for map #{}...",
        offset + 1,
        offset + length,
        mapUid);

    auto json = co_await getRequest(auth,
        "/api/token/leaderboard/group/Personal_Best/map/" + mapUid + "/top?length="
            + QString::number(length) + "&onlyWorld=true&offset=" + QString::number(offset));

    std::println("  Found leaderboard");

    co_return json;
}

} // namespace qttm::live::leaderboards
