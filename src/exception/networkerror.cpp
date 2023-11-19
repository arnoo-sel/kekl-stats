#include "networkerror.hpp"

namespace qttm
{

void NetworkError::raise() const
{
    throw *this;
}

NetworkError* NetworkError::clone() const
{
    return new NetworkError(*this);
}

NetworkError::NetworkError(const QString& context,
    QNetworkReply::NetworkError           error,
    const QString&                        desctription)
    : QtTmException(context, desctription)
    , m_error(error)
{
}

QNetworkReply::NetworkError NetworkError::error() const
{
    return m_error;
}

} // namespace qttm
