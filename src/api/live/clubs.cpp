#include "clubs.hpp"

#include <print>

#include <QJsonDocument>

#include <QCoroNetworkReply>

#include "../../qttm.hpp"

namespace qttm::live::clubs
{

QCoro::AsyncGenerator<QJsonDocument> activities(Authorization& auth, int clubId)
{
    static const int pageLength = 5;
    int              offset     = 0;

    do
    {
        auto json  = co_await activities_page(auth, clubId, offset, pageLength);
        int  count = json["itemCount"].toInt();
        co_yield json;
        offset += pageLength;
        if (offset >= count)
        {
            break;
        }
    } while (true);
}

QCoro::Task<QJsonDocument> activities_page(Authorization& auth, int clubId, int offset, int length)
{
    std::println("Fetching activities [{}, {}] for club #{}...",
        offset + 1,
        offset + length,
        clubId);

    auto json = co_await getRequest(auth,
        "/api/token/club/" + QString::number(clubId) + "/activity?length=" + QString::number(length)
            + "&offset=" + QString::number(offset) + "&active=true");

    std::println("  Found campaigns...");

    co_return json;
}

QCoro::Task<QJsonDocument> campaign(Authorization& auth, int clubId, int campaignId)
{
    std::println("Fetching campaign details for campaign #{}...", campaignId);

    auto json = co_await getRequest(auth,
        "/api/token/club/" + QString::number(clubId) + "/campaign/" + QString::number(campaignId));

    std::println("  Found campaign details");

    co_return json;
}

} // namespace qttm::live::clubs
