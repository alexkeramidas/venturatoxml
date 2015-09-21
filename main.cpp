#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QTextCodec *utfCodec = QTextCodec::codecForName("UTF-8"); //creating new utf-8 codec
    QTextCodec::setCodecForLocale(utfCodec);// setting the utf-8 codec for the tr() tags
    MainWindow window;
    window.show();

    return app.exec();
}
