#pragma once
#include "framework.h"
#include "resource.h"
#include "commctrl.h"
#include <filesystem>
#include <string>
#include <algorithm>
#include <map>


namespace fs = std::filesystem;

std::wstring BasicFileOpen();
std::wstring BasicFileOpen2();
INT_PTR CALLBACK BlacklistProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK WhitelistProc(HWND, UINT, WPARAM, LPARAM);
void MoveFilesNone(HWND, bool);
void MoveFilesWhite(HWND, bool);
void MoveFilesBlack(HWND, bool);

extern fs::path mainPath;
extern wchar_t fFileDir[1000];                   //Buffer for the source directory.
extern wchar_t fTargetDirPath[1000];             //Buffer for the destination directory.
extern wchar_t fFileName[1000];                  //Buffer for the user-set filename.