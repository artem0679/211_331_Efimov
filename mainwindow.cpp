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
#include <QFileDialog>

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

    loadDataAndDisplay(":/data/broke_data.csv"); // Загрузим данные при старте
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
    textEditData->append("Card Number,Route,Time (time_t),Hash");

    QString line;
    QString previousHash = ""; // Начальный хеш для первой записи
    bool integrityCheckFailed = false; // Флаг нарушения целостности

    while (!in.atEnd()) {
        line = in.readLine().trimmed();
        if (line.isEmpty()) continue;

        QStringList fields = line.split(',', Qt::SkipEmptyParts);
        if (fields.size() != 4) {
            textEditData->append("<span style=\"color:red;\">Ошибка: неверный формат -> " + line + "</span>");
            integrityCheckFailed = true;
            continue;
        }

        QString cardNumber = fields[0];
        QString route = fields[1];
        QString timeStr = fields[2];
        QString hashFromFile = fields[3];

        // Проверка целостности
        QString dataToHash = cardNumber + route + timeStr + previousHash;
        QByteArray dataBytes = dataToHash.toUtf8();
        QByteArray calculatedHashBytes = QCryptographicHash::hash(dataBytes, QCryptographicHash::Md5);
        QString calculatedHashHex = calculatedHashBytes.toHex();

        if (calculatedHashHex != hashFromFile.toLower()) {
            integrityCheckFailed = true;
            previousHash = hashFromFile.toLower();
            textEditData->append("<span style=\"background-color:red; color:white;\">" + line + " *** ОШИБКА ***</span>");
        } else {
            if (integrityCheckFailed) {
                textEditData->append("<span style=\"background-color:red; color:white;\">" + line + " *** ПОСЛЕДОВАТЕЛЬНАЯ ОШИБКА ***</span>");
                // previousHash остаётся от битой записи
            } else {
                textEditData->append(line);
                // Обновляем previousHash только если цепочка цела
                previousHash = calculatedHashHex;
            }
        }

    }

    if (integrityCheckFailed) {
        textEditData->append("<br><span style=\"color:red;\"><b>*** Ошибка целостности! ***</b></span>");
    } else {
        textEditData->append("<br><span style=\"color:green;\"><b>*** Проерка целостности пройдена! ***</b></span>");
    }

    file.close();
}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButtonOpen_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("Open CSV File"),
        ":/data", // Начальная директория
        tr("CSV Files (*.csv);;All Files (*)") // фильтр файлов
        );

    if (!fileName.isEmpty()) {
        // Если файл выбран, загружаем его
        loadDataAndDisplay(fileName);
    }
}

