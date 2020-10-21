/*This is the handler for the whitelist dialog box. It's a user-friendly tool for viewing and editing the contents of the .ini file.
It doesn't handle the actual implimentation of the whitelist when moving the files.*/

#include "MassFileMover.h"
#define MAX_SIZE 1000

INT_PTR CALLBACK WhitelistProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    HWND whitelistWnd = GetDlgItem(hDlg, IDC_WHITELIST);  //The listbox
    HWND whitelistEditWnd = GetDlgItem(hDlg, IDC_WHITEEDITBOX);  //The editbox
    std::wstring buffer; //buffer for strings to be placed in vectors; cleared after use to be reused
    std::wstring addBuffer;
    std::wstring savedString; //string to insert into the ini file
    std::wstring maxSize = std::to_wstring(MAX_SIZE);
    std::vector<std::wstring> newExtList; //User inputs for the whitelist
    std::vector<std::wstring> extList; //The parsed string from the ini file
    wchar_t editboxContents[MAX_SIZE] = { 0 }; //The contents of the edit control where the user inputs extensions
    int i = 0;
    int selitems[MAX_SIZE] = { 0 };   //buffer for selected items
    LRESULT selitemsInBuffer; //amount of items in the buffer, for the reverse-iterator loop
    int addCount = 0;    //number of characters in the edit box, including those which haven't been added to newExtList
    int addedCount = 0; //number of characters which have been added to newExtList; compared against addCount
    int total = 0;  //All elements in the whitelist string that have been iterated over
    int extLen = 0; //Elements in the whitelist string that have not been iterated over, indicating that they haven't been added to the list of extensions
    int stringSize;
    static int boxCount = 0;    //number of strings in the listbox
    LRESULT pos = 0;
    wchar_t whitelist[MAX_SIZE] = { 0 };
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        boxCount = 0;
        fs::current_path(mainPath); //load from the path with the ini file
        //Load the whitelist from the ini file. If it's too long, return an error and close the dialog box. Otherwise, load it into the listbox.
        stringSize = GetPrivateProfileString(
            L"List",
            L"Whitelist",
            L"Error",
            &whitelist[0],
            MAX_SIZE,
            L".\\WhitelistBlacklist.ini"
        );
        if (stringSize > MAX_SIZE - 2)
        {
            buffer = L"Whitelist in ini file is too long. Please shorten the number of entries. Max number of characters: " + maxSize;
            ::MessageBox(hDlg, &buffer[0], L"Error", MB_OK);
            buffer.clear();
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
            break;
        }
        total = 0;  //All elements in the whitelist string that have been iterated over
        extLen = 0; //Elements in the whitelist string that have not been iterated over, indicating that they haven't been added to the list of extensions
        for (int i = 0; whitelist[i] != L'\0'; i++)
        {
            total++;
            if (whitelist[i] == L',')
            {
                //add the characters between the start of the string or the last comma and the total...
                while (total > extLen)
                {
                    buffer.push_back(whitelist[extLen]);
                    extLen++;
                }
                //...and then put the resulting string into the array of strings
                extList.push_back(buffer);
                buffer.clear();
            }
        }
        for (std::size_t i = 0; i < extList.size(); i++)
        {
            buffer = extList[i];
            buffer.erase(remove(buffer.begin(), buffer.end(), L','), buffer.end());
            pos = SendMessage(whitelistWnd, LB_ADDSTRING, 0, (LPARAM)buffer.data());
            SendMessage(whitelistWnd, LB_SETITEMDATA, pos, (LPARAM)i);
            boxCount++;
        }
        buffer.clear();
    case WM_COMMAND:
        switch (wParam)
        {
        case IDOK:
        case IDCANCEL:
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
            break;
        case IDC_WHITEADDBUTTON:
            stringSize = GetWindowTextW(whitelistEditWnd, &editboxContents[0], MAX_SIZE);
            //error messages
            if (stringSize > MAX_SIZE - 2)
            {
                buffer = L"Too many characters in textbox. Max number of characters: " + maxSize;
                ::MessageBox(hDlg, &buffer[0], L"Error", MB_OK);
                buffer.clear();
                break;
            }
            i = 0;
            while (editboxContents[i] != L'\0')
            {
                addCount++;
                if (editboxContents[i] == L',')
                {
                    while (addCount > addedCount)
                    {
                        buffer.push_back(editboxContents[addedCount]);
                        addedCount++;
                    }
                    newExtList.push_back(buffer);
                    buffer.clear();
                }
                i++;

            }
            //if the user has added one extension with no comma, read it in and add a comma
            i = 0;
            if ((newExtList.size() == 0) && (editboxContents[0] != L'\0'))
            {
                i = 0;
                while (editboxContents[i] != L'\0')
                {
                    buffer.push_back(editboxContents[i]);
                    i++;
                }
                newExtList.push_back(buffer + L",");
                buffer.clear();
            }
            //if the user input multiple items but hasn't included a comma for the last one, read it in and add a comma
            else if (editboxContents[addCount] != L',')
            {
                while (addedCount <= addCount)
                {
                    buffer.push_back(editboxContents[addedCount]);
                    addedCount++;
                }
                newExtList.push_back(buffer + L",");
                buffer.clear();
            }
            //add the strings to the listbox
            for (std::size_t i = 0; i < newExtList.size(); i++)
            {
                std::wstring buffer = newExtList[i];
                buffer.erase(remove(buffer.begin(), buffer.end(), L','), buffer.end());
                buffer.erase(remove(buffer.begin(), buffer.end(), L' '), buffer.end());
                buffer.erase(remove(buffer.begin(), buffer.end(), L'\0'), buffer.end());
                if (!buffer.empty())
                {
                    buffer.push_back(L'\0');
                    pos = SendMessage(whitelistWnd, LB_ADDSTRING, 0, (LPARAM)buffer.data());
                    SendMessage(whitelistWnd, LB_SETITEMDATA, pos, (LPARAM)i);
                    boxCount++;
                }
            }
            buffer.clear();
            i = 0;
            SendMessage(whitelistEditWnd, WM_SETTEXT, 0, (LPARAM)L"");
            break;
        case IDC_WHITEDELETEBUTTON:
            selitemsInBuffer = SendMessage(whitelistWnd, LB_GETSELITEMS, MAX_SIZE, (LPARAM)&selitems[0]); //Store item positions and number of items
            for (LRESULT i = selitemsInBuffer - 1; i >= 0; i--)
            {
                SendMessage(whitelistWnd, LB_DELETESTRING, selitems[i], 0);
                boxCount--;
            }
            break;
        case IDC_WHITESAVEBUTTON:
            SendMessage(whitelistWnd, LB_SELITEMRANGE, TRUE, MAKELPARAM(0, boxCount));
            std::fill_n(&selitems[0], boxCount, 0); //clear cell items buffer
            selitemsInBuffer = SendMessage(whitelistWnd, LB_GETSELITEMS, MAX_SIZE, (LPARAM)&selitems[0]);

            for (int i = 0; i <= (selitemsInBuffer - 1); i++)
            {
                SendMessage(whitelistWnd, LB_GETTEXT, i, (LPARAM)buffer.data());
                savedString += buffer.data();
                savedString += L',';
                buffer.clear();
            }
            if (savedString.size() > MAX_SIZE - 2)
            {
                buffer = L"Too many items in listbox. Max combined total number of characters: " + MAX_SIZE;
                ::MessageBox(hDlg, &buffer[0], L"Error", MB_OK);
                buffer.clear();
                break;
            }
            fs::current_path(mainPath); //save to the path with the ini file
            WritePrivateProfileString(
                L"List",
                L"Whitelist",
                (LPWSTR)savedString.data(),
                L".\\WhitelistBlacklist.ini"
            );
            savedString.clear();
            break;
        }
    }
    return (INT_PTR)FALSE;
}