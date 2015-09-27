#include "professional.h"
#include "stringutils.h"

#include <QFile>
#include <QStringList>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QDirIterator>
#include <QStandardPaths>
#include <QFileInfo>

Professional::Professional()
{

}

void Professional::OpenProfessionalSourceFile(){
    QString tempElement, tempText;
    QString fileName = QFileDialog::getOpenFileName(0, QWidget::tr("Open Category Text File"),QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).at(0), "Text files (*.txt)");
    QByteArray line = NULL;
    QFile inFile(fileName);
    QFileInfo inFileInfo(inFile);
    categoriesPath  = inFileInfo.dir().path();
    if (!inFile.open(QIODevice::ReadOnly)) {
        qDebug() << inFile.errorString();
        QMessageBox msgBox;
        msgBox.setText("Δεν έχετε επιλέξει αρχείο\nή το αρχείο είναι κατεστραμμένο!");
        msgBox.exec();
    }else{
        QFile outFile(categoriesPath + "/epaggelmatikos_source_sample.xml");
        outFile.open(QIODevice::WriteOnly);
        QXmlStreamWriter xmlWriter(&outFile);
        xmlWriter.setAutoFormatting(false);
        xmlWriter.writeStartDocument("1.0", true);
        xmlWriter.writeStartElement("ROOT");
        xmlWriter.writeStartElement("categories");
        int counter = 0;
        while (!inFile.atEnd()) {
            line = inFile.readLine();
            tempText = line;
            tempElement = line.remove(0,1);
            tempElement = tempElement.remove(QRegExp("=.*"));
            tempText = tempText.remove(QRegExp("@.*="));
            if(tempElement == "KATHG" && counter > 0){
                xmlWriter.writeEndElement();
                xmlWriter.writeEndElement();
                xmlWriter.writeCharacters("\n");
                xmlWriter.writeStartElement("category");
                xmlWriter.writeTextElement(tempElement,tempText.simplified());
                xmlWriter.writeStartElement("entries");
            }
            else if (tempElement == "KATHG") {
                xmlWriter.writeStartElement("category");
                xmlWriter.writeTextElement(tempElement,tempText.simplified());
                xmlWriter.writeStartElement("entries");
            }
            else if(tempElement == "ONOMA"
                    || tempElement == "ONOMA-B"
                    || tempElement == "ONOMA-S" ) {
                xmlWriter.writeEndElement();
                xmlWriter.writeStartElement("entry");
                xmlWriter.writeTextElement(tempElement,tempText.simplified());
            }
            else if(tempElement == "KIMENO-B" || tempElement == "KIMENO-S"){
                xmlWriter.writeTextElement(tempElement,tempText.simplified() );
            }
            else if(tempElement == "TEL-B" || tempElement == "TEL-S"){
                xmlWriter.writeTextElement(tempElement,tempText.simplified());
            }
            else if(tempElement == "DIEYT1"
                    || tempElement == "DIEYT1-B"
                    || tempElement == "DIEYT1-S"){
                xmlWriter.writeTextElement(tempElement,tempText.simplified().replace(" <9>","\t").replace("<9>","\t"));
            }
            else if(tempElement == "DIEYT2"
                    || tempElement == "DIEYT2-B"
                    || tempElement == "DIEYT2-S"){
                xmlWriter.writeTextElement(tempElement,tempText.simplified().replace(" <9>","\t").replace("<9>","\t"));
            }
            counter ++;
        }
        xmlWriter.writeEndElement();
        xmlWriter.writeEndElement();
        xmlWriter.writeEndElement();
        xmlWriter.writeEndElement();
        inFile.close();
        outFile.close();
        CreateProfessionalXML(categoriesPath);
    }
}

