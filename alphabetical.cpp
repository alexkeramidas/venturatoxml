#include "alphabetical.h"

#include <QFile>
#include <QStringList>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QDirIterator>
#include <QStandardPaths>
#include <QFileInfo>

Alphabetical::Alphabetical()
{

}

void Alphabetical::OpenAlphabeticalSourceFile()
{
    QString tempElement, tempText;
    QString fileName = QFileDialog::getOpenFileName(0, QWidget::tr("Open Alphabetical Text"), QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).at(0) , QWidget::tr("Text files (*.txt)"));
    QByteArray line = NULL;
    QFile inFile(fileName);
    QFileInfo inFileInfo(inFile);
    alphabeticalPath  = inFileInfo.dir().path();
    if (!inFile.open(QIODevice::ReadOnly)) {
        qDebug() << inFile.errorString();
        QMessageBox msgBox;
        msgBox.setText("Δεν έχετε επιλέξει αρχείο\nή το αρχείο είναι κατεστραμμένο!");
        msgBox.exec();
    }else{
        QFile outFile(alphabeticalPath + "/alfavitikos_source_sample.xml");
        outFile.open(QIODevice::WriteOnly);
        QXmlStreamWriter xmlWriter(&outFile);
        xmlWriter.setAutoFormatting(false);
        xmlWriter.writeStartDocument("1.0", true);
        xmlWriter.writeStartElement("ROOT");
        xmlWriter.writeStartElement("entries");
        int counter = 0;
        while (!inFile.atEnd()) {
            line = inFile.readLine();
            tempText = line;
            tempElement = line.remove(0,1);
            tempElement = tempElement.remove(QRegExp("=.*"));
            tempText = tempText.remove(QRegExp("@.*="));
            if((tempElement == "ONOMA" || tempElement == "ONOMA-B")  && counter != 0){
                xmlWriter.writeEndElement();
                xmlWriter.writeCharacters("\n");
                xmlWriter.writeStartElement("entry");
                xmlWriter.writeTextElement(tempElement,tempText.simplified());
            } else if(tempElement == "ONOMA" || tempElement == "ONOMA-B") {
                xmlWriter.writeStartElement("entry");
                xmlWriter.writeTextElement(tempElement,tempText.simplified());
            }
            else if(tempElement == "EPPAG1" || tempElement == "EPPAG1-B"){
                xmlWriter.writeTextElement(tempElement,tempText.simplified() );
            }
            else if(tempElement == "EPPAG2" || tempElement == "EPPAG2-B"){
                xmlWriter.writeTextElement(tempElement,tempText.simplified());
            }
            else if(tempElement == "DIEYT1" || tempElement == "DIEYT1-B"){
                xmlWriter.writeTextElement(tempElement,tempText.simplified().replace("<9>","\t"));
            }
            else if(tempElement == "DIEYT2" || tempElement == "DIEYT2-B"){
                xmlWriter.writeTextElement(tempElement,tempText.simplified().replace("<9>","\t"));
            }
            else if(tempElement == "WEB-B" || tempElement == "EMAIL-B"){
                xmlWriter.writeTextElement(tempElement,tempText.simplified() );
            }
            else if(tempElement == "VLEPE"){
                xmlWriter.writeTextElement(tempElement,tempText.simplified() );
            }
            counter ++;
        }
        xmlWriter.writeEndElement();
        xmlWriter.writeEndElement();
        xmlWriter.writeEndElement();
        inFile.close();
        outFile.close();
        CreateAlphabeticalXML(alphabeticalPath);
    }
}

