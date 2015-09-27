#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <QString>
#include <QStringList>

class StringUtils
{
public:
    /* Variable declarations */

    /* Function Declarations */
    StringUtils();
    QString removeAccents(QString s);

private:
    /* Variable declarations */
    QString  diacriticLetters_;
    QStringList noDiacriticLetters_;

    /* Function Declarations */


};

#endif // STRINGUTILS_H
