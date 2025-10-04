#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QIODevice>
#include <QFile>
#include <QInputDialog>
#include <QMessageBox>
#include <QApplication>
#include <QRegularExpression>
#include <QString>
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
    QString storedPinHash;
    QFile hashFile(":/data/pin_hash.txt");
    if (!hashFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть файл с хешем ПИН-кода");
        return false;
    }
    QTextStream in(&hashFile);
    storedPinHash = in.readLine().trimmed(); // Читаем первую строку и убираем пробелы
    hashFile.close();


    // ввод пинкода пользователем
    QString enteredPin = QInputDialog::getText(this, tr("ПИН-код"), tr("Введите ПИН-код:"), QLineEdit::Password);


    if (enteredPin.isEmpty()) {
        return false;
    }

    // вычисление хэша от пинкода-
    QByteArray pinBytes = enteredPin.toUtf8();
    QByteArray hashBytes = QCryptographicHash::hash(pinBytes, QCryptographicHash::Md5);
    QString enteredPinHash = hashBytes.toHex();

    // Сравниваем вычисленный хеш с хешем из файла ресурсов
    if (enteredPinHash == storedPinHash) {
        return true;
    } else {
        QMessageBox::warning(this, "Ошибка", "Неверный ПИН");
        return false;
    }
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