void Professional::CreateProfessionalXML(QString categoriesPath){
    QString _professionalFilename = categoriesPath + "/epaggelmatikos_source_sample.xml";
    QFile xmlInFile(_professionalFilename);
    if (!xmlInFile.open(QIODevice::ReadOnly ))
    {
        // Error while loading file
        qDebug() << xmlInFile.errorString();
        QMessageBox msgBox;
        msgBox.setText("Παρουσιάστηκε πρόβλημα στο αρχείο!");
        msgBox.exec();
    }else{
        professionalXMLReader.setDevice(&xmlInFile);

        QFile xmlOutFile(categoriesPath + "/epaggelmatikos.xml");
        xmlOutFile.open(QIODevice::WriteOnly);
        professionalFinalXMLWriter.setDevice(&xmlOutFile);

        if(professionalXMLReader.readNextStartElement() && professionalXMLReader.name() == "ROOT"){
            professionalFinalXMLWriter.writeStartDocument("1.0", true);
            professionalFinalXMLWriter.writeProcessingInstruction("whitespace-handling","use-tags");
            professionalFinalXMLWriter.writeNamespace("http://ns.adobe.com/AdobeInDesign/3.0/","aid3");
            professionalFinalXMLWriter.writeNamespace("http://ns.adobe.com/AdobeInDesign/4.0/","aid4");
            professionalFinalXMLWriter.writeNamespace("http://ns.adobe.com/AdobeInDesign/5.0/","aid5");
            professionalFinalXMLWriter.writeStartElement("ROOT");
            processProfessionalEntries();
        }

        if (professionalXMLReader.tokenType() == QXmlStreamReader::Invalid)
            professionalXMLReader.readNext();

        if (professionalXMLReader.hasError()) {
            professionalXMLReader.raiseError();
            qDebug() << professionalXMLReader.errorString();
        }
        professionalFinalXMLWriter.writeEndElement();
        xmlInFile.close();
        xmlOutFile.close();
        QFile::remove(_professionalFilename);
        QMessageBox msgBox;
        msgBox.setText("Το επαγγγελματικό αρχείο είναι έτοιμο!");
        msgBox.exec();
    }
}

void Professional::processProfessionalEntries(){
    professionalXMLReader.readNextStartElement();
    if(!professionalXMLReader.isStartElement() || professionalXMLReader.name() != "categories")
        return;
    while (professionalXMLReader.readNextStartElement()) {
        if (professionalXMLReader.name() == "category"){
            while (professionalXMLReader.readNextStartElement()) {
                if(professionalXMLReader.name() == "KATHG"){
                    QString katigoria = readNextProfessionalText();
                    professionalFinalXMLWriter.writeStartElement("Category");
                    if(katigoria.length() <= 33){
                        professionalFinalXMLWriter.writeTextElement("KATHG",katigoria);
                        professionalFinalXMLWriter.writeEmptyElement("http://ns.adobe.com/AdobeInDesign/3.0/","br");
                    }
                    else{

                        QStringList katigoriaList = katigoria.split(" ",QString::KeepEmptyParts,Qt::CaseInsensitive);
                        int listlength = katigoriaList.length();
                        QString katigoriaLine = "";
                        for(int i = 0; i < listlength; ++i){
                            katigoriaLine += katigoriaList[i] + " ";
                            if(i < listlength - 1 ){
                                if(katigoriaLine.length() +  katigoriaList[i+1].length() > 33){
                                    professionalFinalXMLWriter.writeTextElement("KATHG",katigoriaLine.simplified());
                                    professionalFinalXMLWriter.writeEmptyElement("http://ns.adobe.com/AdobeInDesign/3.0/","br");
                                    katigoriaLine = "";
                                }
                            }else{
                                professionalFinalXMLWriter.writeTextElement("KATHG",katigoriaLine.simplified());
                                professionalFinalXMLWriter.writeEmptyElement("http://ns.adobe.com/AdobeInDesign/3.0/","br");
                            }
                        }
                    }
                    writeCategoryImageFiles(katigoria);
                    professionalFinalXMLWriter.writeEndElement();
                } else if(professionalXMLReader.name() == "entry")
                    processProfessionalEntry();
                else
                    professionalXMLReader.skipCurrentElement();
            }
        }
    }
    professionalFinalXMLWriter.writeEndElement();
}

