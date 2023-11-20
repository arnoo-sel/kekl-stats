#include "leaderboards.hpp"

#include <print>

#include <QJsonArray>
#include <QJsonObject>

#include "../../qttm.hpp"

namespace qttm::live::leaderboards
{

QCoro::AsyncGenerator<QJsonDocument> top(
    Authorization& auth,
    const QString  mapUid,
    const QString  groupUid)
{
    static const int pageLength = 100;
    int              offset     = 0;

    do
    {
        QJsonDocument json  = co_await top_page(auth, mapUid, offset, pageLength, groupUid);
        int           count = json["tops"].toArray().first()["top"].toArray().size();
        co_yield json;
        offset += pageLength;
        if (count < pageLength)
        {
            break;
        }
    } while (true);
}

QCoro::Task<QJsonDocument> top_page(
    Authorization& auth,
    const QString  mapUid,
    int            offset,
    int            length,
    const QString  groupUid)
{
    std::println(
        "Fetching leaderboard positions [{}, {}] for map #{}...",
        offset + 1,
        offset + length,
        mapUid);

    auto json = co_await getRequest(
        auth,
        "/api/token/leaderboard/group/" + groupUid + "/map/" + mapUid + "/top?length="
            + QString::number(length) + "&onlyWorld=true&offset=" + QString::number(offset));

    std::println("  Found leaderboard");

    co_return json;
}

QCoro::Task<QJsonDocument> medals(Authorization& auth, const QString mapUid, const QString groupUid)
{
    std::println("Fetching medal records for map #{}...", mapUid);

    auto json = co_await getRequest(
        auth,
        "/api/token/leaderboard/group/" + groupUid + "/map/" + mapUid + "/medals");

    std::println("  Found medal records");

    co_return json;
}

QCoro::Task<QJsonDocument> surround(
    Authorization& auth,
    const QString  mapUid,
    int            score,
    const QString  groupUid)
{
    std::println("Fetching surrounding records for score {} on map #{}...", score, mapUid);

    auto json = co_await getRequest(
        auth,
        "/api/token/leaderboard/group/" + groupUid + "/map/" + mapUid
            + "/surround/1/1?score=" + QString::number(score));

    std::println("  Found surrounding records");

    co_return json;
}

QCoro::Task<QJsonDocument> trophies(Authorization& auth, const QStringList accountIds)
{
    std::println("Fetching trophy rankings for players [{}]...", accountIds.join(','));

    QJsonArray list;
    for (const auto& accountId : accountIds)
    {
        list.append(QJsonObject{{"accountId", accountId}});
    }

    QJsonObject body;
    body["listPlayer"] = list;

    auto json = co_await postRequest(
        auth,
        "/api/token/leaderboard/trophy/player",
        QJsonDocument(body).toJson(QJsonDocument::JsonFormat::Compact));

    std::println("  Found trophy rankings");

    co_return json;
}

} // namespace qttm::live::leaderboards
