#pragma once

#include "defines.h"
#include "serialmousereader.h"

class MicrosoftReader : public SerialMouseReader
{
private:
    static const int8_t BIT_START = 0x40;
    static const int8_t BIT_BTNLEFT = 0x20;
    static const int8_t BIT_BTNRIGHT = 0x10;
    static const int8_t BIT_DIRX = 0x03;
    static const int8_t BIT_DIRY = 0x0C;

public:
    MicrosoftReader(QextSerialPort* &initMousePort, QextPortInfo* portInfo);
    ~MicrosoftReader();

    virtual MouseDataPacket GetNextPacket(int8_t* buffer, qint64 &bufferIndex, qint64 bufferSize);
    virtual MouseMode GetMouseMode();
};
