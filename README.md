# README #

Necromouser is a C++ application enabling the use of old serial mice on modern Windows machines. Specifically, it supports devices using the Microsoft Mouse or Mouse Systems protocols and provides additional features not present in the Microsoft Mouse driver included with Windows.

### Dependencies ###
* [Windows SDK](https://www.microsoft.com/en-us/download/details.aspx?id=8279)
* [Windows WDK](https://msdn.microsoft.com/en-us/windows/hardware/hh852365.aspx)
* [QtCreator](https://www.qt.io/download/)
* [QextSerialPort](https://github.com/qextserialport/qextserialport)

### Setup ###

1. Install SDK/WDK/QtCreator
2. Clone the QextSerialPort repo into Necromouser\3rdparty
3. Open Necromouser in QtCreator and Build->Run qMake
4. Build->Build All

### Important Notes ###

* At this time, if you are using a Microsoft Mode mouse, you must disable it first in Device Manager. 
* Necromouser should be ran with administrator privileges if possible, otherwise it will not be able to interact with certain applications (ex. Task Manager).
* Packet parsing is currently somewhat buggy with regards to middle click/scrolling emulation. If you find a button gets stuck, click/release the button with another mouse to un-stick it.

### License ###

As per the community edition QtCreator license agreement, Necromouser is released under the LGPL v3. 