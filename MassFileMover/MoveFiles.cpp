#include "MassFileMover.h"
#define MAX_LENGTH 1000

unsigned int BinarySearch(std::wstring* arr, int bottom, int top, std::wstring entry)
{
    if (bottom <= top)
    {
        int mid = (bottom + top) / 2;
        if (arr[mid] == entry)
        {
            return 0;
        }
        if (arr[mid] < entry)
        {
            return BinarySearch(arr, mid + 1, top, entry);
        }
        else if (arr[mid] > entry)
        {
            return BinarySearch(arr, bottom, mid - 1, entry);
        }
    }
    if (bottom > top)
    {
        return 1;
    }
}

void MoveFilesNone(HWND mainWnd, bool checkmark)
{
    fs::current_path(fFileDir);
    int i = 1;                                                      //counter for number at the end of filenames
    int totalFiles = 0;
    std::wstring targetFile;                                        //the filename after moving

    //Checkmark that indicates that the user wants to keep filenames is unchecked
    if (checkmark == false)
    {

        //register number of files in the target directory for error checking
        for (auto& r : fs::directory_iterator(fTargetDirPath))
        {
            if (r.is_directory() == false)
            {
                totalFiles++;
            }
        }
        //check for possible accidental selection of large directory
        if (totalFiles > 5000)
        {
            int messageBoxResult = 0;
            std::wstring warning = L"You are about to move a large number of files! Total number: " + std::to_wstring(totalFiles) + L" ...Are you sure you want to continue?";
            messageBoxResult = ::MessageBox(mainWnd, &warning[0], L"Warning", MB_YESNO);
            if (messageBoxResult == 7)
            {
                return;
            }
        }

        for (auto& p : fs::recursive_directory_iterator(fFileDir))
        {
            if (p.is_directory() == false)
            {
                targetFile = std::wstring(fTargetDirPath) + L"\\" + fFileName + std::to_wstring(i) + (std::wstring)p.path().extension();
                targetFile.erase(remove(targetFile.begin(), targetFile.end(), '\"'), targetFile.end());                             //Get rid of the double quotes from the filename
                fs::rename(p.path(), (fs::path)targetFile);
                i++;
            }
        }
    }
    //User wants to keep filenames
    else
    {
        std::map<std::wstring, int> filenameMap;
        bool duplicateFound = false;
        //register all files in the target directory for duplicate and error checking
        for (auto& r : fs::directory_iterator(fTargetDirPath))
        {
            if (r.is_directory() == false)
            {
                filenameMap[(std::wstring)r.path().filename()] = 1;
                totalFiles++;
            }
        }
        //check for the possibility of accidentally selecting a large directory
        if (totalFiles > 5000)
        {
            int messageBoxResult = 0;
            std::wstring warning = L"You are about to move " + std::to_wstring(totalFiles) + L" files. Are you sure you want to continue?";
            messageBoxResult = ::MessageBox(mainWnd, &warning[0], L"Warning", MB_YESNO);
            if (messageBoxResult == 7)
            {
                return;
            }
        }
        //move files
        for (auto& p : fs::recursive_directory_iterator(fFileDir))
        {
            if (p.is_directory() == false)
            {
                //check for duplicate filenames
                duplicateFound = false;
                if (!filenameMap[p.path().filename()])
                {
                    filenameMap[p.path().filename()] = 1;
                }
                else
                {
                    for (auto& q : fs::directory_iterator(fTargetDirPath))
                    {
                        if (p.path().filename() == q.path().filename())
                        {
                            filenameMap[p.path().filename()]++;
                            duplicateFound = true;
                        }
                    }
                }
                //if there weren't duplicates
                if (duplicateFound == false)
                {
                    targetFile = std::wstring(fTargetDirPath) + L"\\" + (std::wstring)p.path().filename();
                    targetFile.erase(remove(targetFile.begin(), targetFile.end(), '\"'), targetFile.end());                             //Get rid of the double quotes from the filename
                    fs::rename(p.path(), (fs::path)targetFile);
                    i++;
                }
                //if there were duplicates
                else
                {
                    targetFile = std::wstring(fTargetDirPath) + L"\\" + (std::wstring)p.path().stem() + L"(" + std::to_wstring(filenameMap[p.path().filename()] - 1) + L")" + (std::wstring) p.path().extension();
                    targetFile.erase(remove(targetFile.begin(), targetFile.end(), '\"'), targetFile.end());                             //Get rid of the double quotes from the filename
                    fs::rename(p.path(), (fs::path)targetFile);
                    i++;
                }
            }
        }
    }
    ::MessageBox(mainWnd, L"Transfer successful!", L"Success", NULL);
}

