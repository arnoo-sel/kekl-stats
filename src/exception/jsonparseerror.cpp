#include "jsonparseerror.hpp"

namespace qttm
{

void JsonParseError::raise() const
{
    throw *this;
}

JsonParseError* JsonParseError::clone() const
{
    return new JsonParseError(*this);
}

JsonParseError::JsonParseError(
    const QString&         context,
    const QJsonParseError& error,
    const QString&         desctription)
    : QtTmException(context, desctription)
    , m_error(error)
{
}

QJsonParseError JsonParseError::error() const
{
    return m_error;
}

} // namespace qttm
