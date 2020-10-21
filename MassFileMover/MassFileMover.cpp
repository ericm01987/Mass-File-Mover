// MassFileMover.cpp : Defines the entry point for the application.
//
#include "MassFileMover.h"

#define MAX_LOADSTRING      100
#define IDC_BROWSEBUTTON    500
#define IDC_BROWSEBUTTON2   501
#define IDC_EXECUTEBUTTON   502
#define IDC_RADIONONE       503
#define IDC_RADIOWHT        504
#define IDC_RADIOBLK        505
#define IDC_CHECKMARK       506

#define RADIO_NONE          0
#define RADIO_WHITE         1
#define RADIO_BLACK         2

#define ROW1                40
#define ROW2                90
#define ROW3                140
#define COLUMN1             25
#define COLUMN2             200
#define COLUMN3             720
#define TEXTBOXSIZE         500,30

#define MAX_LENGTH          1000

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
HWND hWnd;                                      // the main window
HWND BrowseButton;
HWND BrowseButton2;
HWND ExecuteButton;
HWND hwndDirPath;
HWND hwndFileName;
HWND hwndTargetDirPath;
HWND staticSourceFolder;
HWND staticDestinationFolder;
HWND staticFileName;
HWND radioWhtBlkList;
HWND Blacklist1;
HWND hwndEdit;
HWND checkButton;
HWND radioNone;
HWND radioBlack;
HWND radioWhite;
HBRUSH hbrBkgnd = NULL;
HFONT hFont;
HFONT hFont2;
HDC staticTextDC;
std::wstring sourceMaxLengthError;
std::wstring destinationMaxLengthError;
int whtBlk = RADIO_NONE;
int sourceLength;                               //Holds the length of the source folder textbox
int destinationLength;                          //Holds the length of the destination folder textbox
wchar_t fFileDir[MAX_LENGTH];                   //Buffer for the source directory.
wchar_t fTargetDirPath[MAX_LENGTH];             //Buffer for the destination directory.
wchar_t fFileName[MAX_LENGTH];                  //Buffer for the user-set filename.
bool checkboxIsChecked;                         //true = keep filenames; false = change them according to what's in hwndFileName
fs::path mainPath;                              //the program's path

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    HelpProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Blacklist(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.
    LoadLibrary(TEXT("ComCtl32.dll"));
    LoadLibrary(TEXT("Msftedit.dll"));
    INITCOMMONCONTROLSEX icex;           // Structure for control initialization.
    icex.dwICC = ICC_LISTVIEW_CLASSES;
    InitCommonControlsEx(&icex);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MASSFILEMOVER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MASSFILEMOVER));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int)msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MASSFILEMOVER));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_MASSFILEMOVER);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 900, 550, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    mainPath = fs::current_path();

    return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        BrowseButton = CreateWindow(
            L"BUTTON",  // Predefined class; Unicode assumed 
            L"Browse",      // Button text 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
            COLUMN3,         // x position 
            ROW1,         // y position 
            100,        // Button width
            30,        // Button height
            hWnd,     // Parent window
            (HMENU)IDC_BROWSEBUTTON,       // Window Handle
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            NULL);      // Pointer not needed.

        BrowseButton2 = CreateWindow(
            L"BUTTON",  // Predefined class; Unicode assumed 
            L"Browse",      // Button text 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
            COLUMN3,         // x position 
            ROW2,         // y position 
            100,        // Button width
            30,        // Button height
            hWnd,     // Parent window
            (HMENU)IDC_BROWSEBUTTON2,       // Window Handle
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            NULL);      // Pointer not needed.

        ExecuteButton = CreateWindow(
            L"BUTTON",  // Predefined class; Unicode assumed 
            L"Execute",      // Button text 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
            COLUMN1,         // x position 
            (ROW3 + 90),         // y position 
            COLUMN3 + 100 - COLUMN1,        // Button width; expands from column 1 to the end of the buttons in column 3
            30,        // Button height
            hWnd,     // Parent window
            (HMENU)IDC_EXECUTEBUTTON,       //Window Handle
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            NULL);      // Pointer not needed.

        checkButton = CreateWindow(
            L"BUTTON",  // Predefined class; Unicode assumed 
            L"Keep original file names",      // Button text 
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,  // Styles 
            COLUMN2,         // x position 
            (ROW3 + 40),         // y position 
            500,        // Button width
            30,        // Button height
            hWnd,     // Parent window
            (HMENU)IDC_CHECKMARK,       //Window Handle
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            NULL);      // Pointer not needed.

        radioNone = CreateWindowEx(WS_EX_WINDOWEDGE,
            L"BUTTON",
            L"None",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
            COLUMN1, (ROW3 + 150),
            300, 30,
            hWnd,
            (HMENU)IDC_RADIONONE,
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        radioWhite = CreateWindowEx(WS_EX_WINDOWEDGE,
            L"BUTTON",
            L"Whitelist",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
            COLUMN1, (ROW3 + 200),
            300, 30,
            hWnd,
            (HMENU)IDC_RADIOWHT,
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        radioBlack = CreateWindowEx(WS_EX_WINDOWEDGE,
            L"BUTTON",
            L"Blacklist",
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
            COLUMN1, (ROW3 + 250),
            300, 30,
            hWnd,
            (HMENU)IDC_RADIOBLK,
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);
        SendMessage(radioNone, BM_CLICK, NULL, NULL);
        hFont2 = CreateFont(30, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
            CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT(""));
        SendMessage(radioNone, WM_SETFONT, (WPARAM)hFont2, TRUE);
        SendMessage(radioBlack, WM_SETFONT, (WPARAM)hFont2, TRUE);
        SendMessage(radioWhite, WM_SETFONT, (WPARAM)hFont2, TRUE);

        //Static text boxes
        staticSourceFolder = CreateWindow(
            L"STATIC",  // Predefined class; Unicode assumed 
            L"Source Dir: ",      // text 
            WS_VISIBLE | WS_CHILD | SS_LEFT,  // Styles 
            COLUMN1, ROW1,         //position; x,  y
            200, 30,               //width, height
            hWnd,                  //Parent window
            NULL,                  //Window Handle
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            NULL);
        staticDestinationFolder = CreateWindow(
            L"STATIC",  // Predefined class; Unicode assumed 
            L"Destination Dir: ",      // text 
            WS_VISIBLE | WS_CHILD | SS_LEFT,  // Styles 
            COLUMN1, ROW2,         //position; x,  y
            200, 30,               //width, height
            hWnd,                  //Parent window
            NULL,                  //Window Handle
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            NULL);
        staticFileName = CreateWindow(
            L"STATIC",  // Predefined class; Unicode assumed 
            L"File Name: ",      // text 
            WS_VISIBLE | WS_CHILD | SS_LEFT,  // Styles 
            COLUMN1, ROW3,         //position; x,  y
            200, 30,               //width, height
            hWnd,                  //Parent window
            NULL,                  //Window Handle
            (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
            NULL);
        hFont = CreateFont(26, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
            CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT(""));
        SendMessage(staticDestinationFolder, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(staticSourceFolder, WM_SETFONT, (WPARAM)hFont, TRUE);
        SendMessage(staticFileName, WM_SETFONT, (WPARAM)hFont, TRUE);


        //Source folder
        hwndDirPath = CreateWindowEx(0, MSFTEDIT_CLASS, TEXT(""),
            WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP | ES_AUTOHSCROLL,
            COLUMN2, ROW1,    //position; x, y
            TEXTBOXSIZE,    //width, height
            hWnd, NULL, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);

        //Destination folder
        hwndTargetDirPath = CreateWindowEx(0, MSFTEDIT_CLASS, TEXT(""),
            WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP | ES_AUTOHSCROLL,
            COLUMN2, ROW2,
            TEXTBOXSIZE,
            hWnd, NULL, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);

        //Filename
        hwndFileName = CreateWindowEx(0, MSFTEDIT_CLASS, TEXT(""),
            WS_VISIBLE | WS_CHILD | WS_BORDER | WS_TABSTOP | ES_AUTOHSCROLL,
            COLUMN2, ROW3,
            TEXTBOXSIZE,
            hWnd, NULL, (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), NULL);

    }
    break;

    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
            //Popup dialog boxes
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_BLACKLIST:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_BLACKLIST), hWnd, BlacklistProc);
            break;
        case IDM_WHITELIST:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_WHITELIST), hWnd, WhitelistProc);
            break;
        case IDM_HELP:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_HELP), hWnd, HelpProc);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
            //Child window dialog boxes
        case IDC_BROWSEBUTTON:
            SetWindowText(hwndDirPath, NULL);
            fileSourceDir.clear();
            BasicFileOpen();
            SetWindowText(hwndDirPath, fileSourceDir.data());
            if (fileDestinationDir.empty() == true)
            {
                SetWindowText(hwndTargetDirPath, fileSourceDir.data());
                fileDestinationDir = fileSourceDir;
            }
            break;
        case IDC_BROWSEBUTTON2:
            SetWindowText(hwndTargetDirPath, NULL);
            fileDestinationDir.clear();
            BasicFileOpen2();
            SetWindowText(hwndTargetDirPath, fileDestinationDir.data());
            if (fileSourceDir.empty() == true)
            {
                SetWindowText(hwndDirPath, fileDestinationDir.data());
                fileSourceDir = fileDestinationDir;
            }
            break;
        case IDC_CHECKMARK:
            if (IsDlgButtonChecked(hWnd, IDC_CHECKMARK) == BST_CHECKED)
            {
                checkboxIsChecked = true;
                EnableWindow(hwndFileName, FALSE);
            }
            else
            {
                checkboxIsChecked = false;
                EnableWindow(hwndFileName, TRUE);
            }
            break;
        case IDC_EXECUTEBUTTON:
            //Get text from the three textboxes, store them as the source folder, destination folder, and file name respectively.
            sourceLength = GetWindowText(hwndDirPath, fFileDir, MAX_LENGTH);
            if (sourceLength == 0)
            {
                ::MessageBox(hWnd, L"Please enter a source directory.", L"Error", MB_OK);
                break;
            }
            else if (sourceLength > MAX_LENGTH - 2)
            {
                sourceMaxLengthError = L"Source directory path is too long. Max number of characters: " + std::to_wstring(MAX_LENGTH);
                ::MessageBox(hWnd, &sourceMaxLengthError[0], L"Error", MB_OK);
                break;
            }
            else if (fs::is_directory(fFileDir) == false)
            {
                ::MessageBox(hWnd, L"Input in Source Dir box is not a directory.", L"Error", MB_OK);
                break;
            }

            destinationLength = GetWindowText(hwndTargetDirPath, fTargetDirPath, MAX_LENGTH);
            if (destinationLength == 0)
            {
                ::MessageBox(hWnd, L"Please enter a destination directory.", L"Error", MB_OK);
                break;
            }
            else if (destinationLength > MAX_LENGTH - 2)
            {
                destinationMaxLengthError = L"Destination directory path is too long. Max length: " + std::to_wstring(MAX_LENGTH);
                ::MessageBox(hWnd, &destinationMaxLengthError[0], L"Error", MB_OK);
                break;
            }
            else if (fs::is_directory(fTargetDirPath) == false)
            {
                ::MessageBox(hWnd, L"Input in Destination Dir box is not a directory.", L"Error", MB_OK);
                break;
            }

            GetWindowText(hwndFileName, fFileName, MAX_LENGTH);
            if (whtBlk == RADIO_WHITE)
                MoveFilesWhite(hWnd, checkboxIsChecked);
            else if (whtBlk == RADIO_BLACK)
                MoveFilesBlack(hWnd, checkboxIsChecked);
            else
                MoveFilesNone(hWnd, checkboxIsChecked);
            break;
        case IDC_RADIONONE:
            whtBlk = RADIO_NONE;
            break;
        case IDC_RADIOWHT:
            whtBlk = RADIO_WHITE;
            break;
        case IDC_RADIOBLK:
            whtBlk = RADIO_BLACK;
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_CTLCOLORSTATIC:
    {
        HDC hdcStatic = (HDC)wParam;
        SetTextColor(hdcStatic, RGB(0, 0, 0));
        if (hbrBkgnd == NULL)
        {
            hbrBkgnd = CreateSolidBrush(RGB(255, 255, 255));
        }
        return (INT_PTR)hbrBkgnd;
    }
    break;
    case WM_DESTROY:
    {
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    break;
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

INT_PTR CALLBACK HelpProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}