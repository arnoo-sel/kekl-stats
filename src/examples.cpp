#include "examples.hpp"

#include <print>

#include <QJsonArray>
#include <QJsonObject>
#include <QTimeZone>

#include "qttm.hpp"

// This will print a list of all KEKL mappers with how many KEKL maps they built
QCoro::Task<> keklMapCountPerMapper(qttm::Authorization& auth)
{
    //==============================================================================================
    // 1st: we want to get all the KEKL campaign ids.
    // To do so, we query all the activities in the KEKL club (clubId:43173),
    // and filter for activities that are campaigns and have the right names.

    // We're going to store the matching campaign ids in this list.
    QList<int> campaignIds;

    // The qttm::live::clubs::activities helper function is going to call the
    // Live/Clubs/Get-Club-Activities endpoint as many times as needed to get all the activities in
    // a club. See https://webservices.openplanet.dev/live/clubs/activities
    QCORO_FOREACH(const auto json, qttm::live::clubs::activities(auth, 43173))
    {
        for (const auto activityRef : json["activityList"].toArray())
        {
            const auto activity = activityRef.toObject();

            auto id   = activity["campaignId"].toInt();
            auto type = activity["activityType"].toString();
            auto name = activity["name"].toString();

            // We only want campaigns
            if (type != "campaign")
            {
                continue;
            }

            // All "KEKL Maps XYZ" campaign contain the following string, so we use it as a filter.
            // The KEKL-Hunt website used the same.
            if (name.contains("$g$z$o") == false)
            {
                continue;
            }

            std::println("    Found campaign: {}", qttm::utils::tmStringToPlainText(name));
            campaignIds.append(id);
        }
    }

    //==============================================================================================
    // 2nd: we want to get the UIDs of all the maps in those campaigns.
    // To do so, we are going to query the campaign info for every campaign id we fetched in step 1.

    // We're going to store the UIDs of all the KEKL maps in this list.
    QStringList mapUids;

    for (int campaignId : campaignIds)
    {
        // The qttm::live::clubs::campaign helper function is going to call the
        // Live/Club/Get-Club-Campaign-By-Id endpoint.
        // See https://webservices.openplanet.dev/live/clubs/campaign-by-id
        const auto json = co_await qttm::live::clubs::campaign(auth, 43173, campaignId);

        for (const auto mapRef : json["campaign"]["playlist"].toArray())
        {
            auto map = mapRef.toObject();

            auto uid = map["mapUid"].toString();

            std::println("    Found map: {}", uid);
            mapUids.append(uid);
        }
    }

    // It should not happen in the KEKL campaigns, but just in case, we remove duplicate map UIDs.
    mapUids.sort();
    {
        auto&& [a, b] = std::ranges::unique(mapUids);
        mapUids.erase(a, b);
    }

    //==============================================================================================
    // 3rd: we want to get the author-id for every map fetched at step 2.
    // To do so, we are going to query the map info for every map uid we fetched in step 2.
    // Luckily, Nadeo allows to to query the info of 100 maps per single request.

    // We're going to store the author-ids in this list.
    QStringList authorIds;

    // We use std::views::chunk to create chunks of 100 map uids from our full list of map uids
    for (const auto& slice : std::views::chunk(mapUids, 100))
    {
        // The qttm::live::maps::info_multiple helper function is going to call the
        // Live/Maps/Get-Map-Info-Multiple endpoint.
        // See https://webservices.openplanet.dev/live/maps/info-multiple
        const auto json =
            co_await qttm::live::maps::info_multiple(auth, slice | std::ranges::to<QList>());

        for (const auto mapRef : json["mapList"].toArray())
        {
            auto map = mapRef.toObject();

            auto authorId = map["author"].toString();
            auto name     = map["name"].toString();

            std::println("\tFound author for map: {}   {}",
                authorId,
                qttm::utils::tmStringToPlainText(name));

            authorIds.append(authorId);
        }
    }

    //==============================================================================================
    // There are 2 things we want to do with the list of author-ids:
    // - find out the player-name associated with each unique author-id
    // - count how many times each unique author-id appears in our list (it'll tell us how many
    //   KEKL maps that author has built)
    //
    // Both those things will require our list of author-ids to be sorted (so all duplicates are
    // next to each others).
    authorIds.sort();

    //==============================================================================================
    // 4th: we want to find out the player-name associated with each unique author-id
    // To do so we are going to create a 2nd list of author-ids containing no duplicates, then we're
    // going to query the player-name for all of those author-ids.

    // We're going to store the unique author-ids in this list.
    QStringList uniqueAuthorIds;
    std::ranges::unique_copy(authorIds, std::back_inserter(uniqueAuthorIds));

    // We going to store the player-names and their associated author-id in this map.
    // The key will be the author-id, the value will be the player-name.
    QMap<QString, QString> namesById;

    // We can only query for 50 player-names at a time, so we use std::views::chunk to create chunks
    // of 50 author-ids from our full list of author-ids
    for (const auto& slice : std::views::chunk(uniqueAuthorIds, 50))
    {
        // The qttm::oauth::accounts::display_names helper function is going to call the
        // OAuth/Accounts/Get-Account-Name endpoint.
        // See https://webservices.openplanet.dev/oauth/reference/accounts/id-to-name
        const auto json =
            co_await qttm::oauth::accounts::display_names(auth, slice | std::ranges::to<QList>());

        const auto jsonObject = json.object();
        for (auto it = jsonObject.constBegin(); it != jsonObject.constEnd(); ++it)
        {
            auto author = it.key();
            auto name   = it.value().toString();

            std::println("\tFound name: {} : {}", author, name);
            namesById.insert(author, name);
        }
    }

    // We've got everything we need from Nadeo. Now we compute the KEKL Maps leaderboard.
    std::println("Nothing more to fetch");
    std::println("Computing...");

    //==============================================================================================
    // 5th: we want to count how many times each unique author-id appears in our full list of
    // author-ids.

    // We'll store the result of our computation in this multi-map.
    // The key will be the number of maps, the value is a pair of strings: the author-id and its
    // associated player-name.
    QMultiMap<int, std::pair<QString, QString>> authorsTally;

    auto slice_begin = authorIds.cbegin();
    while (slice_begin != authorIds.cend())
    {
        auto slice_end = std::upper_bound(slice_begin, authorIds.cend(), *slice_begin);
        authorsTally.insert(std::distance(slice_begin, slice_end),
            {*slice_begin, namesById[*slice_begin]});
        slice_begin = slice_end;
    }

    // Finally, we print the results
    std::println("Printing authors list:");
    for (auto it = authorsTally.cbegin(); it != authorsTally.cend(); ++it)
    {
        std::println("\t{} : {} : {}", it.key(), it.value().first, it.value().second);
    }

    std::println("Done");
}

