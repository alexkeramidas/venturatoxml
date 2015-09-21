#ifndef ALPHABETICAL_H
#define ALPHABETICAL_H

#include <QString>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

class Alphabetical
{
public:
    Alphabetical();
    void OpenAlphabeticalSourceFile(QString samplepath);

private:
    /* Variable declarations */
    QString _alphabeticalFilename;
    QXmlStreamReader alphabeticalXMLReader;
    QXmlStreamWriter alphabeticalFinalXMLWriter;

    /* Function Declarations */
    void CreateAlphabeticalXML(QString samplepath);
    void processAlphabeticalEntries();
    void processAlphabeticalEntry();
    QString readNextAlphabeticalText();
};

#endif // ALPHABETICAL_H
