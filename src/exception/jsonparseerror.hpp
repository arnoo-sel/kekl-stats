#pragma once

#include <QJsonParseError>

#include "qttmexception.hpp"

namespace qttm
{

class JsonParseError : public QtTmException
{
  public:
    void            raise() const override;
    JsonParseError* clone() const override;

    JsonParseError(
        const QString&         context,
        const QJsonParseError& error,
        const QString&         desctription);

    QJsonParseError error() const;

  private:
    QJsonParseError m_error;
};

} // namespace qttm
