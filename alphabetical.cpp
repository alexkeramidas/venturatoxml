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
    /*Read Entries List*/
    QString tempElement, tempText;
    QString alphabeticalTextFileName = QFileDialog::getOpenFileName(0, QWidget::tr("Open Alphabetical Text"), QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).at(0) , QWidget::tr("Text files (*.txt)"));
    QByteArray line = NULL;
    QFile alphabeticalTextFile(alphabeticalTextFileName);
    QFileInfo inFileInfo(alphabeticalTextFile);
    alphabeticalTextFilePath  = inFileInfo.dir().path();

    /*Read Image List and Create Image Map*/
    QString alphabeticalImageListFileName = QFileDialog::getOpenFileName(0, QWidget::tr("Open Image List File"), alphabeticalTextFilePath, QWidget::tr("Text files (*.txt)"));
    QByteArray imageLine = NULL;
    QFile alphabeticalImageListFile(alphabeticalImageListFileName);
    QFileInfo imageListFileInfo(alphabeticalImageListFile);
    alphabeticalImageListFilePath  = imageListFileInfo.dir().path();
    if (!alphabeticalImageListFile.open(QIODevice::ReadOnly)) {
        qDebug() << alphabeticalImageListFile.errorString();
        QMessageBox msgBox;
        msgBox.setText("Δεν έχετε επιλέξει σωστό αρχείο φωτογραφιών\nή το αρχείο είναι κατεστραμμένο!");
        msgBox.exec();
    }else{
        while (!alphabeticalImageListFile.atEnd()) {
            imageLine = alphabeticalImageListFile.readLine();
            QString tempImage = imageLine;
            QStringList image = tempImage.split("\t");
            QString imageCode = image[0].simplified();
            QString imageName = image[1].simplified();
            imageMap.insert(imageName,imageCode);
        }
    }

    /*Create image file list*/
    QString alphabeticalImagesFolderPath = alphabeticalImageListFilePath;
//    QString alphabeticalImagesFolderPath = alphabeticalImageListFilePath + "/images";
    if(QDir(alphabeticalImagesFolderPath).exists()){
        QDirIterator dirIt(alphabeticalImagesFolderPath,QDirIterator::NoIteratorFlags);
        while (dirIt.hasNext()) {
            dirIt.next();
            if (QFileInfo(dirIt.filePath()).isFile())
                if (QFileInfo(dirIt.filePath()).suffix() == "tif"){
                    dirIt.fileName();
                    imageFilesList.insert(dirIt.fileName().split("-")[0], dirIt.fileName());
                }
        }
    }


    /*Create Temporary XML File*/
    if (!alphabeticalTextFile.open(QIODevice::ReadOnly)) {
        qDebug() << alphabeticalTextFile.errorString();
        QMessageBox msgBox;
        msgBox.setText("Δεν έχετε επιλέξει αρχείο\nή το αρχείο είναι κατεστραμμένο!");
        msgBox.exec();
    }else{
        QFile outFile(alphabeticalTextFilePath + "/alfavitikos_source_sample.xml");
        outFile.open(QIODevice::WriteOnly);
        QXmlStreamWriter xmlWriter(&outFile);
        xmlWriter.setAutoFormatting(false);
        xmlWriter.writeStartDocument("1.0", true);
        xmlWriter.writeStartElement("ROOT");
        xmlWriter.writeStartElement("entries");
        int counter = 0;
        while (!alphabeticalTextFile.atEnd()) {
            line = alphabeticalTextFile.readLine();
            tempText = line;
            tempElement = line.remove(0,1);
            tempElement = tempElement.remove(QRegExp("=.*"));
            tempText = tempText.remove(QRegExp("@.*=")).simplified();
            if (imageMap.contains(tempText)){
                QString tempFileName = imageMap.value(tempText);
                tempFileName = tempFileName.remove("-", Qt::CaseInsensitive);
                if(imageFilesList.value(tempFileName) != NULL && counter != 0){
                    xmlWriter.writeEndElement();
                    xmlWriter.writeStartElement("entry");
                    xmlWriter.writeTextElement(tempElement,tempText.simplified());
                    xmlWriter.writeEmptyElement("image");
//                    xmlWriter.writeAttribute("href","file:///images/" + imageFilesList.value(tempFileName));
                    xmlWriter.writeAttribute("href","file:///" + imageFilesList.value(tempFileName));
                } else if(imageFilesList.value(tempFileName) == NULL) {
                    xmlWriter.writeStartElement("entry");
                    xmlWriter.writeTextElement(tempElement,tempText.simplified());
                }
            } else {
                if((tempElement == "ONOMA" || tempElement == "ONOMA-B")  && counter != 0){
                    xmlWriter.writeEndElement();
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
            }
            counter ++;
        }
        xmlWriter.writeEndElement();
        xmlWriter.writeEndElement();
        xmlWriter.writeEndElement();
        alphabeticalTextFile.close();
        outFile.close();
        CreateAlphabeticalXML(alphabeticalTextFilePath);
    }
}

