#pragma once

#include <QNetworkReply>

#include "qttmexception.hpp"

namespace qttm
{

class NetworkError : public QtTmException
{
  public:
    void          raise() const override;
    NetworkError* clone() const override;

    NetworkError(const QString&     context,
        QNetworkReply::NetworkError error,
        const QString&              desctription);

    QNetworkReply::NetworkError error() const;

  private:
    QNetworkReply::NetworkError m_error;
};

} // namespace qttm
