#include "oauth.hpp"

#include <QJsonDocument>

#include "../../qttm.hpp"

namespace qttm::oauth
{

QCoro::Task<QJsonDocument> getRequest(Authorization& auth, const QString absolutePathReference)
{
    // query the OAuth token if needed
    auto oAuthToken = co_await auth.oAuthToken();

    // create the URL to query
    QUrl url = "https://api.trackmania.com" + absolutePathReference;

    // create the network request
    QNetworkRequest request;
    request.setUrl(url);
    request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::KnownHeaders::UserAgentHeader, auth.userAgent());
    request.setRawHeader("Authorization", "Bearer " + oAuthToken);

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

} // namespace qttm::oauth
