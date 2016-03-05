#include "packetparser.h"

PacketParser::PacketParser()
{
    m_iScrollDivisor = 1;
    m_iAccelerationX = 1;
    m_iAccelerationY = 1;
}

void PacketParser::ChangeSettings(bool emulateMiddleClick, bool emulateScrolling, int scrollDivisor, bool swapAxis, int accelX, int accelY, bool invertX, bool invertY)
{
    m_bEmulateMiddleClick = emulateMiddleClick;
    m_bEmulateScrolling = emulateScrolling;
    m_iScrollDivisor = scrollDivisor;
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

    //Process mouse buttons
    if(((m_mdpLastPacket.LeftBtn && m_mdpLastPacket.RightBtn && !currentPacket.LeftBtn && !currentPacket.RightBtn && m_bEmulateMiddleClick) ||
        (m_mdpLastPacket.MidBtn && !currentPacket.MidBtn && !m_bEmulateMiddleClick)) && (!m_bIsScrolling && m_bMiddleClickDown)) //If we aren't scrolling and either let go of a chord or the middle mouse button
    {
        qDebug() << "Parsed - middle mouse up!";
        m_bMiddleClickDown = false;

        //If we are emulating scrolling, we never sent a mouse down event. Do so here.
        if(m_bEmulateScrolling)
        {
            mouseInput->mi.dwFlags |= MOUSEEVENTF_MIDDLEDOWN;
            SendInput(1, mouseInput, sizeof(*mouseInput));

            //Since this is the start of a new input, we have to clear the flags
            mouseInput->mi.dwFlags = NULL;
        }

        //Send the mouse up event
        mouseInput->mi.dwFlags |= MOUSEEVENTF_MIDDLEUP;
    }
    else if((!m_mdpLastPacket.LeftBtn && !m_mdpLastPacket.RightBtn && currentPacket.LeftBtn && currentPacket.RightBtn && m_bEmulateMiddleClick) ||
            (!m_mdpLastPacket.MidBtn && !m_mdpLastPacket.MidBtn && !m_bEmulateMiddleClick)) //If both buttons were pressed down and we're emulating middle click, or if a real middle button was pressed
    {
        qDebug() << "Parsed - middle mouse down!";
        m_bMiddleClickDown = true;

        //Only do the middle button down event if we aren't emulating scrolling, since it uses middle mouse down to trigger.
        //Middle click can still be done by not moving the mouse, both down and up events will fire when the button is up
        if(!m_bEmulateScrolling)
            mouseInput->mi.dwFlags |= MOUSEEVENTF_MIDDLEDOWN;
    }
    else if((m_bEmulateScrolling && !m_bIsScrolling) &&
            ((m_mdpLastPacket.LeftBtn && m_mdpLastPacket.RightBtn && currentPacket.LeftBtn && currentPacket.RightBtn && m_bEmulateMiddleClick) ||
             (m_mdpLastPacket.MidBtn && currentPacket.MidBtn && !m_bEmulateMiddleClick))) //If move the mouse while middle is down and scroll emulation is enabled
    {
        qDebug() << "Parsed - scrolling has begun!";
        m_bIsScrolling = true;
    }
    else if((m_bEmulateScrolling && m_bIsScrolling) &&
            ((m_mdpLastPacket.LeftBtn && m_mdpLastPacket.RightBtn && !currentPacket.LeftBtn && !currentPacket.RightBtn && m_bEmulateMiddleClick) ||
             (m_mdpLastPacket.MidBtn && !currentPacket.MidBtn && !m_bEmulateMiddleClick))) //If we are scrolling and either let go of a chord or the middle mouse button
    {
        qDebug() << "Parsed - scrolling has stopped!";
        m_bIsScrolling = false;
        m_bMiddleClickDown = false;
    }
    else
    {
        //Parse other button inputs separate since they're independent of each other.
        //Check for both buttons not being down as a way to prevent scrolling bug where no mouse can click on anything outside the window

        //Left Button
        if(!m_mdpLastPacket.LeftBtn && !m_mdpLastPacket.RightBtn && currentPacket.LeftBtn)
        {
            qDebug() << "Parsed - left mouse down!";
            mouseInput->mi.dwFlags |= MOUSEEVENTF_LEFTDOWN;
        }
        else if(m_mdpLastPacket.LeftBtn && !currentPacket.LeftBtn)
        {
            //Separate handle for middle click up, for if only one button is released
            if(m_bMiddleClickDown && m_bEmulateMiddleClick)
            {
                m_bMiddleClickDown = false;
                m_bIsScrolling = false;
                qDebug() << "Parsed - middle mouse up! (Else Block)";
                mouseInput->mi.dwFlags |= MOUSEEVENTF_MIDDLEUP;
            }
            else //Now that we know this isn't part of a middle click, we can send the left button up event
            {
                qDebug() << "Parsed - left mouse up!";
                mouseInput->mi.dwFlags |= MOUSEEVENTF_LEFTUP;
            }
        }

        //Right Button
        if(!m_mdpLastPacket.RightBtn && !m_mdpLastPacket.LeftBtn && currentPacket.RightBtn)
        {
            qDebug() << "Parsed - right mouse down!";
            mouseInput->mi.dwFlags |= MOUSEEVENTF_RIGHTDOWN;
        }
        else if(m_mdpLastPacket.RightBtn && !currentPacket.RightBtn)
        {
            //Separate handle for middle click up, for if only one button is released
            if(m_bMiddleClickDown && m_bEmulateMiddleClick)
            {
                m_bMiddleClickDown = false;
                m_bIsScrolling = false;
                qDebug() << "Parsed - middle mouse up! (Else Block)";
                mouseInput->mi.dwFlags |= MOUSEEVENTF_MIDDLEUP;
            }
            else //Now that we know this isn't part of a middle click, we can send the left button up event
            {
                qDebug() << "Parsed - right mouse up!";
                mouseInput->mi.dwFlags |= MOUSEEVENTF_RIGHTUP;
            }
        }

        //Middle Button.
        if(!m_mdpLastPacket.MidBtn && currentPacket.MidBtn)
        {
            qDebug() << "Parsed - middle mouse down! (Native button)";
            mouseInput->mi.dwFlags |= MOUSEEVENTF_MIDDLEDOWN;
        }
        else if(m_mdpLastPacket.MidBtn && !currentPacket.MidBtn)
        {
            m_bMiddleClickDown = false;
            m_bIsScrolling = false;
            qDebug() << "Parsed - middle mouse up! (Native button)";
            mouseInput->mi.dwFlags |= MOUSEEVENTF_MIDDLEUP;
        }

    }

    //Process cursor movement
    if(!m_bIsScrolling)
    {
        mouseInput->mi.dwFlags |= MOUSEEVENTF_MOVE;
        mouseInput->mi.dx = currentPacket.DirX;
        mouseInput->mi.dy = currentPacket.DirY;
    }
    else //Process scrolling
    {
        if(abs(currentPacket.DirX) > abs(currentPacket.DirY)) //Scroll sideways
        {
            mouseInput->mi.dwFlags |= MOUSEEVENTF_HWHEEL;
            mouseInput->mi.mouseData = MOUSE_SCROLL_CONST * (currentPacket.DirX / m_iScrollDivisor);

            //If our divisor resulted in 0 movement even with moving the mouse, fix it so you get at least one "roll"
            if((currentPacket.DirX > 0) && (mouseInput->mi.mouseData == 0))
                mouseInput->mi.mouseData = MOUSE_SCROLL_CONST;
        }
        else if(abs(currentPacket.DirY) > abs(currentPacket.DirX)) //Scroll up and down
        {
            mouseInput->mi.dwFlags |= MOUSEEVENTF_WHEEL;
            mouseInput->mi.mouseData = -MOUSE_SCROLL_CONST * (currentPacket.DirY / m_iScrollDivisor);

            //If our divisor resulted in 0 movement without moving the mouse, fix it so you get at least one "roll"
            if((currentPacket.DirY > 0) && (mouseInput->mi.mouseData == 0))
                mouseInput->mi.mouseData = -MOUSE_SCROLL_CONST;
        }
    }

    //Send our constructed input
    SendInput(1, mouseInput, sizeof(*mouseInput));

    //Sanity check - force scrolling off if the buttons are up
    if((m_bEmulateScrolling && m_bIsScrolling) && ((!currentPacket.MidBtn && !m_bEmulateMiddleClick) || (m_bEmulateMiddleClick && !currentPacket.LeftBtn && !currentPacket.RightBtn)))
    {
        m_bIsScrolling = false;
        m_bMiddleClickDown = false;
    }

    m_mdpLastPacket = currentPacket;
}
