#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QCryptographicHash>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButtonOpen_clicked();

private:
    Ui::MainWindow *ui;
    QTextEdit *textEditData;

    void loadDataAndDisplay(const QString &fileName);
    void loadDataAndDisplayFromData(const QByteArray &data);
    bool verifyPin();
};
#endif // MAINWINDOW_H
