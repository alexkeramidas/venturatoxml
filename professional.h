#ifndef PROFESSIONAL_H
#define PROFESSIONAL_H

#include <QString>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

class Professional
{
public:
    Professional();
    void OpenProfessionalSourceFile(QString samplepath);

private:
    /* Variable declarations */
    QString _professionalFilename;
    QXmlStreamReader professionalXMLReader;
    QXmlStreamWriter professionalFinalXMLWriter;

    /* Function Declarations */
    void CreateProfessionalXML(QString samplepath);
    void processProfessionalEntries();
    void processProfessionalEntry();
    QString readNextProfessionalText();
};

#endif // PROFESSIONAL_H
