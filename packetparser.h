#pragma once

#include <Windows.h> //Needed to control the cursor
#include <cmath>
#include <QtDebug>
#include "defines.h"

#define MOUSE_SCROLL_CONST 120

class PacketParser
{
private:

    MouseDataPacket m_mdpLastPacket;

    int m_iAccelerationX;
    int m_iAccelerationY;
    bool m_bSwapAxis;
    bool m_bInvertX;
    bool m_bInvertY;

public:
    PacketParser();

    void ChangeSettings(bool swapAxis, int accelX, int accelY, bool invertX, bool invertY);
    void Parse(MouseDataPacket currentPacket);
};