void Professional::processProfessionalEntry(){
    if (!professionalXMLReader.isStartElement() || professionalXMLReader.name() != "entry")
        return;

    QString onoma, odos, tilefono, tag, kimeno,telb;
    while (professionalXMLReader.readNextStartElement()) {

        if (professionalXMLReader.name() == "ONOMA"
                || professionalXMLReader.name() == "ONOMA-B"
                || professionalXMLReader.name() == "ONOMA-S"){
            tag = professionalXMLReader.name().toString();
            onoma = readNextProfessionalText() + " ";
        }
        else if (professionalXMLReader.name() == "DIEYT1"
                 || professionalXMLReader.name() == "DIEYT2"
                 || professionalXMLReader.name() == "DIEYT1-B"
                 || professionalXMLReader.name() == "DIEYT2-B"
                 || professionalXMLReader.name() == "DIEYT1-S"
                 || professionalXMLReader.name() == "DIEYT2-S"){
            QStringList diefthinsi = readNextProfessionalText().split("\t",QString::KeepEmptyParts,Qt::CaseInsensitive);
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
        else if(professionalXMLReader.name() == "KIMENO-B" || professionalXMLReader.name() == "KIMENO-S")
            kimeno = readNextProfessionalText().simplified();
        else if(professionalXMLReader.name() == "TEL-B" || professionalXMLReader.name() == "TEL-S")
            telb = readNextProfessionalText().simplified();
    }

    professionalFinalXMLWriter.writeStartElement("entry");
    if(onoma.length() > 0 ){
        if(tag == "ONOMA" && onoma != ""){
            professionalFinalXMLWriter.writeTextElement("ONOMA", onoma);
            if(onoma.length() + odos.length() < 46){
                if(odos != ""){
                    professionalFinalXMLWriter.writeTextElement("DIEYT2", odos);
                    professionalFinalXMLWriter.writeEmptyElement("http://ns.adobe.com/AdobeInDesign/3.0/","tab");
                    professionalFinalXMLWriter.writeTextElement("DIEYT2", tilefono);
                }
            } else {
                if(onoma.length() >= 46 && odos.length() < 46 ){
                    if(odos != ""){
                        professionalFinalXMLWriter.writeEmptyElement("http://ns.adobe.com/AdobeInDesign/3.0/","br");
                        professionalFinalXMLWriter.writeTextElement("DIEYT1", odos);
                        professionalFinalXMLWriter.writeEmptyElement("http://ns.adobe.com/AdobeInDesign/3.0/","tab");
                        professionalFinalXMLWriter.writeTextElement("DIEYT1", tilefono);
                    }
                }else if(onoma.length() >= 46 && odos.length() >= 46 ){
                    if(odos != ""){
                        professionalFinalXMLWriter.writeEmptyElement("http://ns.adobe.com/AdobeInDesign/3.0/","br");
                        professionalFinalXMLWriter.writeTextElement("DIEYT1", odos);
                        professionalFinalXMLWriter.writeEmptyElement("http://ns.adobe.com/AdobeInDesign/3.0/","br");
                        professionalFinalXMLWriter.writeEmptyElement("http://ns.adobe.com/AdobeInDesign/3.0/","tab");
                        professionalFinalXMLWriter.writeTextElement("DIEYT1", tilefono);
                    }
                } else {
                    if(odos != ""){
                        professionalFinalXMLWriter.writeEmptyElement("http://ns.adobe.com/AdobeInDesign/3.0/","br");
                        professionalFinalXMLWriter.writeTextElement("DIEYT1", odos);
                        professionalFinalXMLWriter.writeEmptyElement("http://ns.adobe.com/AdobeInDesign/3.0/","tab");
                        professionalFinalXMLWriter.writeTextElement("DIEYT1", tilefono);
                    }
                }
            }
        }
        else if(tag == "ONOMA-B" && onoma != ""){
            professionalFinalXMLWriter.writeTextElement("ONOMA-B", onoma);
            if(kimeno != ""){
                professionalFinalXMLWriter.writeEmptyElement("http://ns.adobe.com/AdobeInDesign/3.0/","br");
                professionalFinalXMLWriter.writeTextElement("KIMENO-B", kimeno);
            }
            if(onoma.length() < 30){
                if(odos != ""){
                    professionalFinalXMLWriter.writeEmptyElement("http://ns.adobe.com/AdobeInDesign/3.0/","br");
                    if(odos.length() < 40)
                        professionalFinalXMLWriter.writeTextElement("DIEYT1-B", odos);
                    professionalFinalXMLWriter.writeEmptyElement("http://ns.adobe.com/AdobeInDesign/3.0/","tab");
                    professionalFinalXMLWriter.writeTextElement("DIEYT1-B", telb);
                }
            }
            else if(onoma.length() < 30 && odos.length() <= 45){
                if(odos != ""){
                    professionalFinalXMLWriter.writeTextElement("DIEYT2-B", odos);
                    professionalFinalXMLWriter.writeEmptyElement("http://ns.adobe.com/AdobeInDesign/3.0/","tab");
                    professionalFinalXMLWriter.writeTextElement("DIEYT2-B", telb);
                }
            }
            else if(onoma.length() < 30 && odos.length() > 45){
                if(odos != ""){
                    professionalFinalXMLWriter.writeEmptyElement("http://ns.adobe.com/AdobeInDesign/3.0/","br");
                    if(odos.length() < 40)
                        professionalFinalXMLWriter.writeTextElement("DIEYT1-B", odos);
                    professionalFinalXMLWriter.writeEmptyElement("http://ns.adobe.com/AdobeInDesign/3.0/","tab");
                    professionalFinalXMLWriter.writeTextElement("DIEYT1-B", telb);
                }
            }
            else{
                if(odos != ""){
                    professionalFinalXMLWriter.writeEmptyElement("http://ns.adobe.com/AdobeInDesign/3.0/","br");
                    if(odos.length() < 40)
                        professionalFinalXMLWriter.writeTextElement("DIEYT1-B", odos);
                    professionalFinalXMLWriter.writeEmptyElement("http://ns.adobe.com/AdobeInDesign/3.0/","tab");
                    professionalFinalXMLWriter.writeTextElement("DIEYT1-B", telb);
                }
            }
        }
        else if(tag == "ONOMA-S" && onoma != ""){
            professionalFinalXMLWriter.writeTextElement("ONOMA-S", onoma);
            if(kimeno != ""){
                professionalFinalXMLWriter.writeEmptyElement("http://ns.adobe.com/AdobeInDesign/3.0/","br");
                professionalFinalXMLWriter.writeTextElement("KIMENO-S", kimeno);
            }
            if(onoma.length() < 30){
                if(odos != ""){
                    professionalFinalXMLWriter.writeEmptyElement("http://ns.adobe.com/AdobeInDesign/3.0/","br");
                    if(odos.length() < 40)
                        professionalFinalXMLWriter.writeTextElement("DIEYT1-S", odos);
                    professionalFinalXMLWriter.writeEmptyElement("http://ns.adobe.com/AdobeInDesign/3.0/","tab");
                    professionalFinalXMLWriter.writeTextElement("DIEYT1-S", telb);
                }
            }
            else if(onoma.length() < 30 && odos.length() <= 45){
                if(odos != ""){
                    professionalFinalXMLWriter.writeTextElement("DIEYT2-S", odos);
                    professionalFinalXMLWriter.writeEmptyElement("http://ns.adobe.com/AdobeInDesign/3.0/","tab");
                    professionalFinalXMLWriter.writeTextElement("DIEYT2-S", telb);
                }
            }
            else if(onoma.length() < 30 && odos.length() > 45){
                if(odos != ""){
                    professionalFinalXMLWriter.writeEmptyElement("http://ns.adobe.com/AdobeInDesign/3.0/","br");
                    if(odos.length() < 40)
                        professionalFinalXMLWriter.writeTextElement("DIEYT1-S", odos);
                    professionalFinalXMLWriter.writeEmptyElement("http://ns.adobe.com/AdobeInDesign/3.0/","tab");
                    professionalFinalXMLWriter.writeTextElement("DIEYT1-S", telb);
                }
            }
            else{
                if(odos != ""){
                    professionalFinalXMLWriter.writeEmptyElement("http://ns.adobe.com/AdobeInDesign/3.0/","br");
                    if(odos.length() < 40)
                        professionalFinalXMLWriter.writeTextElement("DIEYT1-S", odos);
                    professionalFinalXMLWriter.writeEmptyElement("http://ns.adobe.com/AdobeInDesign/3.0/","tab");
                    professionalFinalXMLWriter.writeTextElement("DIEYT1-S", telb);
                }
            }
        }
    }
    professionalFinalXMLWriter.writeEndElement();
    professionalFinalXMLWriter.writeEmptyElement("http://ns.adobe.com/AdobeInDesign/3.0/","br");
}

void Professional::writeCategoryImageFiles(QString katigoria){
    StringUtils stringUtils;
    QString categoryImagesPath = categoriesPath + "/" + stringUtils.removeAccents(katigoria).toUpper();
    if(QDir(categoryImagesPath).exists()){
        QDirIterator dirIt(categoryImagesPath,QDirIterator::NoIteratorFlags);
        while (dirIt.hasNext()) {
            dirIt.next();
            if (QFileInfo(dirIt.filePath()).isFile())
                if (QFileInfo(dirIt.filePath()).suffix() == "tif"){
                    professionalFinalXMLWriter.writeEmptyElement("image");
                    professionalFinalXMLWriter.writeAttribute("href","file:///" + dirIt.filePath());
                }
        }
    }
}

QString Professional::readNextProfessionalText() {
    return professionalXMLReader.readElementText();
}
