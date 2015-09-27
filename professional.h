#ifndef PROFESSIONAL_H
#define PROFESSIONAL_H

#include <QString>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

class Professional
{
public:
    Professional();
    void OpenProfessionalSourceFile();

private:
    /* Variable declarations */
    QString _professionalFilename;
    QString aid3;
    QXmlStreamReader professionalXMLReader;
    QXmlStreamWriter professionalFinalXMLWriter;
    QString categoriesPath;

    /* Function Declarations */
    void CreateProfessionalXML(QString categoriesPath);
    void processProfessionalEntries();
    void processProfessionalEntry();
    void writeCategoryImageFiles(QString katigoria);
    QString readNextProfessionalText();
    QString greekUpperCase(const QString &str);
};

#endif // PROFESSIONAL_H
