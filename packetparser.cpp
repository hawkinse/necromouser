#include "packetparser.h"

PacketParser::PacketParser()
{
    m_iAccelerationX = 1;
    m_iAccelerationY = 1;
}

void PacketParser::ChangeSettings(bool swapAxis, int accelX, int accelY, bool invertX, bool invertY)
{
    m_bSwapAxis = swapAxis;
    m_iAccelerationX = accelX;
    m_iAccelerationY = accelY;
    m_bInvertX = invertX;
    m_bInvertY = invertY;
}

void PacketParser::Parse(MouseDataPacket currentPacket)
{
    INPUT* mouseInput = new INPUT();
    mouseInput->type = INPUT_MOUSE;

    currentPacket.DirX *= m_iAccelerationX;
    currentPacket.DirY *= m_iAccelerationY;

    if(m_bInvertX)
        currentPacket.DirX *= -1;
    if(m_bInvertY)
        currentPacket.DirY *= -1;

    if(m_bSwapAxis)
    {
        int DirX = currentPacket.DirX;
        currentPacket.DirX = currentPacket.DirY;
        currentPacket.DirY = DirX;
    }

    //Process mouse
    //Left button
    if(!m_mdpLastPacket.LeftBtn && currentPacket.LeftBtn)
    {
        //Left button down
        mouseInput->mi.dwFlags |= MOUSEEVENTF_LEFTDOWN;
    }
    else if(m_mdpLastPacket.LeftBtn && !currentPacket.LeftBtn)
    {
        mouseInput->mi.dwFlags |= MOUSEEVENTF_LEFTUP;
    }

    //Right button
    if(!m_mdpLastPacket.RightBtn && currentPacket.RightBtn)
    {
        //Left button down
        mouseInput->mi.dwFlags |= MOUSEEVENTF_RIGHTDOWN;
    }
    else if(m_mdpLastPacket.RightBtn && !currentPacket.RightBtn)
    {
        mouseInput->mi.dwFlags |= MOUSEEVENTF_RIGHTUP;
    }

    //Middle button
    if(!m_mdpLastPacket.MidBtn && currentPacket.MidBtn)
    {
        //Left button down
        mouseInput->mi.dwFlags |= MOUSEEVENTF_MIDDLEDOWN;
    }
    else if(m_mdpLastPacket.MidBtn && !currentPacket.MidBtn)
    {
        mouseInput->mi.dwFlags |= MOUSEEVENTF_MIDDLEUP;
    }

    mouseInput->mi.dwFlags |= MOUSEEVENTF_MOVE;
    mouseInput->mi.dx = currentPacket.DirX;
    mouseInput->mi.dy = currentPacket.DirY;

    //Send our constructed input
    SendInput(1, mouseInput, sizeof(*mouseInput));

    m_mdpLastPacket = currentPacket;
}
