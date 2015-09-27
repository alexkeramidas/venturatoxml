#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include "alphabetical.h"
#include "professional.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void read();

private slots:
    void alphabeticalButton_clicked();
    void epaggelmatikosButton_clicked();

private:
    /* Variable declarations */
    Ui::MainWindow *ui;
    QPushButton *alphabeticalButton;
    QPushButton *epaggelmatikosButton;
    Alphabetical alphabetical;
    Professional professional;    
};

#endif // MAINWINDOW_H