void MoveFilesWhite(HWND mainWnd, bool checkmark)
{
    fs::current_path(mainPath);  //set path to path with the ini file
    wchar_t Whitelist[MAX_LENGTH] = {0};
    std::vector<std::wstring> extList;

    //get the whitelist
    GetPrivateProfileString(
        L"List",
        L"Whitelist",
        L"",
        &Whitelist[0],
        MAX_LENGTH,
        L".\\WhitelistBlacklist.ini"
    );

    int total = 0;
    int totalFiles = 0; //the number of files in the source directory
    std::wstring buffer;
    

    //parse the whitelist into strings
    for (int i = 0, extLen = 0; Whitelist[i] != '\0'; i++)
    {
        total++;
        if (Whitelist[i] == L',')
        {
            while (total > extLen)
            {
                buffer.push_back(Whitelist[extLen]);
                extLen++;
            }
            extList.push_back(L'.' + buffer);
            extList[extList.size() - 1].erase(remove(extList[extList.size() - 1].begin(), extList[extList.size() - 1].end(), L','), extList[extList.size() - 1].end());
            buffer.clear();
        }
    }
    if (extList.empty() == true)
    {
        ::MessageBox(mainWnd, L"Whitelist is empty", L"Error", MB_OK);
        return;
    }
    extList.push_back(L"\0");
    std::wstring whiteListEntry;                                    //Holds the blacklist entry that's being tried against the extension.
    std::wstring targetFile;                                        //the filename after moving        

    //User wants files renamed
    if (checkmark == false)
    {

        //register number of files in the target directory for error checking
        for (auto& r : fs::directory_iterator(fTargetDirPath))
        {
            if (r.is_directory() == false)
            {
                totalFiles++;
            }
        }
        //check for possible accidental selection of large directory
        if (totalFiles > 5000)
        {
            int messageBoxResult = 0;
            std::wstring warning = L"You are about to move a large number of files! Total number: " + std::to_wstring(totalFiles) + L" ...Are you sure you want to continue?";
            messageBoxResult = ::MessageBox(mainWnd, &warning[0], L"Warning", MB_YESNO);
            if (messageBoxResult == 7)
            {
                return;
            }
        }

        int i = 1;                                                      //counter for number at the end of filenames
        for (auto& p : fs::recursive_directory_iterator(fFileDir))
        {
            if (p.is_directory() == false)
            {
                unsigned int pos = BinarySearch(&extList[0], 0, (int)extList.size() - 2, p.path().extension());
                if (pos == 0)   //hit found; move the file
                {
                    targetFile = std::wstring(fTargetDirPath) + L"\\" + fFileName + std::to_wstring(i) + (std::wstring)p.path().extension();
                    targetFile.erase(remove(targetFile.begin(), targetFile.end(), '\"'), targetFile.end());                             //Get rid of the double quotes from the filename
                    fs::rename(p.path(), (fs::path)targetFile);
                    i++;
                }
                else if ((pos != 0) && (pos != 1))
                {
                    ::MessageBox(mainWnd, L"Error in parsing whitelist", L"Error", MB_OK);
                    return;
                }
            }
        }
    }
    //User wants to keep filenames
    else
    {
        std::map<std::wstring, int> filenameMap;
        bool duplicateFound = false;
        //register all files in the target directory for duplicate and error checking
        for (auto& r : fs::directory_iterator(fTargetDirPath))
        {
            if (r.is_directory() == false)
            {
                filenameMap[(std::wstring)r.path().filename()] = 1;
                totalFiles++;
            }
        }
        //check for the possibility of accidentally selecting a large directory
        if (totalFiles > 5000)
        {
            int messageBoxResult = 0;
            std::wstring warning = L"You are about to move " + std::to_wstring(totalFiles) + L" files. Are you sure you want to continue?";
            messageBoxResult = ::MessageBox(mainWnd, &warning[0], L"Warning", MB_YESNO);
            if (messageBoxResult == 7)
            {
                return;
            }
        }
        //move files
        for (auto& p : fs::recursive_directory_iterator(fFileDir))
        {
            if (p.is_directory() == false)
            {
                int pos = BinarySearch(&extList[0], 0, (int)extList.size() - 1, p.path().extension());
                if (pos == 0)  //hit found; move the file
                {
                    //check for duplicate filenames
                    duplicateFound = false;
                    if (!filenameMap[p.path().filename()])
                    {
                        filenameMap[p.path().filename()] = 1;
                    }
                    else
                    {
                        for (auto& q : fs::directory_iterator(fTargetDirPath))
                        {
                            if (p.path().filename() == q.path().filename())
                            {
                                filenameMap[p.path().filename()]++;
                                duplicateFound = true;
                            }
                        }
                    }
                    //if there weren't duplicates
                    if (duplicateFound == false)
                    {
                        targetFile = std::wstring(fTargetDirPath) + L"\\" + (std::wstring)p.path().filename();
                        targetFile.erase(remove(targetFile.begin(), targetFile.end(), '\"'), targetFile.end());                             //Get rid of the double quotes from the filename
                        fs::rename(p.path(), (fs::path)targetFile);
                    }
                    //if there were duplicates
                    else
                    {
                        targetFile = std::wstring(fTargetDirPath) + L"\\" + (std::wstring)p.path().stem() + L"(" + (std::to_wstring(filenameMap[p.path().filename()] - 1)) + L")" + (std::wstring)p.path().extension();
                        targetFile.erase(remove(targetFile.begin(), targetFile.end(), '\"'), targetFile.end());                             //Get rid of the double quotes from the filename
                        fs::rename(p.path(), (fs::path)targetFile);
                    }
                }
                else if ((pos != 0) && (pos != 1))
                {
                    ::MessageBox(mainWnd, L"Error in parsing whitelist,", L"Error", MB_OK);
                    return;
                }
            }
        }
    }
    ::MessageBox(mainWnd, L"Transfer successful!", L"Success", NULL);
}

