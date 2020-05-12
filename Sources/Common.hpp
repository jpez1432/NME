
#ifndef COMMON_HPP
#define COMMON_HPP

#include "Config.hpp"
#include "Logger.hpp"

extern CConfig *Config;
extern CLogger *Logger;

extern int CurrentVertex;
extern int CurrentFace;
extern int CurrentGroup;
extern int CurrentMaterial;

extern glm::vec3 Cursor;
extern bool Dirty;
extern std::string InterfaceFile;

std::string GetInfo();
std::string FileOpen(HWND hWnd, char *Filter, char *InitPath, char *DefaultExt);
std::string FileSave(HWND hWnd, char *Filter, char *InitPath, char *DefaultExt);
std::string FolderBrowse(HWND hWnd, std::string Caption);
const char *UpperStr(std::string Text);
bool StrCompare(const std::string& Str1, const std::string& Str2);

#endif

