#include "live.hpp"

#include <QJsonDocument>

#include "../../qttm.hpp"

namespace qttm::live
{

QCoro::Task<QJsonDocument> getRequest(Authorization& auth, const QString absolutePathReference)
{
    // query the NadeoLiveServices token if needed
    auto nadeoLiveServicesToken = co_await auth.nadeoLiveServicesToken();

    // create the URL to query
    QUrl url = "https://live-services.trackmania.nadeo.live" + absolutePathReference;

    // create the network request
    QNetworkRequest request;
    request.setUrl(url);
    request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::KnownHeaders::UserAgentHeader, auth.userAgent());
    request.setRawHeader("Authorization", "nadeo_v1 t=" + nadeoLiveServicesToken);

    // queue the network request and wait for completion
    auto reply = co_await auth.requestQueue().get(std::move(request));

    // check for network errors
    if (reply->error() != QNetworkReply::NetworkError::NoError)
    {
        reply->deleteLater();
        throw NetworkError{"GET request: " + url.toString(), reply->error(), reply->errorString()};
    }

    // read the network reply, parse its JSON content
    auto            raw = reply->readAll();
    QJsonParseError jsonError;
    auto            json = QJsonDocument::fromJson(raw, &jsonError);
    if (jsonError.error != QJsonParseError::ParseError::NoError)
    {
        reply->deleteLater();
        throw JsonParseError("GET request: " + url.toString(), jsonError, jsonError.errorString());
    }

    // done. clean-up and return the JSON object
    reply->deleteLater();
    co_return json;
}

} // namespace qttm::live
