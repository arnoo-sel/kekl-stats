#include "core.hpp"

#include <QJsonDocument>

#include "../../qttm.hpp"

namespace qttm::core
{

QCoro::Task<QJsonDocument> getRequest(Authorization& auth, const QString absolutePathReference)
{
    // query the NadeoServices token if needed
    auto nadeoServicesToken = co_await auth.nadeoServicesToken();

    // create the URL to query
    QUrl url = "https://prod.trackmania.core.nadeo.online" + absolutePathReference;

    // create the network request
    QNetworkRequest request;
    request.setUrl(url);
    request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::KnownHeaders::UserAgentHeader, auth.userAgent());
    request.setRawHeader("Authorization", "nadeo_v1 t=" + nadeoServicesToken);

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

QCoro::Task<QJsonDocument>
postRequest(Authorization& auth, const QString absolutePathReference, QByteArray body)
{
    // query the NadeoServices token if needed
    auto nadeoServicesToken = co_await auth.nadeoServicesToken();

    // create the URL to query
    QUrl url = "https://prod.trackmania.core.nadeo.online" + absolutePathReference;

    // create the network request
    QNetworkRequest request;
    request.setUrl(url);
    request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::KnownHeaders::UserAgentHeader, auth.userAgent());
    request.setRawHeader("Authorization", "nadeo_v1 t=" + nadeoServicesToken);

    // queue the network request and wait for completion
    auto reply = co_await auth.requestQueue().post(std::move(request), std::move(body));

    // check for network errors
    if (reply->error() != QNetworkReply::NetworkError::NoError)
    {
        reply->deleteLater();
        throw NetworkError{"POST request: " + url.toString(), reply->error(), reply->errorString()};
    }

    // read the network reply, parse its JSON content
    auto            raw = reply->readAll();
    QJsonParseError jsonError;
    auto            json = QJsonDocument::fromJson(raw, &jsonError);
    if (jsonError.error != QJsonParseError::ParseError::NoError)
    {
        reply->deleteLater();
        throw JsonParseError("POST request: " + url.toString(), jsonError, jsonError.errorString());
    }

    // done. clean-up and return the JSON object
    reply->deleteLater();
    co_return json;
}

} // namespace qttm::core
