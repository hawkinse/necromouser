#include "microsoftreader.h"

MicrosoftReader::MicrosoftReader(QextSerialPort* &initMousePort, QextPortInfo* portInfo)
    : SerialMouseReader(initMousePort, portInfo)
{
    incompletePacketBuffer = new int8_t[3];

    initMousePort = new QextSerialPort(portInfo->portName, QextSerialPort::EventDriven);
    mousePort = initMousePort;

    mousePort->setBaudRate((BaudRateType::BAUD1200));
    mousePort->setFlowControl(FlowType::FLOW_HARDWARE); //?
    mousePort->setParity(ParityType::PAR_NONE);
    mousePort->setDataBits(DataBitsType::DATA_7);
    mousePort->setStopBits(StopBitsType::STOP_1);
    mousePort->setDtr(true);
    mousePort->setRts(true);

    qDebug() << "Serial port set for Microsoft Protocol, ready to open!\n";
}

MicrosoftReader::~MicrosoftReader()
{

}

MouseDataPacket MicrosoftReader::GetNextPacket(int8_t *buffer, qint64 &bufferIndex, qint64 bufferSize)
{
    MouseDataPacket packet;
    int8_t* packetBytes = new int8_t[3];
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
        if(!(buffer[bufferIndex] & BIT_START))
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
        while(packetByteIndex < 3 && (bufferSize - bufferIndex))
        {
            packetBytes[packetByteIndex++] = buffer[bufferIndex++];
        }
    }

    //See if we've managed to store a complete backet in packetBytes
    if(packetByteIndex > 2)
    {
        packet.LeftBtn = (packetBytes[0] & BIT_BTNLEFT) ? true : false;
        packet.RightBtn = (packetBytes[0] & BIT_BTNRIGHT) ? true : false;

        packet.DirX = ((packetBytes[0] & BIT_DIRX) * 64);
        packet.DirY = ((packetBytes[0] & BIT_DIRY) * 16);

        packet.DirX += packetBytes[1];
        packet.DirY += packetBytes[2];

        //Correct sign on direction
        if(packet.DirX > 127)
            packet.DirX -= 256;
        if(packet.DirY > 127)
            packet.DirY -= 256;

        //Mark packet as valid
        packet.ValidPacket = true;

        qDebug() << "Packet: " << (packet.LeftBtn ? "L " : "  ") << (packet.RightBtn ? "R " : "  ") << "X: " << packet.DirX << "Y: " << packet.DirY << "\n";
    }
    else //packetBytes is incomplete. Store it in global
    {
        incompletePacketBufferSize = packetByteIndex;
        for(int copyIndex = 0; copyIndex < 3; copyIndex++)
            incompletePacketBuffer[copyIndex] = packetBytes[copyIndex];
    }

    delete packetBytes;
    return packet;
}

MouseMode MicrosoftReader::GetMouseMode()
{
    return MouseMode::MOUSESYS;
}
