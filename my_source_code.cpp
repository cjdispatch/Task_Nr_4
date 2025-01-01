#include <windows.h>
#include <commdlg.h> // For file dialog
#include <string>

// Global variables
HWND hEdit;

// Function to open a file
void OpenFile(HWND hwnd) {
    char fileName[MAX_PATH] = "";
    OPENFILENAME ofn;

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    ofn.lpstrDefExt = "txt";

    if (GetOpenFileName(&ofn)) {
        FILE* file;
        fopen_s(&file, fileName, "r");
        if (file) {
            char buffer[1024] = {0};
            fread(buffer, 1, sizeof(buffer), file);
            fclose(file);
            SetWindowText(hEdit, buffer);
        }
    }
}

// Function to save a file
void SaveFile(HWND hwnd) {
    char fileName[MAX_PATH] = "";
    OPENFILENAME ofn;

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT;
    ofn.lpstrDefExt = "txt";

    if (GetSaveFileName(&ofn)) {
        FILE* file;
        fopen_s(&file, fileName, "w");
        if (file) {
            int length = GetWindowTextLength(hEdit);
            char* buffer = new char[length + 1];
            GetWindowText(hEdit, buffer, length + 1);
            fwrite(buffer, 1, length, file);
            fclose(file);
            delete[] buffer;
        }
    }
}

// Window Procedure
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        // Create a multiline text box
        hEdit = CreateWindow("EDIT", "",
                             WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL,
                             10, 10, 500, 400, hwnd, NULL, NULL, NULL);
        break;
    }

    case WM_COMMAND: {
        switch (LOWORD(wParam)) {
        case 1: // Open File
            OpenFile(hwnd);
            break;
        case 2: // Save File
            SaveFile(hwnd);
            break;
        case 3: // About
            MessageBox(hwnd, "SD1 Application v1.0\nCreated by Chijioke Kalu", "About", MB_OK | MB_ICONINFORMATION);
            break;
        case 4: // Exit
            PostQuitMessage(0);
            break;
        }
        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

// Main Function
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const char CLASS_NAME[] = "MainWindow";
    WNDCLASS wc = {};

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    if (!RegisterClass(&wc)) {
        MessageBox(NULL, "Window registration failed", "Error", MB_ICONERROR);
        return 0;
    }

    HWND hwnd = CreateWindowEx(
        0, CLASS_NAME, "SD1 Application",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 500,
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL) {
        return 0;
    }

    // Create a menu
    HMENU hMenu = CreateMenu();
    HMENU hFileMenu = CreateMenu();

    AppendMenu(hFileMenu, MF_STRING, 1, "Open");
    AppendMenu(hFileMenu, MF_STRING, 2, "Save");
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hFileMenu, "File");
    AppendMenu(hMenu, MF_STRING, 3, "About");
    AppendMenu(hMenu, MF_STRING, 4, "Exit");

    SetMenu(hwnd, hMenu);

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
