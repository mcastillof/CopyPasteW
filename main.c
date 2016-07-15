/*
    Copyright Matías Castillo Felmer 2016

    This file is part of CopyPasteW.

    CopyPasteW is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    CopyPasteW is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with CopyPasteW.  If not, see <http://www.gnu.org/licenses/>.

*/

#define WINVER 0x0500
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define VERSION             "CopyPasteW V0.01"
#define LEFT_CLICK          GetKeyState(VK_LBUTTON) < 0
#define MIDDLE_CLICK        GetKeyState(VK_MBUTTON) < 0


long timediff(clock_t t1, clock_t t2) {
    long elapsed;
    elapsed = ((double)t2 - t1) / CLOCKS_PER_SEC * 1000;
    return elapsed;
}


BOOL keyPresed()
{
    int i;

    for(i = 0x05 ; i <= 0xFF ; i++)
        if (GetKeyState(i) < 0)
            return TRUE;
    return FALSE;
}


void leftClick ( )
{
    INPUT   ip;

    ip.type = INPUT_MOUSE;
    ip.ki.wScan = 0; // hardware scan code for key
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;

    ip.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(1,&ip,sizeof(INPUT)); //left click down

    ip.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1,&ip,sizeof(INPUT)); //left click up


}


void copy()
{
    INPUT ip1, ip2;


    ip1.type = INPUT_KEYBOARD;
    ip1.ki.wScan = 0;
    ip1.ki.time = 0;
    ip1.ki.dwExtraInfo = 0;

    ip2 = ip1;


    ip1.ki.wVk = 0x11;
    ip1.ki.dwFlags = 0;
    SendInput(1, &ip1, sizeof(INPUT));  // Press the "Ctrl" key

    ip2.ki.wVk = 0x43;
    ip2.ki.dwFlags = 0;
    SendInput(1, &ip2, sizeof(INPUT));  // Press the "C" key

    ip2.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &ip2, sizeof(INPUT));  // Release the "C" key

    ip1.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &ip1, sizeof(INPUT));  // Release the "Ctrl" key
}


void paste()
{
    INPUT ip1, ip2;

    ip1.type = INPUT_KEYBOARD;
    ip1.ki.wScan = 0;
    ip1.ki.time = 0;
    ip1.ki.dwExtraInfo = 0;

    ip2 = ip1;

    ip1.ki.wVk = VK_CONTROL;
    ip1.ki.dwFlags = 0;
    SendInput(1, &ip1, sizeof(INPUT)); // Press "Ctrl" key

    ip2.ki.wVk = 0x56;
    ip2.ki.dwFlags = 0;
    SendInput(1, &ip2, sizeof(INPUT)); // Press the "V" key

    ip2.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &ip2, sizeof(INPUT)); // Release the "V" key

    ip1.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &ip1, sizeof(INPUT)); // Release "Ctrl" key
}


void message()
{
    MessageBoxA(NULL,"Copy and paste functions using mouse like in X11, but for Microsoft Windows.\n\n"
                     "To disable CopyPasteW, go to taskmanager and kill CopyPasteW.exe\n\n"
                     "Copyright Matias Castillo Felmer 2016. License GPL v3, get the code and the latest"
                     "version in https://github.com/mcastillof/CopyPasteW", VERSION
                     ,MB_OK);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    clock_t t1, t2;

    message();

    while (1)
    {
        if(LEFT_CLICK)
        {
            t1 = clock();

            while(LEFT_CLICK)
            {
                t2 = clock();
            }
            if (timediff(t1, t2) > 130) //timediff > 130 milliseconds, is a selection (drag) instead of a click
            {
                if(!keyPresed())
                {
                    copy();
                }
            }
        }
        else if (MIDDLE_CLICK)
        {
            t1 = clock();

            while(MIDDLE_CLICK)
            {
                t2 = clock();
            }
            if (timediff(t1, t2) < 300) //timediff < 300 milliseconds, is a click instead of a selection (drag)
            {
                if(!keyPresed())
                {
                    leftClick();
                    paste();
                }
            }
        }

        Sleep(1); // Pause for 1 milliseconds.
    }

    return 0;
}
