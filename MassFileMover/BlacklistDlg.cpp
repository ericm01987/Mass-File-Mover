/*This is the handler for the blacklist dialog box. It's a user-friendly tool for viewing and editing the contents of the .ini file.
It doesn't handle the actual implimentation of the blacklist when moving the files.*/

#include "MassFileMover.h"
#define MAX_SIZE 1000

INT_PTR CALLBACK BlacklistProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    HWND blacklistWnd = GetDlgItem(hDlg, IDC_BLACKLIST);  //The listbox
    HWND blacklistEditWnd = GetDlgItem(hDlg, IDC_BLKEDIT);  //The editbox
    std::wstring buffer; //buffer for strings to be placed in vectors; cleared after use to be reused
    std::wstring addBuffer;
    std::wstring savedString; //string to insert into the ini file
    std::wstring maxSize = std::to_wstring(MAX_SIZE);
    std::vector<std::wstring> newExtList; //User inputs for the blacklist
    std::vector<std::wstring> extList; //The parsed string from the ini file
    wchar_t editboxContents[MAX_SIZE] = { 0 }; //The contents of the edit control where the user inputs extensions
    int i = 0;
    int selitems[MAX_SIZE] = { 0 };   //buffer for selected items
    int selitemsInBuffer; //amount of items in the buffer, for the reverse-iterator loop
    int addCount = 0;    //number of characters in the edit box, including those which haven't been added to newExtList
    int addedCount = 0; //number of characters which have been added to newExtList; compared against addCount
    int total = 0;  //All elements in the blacklist string that have been iterated over
    int extLen = 0; //Elements in the blacklist string that have not been iterated over, indicating that they haven't been added to the list of extensions
    int stringSize;
    static int boxCount = 0;    //number of strings in the listbox
    wchar_t blacklist[1000] = {0};
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        fs::current_path(mainPath); //load from the path with the ini file
        boxCount = 0;
        //Load the blacklist from the ini file. If it's too long, return an error and close the dialog box. Otherwise, load it into the listbox.
        stringSize = GetPrivateProfileString(
            L"List",
            L"Blacklist",
            L"Error",
            &blacklist[0],
            MAX_SIZE,
            L".\\WhitelistBlacklist.ini"
        );
        if (stringSize > MAX_SIZE - 2)
        {
            buffer = L"Blacklist in ini file is too long. Please shorten the number of entries. Max number of characters: " + std::to_wstring(MAX_SIZE - 2);
            ::MessageBox(hDlg, &buffer[0], L"Error", MB_OK);
            buffer.clear();
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
            break;
        }
        total = 0;  //All elements in the blacklist string that have been iterated over
        extLen = 0; //Elements in the blacklist string that have not been iterated over, indicating that they haven't been added to the list of extensions
        for (int i = 0; blacklist[i] != L'\0'; i++)
        {
            total++;
            if (blacklist[i] == L',')
            {
                //add the characters between the start of the string or the last comma and the total...
                while (total > extLen)
                {
                    buffer.push_back(blacklist[extLen]);
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
            LRESULT pos = SendMessage(blacklistWnd, LB_ADDSTRING, 0, (LPARAM)buffer.data());
            SendMessage(blacklistWnd, LB_SETITEMDATA, pos, (LPARAM)i);
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
        case IDC_BLACKLISTADDBUTTON:
            stringSize = GetWindowTextW(blacklistEditWnd, &editboxContents[0], MAX_SIZE);
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
                    LRESULT pos = SendMessage(blacklistWnd, LB_ADDSTRING, 0, (LPARAM)buffer.data());
                    SendMessage(blacklistWnd, LB_SETITEMDATA, pos, (LPARAM)i);
                    boxCount++;
                }
            }
            buffer.clear();
            i = 0;
            SendMessage(blacklistEditWnd, WM_SETTEXT, 0, (LPARAM)L"");
            break;
        case IDC_BLACKLISTDELETEBUTTON:
            selitemsInBuffer = SendMessage(blacklistWnd, LB_GETSELITEMS, MAX_SIZE, (LPARAM)&selitems[0]); //Store item positions and number of items
            for (int i = selitemsInBuffer - 1; i >= 0; i--)
            {
                SendMessage(blacklistWnd, LB_DELETESTRING, selitems[i], 0);
                boxCount--;
            }
            break;
        case IDC_BLACKLISTSAVEBUTTON:
            SendMessage(blacklistWnd, LB_SELITEMRANGE, TRUE, MAKELPARAM(0, boxCount));
            std::fill_n(&selitems[0], boxCount, 0); //clear cell items buffer
            selitemsInBuffer = SendMessage(blacklistWnd, LB_GETSELITEMS, MAX_SIZE, (LPARAM)&selitems[0]);
            for (int i = 0; i <= (selitemsInBuffer - 1); i++)
            {
                SendMessage(blacklistWnd, LB_GETTEXT, i, (LPARAM)buffer.data());
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
                L"Blacklist",
                (LPWSTR)savedString.data(),
                L".\\WhitelistBlacklist.ini"
            );
            savedString.clear();
            break;
        }
    }
    return (INT_PTR)FALSE;
}