#include "utils.hpp"

#include <QRegularExpression>

namespace qttm
{

QString utils::tmStringToPlainText(QString tmString)
{
    // remove color codes
    static const auto r0 = QRegularExpression("\\$[0-9a-fA-F]{3}");
    tmString.remove(r0);

    // remove links
    static const auto r1 = QRegularExpression("\\$[lL]\\[.*\\]");
    tmString.remove(r1);

    // remove formatting
    static const auto r2 = QRegularExpression("\\$[oOiIwWnNtTsSgGzZlL]");
    tmString.remove(r2);

    // replace '$$' with single '$' sign
    tmString.replace("$$", "$");

    // attempts to remove as much unicode weirdness as possible
    tmString = tmString.normalized(QString::NormalizationForm::NormalizationForm_KC);
    tmString = tmString.simplified();

    return tmString;
}

} // namespace qttm
