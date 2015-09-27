#ifndef ALPHABETICAL_H
#define ALPHABETICAL_H

#include <QString>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

class Alphabetical
{
public:
    Alphabetical();
    void OpenAlphabeticalSourceFile();

private:
    /* Variable declarations */
    QString _alphabeticalFilename;
    QXmlStreamReader alphabeticalXMLReader;
    QXmlStreamWriter alphabeticalFinalXMLWriter;
    QString alphabeticalPath;

    /* Function Declarations */
    void CreateAlphabeticalXML(QString alphabeticalPath);
    void processAlphabeticalEntries();
    void processAlphabeticalEntry();
    QString readNextAlphabeticalText();
};

#endif // ALPHABETICAL_H
