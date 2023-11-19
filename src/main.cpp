#include <print>

#include <QCoreApplication>
#include <QJsonArray>
#include <QJsonObject>

#include "examples.hpp"
#include "qttm.hpp"

QCoro::Task<> run(qttm::Authorization& auth)
{
    try
    {
        {
            // Example 1: "handcrafted" request to get a player's club tag
            //
            // This example uses the Core/Accounts/Get-Player-Club-Tags endpoint.
            // See https://webservices.openplanet.dev/core/accounts/club-tags
            //
            // This endpoint uses the "Core" part of the API, and is done with a GET request.
            // We therefore use qttm::core::getRequest to do the request.
            // - the 1st argument is always the Authorization object
            // - the 2nd argument is the path+query part of the URL (anything after the domain name)
            //
            // If no error occurs, the response is return as a JSON object. Otherwise an exception
            // is thrown.
            auto json = co_await qttm::core::getRequest(auth,
                "/accounts/clubTags/?accountIdList=45c9afc8-7b34-47df-b35a-6ac34f6037cd");

            // We can now dig into the json object, referencing the documentation to find what we
            // need. 'json' is an object of type QJsonDocument. See
            // https://doc.qt.io/qt-6/qjsondocument.html
            auto clubTag = json.array().at(0)["clubTag"].toString();
            std::println("arnoosel's club tag is: {}", clubTag);

            // qttm::utils::tmStringToPlainText() cleans-up a tm-formatted string to be more easily
            // readable in plain-text.
            std::println("arnoosel's club tag is: {}", qttm::utils::tmStringToPlainText(clubTag));
        }
        //==========================================================================================

        {
            // Example 2: request a campaign's info using a helper function
            //
            // This example uses the Live/Clubs/Get-Club-Campaign-By-Id endpoint.
            // See https://webservices.openplanet.dev/live/clubs/campaign-by-id
            //
            // We could handcraft the request just like in example 1. But I've written a helper
            // function for this endpoint. All it really does is take care of putting the request
            // URL together correctly.
            //
            // The helper function is qttm::live::clubs::campaign
            // - the 1st argument is always the Authorization object
            // - the 2nd argument is the club id
            // - the 3rd argument is the campaign id
            //
            // If no error occurs, the response is return as a JSON object. Otherwise an exception
            // is thrown.
            auto json = co_await qttm::live::clubs::campaign(auth, 43173, 30858);

            std::println("There are {} maps in the {} campaign",
                json["mapsCount"].toInt(),
                json["name"].toString());
        }
        //==========================================================================================

        {
            // Example 3: querying a map leaderboard
            //
            // Some API endpoint return "paged" result. That is: we have to request only so many
            // results in a single request, then do another request to get the next results.
            //
            // One such endpoint is Live/Leaderboards/Get-Map-Leaderboards.
            // See https://webservices.openplanet.dev/live/leaderboards/top
            //
            // We can use the qttm::live::leaderboards::top_page helper function to request a single
            // page at a time.
            // But we can also use the qttm::live::leaderboards::top helper function which takes
            // care of performing as many requests as needed to get all the results.
            //
            // Those functions that returns multiple results must be use in a different way:
            std::println("All personal bests on \"FULL YUMPS\":");

            QCORO_FOREACH(const auto json,
                qttm::live::leaderboards::top(auth, "svVV5PfsXC_mhc_6iP_UmIkMMgl"))
            {
                // We will enter this block for each page of result.
                auto top = json["tops"].toArray().first()["top"].toArray();
                for (const auto pbRef : top)
                {
                    auto pb = pbRef.toObject();
                    std::println("  {:3}  {}ms", pb["position"].toInt(), pb["score"].toInt());
                }
            }

            std::println("Done fetching personal bests");
        }

        //==========================================================================================

        // Example 4: KEKL "Maps Created" leaderboard
        //
        // This function combines a bunch of different requests to compute how many KEKL maps
        // have been built my every KEKL mapper.
        // The function is in examples.cpp

        // Uncomment the following line to run the example (it takes a couple minutes):
        // co_await keklMapCountPerMapper(auth);

        //==========================================================================================

        // Example 5: Players PB on KEKL Maps
        //
        // This examples queries all the PBs of Lars and Scrapie on all KEKL maps.
        // The function is in examples.cpp

        // Uncomment the following lines to run the example (it takes a couple minutes):
        // co_await keklMapPlayedByPlayers(auth,
        //     {"da4642f9-6acf-43fe-88b6-b120ff1308ba", "e3ff2309-bc24-414a-b9f1-81954236c34b"});

        //==========================================================================================

        {
            const auto json = co_await qttm::live::leaderboards::trophies(auth,
                {"45c9afc8-7b34-47df-b35a-6ac34f6037cd"});

            std::println("{}", json.toJson());
        }

        qApp->exit();
    }
    catch (const qttm::JsonParseError& e)
    {
        std::println("A JSON parsing error occured:");
        std::println("  - context: {}", e.context());
        std::println("  - description: {}", e.description());
        qApp->exit(-1);
    }
    catch (const qttm::NetworkError& e)
    {
        std::println("A network error occured:");
        std::println("  - context: {}", e.context());
        std::println("  - status: {}", std::to_underlying(e.error()));
        std::println("  - description: {}", e.description());
        qApp->exit(-1);
    }
}

int main(int argc, char* argv[])
{
    QCoreApplication      app(argc, argv);
    QNetworkAccessManager networkAccessManager;

    // The 2nd argument determines how spaced out are the requests to Nadeo services.
    // Their documentation does not specify a specific rate limit.
    qttm::RequestQueue requestQueue(networkAccessManager, std::chrono::milliseconds(500));

    // Object that handles authentication and authorization to Nadeo services.
    // 2nd argument: user-agent string to use for the requests to Nadeo. Must provide a way to
    //               contact the application author. See https://webservices.openplanet.dev/auth
    // 3rd argument: ubisoft credentials. See https://webservices.openplanet.dev/auth
    // 4th argument: oauth-app credentials. Only needed to request actual player-names from account
    //               ids. See https://webservices.openplanet.dev/oauth/auth. Use "std::nullopt"
    //               instead of the OAuthCredentials object if you do not require OAuth.
#error Set your credentials below then remove this line.
    qttm::Authorization auth(requestQueue,
        "app-name / username / contact-email",
        qttm::NadeoCredentials{"ubisoft-email", "ubisoft-password"},
        qttm::OAuthCredentials{"app-identifier", "app-secret"});

    run(auth);
    return app.exec();
}
