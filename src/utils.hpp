#pragma once

#include <format>

#include <QString>

template <>
struct std::formatter<QString> : std::formatter<std::string>
{
    auto format(const QString& str, std::format_context& ctx) const
    {
        return std::formatter<std::string>::format(str.toStdString(), ctx);
    }
};

template <>
struct std::formatter<QByteArray> : std::formatter<std::string>
{
    auto format(const QByteArray& str, std::format_context& ctx) const
    {
        return std::formatter<std::string>::format(str.toStdString(), ctx);
    }
};

namespace qttm::utils
{

// Attempts to clean up a tm-formatted string to be easily readable and searchable
QString tmStringToPlainText(QString tmString);

} // namespace qttm::utils
