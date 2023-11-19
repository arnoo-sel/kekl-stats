#pragma once

#include <QByteArray>

#include "requestqueue.hpp"

namespace qttm
{

struct NadeoCredentials
{
    QByteArray email;
    QByteArray password;
};

struct OAuthCredentials
{
    QByteArray identifier;
    QByteArray secret;
};

class Authorization
{
  public:
    Authorization(RequestQueue&                requestQueue,
        const QByteArray&                      userAgent,
        const std::optional<NadeoCredentials>& nadeoCredentials,
        const std::optional<OAuthCredentials>& oAuthCredentials);

    QCoro::Task<QByteArray> nadeoServicesToken();
    QCoro::Task<QByteArray> nadeoLiveServicesToken();
    QCoro::Task<QByteArray> oAuthToken();

    RequestQueue&     requestQueue();
    const QByteArray& userAgent() const;

  private:
    QCoro::Task<QByteArray> getUbiTicket();
    QCoro::Task<QByteArray> getNadeoServicesToken(const QByteArray ubiTicket);
    QCoro::Task<QByteArray> getNadeoLiveServicesToken(const QByteArray ubiTicket);
    QCoro::Task<QByteArray> getOAuthToken();

    RequestQueue&                   m_requestQueue;
    QByteArray                      m_userAgent;
    std::optional<NadeoCredentials> m_nadeoCredentials;
    std::optional<OAuthCredentials> m_oAuthCredentials;

    QByteArray m_nadeoLiveServicesAccessToken;
    QByteArray m_nadeoLiveServicesRefreshToken;
    QDateTime  m_nadeoLiveServicesTokenExpirationTime;
    QDateTime  m_nadeoLiveServicesTokenRefreshTime;

    QByteArray m_nadeoServicesAccessToken;
    QDateTime  m_nadeoServicesTokenExpirationTime;

    QByteArray m_oAuthToken;
    QDateTime  m_oAuthTokenExpirationTime;
};

} // namespace qttm
