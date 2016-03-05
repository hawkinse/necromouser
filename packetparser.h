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

    bool m_bEmulateMiddleClick;
    bool m_bEmulateScrolling;
    int m_iScrollDivisor;
    int m_iAccelerationX;
    int m_iAccelerationY;
    bool m_bSwapAxis;
    bool m_bInvertX;
    bool m_bInvertY;

    bool m_bIsScrolling;
    bool m_bMiddleClickDown;



public:
    PacketParser();

    void ChangeSettings(bool emulateMiddleClick, bool emulateScrolling, int scrollDivisor, bool swapAxis, int accelX, int accelY, bool invertX, bool invertY);
    void Parse(MouseDataPacket currentPacket);
};
