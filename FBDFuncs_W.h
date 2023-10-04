#ifndef FBD_FUNCS_W_H
#define FBD_FUNCS_W_H

#include <stdio.h>
#include <wchar.h>
#include <windows.h>

int writeLineRegister(FILE* file, const WCHAR* content);
int writeToLog(WCHAR* logsAdress, const WCHAR* variableName, const WCHAR* variableValue);
int defaultLogSetter();
int defaultBackupSetter();
int settingsSetter(char* settingsfilename);
int settingsSetterDefaultFinal(char* settingsfilename);
WCHAR* settingsPath(char* settingsfilename);
WCHAR* newBackupFolderName(WCHAR* drivePathsP[], int maxDrivesPconst, WCHAR* fullPath);
void createBackupFile(int Params, const WCHAR* filePath, const WCHAR* folderOutput, const WCHAR* registerAdress, int* registerLine);
void fileExplorer(WCHAR* Real_filepathInit, WCHAR* filepathInit, WCHAR* Real_filepathDEST, WCHAR* filepathDEST, DWORD rootsize, DWORD pathsize, WCHAR* usedPATH, const WCHAR* registerAdress);

#endif