void Alphabetical::CreateAlphabeticalXML(QString alphabeticalPath)
{
    QString _alphabeticalFilename = alphabeticalPath + "/alfavitikos_source_sample.xml";
    QFile xmlInFile(_alphabeticalFilename);
    if (!xmlInFile.open(QIODevice::ReadOnly ))
    {
        qDebug() << xmlInFile.errorString();
        QMessageBox msgBox;
        msgBox.setText("Παρουσιάστηκε πρόβλημα στο αρχείο!");
        msgBox.exec();
    }else{
        alphabeticalXMLReader.setDevice(&xmlInFile);
        QFile xmlOutFile(alphabeticalPath + "/alfavitikos.xml");
        xmlOutFile.open(QIODevice::WriteOnly);
        alphabeticalFinalXMLWriter.setDevice(&xmlOutFile);

        if(alphabeticalXMLReader.readNextStartElement() && alphabeticalXMLReader.name() == "ROOT"){
            alphabeticalFinalXMLWriter.writeStartDocument("1.0", true);
            alphabeticalFinalXMLWriter.writeCharacters("\n");
            alphabeticalFinalXMLWriter.writeStartElement("ROOT");
            processAlphabeticalEntries();
        }

        if (alphabeticalXMLReader.tokenType() == QXmlStreamReader::Invalid)
            alphabeticalXMLReader.readNext();

        if (alphabeticalXMLReader.hasError()) {
            alphabeticalXMLReader.raiseError();
            qDebug() << alphabeticalXMLReader.errorString();
        }
        alphabeticalFinalXMLWriter.writeEndElement();
        xmlInFile.close();
        xmlOutFile.close();
        QFile::remove(_alphabeticalFilename);
        QMessageBox msgBox;
        msgBox.setText("Το αλφαβητικό αρχείο είναι έτοιμο!");
        msgBox.exec();
    }
}

void Alphabetical::processAlphabeticalEntries(){
    alphabeticalXMLReader.readNextStartElement();
    if(!alphabeticalXMLReader.isStartElement() || alphabeticalXMLReader.name() != "entries")
        return;
    alphabeticalFinalXMLWriter.writeStartElement("entries");
    while (alphabeticalXMLReader.readNextStartElement()) {
        if (alphabeticalXMLReader.name() == "entry")
            processAlphabeticalEntry();
        else
            alphabeticalXMLReader.skipCurrentElement();
    }
    alphabeticalFinalXMLWriter.writeEndElement();
    alphabeticalFinalXMLWriter.writeCharacters("\n");
}

