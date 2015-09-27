#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "alphabetical.h"
#include "professional.h"
#include <QCoreApplication>
#include <QDebug>


QByteArray Transformer(QByteArray line, int counter);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    alphabeticalButton = new QPushButton("Δημιουργία\nΑλφαβητικού\nΑρχείου", this);
    epaggelmatikosButton = new QPushButton("Δημιουργία\nΕπαγγελματικού\nΑρχείου", this);
    alphabeticalButton -> setGeometry(QRect(QPoint(50, 50), QSize(100, 100)));
    epaggelmatikosButton -> setGeometry(QRect(QPoint(150,50), QSize(100,100)));
    connect(alphabeticalButton, SIGNAL (released()), this, SLOT (alphabeticalButton_clicked()));
    connect(epaggelmatikosButton, SIGNAL (released()), this, SLOT (epaggelmatikosButton_clicked()));    
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::alphabeticalButton_clicked()
{
    alphabetical.OpenAlphabeticalSourceFile();
}

void MainWindow::epaggelmatikosButton_clicked(){
    professional.OpenProfessionalSourceFile();
}

