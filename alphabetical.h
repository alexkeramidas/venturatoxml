#ifndef ALPHABETICAL_H
#define ALPHABETICAL_H

#include <QString>
#include <QMap>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

class Alphabetical
{
public:
    Alphabetical();
    void OpenAlphabeticalSourceFile();
private:
    /* Variable declarations */
    QString _alphabeticalFilename,aid3,aid4,aid5;
    QXmlStreamReader alphabeticalXMLReader;
    QXmlStreamWriter alphabeticalFinalXMLWriter;
    QString alphabeticalTextFilePath, alphabeticalImageListFilePath;
    QMap<QString, QString> imageMap, imageFilesList;

    /* Function Declarations */
    void CreateAlphabeticalXML(QString alphabeticalTextFilePath);
    void processAlphabeticalEntries();
    void processAlphabeticalEntry();
    void singlePhone(QString singlePhone);
    void streetAndPhone(QString element, QString odos, QString tilefono);
    void nameStreetAndPhone(QString element, QString odos, QString tilefono);
    QString removeMobileIndicator(QString singlePhone);
    QString readNextAlphabeticalText();
};

#endif // ALPHABETICAL_H