void Alphabetical::CreateAlphabeticalXML(QString alphabeticalPath)
{
    QString _alphabeticalFilename = alphabeticalPath + "/alfavitikos_source_sample.xml";
    aid3 = "http://ns.adobe.com/AdobeInDesign/3.0/";
    aid4 = "http://ns.adobe.com/AdobeInDesign/4.0/";
    aid5 = "http://ns.adobe.com/AdobeInDesign/5.0/";
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
            alphabeticalFinalXMLWriter.writeProcessingInstruction("whitespace-handling","use-tags");
            alphabeticalFinalXMLWriter.writeNamespace(aid3,"aid3");
            alphabeticalFinalXMLWriter.writeNamespace(aid4,"aid4");
            alphabeticalFinalXMLWriter.writeNamespace(aid5,"aid5");
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
}

void Alphabetical::processAlphabeticalEntry(){
    if (!alphabeticalXMLReader.isStartElement() || alphabeticalXMLReader.name() != "entry")
        return;

    QString onoma, epaggelma, vlepe, odos, tilefono, tag, website, email, image;
    while (alphabeticalXMLReader.readNextStartElement()) {
        if (alphabeticalXMLReader.name() == "ONOMA" || alphabeticalXMLReader.name() == "ONOMA-B"){
            tag = alphabeticalXMLReader.name().toString();
            onoma = readNextAlphabeticalText() + " ";
        }
        else if (alphabeticalXMLReader.name() == "EPPAG1"
                 || alphabeticalXMLReader.name() == "EPPAG1-B"
                 || alphabeticalXMLReader.name() == "EPPAG2-B"
                 || alphabeticalXMLReader.name() == "EPPAG2")
            epaggelma = readNextAlphabeticalText().simplified() + " ";
        else if (alphabeticalXMLReader.name() == "DIEYT1" ||
                 alphabeticalXMLReader.name() == "DIEYT1-B" ||
                 alphabeticalXMLReader.name() == "DIEYT2" ||
                 alphabeticalXMLReader.name() == "DIEYT2-B"){
            QStringList diefthinsi = readNextAlphabeticalText().split("\t",QString::KeepEmptyParts,Qt::CaseInsensitive);
            if (diefthinsi.length() == 2){
                odos = diefthinsi[0].simplified();
                tilefono = diefthinsi[1].simplified();
                tilefono = tilefono.simplified();
            } else if (diefthinsi.length() == 1){
                odos = diefthinsi[0].simplified();
                tilefono = "0123.456789";
                if(odos.contains("<9>")){
                    tilefono = odos.replace("<9>","").simplified();
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
        else if(alphabeticalXMLReader.name() == "image"){
            image = alphabeticalXMLReader.attributes().at(0).value().toString();
            readNextAlphabeticalText();
        }
    }
    if(onoma.simplified() != ""){
        alphabeticalFinalXMLWriter.writeStartElement("entry");
        if(tag == "ONOMA" && onoma != ""){
            alphabeticalFinalXMLWriter.writeStartElement("ONOMA");
            alphabeticalFinalXMLWriter.writeCharacters(onoma);
            alphabeticalFinalXMLWriter.writeEndElement();
            if(onoma.length() + epaggelma.length() + odos.length() < 56){
                if(epaggelma != "")
                    alphabeticalFinalXMLWriter.writeTextElement("EPAG2", epaggelma);
                if(odos != "")
                    nameStreetAndPhone("DIEYT2", odos, tilefono);
            } else {
                if(onoma.length() + epaggelma.length() >= 56 && epaggelma.length() + odos.length() < 56 ){
                    if(epaggelma != ""){
                        alphabeticalFinalXMLWriter.writeEmptyElement(aid3,"br");
                        alphabeticalFinalXMLWriter.writeTextElement("EPAG1", epaggelma);
                    }
                    if(odos != "")
                        nameStreetAndPhone("DIEYT3", odos, tilefono);
                }else if(onoma.length() + epaggelma.length() >= 56 && epaggelma.length() + odos.length() >= 56 ){
                    if(epaggelma != ""){
                        alphabeticalFinalXMLWriter.writeEmptyElement(aid3,"br");
                        alphabeticalFinalXMLWriter.writeTextElement("EPAG1", epaggelma);
                    }
                    if(odos != "")
                        streetAndPhone("DIEYT1", odos, tilefono);
                } else {
                    if(epaggelma != "")
                        alphabeticalFinalXMLWriter.writeTextElement("EPAG2", epaggelma);
                    if(odos != "")
                        streetAndPhone("DIEYT1", odos, tilefono);
                }
            }
            if(image != ""){
                alphabeticalFinalXMLWriter.writeEmptyElement(aid3,"br");
                alphabeticalFinalXMLWriter.writeEmptyElement("image");
                alphabeticalFinalXMLWriter.writeAttribute("href",image);
            }
        }
        else if(tag == "ONOMA-B" && onoma != ""){
            alphabeticalFinalXMLWriter.writeStartElement("ONOMA-B");
            alphabeticalFinalXMLWriter.writeCharacters(onoma);
            alphabeticalFinalXMLWriter.writeEndElement();
            if(onoma.length() + epaggelma.length() < 30){
                if(epaggelma != ""){
                    alphabeticalFinalXMLWriter.writeStartElement("EPAG2-B");
                    alphabeticalFinalXMLWriter.writeCharacters(epaggelma);
                    alphabeticalFinalXMLWriter.writeEndElement();
                }
                if(odos != ""){
                    if(odos.length() + tilefono.length() < 40){
                        tilefono = removeMobileIndicator(tilefono);
                        streetAndPhone("DIEYT1-B", odos, tilefono);
                    } else {
                        QStringList tilefona = tilefono.split(",",QString::KeepEmptyParts,Qt::CaseInsensitive);
                        QString tilefono0 = tilefona[0];
                        QString tilefono1 = "";
                        int i;
                        for(i=1; i<tilefona.length() - 1; i++)
                            tilefono1 += tilefona[i] + ", ";
                        tilefono1 += tilefona[i];
                        tilefono0 = removeMobileIndicator(tilefono0);
                        tilefono1 = removeMobileIndicator(tilefono1);
                        streetAndPhone("DIEYT1-B", odos, tilefono0);
                        singlePhone(tilefono1);
                    }
                }
            } else if(onoma.length() < 30 && epaggelma.length() + odos.length() <= 45){
                if(epaggelma != ""){
                    alphabeticalFinalXMLWriter.writeStartElement("EPAG2-B");
                    alphabeticalFinalXMLWriter.writeCharacters(epaggelma);
                    alphabeticalFinalXMLWriter.writeEndElement();
                }
                if(odos != ""){
                    tilefono = removeMobileIndicator(tilefono);
                    streetAndPhone("DIEYT2-B", odos, tilefono);
                }
            }
            else if(onoma.length() < 30 && epaggelma.length() + odos.length() > 45){
                if(epaggelma != ""){
                    alphabeticalFinalXMLWriter.writeEmptyElement(aid3,"br");
                    alphabeticalFinalXMLWriter.writeStartElement("EPAG1-B");
                    alphabeticalFinalXMLWriter.writeCharacters(epaggelma);
                    alphabeticalFinalXMLWriter.writeEndElement();
                }
                if(odos != ""){
                    if(odos.length() + tilefono.length() < 40){
                        tilefono = removeMobileIndicator(tilefono);
                        streetAndPhone("DIEYT1-B", odos, tilefono);
                    }
                    else {
                        QStringList tilefona = tilefono.split(",",QString::KeepEmptyParts,Qt::CaseInsensitive);
                        QString tilefono0 = tilefona[0];
                        QString tilefono1 = "";
                        int i;
                        for(i=1; i<tilefona.length() - 1; i++)
                            tilefono1 += tilefona[i] + ", ";
                        tilefono1 += tilefona[i];
                        tilefono0 = removeMobileIndicator(tilefono0);
                        tilefono1 = removeMobileIndicator(tilefono1);
                        streetAndPhone("DIEYT1-B", odos, tilefono0);
                        singlePhone(tilefono1);
                    }
                }
            }
            else{
                if(epaggelma != ""){
                    alphabeticalFinalXMLWriter.writeEmptyElement(aid3,"br");
                    alphabeticalFinalXMLWriter.writeStartElement("EPAG1-B");
                    alphabeticalFinalXMLWriter.writeCharacters(epaggelma);
                    alphabeticalFinalXMLWriter.writeEndElement();
                }
                if(odos != ""){
                    if(odos.length() + tilefono.length() < 40){
                        tilefono = removeMobileIndicator(tilefono);
                        streetAndPhone("DIEYT1-B", odos, tilefono);
                    } else {
                        QStringList tilefona = tilefono.split(",",QString::KeepEmptyParts,Qt::CaseInsensitive);
                        QString tilefono0 = tilefona[0];
                        QString tilefono1 = "";
                        int i;
                        for(i=1; i<tilefona.length() - 1; i++)
                            tilefono1 += tilefona[i] + ", ";
                        tilefono1 += tilefona[i];
                        tilefono0 = removeMobileIndicator(tilefono0);
                        tilefono1 = removeMobileIndicator(tilefono1);
                        streetAndPhone("DIEYT1-B", odos, tilefono0);
                        singlePhone(tilefono1);
                    }
                }
            }
            if(website != ""){
                alphabeticalFinalXMLWriter.writeEmptyElement(aid3,"br");
                alphabeticalFinalXMLWriter.writeTextElement("WEB-B", website);
            }
            if(email != ""){
                alphabeticalFinalXMLWriter.writeEmptyElement(aid3,"br");
                alphabeticalFinalXMLWriter.writeTextElement("EMAIL-B", email);
            }
            if(vlepe != ""){
                alphabeticalFinalXMLWriter.writeEmptyElement(aid3,"br");
                alphabeticalFinalXMLWriter.writeTextElement("VLEPE", vlepe);
            }
            if(image != ""){
                alphabeticalFinalXMLWriter.writeEmptyElement(aid3,"br");
                alphabeticalFinalXMLWriter.writeEmptyElement("image");
                alphabeticalFinalXMLWriter.writeAttribute("href",image);
            }
        }
        alphabeticalFinalXMLWriter.writeEndElement();
        alphabeticalFinalXMLWriter.writeEmptyElement(aid3,"br");
    }
}

QString Alphabetical::readNextAlphabeticalText() {
    return alphabeticalXMLReader.readElementText();
}

QString Alphabetical::removeMobileIndicator(QString tilefono)
{
    if(tilefono.contains(",Κιν.:-"))
        tilefono.remove(",Κιν.:-",Qt::CaseInsensitive);
    if(tilefono.contains(", Κιν.:-"))
        tilefono.remove(", Κιν.:-",Qt::CaseInsensitive);
    return tilefono;
}

void Alphabetical::streetAndPhone(QString element, QString odos, QString tilefono)
{
    alphabeticalFinalXMLWriter.writeEmptyElement(aid3,"br");
    alphabeticalFinalXMLWriter.writeStartElement(element);
    alphabeticalFinalXMLWriter.writeCharacters(odos.simplified());
    alphabeticalFinalXMLWriter.writeEmptyElement(aid3,"tab");
    alphabeticalFinalXMLWriter.writeCharacters(tilefono.simplified());
    alphabeticalFinalXMLWriter.writeEndElement();
}

void Alphabetical::singlePhone(QString tilefono)
{
    if(tilefono.length() > 9){
        alphabeticalFinalXMLWriter.writeEmptyElement(aid3,"br");
        alphabeticalFinalXMLWriter.writeStartElement("DIEYT1-B2");
        alphabeticalFinalXMLWriter.writeEmptyElement(aid3,"tab");
        alphabeticalFinalXMLWriter.writeCharacters(tilefono.simplified());
        alphabeticalFinalXMLWriter.writeEndElement();
    }
}

void Alphabetical::nameStreetAndPhone(QString element, QString odos, QString tilefono)
{
    alphabeticalFinalXMLWriter.writeStartElement(element);
    alphabeticalFinalXMLWriter.writeCharacters(odos.simplified());
    alphabeticalFinalXMLWriter.writeEmptyElement(aid3,"tab");
    alphabeticalFinalXMLWriter.writeCharacters(tilefono.simplified());
    alphabeticalFinalXMLWriter.writeEndElement();
}