void MoveFilesBlack(HWND mainWnd, bool checkmark)
{
    fs::current_path(mainPath);  //set path to path with the ini file
    wchar_t Blacklist[MAX_LENGTH];
    std::vector<std::wstring> extList;
    int totalFiles = 0;

    GetPrivateProfileString(
        L"List",
        L"Blacklist",
        L"Error",
        &Blacklist[0],
        MAX_LENGTH,
        L".\\WhitelistBlacklist.ini"
    );

    int total = 0;
    int extLen = 0;
    std::wstring buffer;

    for (int i = 0; Blacklist[i] != '\0'; i++)
    {
        total++;
        if (Blacklist[i] == L',')
        {
            while (total > extLen)
            {
                buffer.push_back(Blacklist[extLen]);
                extLen++;
            }
            extList.push_back(L'.' + buffer);
            extList[extList.size() - 1].erase(remove(extList[extList.size() - 1].begin(), extList[extList.size() - 1].end(), L','), extList[extList.size() - 1].end());
            buffer.clear();
        }
    }
    if (extList.empty() == true)
    {
        ::MessageBox(mainWnd, L"Blacklist is empty", L"Error", MB_OK);
        return;
    }
    extList.push_back(L"\0");
    std::wstring blackListEntry;                                    //Holds the blacklist entry that's being tried against the file extension.
    std::wstring targetFile;                                        //the filename after moving

    //User wants files renamed
    if (checkmark == false)
    {
        //register number of files in the target directory for error checking
        for (auto& r : fs::directory_iterator(fTargetDirPath))
        {
            if (r.is_directory() == false)
            {
                totalFiles++;
            }
        }
        //check for possible accidental selection of large directory
        if (totalFiles > 5000)
        {
            int messageBoxResult = 0;
            std::wstring warning = L"You are about to move a large number of files! Total number: " + std::to_wstring(totalFiles) + L" ...Are you sure you want to continue?";
            messageBoxResult = ::MessageBox(mainWnd, &warning[0], L"Warning", MB_YESNO);
            if (messageBoxResult == 7)
            {
                return;
            }
        }

        int i = 1;                                                          //counter for number at the end of filenames
        for (auto& p : fs::recursive_directory_iterator(fFileDir))
        {
            if (p.is_directory() == false)
            {
                unsigned int pos = BinarySearch(&extList[0], 0, (int)extList.size() - 2, p.path().extension());
                if (pos == 1) //hit not found; move the file
                {
                    targetFile = std::wstring(fTargetDirPath) + L"\\" + fFileName + std::to_wstring(i) + (std::wstring)p.path().extension();
                    targetFile.erase(remove(targetFile.begin(), targetFile.end(), '\"'), targetFile.end()); //Get rid of the double quotes from the filename
                    fs::rename(p.path(), (fs::path)targetFile);
                    i++;
                }
                else if ((pos != 1) && (pos != 0))
                {
                    ::MessageBox(mainWnd, L"Error parsing blacklist", L"Error", MB_OK);
                }
            }
        }
    }
    //User wants to keep filenames
    else
    {
        std::map<std::wstring, int> filenameMap;
        bool duplicateFound = false;
        //register all files in the target directory for duplicate and error checking
        for (auto& r : fs::directory_iterator(fTargetDirPath))
        {
            if (r.is_directory() == false)
            {
                filenameMap[(std::wstring)r.path().filename()] = 1;
                totalFiles++;
            }
        }
        //check for possible accidental selection of large directory
        if (totalFiles > 5000)
        {
            int messageBoxResult = 0;
            std::wstring warning = L"You are about to move a large number of files! Total number: " + std::to_wstring(totalFiles) + L" ...Are you sure you want to continue?";
            messageBoxResult = ::MessageBox(mainWnd, &warning[0], L"Warning", MB_YESNO);
            if (messageBoxResult == 7)
            {
                return;
            }
        }

        //move files
        for (auto& p : fs::recursive_directory_iterator(fFileDir))
        {
            if (p.is_directory() == false)
            {
                unsigned int pos = BinarySearch(&extList[0], 0, (int)extList.size() - 1, p.path().extension());
                if (pos == 1) //hit not found; move the file
                {
                    duplicateFound = false;
                    if (!filenameMap[(std::wstring)p.path().filename()])
                    {
                        filenameMap[(std::wstring)p.path().filename()] = 1;
                    }
                    else
                    {
                        for (auto& q : fs::directory_iterator(fTargetDirPath))
                        {
                            if (q.path().filename() == p.path().filename())
                            {
                                filenameMap[(std::wstring)p.path().filename()]++;
                                duplicateFound = true;
                            }
                        }
                    }
                    if (duplicateFound == false)
                    {
                        targetFile = std::wstring(fTargetDirPath) + L"\\" + (std::wstring)p.path().filename();
                        targetFile.erase(remove(targetFile.begin(), targetFile.end(), '\"'), targetFile.end());                             //Get rid of the double quotes from the filename
                        fs::rename(p.path(), (fs::path)targetFile);
                    }
                    else
                    {
                        targetFile = std::wstring(fTargetDirPath) + L"\\" + (std::wstring)p.path().stem() + L"(" + std::to_wstring(filenameMap[(std::wstring)p.path().filename()] - 1) + L")" + (std::wstring)p.path().extension();
                        targetFile.erase(remove(targetFile.begin(), targetFile.end(), '\"'), targetFile.end());                             //Get rid of the double quotes from the filename
                        fs::rename(p.path(), (fs::path)targetFile);
                    }
                }
                if ((pos != 1) && (pos != 0))
                {
                    ::MessageBox(mainWnd, L"Error parsing blacklist", L"Error", MB_OK);
                    return;
                }
            }
        }
    }
    ::MessageBox(mainWnd, L"Transfer successful!", L"Success", NULL);
}