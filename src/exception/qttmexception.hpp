#pragma once

#include <QException>

namespace qttm
{

class QtTmException : public QException
{
  public:
    QtTmException(const QString& context, const QString& description);

    const QString& context() const;
    const QString& description() const;

  private:
    QString m_context;
    QString m_description;
};

} // namespace qttm
