#include "qttmexception.hpp"

namespace qttm
{

QtTmException::QtTmException(const QString& context, const QString& description)
    : m_context(context)
    , m_description(description)
{
}

const QString& QtTmException::context() const
{
    return m_context;
}

const QString& QtTmException::description() const
{
    return m_description;
}

} // namespace qttm
