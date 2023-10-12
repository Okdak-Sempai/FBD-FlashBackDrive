#ifndef FBD_ALL_DRIVES_FUNCS_W_H
#define FBD_ALL_DRIVES_FUNCS_W_H

#include <windows.h>

int allDriveSelect(WCHAR* drivePathsP[], int maxDrivesP, WCHAR* drivesLetters, int* drivesLettersCounter, WCHAR** userPathChoice);
void allGetDriveNames(WCHAR* drivePaths[], int maxDrives);

#endif