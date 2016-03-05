#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    mousePort = nullptr;
    reader = nullptr;
    parser = nullptr;

    ui->setupUi(this);

    //Init the parser
    parser = new PacketParser();

    //Add serial ports
    serialPorts = QextSerialEnumerator::getPorts();
    foreach(QextPortInfo portInfo, serialPorts)
        ui->cbSerialPort->addItem(portInfo.portName);

    //Add mouse modes. These must correspond in order with the modes in MouseMode!
    ui->cbProtocal->addItem("Microsoft");
    ui->cbProtocal->addItem("Mouse Systems");

    //Init the slider label
    ui->lblSensitivity->setText(QString::number((ui->hsScrollSensitivity->value())));
}

MainWindow::~MainWindow()
{
    delete ui;

    if(mousePort)
    {
        mousePort->close();
        delete mousePort;
    }

    if(reader)
        delete reader;

    if(parser)
        delete parser;
}

void MainWindow::updateParserSettings()
{
    parser->ChangeSettings(ui->ckbMiddleClickEmulation->isChecked(), ui->ckbScrollEmulation->isChecked(), ui->hsScrollSensitivity->value(),
                           ui->ckbSwapAxis->isChecked(), ui->sbXAccel->value(), ui->sbYAccel->value(), ui->ckbInvertX->isChecked(), ui->ckbInvertY->isChecked());
}

void MainWindow::initReader()
{
    //Close and delete old reader and port
    if(reader)
        delete reader;

    if(mousePort)
    {
        if(mousePort->isOpen())
            mousePort->close();
        delete mousePort;
    }

    //Get our port from the UI
    QextPortInfo* portInfo = new QextPortInfo();
    portInfo->portName = ui->cbSerialPort->currentText();

    //Set up new reader of specified mode
    switch(ui->cbProtocal->currentIndex())
    {
        case MouseMode::MICROSOFT:
            reader = new MicrosoftReader(mousePort, portInfo);
            break;
        case MouseMode::MOUSESYS:
            reader = new MouseSysReader(mousePort, portInfo);
            break;
        default:
            qDebug() << "Unrecognized mode with index " << ui->cbProtocal->currentIndex();
            delete portInfo;
            return;
    }

    //Init parser settings
    updateParserSettings();

    //Open up the mouse port. We have to do this ourseves since SerialMouseReader has no access to our onReadReady call or the packet parser
    if(mousePort->open(QIODevice::ReadOnly) == true)
    {
        connect(mousePort, SIGNAL(readyRead()), this, SLOT(onReadReady()));
        if(!(mousePort->lineStatus() & LS_DSR))
            qDebug() << "Warning - device is not turned on!";
        qDebug() << "Port has been opened: " << mousePort->portName();

    }
    else
        qDebug() << "Failed to open serial port!\n";

    delete portInfo;
}

void MainWindow::onReadReady()
{
    if(!parser || !reader)
    {
        qDebug() << "Parser and reader are not initialized! Bytes will be discarded.\n";

        //Unfortunately, QextSerialPort uses chars instead of int8_t. Hopefully Microsoft never changes size of char.
        char* readBytes = new char[mousePort->bytesAvailable()];
        mousePort->read(readBytes, mousePort->bytesAvailable());
        delete readBytes;
    }

    qint64 bufferSize = mousePort->bytesAvailable();
    char* byteBuffer = new char[bufferSize];
    qint64 bufferIndex = 0;

    mousePort->read(byteBuffer, bufferSize);

    while(bufferIndex < bufferSize)
    {
        MouseDataPacket packet = reader->GetNextPacket((int8_t *)byteBuffer, bufferIndex, bufferSize);
        if(packet.ValidPacket)
        {
            if(!ui->ckbDebugMode->isChecked())
                parser->Parse(packet);

            //Display raw packet on UI
            QString rawPacketString;
            rawPacketString.append("Last Raw Packet: ");
            rawPacketString.append((packet.LeftBtn ? "L " : "  "));
            rawPacketString.append((packet.MidBtn ? "M " : "  "));
            rawPacketString.append((packet.RightBtn ? "R " : "  "));
            rawPacketString.append("X: ");
            rawPacketString.append(QString::number(packet.DirX));
            rawPacketString.append(" Y: ");
            rawPacketString.append(QString::number(packet.DirY));
            ui->lblStatus->setText(rawPacketString);
        }
    }

    delete byteBuffer;
}

void MainWindow::on_ckbMiddleClickEmulation_toggled(bool checked)
{
    updateParserSettings();
}

void MainWindow::on_ckbScrollEmulation_toggled(bool checked)
{
    updateParserSettings();
}

void MainWindow::on_pbStart_clicked()
{
    initReader();
}

void MainWindow::on_hsScrollSensitivity_sliderMoved(int position)
{
    ui->lblSensitivity->setText(QString::number((position)));
    updateParserSettings();
}

void MainWindow::on_ckbSwapAxis_clicked()
{
    updateParserSettings();
}

void MainWindow::on_sbXAccel_valueChanged(int arg1)
{
    updateParserSettings();
}

void MainWindow::on_sbYAccel_valueChanged(const QString &arg1)
{
    updateParserSettings();
}

void MainWindow::on_ckbInvertX_toggled(bool checked)
{
    updateParserSettings();
}

void MainWindow::on_ckbInvertY_toggled(bool checked)
{
    updateParserSettings();
}
