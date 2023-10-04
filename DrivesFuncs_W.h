#ifndef DRIVE_FUNCS_W_H
#define DRIVE_FUNCS_W_H

#include <windows.h>

void getDriveNames(WCHAR* drivePaths[], int maxDrives);
void printDrives(WCHAR* drivePathsP[], int maxDrivesP);
void driveSelect(WCHAR* drivePathsP[], int maxDrivesP, WCHAR** userPathChoice);
void driveNameGetter(WCHAR* drivePathsP[], int maxDrivesP, WCHAR** userPathChoice, WCHAR** driveName);

#endif

