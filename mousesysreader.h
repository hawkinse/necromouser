#pragma once

#include "defines.h"
#include "serialmousereader.h"

class MouseSysReader : public SerialMouseReader
{

private:
    static const int8_t START_ID = 0x10; //The upper five bytes are 10000 bin, 0x10 hex, or 16 dec
    static const int8_t START_SHIFT = 3; //The number of bits to shift in order to hit the start ID

    static const int8_t BIT_BTNLEFT = 0x04;
    static const int8_t BIT_BTNMID = 0x02;
    static const int8_t BIT_BTNRIGHT = 0x01;

    //Start byte contains the START_ID key and mouse button state
    static const int8_t BYTE_START = 0;

    static const int8_t BYTE_X1 = 1;
    static const int8_t BYTE_Y1 = 2;
    static const int8_t BYTE_X2 = 3;
    static const int8_t BYTE_Y2 = 4;

public:
    MouseSysReader(QextSerialPort* &initMousePort, QextPortInfo* portInfo);
    ~MouseSysReader();

    virtual MouseDataPacket GetNextPacket(int8_t* buffer, qint64 &bufferIndex, qint64 bufferSize);
    virtual MouseMode GetMouseMode();
};
