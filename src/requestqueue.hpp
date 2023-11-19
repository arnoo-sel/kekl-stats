#pragma once

#include <queue>

#include <QNetworkReply>
#include <QPromise>
#include <QTimer>

#include <QCoroTask>

namespace qttm
{
class RequestQueue
{
  public:
    RequestQueue(QNetworkAccessManager& networkAccessManager, std::chrono::milliseconds limit);

    QCoro::Task<QNetworkReply*> get(QNetworkRequest&& request);
    QCoro::Task<QNetworkReply*> post(QNetworkRequest&& request, const QByteArray body);

  private:
    QNetworkAccessManager& m_networkAccessManager;
    QTimer                 m_timer;
};
} // namespace qttm
