#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <QLabel>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    serial = new QSerialPort(this);

    ui->serialPortBox->setEnabled(true);
    ui->baudRateBox->setEnabled(true);
    ui->dataBitsBox->setEnabled(true);
    ui->parityBox->setEnabled(true);
    ui->stopBitsBox->setEnabled(true);
    ui->flowControlBox->setEnabled(true);
    ui->connectButton->setEnabled(true);
    ui->disconnectButton->setEnabled(false);
    ui->sendButton->setEnabled(false);

    status = new QLabel;
    ui->statusBar->addWidget(status);

    connect(serial, static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
            this, &MainWindow::handleError);
    connect(serial, &QSerialPort::readyRead, this, &MainWindow::readData);

    connect(ui->connectButton,      SIGNAL (clicked()), this, SLOT (openSerialPort()));
    connect(ui->disconnectButton,   SIGNAL (clicked()), this, SLOT (closeSerialPort()));
    connect(ui->clearConsoleButton, SIGNAL (clicked()), this, SLOT (clearConsole()));
    connect(ui->sendButton,         SIGNAL (clicked()), this, SLOT (writeData()));
    connect(ui->clearInputButton,   SIGNAL (clicked()), this, SLOT (clearInput()));
    connect(ui->x00,                SIGNAL (clicked()), this, SLOT (insertSpecialChar00()));
    connect(ui->x01,                SIGNAL (clicked()), this, SLOT (insertSpecialChar01()));
    connect(ui->x02,                SIGNAL (clicked()), this, SLOT (insertSpecialChar02()));
    connect(ui->x03,                SIGNAL (clicked()), this, SLOT (insertSpecialChar03()));
    connect(ui->x04,                SIGNAL (clicked()), this, SLOT (insertSpecialChar04()));
    // connect(ui->x05,             SIGNAL (clicked()), this, SLOT (insertSpecialChar05()));
    // connect(ui->x06,             SIGNAL (clicked()), this, SLOT (insertSpecialChar06()));
    // connect(ui->x07,             SIGNAL (clicked()), this, SLOT (insertSpecialChar07()));
    // connect(ui->x08,             SIGNAL (clicked()), this, SLOT (insertSpecialChar08()));
    connect(ui->x09,                SIGNAL (clicked()), this, SLOT (insertSpecialChar09()));
    connect(ui->x0A,                SIGNAL (clicked()), this, SLOT (insertSpecialChar0A()));
    // connect(ui->x0B,             SIGNAL (clicked()), this, SLOT (insertSpecialChar0B()));
    // connect(ui->x0C,             SIGNAL (clicked()), this, SLOT (insertSpecialChar0C()));
    connect(ui->x0D,                SIGNAL (clicked()), this, SLOT (insertSpecialChar0D()));
    // connect(ui->x0E,             SIGNAL (clicked()), this, SLOT (insertSpecialChar0E()));
    // connect(ui->x0F,             SIGNAL (clicked()), this, SLOT (insertSpecialChar0F()));
    // connect(ui->x10,             SIGNAL (clicked()), this, SLOT (insertSpecialChar10()));
    connect(ui->x11,                SIGNAL (clicked()), this, SLOT (insertSpecialChar11()));
    connect(ui->x12,                SIGNAL (clicked()), this, SLOT (insertSpecialChar12()));
    connect(ui->x13,                SIGNAL (clicked()), this, SLOT (insertSpecialChar13()));
    connect(ui->x14,                SIGNAL (clicked()), this, SLOT (insertSpecialChar14()));
    // connect(ui->x15,             SIGNAL (clicked()), this, SLOT (insertSpecialChar15()));
    // connect(ui->x16,             SIGNAL (clicked()), this, SLOT (insertSpecialChar16()));
    // connect(ui->x17,             SIGNAL (clicked()), this, SLOT (insertSpecialChar17()));
    // connect(ui->x18,             SIGNAL (clicked()), this, SLOT (insertSpecialChar18()));
    // connect(ui->x19,             SIGNAL (clicked()), this, SLOT (insertSpecialChar19()));
    // connect(ui->x1A,             SIGNAL (clicked()), this, SLOT (insertSpecialChar1A()));
    // connect(ui->x1B,             SIGNAL (clicked()), this, SLOT (insertSpecialChar1B()));
    connect(ui->x1C,                SIGNAL (clicked()), this, SLOT (insertSpecialChar1C()));
    connect(ui->x1D,                SIGNAL (clicked()), this, SLOT (insertSpecialChar1D()));
    connect(ui->x1E,                SIGNAL (clicked()), this, SLOT (insertSpecialChar1E()));
    connect(ui->x1F,                SIGNAL (clicked()), this, SLOT (insertSpecialChar1F()));

    fillPortsInfo();
    fillPortsParameters();

    fillPortsTimer = new QTimer(this);
    connect(fillPortsTimer, SIGNAL(timeout()), this, SLOT(fillPortsInfo()));
    fillPortsTimer->start(500);
}

