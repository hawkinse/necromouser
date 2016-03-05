#pragma once
#include <cstdint>

struct MouseDataPacket
{
    bool ValidPacket;
    int DirX;
    int DirY;
    bool LeftBtn;
    bool RightBtn;
    bool MidBtn;

    //Set default values
    MouseDataPacket()
    {
        ValidPacket = false;
        DirX = 0;
        DirY = 0;
        LeftBtn = false;
        RightBtn = false;
        MidBtn = false;
    }
};

enum MouseMode
{
    MICROSOFT = 0,
    MOUSESYS,
    NONE
};
