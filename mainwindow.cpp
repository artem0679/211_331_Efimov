#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QIODevice>
#include <QFile>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    textEditData = ui->textEditData;
    if (!textEditData) {
        QMessageBox::critical(this, "Error", "Could not find textEditData in UI file.");
        return;
    }
    loadDataAndDisplay(":/data/data.csv"); // Загрузим данные при старте
}

void MainWindow::loadDataAndDisplay(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "Could not open file: " + fileName);
        return;
    }

    QTextStream in(&file);
    textEditData->clear();

    // Заголовок
    textEditData->append("Card Number,Route,Time (time_t),Hash");

    QString line;
    while (!in.atEnd()) {
        line = in.readLine().trimmed();
        if (!line.isEmpty()) {
            textEditData->append(line);
        }
    }
    file.close();
}

MainWindow::~MainWindow()
{
    delete ui;
}
