#include "serialmousereader.h"

SerialMouseReader::SerialMouseReader(QextSerialPort* &initMousePort, QextPortInfo* portInfo)
{

}

SerialMouseReader::~SerialMouseReader()
{

}

MouseDataPacket SerialMouseReader::GetNextPacket(int8_t* buffer, qint64 &bufferIndex, qint64 bufferSize)
{
    MouseDataPacket packet;
    return packet;
}

MouseMode SerialMouseReader::GetMouseMode()
{
    return MouseMode::NONE;
}