void Alphabetical::processAlphabeticalEntry(){
    if (!alphabeticalXMLReader.isStartElement() || alphabeticalXMLReader.name() != "entry")
        return;

    QString onoma, epaggelma, vlepe, odos, tilefono, tag, website, email;
    while (alphabeticalXMLReader.readNextStartElement()) {

        if (alphabeticalXMLReader.name() == "ONOMA" || alphabeticalXMLReader.name() == "ONOMA-B"){
            tag = alphabeticalXMLReader.name().toString();
            onoma = readNextAlphabeticalText().simplified();
        }
        else if (alphabeticalXMLReader.name() == "EPPAG1"
                 || alphabeticalXMLReader.name() == "EPPAG2-B"
                 || alphabeticalXMLReader.name() == "EPPAG2")

            epaggelma = readNextAlphabeticalText().simplified();
        else if (alphabeticalXMLReader.name() == "EPPAG1-B")
            epaggelma = readNextAlphabeticalText().simplified();
        else if (alphabeticalXMLReader.name() == "DIEYT1" ||
                 alphabeticalXMLReader.name() == "DIEYT1-B" ||
                 alphabeticalXMLReader.name() == "DIEYT2" ||
                 alphabeticalXMLReader.name() == "DIEYT2-B"){
            QStringList diefthinsi = readNextAlphabeticalText().split("\t",QString::KeepEmptyParts,Qt::CaseInsensitive);
            if (diefthinsi.length() == 2){
                odos = diefthinsi[0];
                tilefono = diefthinsi[1];
                tilefono = tilefono;
            } else if (diefthinsi.length() == 1){
                odos = diefthinsi[0];
                tilefono = "0123.456789";
                if(odos.contains("<9>")){
                    tilefono = odos.replace("<9>","");
                    odos = "Δεν υπάρχει διεύθυνση";
                }
            } else {
                odos = "Δεν υπάρχει διεύθυνση";
                tilefono = "0123.456789";
            }
        }
        else if(alphabeticalXMLReader.name() == "WEB-B")
            website = readNextAlphabeticalText().simplified();
        else if(alphabeticalXMLReader.name() == "EMAIL-B")
            email = readNextAlphabeticalText().simplified();
        else if(alphabeticalXMLReader.name() == "VLEPE")
            vlepe = readNextAlphabeticalText().simplified();
    }
    alphabeticalFinalXMLWriter.writeStartElement("entry");
    if(tag == "ONOMA" && onoma != ""){
        alphabeticalFinalXMLWriter.writeTextElement("ONOMA", onoma + " ");
        if(onoma.length() + epaggelma.length() + odos.length() < 56){
            if(epaggelma != "")
                alphabeticalFinalXMLWriter.writeTextElement("EPAG2", epaggelma + " ");
            if(odos != ""){
                alphabeticalFinalXMLWriter.writeTextElement("DIEYT2", odos + "\t" + tilefono);
            }
        } else {
            if(onoma.length() + epaggelma.length() >= 56 && epaggelma.length() + odos.length() < 56 ){
                if(epaggelma != ""){
                    alphabeticalFinalXMLWriter.writeCharacters("\n");
                    alphabeticalFinalXMLWriter.writeTextElement("EPAG1", epaggelma + " ");
                }
                if(odos != ""){
                    alphabeticalFinalXMLWriter.writeTextElement("DIEYT3", odos + "\t" + tilefono);
                }
            }else if(onoma.length() + epaggelma.length() >= 56 && epaggelma.length() + odos.length() >= 56 ){
                if(epaggelma != ""){
                    alphabeticalFinalXMLWriter.writeCharacters("\n");
                    alphabeticalFinalXMLWriter.writeTextElement("EPAG1", epaggelma + " ");
                }
                if(odos != ""){
                    alphabeticalFinalXMLWriter.writeCharacters("\n");
                    alphabeticalFinalXMLWriter.writeTextElement("DIEYT1", odos + "\t" + tilefono);
                }
            } else {
                if(epaggelma != ""){
                    alphabeticalFinalXMLWriter.writeTextElement("EPAG2", epaggelma);
                }
                if(odos != ""){
                    alphabeticalFinalXMLWriter.writeCharacters("\n");
                    alphabeticalFinalXMLWriter.writeTextElement("DIEYT1", odos + "\t" + tilefono);
                }
            }
        }
    }
    else if(tag == "ONOMA-B" && onoma != ""){
        alphabeticalFinalXMLWriter.writeTextElement("ONOMA-B", onoma);
        if(onoma.length() + epaggelma.length() < 30){
            if(epaggelma != "")
                alphabeticalFinalXMLWriter.writeTextElement("EPAG2-B", epaggelma);
            if(odos != ""){
                alphabeticalFinalXMLWriter.writeCharacters("\n");
                if(odos.length() + tilefono.length() < 40)
                    alphabeticalFinalXMLWriter.writeTextElement("DIEYT1-B", odos + "\t" + tilefono);
                else {
                    QStringList tilefona = tilefono.split(",",QString::KeepEmptyParts,Qt::CaseInsensitive);
                    QString tilefono0 = tilefona[0];
                    QString tilefono1 = "";
                    int i;
                    for(i=1; i<tilefona.length() - 1; i++)
                        tilefono1 += tilefona[i] + ", ";
                    tilefono1 += tilefona[i];
                    alphabeticalFinalXMLWriter.writeTextElement("DIEYT1-B", odos + "\t" + tilefono0);
                    alphabeticalFinalXMLWriter.writeCharacters("\n");
                    alphabeticalFinalXMLWriter.writeTextElement("DIEYT1-B2", "\t" + tilefono1.simplified());
                }
            }
        }
        else if(onoma.length() < 30 && epaggelma.length() + odos.length() <= 45){
            if(epaggelma != "")
                alphabeticalFinalXMLWriter.writeTextElement("EPAG2-B", epaggelma);
            if(odos != ""){
                alphabeticalFinalXMLWriter.writeTextElement("DIEYT2-B", odos + "\t" + tilefono);
            }
        }
        else if(onoma.length() < 30 && epaggelma.length() + odos.length() > 45){
            if(epaggelma != ""){
                alphabeticalFinalXMLWriter.writeCharacters("\n");
                alphabeticalFinalXMLWriter.writeTextElement("EPAG1-B", epaggelma);
            }
            if(odos != ""){
                alphabeticalFinalXMLWriter.writeCharacters("\n");
                if(odos.length() + tilefono.length() < 40)
                    alphabeticalFinalXMLWriter.writeTextElement("DIEYT1-B", odos + "\t" + tilefono);
                else {
                    QStringList tilefona = tilefono.split(",",QString::KeepEmptyParts,Qt::CaseInsensitive);
                    QString tilefono0 = tilefona[0];
                    QString tilefono1 = "";
                    int i;
                    for(i=1; i<tilefona.length() - 1; i++)
                        tilefono1 += tilefona[i] + ", ";
                    tilefono1 += tilefona[i];
                    alphabeticalFinalXMLWriter.writeTextElement("DIEYT1-B", odos + "\t" + tilefono0);
                    alphabeticalFinalXMLWriter.writeCharacters("\n");
                    alphabeticalFinalXMLWriter.writeTextElement("DIEYT1-B2", "\t" + tilefono1.simplified());
                }
            }
        }
        else{
            if(epaggelma != ""){
                alphabeticalFinalXMLWriter.writeCharacters("\n");
                alphabeticalFinalXMLWriter.writeTextElement("EPAG1-B", epaggelma);
            }
            if(odos != ""){
                alphabeticalFinalXMLWriter.writeCharacters("\n");
                if(odos.length() + tilefono.length() < 40)
                    alphabeticalFinalXMLWriter.writeTextElement("DIEYT1-B", odos + "\t" + tilefono);
                else {
                    QStringList tilefona = tilefono.split(",",QString::KeepEmptyParts,Qt::CaseInsensitive);
                    QString tilefono0 = tilefona[0];
                    QString tilefono1 = "";
                    int i;
                    for(i=1; i<tilefona.length() - 1; i++)
                        tilefono1 += tilefona[i] + ", ";
                    tilefono1 += tilefona[i];
                    alphabeticalFinalXMLWriter.writeTextElement("DIEYT1-B", odos + "\t" + tilefono0);
                    alphabeticalFinalXMLWriter.writeCharacters("\n");
                    alphabeticalFinalXMLWriter.writeTextElement("DIEYT1-B2", "\t" + tilefono1.simplified());
                }
            }
        }
        if(website != ""){
            alphabeticalFinalXMLWriter.writeCharacters("\n");
            alphabeticalFinalXMLWriter.writeTextElement("WEB-B", website);
        }
        if(email != ""){
            alphabeticalFinalXMLWriter.writeCharacters("\n");
            alphabeticalFinalXMLWriter.writeTextElement("EMAIL-B", email);
        }
        if(vlepe != ""){
            alphabeticalFinalXMLWriter.writeCharacters("\n");
            alphabeticalFinalXMLWriter.writeTextElement("VLEPE", vlepe);
        }
    }
    alphabeticalFinalXMLWriter.writeEndElement();
    alphabeticalFinalXMLWriter.writeCharacters("\n");
}

QString Alphabetical::readNextAlphabeticalText() {
    return alphabeticalXMLReader.readElementText();
}
