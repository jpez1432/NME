
#include "Common.hpp"

#include <shlobj.h>

#ifdef _DEBUG32
const char *Build = "[x86 Debug]";
#elif _RELEASE32
const char *Build = "[x86 Release]";
#elif _RELEASE64
const char *Build = "[x64 Release]";
#endif

static inline bool CaseInsCharCompareN(char A, char B) { return(std::toupper(A) == std::toupper(B)); }

const char *UpperStr(std::string Text)
{
    for (unsigned int i = 0; i < Text.size(); i++) {
        Text[i] = std::toupper(Text[i]);
    }
    return Text.c_str();
}

std::string GetInfo()
{
    TCHAR VersionFile[MAX_PATH];
    GetModuleFileName(NULL, VersionFile, MAX_PATH);

    char Info[64];
    DWORD Handle = 0;
    UINT Size = 0;
    LPBYTE Buffer = NULL;
    DWORD VersionSize = GetFileVersionInfoSize(VersionFile, &Handle);

    if(VersionSize != 0) {
        LPSTR VersionData = new char[VersionSize];
        if(GetFileVersionInfo(VersionFile, Handle, VersionSize, VersionData)) {
            if(VerQueryValue(VersionData, "\\", (VOID FAR * FAR*)&Buffer, &Size)) {
                if(Size) {
                    VS_FIXEDFILEINFO *VersionInfo = (VS_FIXEDFILEINFO *)Buffer;
                    if(VersionInfo->dwSignature == 0xfeef04bd) {
                        sprintf(Info, "NME - v%d.%d.%d %s",
                                int(VersionInfo->dwFileVersionMS >> 16) & 0xffff,
                                int(VersionInfo->dwFileVersionMS >>  0) & 0xffff,
                                int(VersionInfo->dwFileVersionLS >> 16) & 0xffff,
                                Build
                               );
                    }
                }
            }
        }
        delete[] VersionData;
    }

    return std::string(Info);
}

std::string FileOpen(HWND hWnd, char *Filter, char *InitPath, char *DefaultExt)
{

    OPENFILENAME ofn;
    char szFile[260];
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = Filter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrDefExt = DefaultExt;
    ofn.lpstrInitialDir = InitPath;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if(GetOpenFileName(&ofn) == TRUE) {
        return std::string(ofn.lpstrFile);
    } else {
        return "";
    }
}

std::string FileSave(HWND hWnd, char *Filter, char *InitPath, char *DefaultExt)
{
    OPENFILENAME ofn;
    char szFile[260];
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = Filter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrDefExt = DefaultExt;
    ofn.lpstrInitialDir = InitPath;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if(GetSaveFileName(&ofn) == TRUE) {
        return std::string(ofn.lpstrFile);
    } else {
        return "";
    }
}

std::string FolderBrowse(HWND hWnd, std::string Caption)
{
    BROWSEINFO bi;
    memset(&bi, 0, sizeof(bi));

    bi.ulFlags = BIF_USENEWUI;
    bi.hwndOwner = hWnd;
    bi.lpszTitle = Caption.c_str();

    LPITEMIDLIST pIDL = SHBrowseForFolder(&bi);

    char Buffer[MAX_PATH] = {'\0'};
    if(SHGetPathFromIDList(pIDL, Buffer) != 0) {
        return std::string(Buffer);
    } else {
        return "";
    }

}

bool StrCompare(const std::string& Str1, const std::string& Str2) {
    return ((Str1.size() == Str2.size()) && equal(Str1.begin(), Str1.end(), Str2.begin(), CaseInsCharCompareN));
}
