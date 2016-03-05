#pragma once

#include <windows.h>
#include <QtDebug>
#include "qextserialport.h"
#include "qextserialenumerator.h"
#include "defines.h"

class SerialMouseReader
{
protected:
    QextSerialPort* mousePort;
    int8_t* incompletePacketBuffer;
    int incompletePacketBufferSize;

public:
    SerialMouseReader(QextSerialPort* &initMousePort, QextPortInfo* portInfo);
    ~SerialMouseReader();

    virtual MouseDataPacket GetNextPacket(int8_t* buffer, qint64 &bufferIndex, qint64 bufferSize) = 0;
    virtual MouseMode GetMouseMode() = 0;
};