// This will print all the personal bests set by the specified  players on all KEKL maps
QCoro::Task<> keklMapPlayedByPlayers(qttm::Authorization& auth, QStringList playerIds)
{
    //==============================================================================================
    // 1st: we want to get all the KEKL campaign ids.
    // To do so, we query all the activities in the KEKL club (clubId:43173),
    // and filter for activities that are campaigns and have the right names.

    // We're going to store the matching campaign ids in this list.
    QList<int> campaignIds;

    // The qttm::live::clubs::activities helper function is going to call the
    // Live/Clubs/Get-Club-Activities endpoint as many times as needed to get all the activities in
    // a club. See https://webservices.openplanet.dev/live/clubs/activities
    QCORO_FOREACH(const auto json, qttm::live::clubs::activities(auth, 43173))
    {
        for (const auto activityRef : json["activityList"].toArray())
        {
            const auto activity = activityRef.toObject();

            auto id   = activity["campaignId"].toInt();
            auto type = activity["activityType"].toString();
            auto name = activity["name"].toString();

            // We only want campaigns
            if (type != "campaign")
            {
                continue;
            }

            // All "KEKL Maps XYZ" campaign contain the following string, so we use it as a filter.
            // The KEKL-Hunt website used the same.
            if (name.contains("$g$z$o") == false)
            {
                continue;
            }

            std::println("    Found campaign: {}", qttm::utils::tmStringToPlainText(name));
            campaignIds.append(id);
        }
    }

    //==============================================================================================
    // 2nd: we want to get the UIDs of all the maps in those campaigns.
    // To do so, we are going to query the campaign info for every campaign id we fetched in step 1.

    // We're going to store the UIDs of all the KEKL maps in this list.
    QStringList mapUids;

    for (int campaignId : campaignIds)
    {
        // The qttm::live::clubs::campaign helper function is going to call the
        // Live/Club/Get-Club-Campaign-By-Id endpoint.
        // See https://webservices.openplanet.dev/live/clubs/campaign-by-id
        const auto json = co_await qttm::live::clubs::campaign(auth, 43173, campaignId);

        for (const auto mapRef : json["campaign"]["playlist"].toArray())
        {
            auto map = mapRef.toObject();

            auto uid = map["mapUid"].toString();

            std::println("    Found map: {}", uid);
            mapUids.append(uid);
        }
    }

    // It should not happen in the KEKL campaigns, but just in case, we remove duplicate map UIDs.
    mapUids.sort();
    {
        auto&& [a, b] = std::ranges::unique(mapUids);
        mapUids.erase(a, b);
    }

    //==============================================================================================
    // 3rd: we want to get some more info for every map fetched at step 2:
    // - the map ID (different from the UID), will be needed to query the records on the maps
    // - the map name
    // - the author ID
    // To do so, we are going to query the map info for every map uid we fetched in step 2.
    // Luckily, Nadeo allows to to query the info of 100 maps per single request.

    // We're goign to store the map info we care about in this list
    struct MapDetails
    {
        QString uid;
        QString id;
        QString name;
        QString authorId;
    };
    QList<MapDetails> mapDetails;

    // We use std::views::chunk to create chunks of 100 map uids from our full list of map uids
    for (const auto& slice : std::views::chunk(mapUids, 100))
    {
        // The qttm::live::maps::info_multiple helper function is going to call the
        // Live/Maps/Get-Map-Info-Multiple endpoint.
        // See https://webservices.openplanet.dev/live/maps/info-multiple
        const auto json =
            co_await qttm::live::maps::info_multiple(auth, slice | std::ranges::to<QList>());

        for (const auto mapRef : json["mapList"].toArray())
        {
            const auto map = mapRef.toObject();

            auto authorId = map["author"].toString();
            auto name     = qttm::utils::tmStringToPlainText(map["name"].toString());
            auto mapId    = map["mapId"].toString();
            auto mapUid   = map["uid"].toString();

            std::println("\tFound map details: {} : {} : {} : {}", mapUid, mapId, authorId, name);
            mapDetails.append({mapUid, mapId, name, authorId});
        }
    }

    //==============================================================================================
    // 4th: we want to get the records (ie: personal bests) of the selected players on all the maps.
    // Nadeo allows us to query the records of several players on several maps using a single query.

    // We're going to store the records in this list.
    struct Record
    {
        QString   mapId;
        QString   playerId;
        int       time;
        QDateTime timestamp;
    };
    QList<Record> records;

    // We're going to query the records of up to 50 players on up to 50 maps with each query.
    // We use std::views::chunk to create chunks of 50 player ids from our full list of player ids
    for (const auto& playersSlice : std::views::chunk(playerIds, 50))
    {
        // We use std::views::chunk to create chunks of 50 map ids from our full list of map ids
        for (const auto& mapsSlice : std::views::chunk(mapDetails, 50))
        {
            // The qttm::core::records::maps_records helper function is going to call the
            // Core/Records/Get-Map_records endpoint.
            // See https://webservices.openplanet.dev/core/records/map-records
            const auto json = co_await qttm::core::records::map_records(auth,
                playersSlice | std::ranges::to<QList>(),
                mapsSlice | std::views::transform([](const auto& d) { return d.id; })
                    | std::ranges::to<QList>());

            for (const auto recordRef : json.array())
            {
                const auto record = recordRef.toObject();

                auto mapId     = record["mapId"].toString();
                auto playerId  = record["accountId"].toString();
                auto time      = record["recordScore"]["time"].toInt();
                auto timestamp = QDateTime::fromString(record["timestamp"].toString(), Qt::ISODate);

                std::println("  Found record {} : {} : {} : {}",
                    mapId,
                    playerId,
                    time,
                    timestamp.toString());
                records.append({mapId, playerId, time, timestamp});
            }
        }
    }

    // We sort the records from most recent to oldest.
    std::ranges::sort(records,
        [](const auto& a, const auto& b) { return a.timestamp > b.timestamp; });

    //----------------------------------------------------------------------------------------------
    // 5th: we want to combine the map info we got in step 3 with the records we got in step 4.

    // We're going to store the combined data in this list.
    struct FullRecord
    {
        QString   mapUid;
        QString   nameName;
        QString   authorId;
        QString   playerId;
        int       time;
        QDateTime timestamp;
    };
    QList<FullRecord> fullRecords;

    // For more efficient lookup in the mapDetails list, we sort it first.
    std::ranges::sort(mapDetails, [](const auto& a, const auto& b) { return a.id < b.id; });

    for (const auto& record : records)
    {
        auto it = std::ranges::lower_bound(mapDetails,
            record.mapId,
            std::ranges::less{},
            &MapDetails::id);

        fullRecords.append(
            {it->uid, it->name, it->authorId, record.playerId, record.time, record.timestamp});
    }

    //==============================================================================================
    // 6th: we want to get a list of all author and player ids, so that we can next request their
    //      player-names.

    // We're going to store all the author and player ids in this list.
    QStringList authorAndPlayerIds;

    authorAndPlayerIds.append(playerIds);
    for (const auto& mapDetail : mapDetails)
    {
        authorAndPlayerIds.append(mapDetail.authorId);
    }

    // We remove the duplicate ids
    authorAndPlayerIds.sort();
    {
        auto&& [a, b] = std::ranges::unique(authorAndPlayerIds);
        authorAndPlayerIds.erase(a, b);
    }

    //==============================================================================================
    // 7th: we want to find out the player-name associated with all the author and player ids.

    // We going to store the player-names and their associated author/player-id in this map.
    // The key will be the author/player-id, the value will be the player-name.
    QMap<QString, QString> namesById;

    // we use std::views::chunk to create chunks of 50 ids from our full list of ids
    for (const auto& slice : std::views::chunk(authorAndPlayerIds, 50))
    {
        // The qttm::oauth::accounts::display_names helper function is going to call the
        // OAuth/Accounts/Get-Account-Name endpoint.
        // See https://webservices.openplanet.dev/oauth/reference/accounts/id-to-name
        const auto json =
            co_await qttm::oauth::accounts::display_names(auth, slice | std::ranges::to<QList>());

        const auto jsonObject = json.object();
        for (auto it = jsonObject.constBegin(); it != jsonObject.constEnd(); ++it)
        {
            auto author = it.key();
            auto name   = it.value().toString();

            std::println("\tFound name: {} : {}", author, name);
            namesById.insert(author, name);
        }
    }

    //==============================================================================================
    // We can now display the results in a CSV-compatible format

    auto formatedTime = [](int time) -> QString
    {
        return QString::number(time / 60000) + ":"
             + QString::number((time % 60000) / 1000).rightJustified(2, '0') + "."
             + QString::number(time % 1000).rightJustified(3, '0');
    };

    for (const auto& fullRecord : fullRecords)
    {
        std::println("{},{},{},{},{},{}",
            fullRecord.mapUid,
            QString(fullRecord.nameName).replace(',', ' '),
            namesById[fullRecord.authorId],
            namesById[fullRecord.playerId],
            formatedTime(fullRecord.time),
            fullRecord.timestamp.toTimeZone(QTimeZone::systemTimeZone()).toString());
    }

    std::println("Done.");
}
