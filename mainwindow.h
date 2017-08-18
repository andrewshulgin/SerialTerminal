#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QtSerialPort/QSerialPort>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void fillPortsInfo();
    void openSerialPort();
    void closeSerialPort();
    void writeData();
    void readData();
    void handleError(QSerialPort::SerialPortError error);
    void clearConsole();
    void clearInput();
    void insertSpecialChar00();
    void insertSpecialChar01();
    void insertSpecialChar02();
    void insertSpecialChar03();
    void insertSpecialChar04();
    void insertSpecialChar05();
    void insertSpecialChar06();
    void insertSpecialChar07();
    void insertSpecialChar08();
    void insertSpecialChar09();
    void insertSpecialChar0A();
    void insertSpecialChar0B();
    void insertSpecialChar0C();
    void insertSpecialChar0D();
    void insertSpecialChar0E();
    void insertSpecialChar0F();
    void insertSpecialChar10();
    void insertSpecialChar11();
    void insertSpecialChar12();
    void insertSpecialChar13();
    void insertSpecialChar14();
    void insertSpecialChar15();
    void insertSpecialChar16();
    void insertSpecialChar17();
    void insertSpecialChar18();
    void insertSpecialChar19();
    void insertSpecialChar1A();
    void insertSpecialChar1B();
    void insertSpecialChar1C();
    void insertSpecialChar1D();
    void insertSpecialChar1E();
    void insertSpecialChar1F();

private:
    void fillPortsParameters();
    void showStatusMessage(const QString &message);

    Ui::MainWindow *ui;
    QLabel *status;
    QSerialPort *serial;
    QTimer *fillPortsTimer;
};

#endif // MAINWINDOW_H
