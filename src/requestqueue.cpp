#include "requestqueue.hpp"

#include <QCoroNetworkReply>
#include <QCoroTimer>

namespace qttm
{

RequestQueue::RequestQueue(QNetworkAccessManager& networkAccessManager,
    std::chrono::milliseconds                     limit)
    : m_networkAccessManager(networkAccessManager)
{
    m_timer.setInterval(limit);
    m_timer.setSingleShot(true);
}

QCoro::Task<QNetworkReply*> RequestQueue::get(QNetworkRequest&& request)
{
    if (m_timer.isActive() == false)
    {
        m_timer.start();
        QNetworkReply* reply = co_await m_networkAccessManager.get(request);
        co_return reply;
    }
    else
    {
        do
        {
            co_await m_timer;
            if (m_timer.isActive() == false)
            {
                m_timer.start();
                QNetworkReply* reply = co_await m_networkAccessManager.get(request);
                co_return reply;
            }
        } while (true);
    }
}

QCoro::Task<QNetworkReply*> RequestQueue::post(QNetworkRequest&& request, const QByteArray body)
{
    if (m_timer.isActive() == false)
    {
        m_timer.start();
        QNetworkReply* reply = co_await m_networkAccessManager.post(request, body);
        co_return reply;
    }
    else
    {
        do
        {
            co_await m_timer;
            if (m_timer.isActive() == false)
            {
                m_timer.start();
                QNetworkReply* reply = co_await m_networkAccessManager.post(request, body);
                co_return reply;
            }
        } while (true);
    }
}

} // namespace qttm