MainWindow::~MainWindow()
{
    closeSerialPort();
    delete fillPortsTimer;
    delete ui;
}

void MainWindow::fillPortsInfo()
{
    if (serial->isOpen())
        return;
    const QList<QSerialPortInfo> infos = QSerialPortInfo::availablePorts();
    QStringList ports;
    for (const QSerialPortInfo &info : infos)
        ports.append(info.portName());
    for (int i = 0; i < ui->serialPortBox->count(); i++)
        if (!ports.contains(ui->serialPortBox->itemText(i)))
            ui->serialPortBox->removeItem(i);
    for (const QString port : ports)
        if (ui->serialPortBox->findText(port) == -1)
            ui->serialPortBox->addItem(port, port);
}

void MainWindow::fillPortsParameters()
{
    ui->baudRateBox->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    ui->baudRateBox->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    ui->baudRateBox->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    ui->baudRateBox->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
    ui->baudRateBox->setCurrentIndex(3);

    ui->dataBitsBox->addItem(QStringLiteral("5"), QSerialPort::Data5);
    ui->dataBitsBox->addItem(QStringLiteral("6"), QSerialPort::Data6);
    ui->dataBitsBox->addItem(QStringLiteral("7"), QSerialPort::Data7);
    ui->dataBitsBox->addItem(QStringLiteral("8"), QSerialPort::Data8);
    ui->dataBitsBox->setCurrentIndex(3);

    ui->parityBox->addItem(tr("None"), QSerialPort::NoParity);
    ui->parityBox->addItem(tr("Even"), QSerialPort::EvenParity);
    ui->parityBox->addItem(tr("Odd"), QSerialPort::OddParity);
    ui->parityBox->addItem(tr("Mark"), QSerialPort::MarkParity);
    ui->parityBox->addItem(tr("Space"), QSerialPort::SpaceParity);

    ui->stopBitsBox->addItem(QStringLiteral("1"), QSerialPort::OneStop);
#ifdef Q_OS_WIN
    ui->stopBitsBox->addItem(tr("1.5"), QSerialPort::OneAndHalfStop);
#endif
    ui->stopBitsBox->addItem(QStringLiteral("2"), QSerialPort::TwoStop);

    ui->flowControlBox->addItem(tr("None"), QSerialPort::NoFlowControl);
    ui->flowControlBox->addItem(tr("RTS/CTS"), QSerialPort::HardwareControl);
    ui->flowControlBox->addItem(tr("XON/XOFF"), QSerialPort::SoftwareControl);
}

void MainWindow::openSerialPort()
{
    serial->setPortName(ui->serialPortBox->currentText());
    serial->setBaudRate(static_cast<QSerialPort::BaudRate>(ui->baudRateBox->itemData(ui->baudRateBox->currentIndex()).toInt()));
    serial->setDataBits(static_cast<QSerialPort::DataBits>(ui->dataBitsBox->itemData(ui->dataBitsBox->currentIndex()).toInt()));
    serial->setParity(static_cast<QSerialPort::Parity>(ui->parityBox->itemData(ui->parityBox->currentIndex()).toInt()));
    serial->setStopBits(static_cast<QSerialPort::StopBits>(ui->stopBitsBox->itemData(ui->stopBitsBox->currentIndex()).toInt()));
    serial->setFlowControl(static_cast<QSerialPort::FlowControl>(ui->flowControlBox->itemData(ui->flowControlBox->currentIndex()).toInt()));
    if (serial->open(QIODevice::ReadWrite)) {
        ui->serialPortBox->setEnabled(false);
        ui->baudRateBox->setEnabled(false);
        ui->dataBitsBox->setEnabled(false);
        ui->parityBox->setEnabled(false);
        ui->stopBitsBox->setEnabled(false);
        ui->flowControlBox->setEnabled(false);
        ui->connectButton->setEnabled(false);
        ui->disconnectButton->setEnabled(true);
        ui->sendButton->setEnabled(true);
        showStatusMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
                          .arg(ui->serialPortBox->currentText())
                          .arg(ui->baudRateBox->currentText())
                          .arg(ui->dataBitsBox->currentText())
                          .arg(ui->parityBox->currentText())
                          .arg(ui->stopBitsBox->currentText())
                          .arg(ui->flowControlBox->currentText()));
    } else {
        QMessageBox::critical(this, tr("Error"), serial->errorString());
        showStatusMessage(tr("Open error"));
    }
}

