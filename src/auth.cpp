#include "auth.hpp"

#include <print>

#include <QJsonDocument>

#include <QCoroNetworkReply>

#include "qttm.hpp"

namespace qttm
{

Authorization::Authorization(RequestQueue& requestQueue,
    const QByteArray&                      userAgent,
    const std::optional<NadeoCredentials>& nadeoCredentials,
    const std::optional<OAuthCredentials>& oAuthCredentials)
    : m_requestQueue(requestQueue)
    , m_userAgent(userAgent)
    , m_nadeoCredentials(nadeoCredentials)
    , m_oAuthCredentials(oAuthCredentials)
{
}

QCoro::Task<QByteArray> Authorization::nadeoServicesToken()
{
    if (QDateTime::currentDateTime() < m_nadeoServicesTokenExpirationTime)
    {
        co_return QByteArray{m_nadeoServicesAccessToken};
    }

    QByteArray ubiTicket          = co_await getUbiTicket();
    QByteArray nadeoServicesToken = co_await getNadeoServicesToken(ubiTicket);
    co_return nadeoServicesToken;
}

QCoro::Task<QByteArray> Authorization::nadeoLiveServicesToken()
{
    if (QDateTime::currentDateTime() < m_nadeoLiveServicesTokenExpirationTime)
    {
        co_return QByteArray{m_nadeoLiveServicesAccessToken};
    }

    QByteArray ubiTicket              = co_await getUbiTicket();
    QByteArray nadeoLiveServicesToken = co_await getNadeoLiveServicesToken(ubiTicket);
    co_return nadeoLiveServicesToken;
}

QCoro::Task<QByteArray> Authorization::oAuthToken()
{
    if (QDateTime::currentDateTime() < m_oAuthTokenExpirationTime)
    {
        co_return QByteArray{m_oAuthToken};
    }

    QByteArray oAuthToken = co_await getOAuthToken();
    co_return oAuthToken;
}

RequestQueue& Authorization::requestQueue()
{
    return m_requestQueue;
}

const QByteArray& Authorization::userAgent() const
{
    return m_userAgent;
}

QCoro::Task<QByteArray> Authorization::getUbiTicket()
{
    std::println("Fetching Ubi ticket...");

    auto authorizationString =
        (m_nadeoCredentials->email + QByteArray(":") + m_nadeoCredentials->password).toBase64();

    QNetworkRequest request;
    request.setUrl(QUrl("https://public-ubiservices.ubi.com/v3/profiles/sessions"));
    request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::KnownHeaders::UserAgentHeader, m_userAgent);
    request.setRawHeader("Ubi-AppId", "86263886-327a-4328-ac69-527f0d20a237");
    request.setRawHeader("Authorization", "Basic " + authorizationString);

    QNetworkReply* reply = co_await m_requestQueue.post(std::move(request), "");

    if (reply->error() != QNetworkReply::NetworkError::NoError)
    {
        reply->deleteLater();
        throw NetworkError{"ubiTicket", reply->error(), reply->errorString()};
    }

    auto raw       = reply->readAll();
    auto json      = QJsonDocument::fromJson(raw);
    auto ubiTicket = json["ticket"].toString().toLocal8Bit();

    std::println("  Got Ubi ticket");
    reply->deleteLater();

    co_return ubiTicket;
}

QCoro::Task<QByteArray> Authorization::getNadeoServicesToken(const QByteArray ubiTicket)
{
    std::println("Fetching NadeoServices token...");

    QNetworkRequest request;
    request.setUrl(QUrl("https://prod.trackmania.core.nadeo.online/v2/"
                        "authentication/token/ubiservices"));
    request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::KnownHeaders::UserAgentHeader, m_userAgent);
    request.setRawHeader("Authorization", "ubi_v1 t=" + ubiTicket);

    QNetworkReply* reply =
        co_await m_requestQueue.post(std::move(request), "{\"audience\":\"NadeoServices\"}");

    if (reply->error() != QNetworkReply::NetworkError::NoError)
    {
        reply->deleteLater();
        throw NetworkError{"nadeoServicesToken", reply->error(), reply->errorString()};
    }

    auto raw  = reply->readAll();
    auto json = QJsonDocument::fromJson(raw);

    m_nadeoServicesAccessToken  = json["accessToken"].toString().toLocal8Bit();

    auto payload =
        QJsonDocument::fromJson(QByteArray::fromBase64(m_nadeoServicesAccessToken.split('.')[1]));

    m_nadeoServicesTokenExpirationTime = QDateTime::fromSecsSinceEpoch(payload["exp"].toInt());

    std::println("  Got NadeoServices token");
    reply->deleteLater();

    co_return QByteArray{m_nadeoServicesAccessToken};
}

QCoro::Task<QByteArray> Authorization::getNadeoLiveServicesToken(const QByteArray ubiTicket)
{
    std::println("Fetching NadeoLiveServices token...");

    QNetworkRequest request;
    request.setUrl(QUrl("https://prod.trackmania.core.nadeo.online/v2/"
                        "authentication/token/ubiservices"));
    request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::KnownHeaders::UserAgentHeader, m_userAgent);
    request.setRawHeader("Authorization", "ubi_v1 t=" + ubiTicket);

    QNetworkReply* reply =
        co_await m_requestQueue.post(std::move(request), "{\"audience\":\"NadeoLiveServices\"}");

    if (reply->error() != QNetworkReply::NetworkError::NoError)
    {
        reply->deleteLater();
        throw NetworkError{"nadeoLiveServicesToken", reply->error(), reply->errorString()};
    }

    auto raw  = reply->readAll();
    auto json = QJsonDocument::fromJson(raw);

    m_nadeoLiveServicesAccessToken  = json["accessToken"].toString().toLocal8Bit();
    m_nadeoLiveServicesRefreshToken = json["refreshToken"].toString().toLocal8Bit();

    auto payload = QJsonDocument::fromJson(
        QByteArray::fromBase64(m_nadeoLiveServicesAccessToken.split('.')[1]));

    m_nadeoLiveServicesTokenExpirationTime = QDateTime::fromSecsSinceEpoch(payload["exp"].toInt());
    m_nadeoLiveServicesTokenRefreshTime    = QDateTime::fromSecsSinceEpoch(payload["rat"].toInt());

    std::println("  Got NadeoLiveServices token");
    reply->deleteLater();

    co_return QByteArray{m_nadeoLiveServicesAccessToken};
}

QCoro::Task<QByteArray> Authorization::getOAuthToken()
{
    std::println("Fetching OAuth token...");

    QNetworkRequest request;
    request.setUrl(QUrl("https://api.trackmania.com/api/access_token"));
    request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader,
        "application/x-www-form-urlencoded");
    request.setHeader(QNetworkRequest::KnownHeaders::UserAgentHeader, m_userAgent);

    QNetworkReply* reply = co_await m_requestQueue.post(std::move(request),
        "grant_type=client_credentials&client_id=" + m_oAuthCredentials->identifier
            + "&client_secret=" + m_oAuthCredentials->secret);

    if (reply->error() != QNetworkReply::NetworkError::NoError)
    {
        reply->deleteLater();
        throw NetworkError{"oAuthToken", reply->error(), reply->errorString()};
    }

    auto raw  = reply->readAll();
    auto json = QJsonDocument::fromJson(raw);

    m_oAuthToken               = json["access_token"].toString().toLocal8Bit();
    m_oAuthTokenExpirationTime = QDateTime::currentDateTime().addSecs(json["expires_in"].toInt());

    std::println("  Got OAuth token");
    reply->deleteLater();

    co_return QByteArray{m_oAuthToken};
}

} // namespace tmc
