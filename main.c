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
#include <stdlib.h>
#include <time.h>
#include <psapi.h>
#pragma comment(lib, "psapi")


#define VERSION             "CopyPasteW V0.03"
#define LEFT_CLICK          GetKeyState(VK_LBUTTON) < 0
#define MIDDLE_CLICK        GetKeyState(VK_MBUTTON) < 0
#define BUFFER_SIZE         255
#define MAX_APPS_EXCLUDED   100

char appsExcluded[MAX_APPS_EXCLUDED][1024];


long timediff(clock_t t1, clock_t t2)
{
    long elapsed;
    elapsed = ((double)t2 - t1) / CLOCKS_PER_SEC * 1000;
    return elapsed;
}


BOOL keyPresed()
{
    int i;

    for (i = 0x05; i <= 0xFF; i++)
        if (GetKeyState(i) < 0)
            return TRUE;
    return FALSE;
}


void leftClick()
{
    INPUT   ip;

    ip.type = INPUT_MOUSE;
    ip.ki.wScan = 0; // hardware scan code for key
    ip.ki.time = 0;
    ip.ki.dwExtraInfo = 0;

    ip.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(1, &ip, sizeof(INPUT)); //left click down

    ip.mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, &ip, sizeof(INPUT)); //left click up
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
    MessageBoxA(NULL, "Copy and paste functions using mouse like in X11, but for Microsoft Windows.\n\n"
        "To disable CopyPasteW, go to taskmanager and kill CopyPasteW.exe\n\n"
        "Some applications or windows may misbehave with copy and paste mouse function because CopyPasteW"
        "emulates this behavior as CTRL+C and CTRL+V keystroke. To exclude this apps or windows, add the "
        "executable filename or the window title (can be just a part of it) to the file "
        "C:\\CopyPasteW.cfg. One app per line in the file. The text in the file is case sensitive. A "
        "restart of CopyPasteW is needed every time you made a change.\n\n"
        "Copyright Matias Castillo Felmer 2016. License GPL v3, get the code and the latest "
        "version in https://github.com/mcastillof/CopyPasteW"
        , VERSION, MB_OK);
}


char* GetActiveWindowTitle()
{
    static char wnd_title[256];
    HWND hwnd = GetForegroundWindow(); // get handle of currently active window
    GetWindowTextA(hwnd, wnd_title, sizeof(wnd_title));
    return wnd_title;
}


char* GetActiveWindowFilename()
{
    static char filename[MAX_PATH];
    DWORD pid;
    HWND hwnd = GetForegroundWindow(); 
    GetWindowThreadProcessId(hwnd,  &pid);
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, false, pid);
    DWORD value = MAX_PATH;
    QueryFullProcessImageNameA(hProcess, 0, filename, &value);
    return filename;
}


BOOL appNotExcluded(char *appTitle, char *appFilename)
{
    int i;

    for (i = 0; i < MAX_APPS_EXCLUDED; i++ )
    {
        if (appsExcluded[i][0] == '\0')
            return true;
        if (strstr(appFilename, appsExcluded[i]))
            return false;
        if (strstr(appTitle, appsExcluded[i]))
            return false;
    }
    return true;
}


void excludeApp(char *app)
{
    int i;

    for (i = 0; i < MAX_APPS_EXCLUDED; i++)
    {
        if (appsExcluded[i][0] == '\0')
        {
            strncpy_s(appsExcluded[i],1023, app, 1023);
            return;
        }
    }
}


void getAppsExcluded()
{
    FILE *file;
    char line[1024];

    fopen_s(&file, "D:\\CopyPasteW.cfg", "r");
    if (file == NULL)
    {
        printf("No config file.\n");
        return;
    }
    
    while (feof(file) == 0)
    {
        fgets(line, 1023, file);
        line[strlen(line)-1] = '\0';
        excludeApp(line);
    }
    if (fclose(file) != 0)
        printf("Can't close the config file\n");
}


void init()
{
    int i;

    for (i = 0; i < MAX_APPS_EXCLUDED ; i++)
    {
        appsExcluded[i][0] = '\0';
    }
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    clock_t t1, t2;
    init();
    message();
    getAppsExcluded();

    while (1)
    {
        if (LEFT_CLICK)
        {
            t1 = t2 = clock();
            while (LEFT_CLICK)
            {
                t2 = clock();
            }

            if (timediff(t1, t2) > 130) //timediff > 130 milliseconds, is a selection (drag) instead of a click
            {
                char *title = GetActiveWindowTitle();
                char *filename = GetActiveWindowFilename();
                filename[0] = 'a';
                filename[1] = '\n';

                if(appNotExcluded(title, filename))
                {
                    if (!keyPresed())
                    {
                        copy();
                    }
                }
            }
        }

        else if (MIDDLE_CLICK)
        {
            char *title = GetActiveWindowTitle();
            char *filename = GetActiveWindowFilename();

            t1 = t2 = clock();
            while (MIDDLE_CLICK)
            {
                t2 = clock();
            }

            if (timediff(t1, t2) < 300) //timediff < 300 milliseconds, is a click instead of a selection (drag)
            {
                char *title = GetActiveWindowTitle();
                char *filename = GetActiveWindowFilename();
                filename[0] = 'a';
                filename[1] = '\n';

                if (appNotExcluded(title, filename))
                {
                    if (!keyPresed())
                    {
                        leftClick();
                        paste();
                    }
                }
            }
        }

        Sleep(1); // Pause for 1 milliseconds.
    }

    return 0;
}