void MainWindow::closeSerialPort()
{
    if (serial->isOpen())
        serial->close();
    fillPortsInfo();
    ui->serialPortBox->setEnabled(true);
    ui->baudRateBox->setEnabled(true);
    ui->dataBitsBox->setEnabled(true);
    ui->parityBox->setEnabled(true);
    ui->stopBitsBox->setEnabled(true);
    ui->flowControlBox->setEnabled(true);
    ui->connectButton->setEnabled(true);
    ui->disconnectButton->setEnabled(false);
    ui->sendButton->setEnabled(false);
    showStatusMessage(tr("Disconnected"));
}

void MainWindow::writeData()
{
    QString input = ui->input->text();
    QByteArray output;
    foreach (QChar c, input) {
        ushort uc = c.unicode();
        if ((0x2400 <= uc && uc < 0x2420) || uc == 0x247F) {
            output.append(QChar(uc - 0x2400).toLatin1());
        } else {
            output.append(c.toLatin1());
        }
    }
    serial->write(output);
}

void MainWindow::readData()
{
    QByteArray data = serial->readAll();
    foreach (QChar c, data) {
        ui->console->moveCursor(QTextCursor::End);
        ushort uc = c.unicode();
        if (uc < 0x20 || uc == 0x7F) {
            switch (uc) {
            case 0x0a:
            case 0x04:
                ui->console->insertPlainText(QChar(uc + 0x2400));
                ui->console->moveCursor(QTextCursor::End);
                ui->console->insertPlainText("\n");
                break;
            default:
                ui->console->insertPlainText(QChar(uc + 0x2400));
                break;
            }
        } else {
            ui->console->insertPlainText(c);
        }
        ui->console->moveCursor(QTextCursor::End);
    }
}

void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError) {
        QMessageBox::critical(this, tr("Critical Error"), serial->errorString());
        closeSerialPort();
    }
}

void MainWindow::showStatusMessage(const QString &message)
{
    status->setText(message);
}

void MainWindow::clearConsole() {
    ui->console->clear();
}

void MainWindow::clearInput() {
    ui->input->clear();
}

void MainWindow::insertSpecialChar00(){ui->input->insert(QChar(0x2400));}
void MainWindow::insertSpecialChar01(){ui->input->insert(QChar(0x2401));}
void MainWindow::insertSpecialChar02(){ui->input->insert(QChar(0x2402));}
void MainWindow::insertSpecialChar03(){ui->input->insert(QChar(0x2403));}
void MainWindow::insertSpecialChar04(){ui->input->insert(QChar(0x2404));}
void MainWindow::insertSpecialChar05(){ui->input->insert(QChar(0x2405));}
void MainWindow::insertSpecialChar06(){ui->input->insert(QChar(0x2406));}
void MainWindow::insertSpecialChar07(){ui->input->insert(QChar(0x2407));}
void MainWindow::insertSpecialChar08(){ui->input->insert(QChar(0x2408));}
void MainWindow::insertSpecialChar09(){ui->input->insert(QChar(0x2409));}
void MainWindow::insertSpecialChar0A(){ui->input->insert(QChar(0x240A));}
void MainWindow::insertSpecialChar0B(){ui->input->insert(QChar(0x240B));}
void MainWindow::insertSpecialChar0C(){ui->input->insert(QChar(0x240C));}
void MainWindow::insertSpecialChar0D(){ui->input->insert(QChar(0x240D));}
void MainWindow::insertSpecialChar0E(){ui->input->insert(QChar(0x240E));}
void MainWindow::insertSpecialChar0F(){ui->input->insert(QChar(0x240F));}
void MainWindow::insertSpecialChar10(){ui->input->insert(QChar(0x2410));}
void MainWindow::insertSpecialChar11(){ui->input->insert(QChar(0x2411));}
void MainWindow::insertSpecialChar12(){ui->input->insert(QChar(0x2412));}
void MainWindow::insertSpecialChar13(){ui->input->insert(QChar(0x2413));}
void MainWindow::insertSpecialChar14(){ui->input->insert(QChar(0x2414));}
void MainWindow::insertSpecialChar15(){ui->input->insert(QChar(0x2415));}
void MainWindow::insertSpecialChar16(){ui->input->insert(QChar(0x2416));}
void MainWindow::insertSpecialChar17(){ui->input->insert(QChar(0x2417));}
void MainWindow::insertSpecialChar18(){ui->input->insert(QChar(0x2418));}
void MainWindow::insertSpecialChar19(){ui->input->insert(QChar(0x2419));}
void MainWindow::insertSpecialChar1A(){ui->input->insert(QChar(0x241A));}
void MainWindow::insertSpecialChar1B(){ui->input->insert(QChar(0x241B));}
void MainWindow::insertSpecialChar1C(){ui->input->insert(QChar(0x241C));}
void MainWindow::insertSpecialChar1D(){ui->input->insert(QChar(0x241D));}
void MainWindow::insertSpecialChar1E(){ui->input->insert(QChar(0x241E));}
void MainWindow::insertSpecialChar1F(){ui->input->insert(QChar(0x241F));}
