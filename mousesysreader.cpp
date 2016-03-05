#include "mousesysreader.h"

MouseSysReader::MouseSysReader(QextSerialPort* &initMousePort, QextPortInfo* portInfo)
    : SerialMouseReader(initMousePort, portInfo)
{
    incompletePacketBuffer = new int8_t[5];

    initMousePort = new QextSerialPort(portInfo->portName, QextSerialPort::EventDriven);
    mousePort = initMousePort;

    mousePort->setBaudRate((BaudRateType::BAUD1200)); //The Mouse Systems protocal supports 300 baud as well, but since they default to 1200 it doesn't seem worth supporting.
    mousePort->setFlowControl(FlowType::FLOW_HARDWARE);
    mousePort->setParity(ParityType::PAR_NONE);
    mousePort->setDataBits(DataBitsType::DATA_8);
    mousePort->setStopBits(StopBitsType::STOP_1);
    mousePort->setDtr(true);
    mousePort->setRts(true);

    qDebug() << "Serial port set for Mouse Systems Protocol, ready to open!\n";
}

MouseSysReader::~MouseSysReader()
{

}

MouseDataPacket MouseSysReader::GetNextPacket(int8_t* buffer, qint64 &bufferIndex, qint64 bufferSize)
{
    //Code is in process of being adapted from Microsoft Mode code
    MouseDataPacket packet;
    int8_t* packetBytes = new int8_t[5];
    int packetByteIndex = 0;

    if(!mousePort)
    {
        qDebug() << "Serial port is not initialized!\n";
        return packet;
    }

    qDebug() << "Buffer size: " << bufferSize << " Buffer index: " << bufferIndex;

    //There are potentially enough bytes to make a packet!
    if(bufferSize - bufferIndex > 0)
    {
        //If this is not the first byte in the chain, see if we have any stored from a previous attempt
        if(!((uint8_t)buffer[bufferIndex] >> START_SHIFT == START_ID))
        {
            if(incompletePacketBufferSize > 0)
            {
                for(int incompleteIndex = 0; incompleteIndex < incompletePacketBufferSize; incompleteIndex++)
                {
                    packetBytes[packetByteIndex++] = incompletePacketBuffer[incompleteIndex];
                }
            }
            else
            {
                qDebug() << "Skipping packet with missing start data!";
                bufferIndex++;
                return packet;
            }
        }

        //Build as much packet data as we can
        while(packetByteIndex < 5 && (bufferSize - bufferIndex))
        {
            packetBytes[packetByteIndex++] = buffer[bufferIndex++];
        }
    }

    //See if we've managed to store a complete backet in packetBytes
    if(packetByteIndex > 4)
    {
        packet.LeftBtn = (packetBytes[BYTE_START] & BIT_BTNLEFT) ? false : true;
        packet.RightBtn = (packetBytes[BYTE_START] & BIT_BTNRIGHT) ? false : true;
        packet.MidBtn = (packetBytes[BYTE_START] & BIT_BTNMID) ? false : true;

        packet.DirX = packetBytes[BYTE_X1];
        packet.DirY = packetBytes[BYTE_Y1];

        packet.DirX += packetBytes[BYTE_X2];
        packet.DirY += packetBytes[BYTE_Y2];

        //Mouse Systems mode has a reversed Y axis
        packet.DirY *= -1;

        //Mark packet as valid
        packet.ValidPacket = true;

        qDebug() << "Packet: " << (packet.LeftBtn ? "L " : "  ") << (packet.MidBtn ? "M " : "  ") << (packet.RightBtn ? "R " : "  ") << "X: " << packet.DirX << "Y: " << packet.DirY << "\n";
    }
    else //packetBytes is incomplete. Store it in global
    {
        incompletePacketBufferSize = packetByteIndex;
        for(int copyIndex = 0; copyIndex < 5; copyIndex++)
            incompletePacketBuffer[copyIndex] = packetBytes[copyIndex];
    }

    delete packetBytes;
    return packet;
}

MouseMode MouseSysReader::GetMouseMode()
{
    return MouseMode::MOUSESYS;
}
