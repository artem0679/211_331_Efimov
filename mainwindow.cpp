#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QIODevice>
#include <QFile>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    textEditData = ui->textEditData;
    if (!verifyPin()) {
        QMessageBox::warning(this, "Доступ запрещён", "Неверный ПИН. Приложение закрывается.");
        QCoreApplication::quit(); // Закрываем приложение полностью
        return;
    }

    loadDataAndDisplay(":/data/data.csv"); // Загрузим данные при старте
}

bool MainWindow::verifyPin()
{
    QString enteredPin = QInputDialog::getText(this, tr("ПИН-код"), tr("Введите ПИН-код:"), QLineEdit::Password);

    if (enteredPin.isEmpty()) {
        return false; // Отменили ввод
    }

    QByteArray pinBytes = enteredPin.toUtf8();
    QByteArray hashBytes = QCryptographicHash::hash(pinBytes, QCryptographicHash::Md5);
    QString enteredPinHash = hashBytes.toHex();

    // Хеш от "1234"
    const QString STORED_PIN_HASH = "81dc9bdb52d04dc20036dbd8313ed055";

    return enteredPinHash == STORED_PIN_HASH;
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
