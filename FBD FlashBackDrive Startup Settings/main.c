#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <fileapi.h>
#include <wchar.h>

#include "FilesFuncs_W.h"
#include "FoldersFuncs_W.h"
#include "Tools_W.h"
#include "DrivesFuncs_W.h"
#include "FBDFuncs_W.h"

// V 1.1

int main()
{
    /*int exitStatus = executeDisableLongPathCode();*/
    unsigned int alldrivesFLAG = 0;
    unsigned int alldrivesCount = 0;
    int onlyRegisterAlldrives;
    WCHAR alldrives[] = { L'A', L'B', L'C', L'D', L'E', L'F', L'G', L'H', L'I', L'J', L'K', L'L', L'M', L'N', L'O', L'P', L'Q', L'R', L'S', L'T', L'U', L'V', L'W', L'X', L'Y', L'Z', L'\0' }; // To keep for comparison
    //WCHAR alldrives[] = { L'a', L'b', L'.', L'd', L'.', L'f', L'g', L'h', L'i', L'j', L'k', L'l', L'.', L'.', L'o', L'p', L'q', L'r', L'.', L't', L'u', L'v', L'w', L'x', L'y', L'z', L'\0' };
    //WCHAR alldrives[] = { L'.', L'.', L'.', L'.', L'.', L'.', L'.', L'.', L'.', L'.', L'.', L'.', L'.', L'.', L'.', L'.', L'.', L'.', L'.', L'.', L'.', L'.', L'.', L'.', L'.', L'.', L'\0' };

    WCHAR* pathToExclude = NULL;
    WCHAR* currentPath = currentDirectory();

    //Logs Folder
    int defaultLogStatus = defaultLogSetter();

    WCHAR* logsAdress = NULL;
    logsAdress = (WCHAR*)CoTaskMemAlloc((wcslen(currentPath) + 1) * sizeof(WCHAR));
    wcscpy_s(logsAdress, wcslen(currentPath) + 1, currentPath);
    int LogsAdressStatus = moveToFolder(&logsAdress, L"FBD Logs");
    if (LogsAdressStatus == -1)
    {
        wprintf(L"Critical Allocation error");
        free(currentPath);
        CoTaskMemFree(logsAdress);
        return -421; //Code 421 is just a code -1 but more easier to separate from others
    }

    //Current logs file
    WCHAR* LogsName = NULL;
    WCHAR* currentDate = getCurrentDateWCHAR();
    WCHAR* currentTime = getCurrentTimeWCHAR();

    if (currentDate != NULL && currentTime != NULL)
    {
        size_t bufferSize = wcslen(currentDate) + wcslen(currentTime) + wcslen(L" FBD Logs ") + 1;
        LogsName = (WCHAR*)malloc(bufferSize * sizeof(WCHAR));
        if (LogsName != NULL)
        {
            swprintf(LogsName, bufferSize, L"FBD Logs %s_%s", currentDate, currentTime);
        }
        else
        {
            wprintf(L"Memory allocation failed for LogsName.\n");
            return -422;
        }
    }
    else
    {
        wprintf(L"Error getting current date or time.\n");
        return -423;
    }

    WCHAR* currentLogsFile = createFileInDirectory(logsAdress, LogsName);

    //Logs savings
    writeToLog(currentLogsFile, L"currentPath", currentPath);
    writeToLog(currentLogsFile, L"defaultLogStatus", intToWCHAR(defaultLogStatus));
    writeToLog(currentLogsFile, L"logsAdress", logsAdress);
    writeToLog(currentLogsFile, L"LogsAdressStatus", intToWCHAR(LogsAdressStatus));
    writeToLog(currentLogsFile, L"LogsName", LogsName);
    writeToLog(currentLogsFile, L"currentDate", currentDate);
    writeToLog(currentLogsFile, L"currentTime", currentTime);
    writeToLog(currentLogsFile, L"alldrivesFLAG", intToWCHAR(alldrivesFLAG));
    writeToLog(currentLogsFile, L"alldrivesCount", intToWCHAR(alldrivesCount));


    //Setting DefaultBackups file
    int defaultbackupStatus = defaultBackupSetter(); writeToLog(currentLogsFile, L"defaultbackupStatus", intToWCHAR(defaultbackupStatus));

    //Settings Setup
    int settingStatus = settingsSetter("FBDsettings"); writeToLog(currentLogsFile, L"settingStatus", intToWCHAR(settingStatus));

    //SettingsAllDrives Setup
    int settingsAllDrivesStatus = settingsAllDrivesSetter("FBDdrivesSettings"); writeToLog(currentLogsFile, L"settingsAllDrivesStatus", intToWCHAR(settingsAllDrivesStatus));

    WCHAR* settingsAllDrivesPath = getSettingsPath("FBDdrivesSettings");

    for (int i = 0; i < 26; i++)
    {
        WCHAR letter = settingsAllDrivesPath[i];
        writeToLog(currentLogsFile, L"alldrives", &letter);
    }

    //Initialisations
    const int maxDrives = 26; writeToLog(currentLogsFile, L"maxDrives", intToWCHAR(maxDrives));
    WCHAR* userChoice = NULL; writeToLog(currentLogsFile, L"userChoice", userChoice);
    WCHAR* drivePaths[26] = { NULL }; // maxDrives which is 20
    WCHAR* rootPath = NULL;  writeToLog(currentLogsFile, L"rootPath", rootPath);
    WCHAR* star_rootPath = NULL; writeToLog(currentLogsFile, L"star_rootPath", star_rootPath);

    wprintf(L"==========================================\n");
    getDriveNames(drivePaths, maxDrives);
    //printDrives(drivePaths, maxDrives);
    for (int n = 0; n < 26; n++)
    {
        writeToLog(currentLogsFile, L"drivePaths", drivePaths[n]);

    }

    wprintf(L"\n");
    writeToLog(currentLogsFile, L"settingsPath('FBDsettings:')", settingsPath("FBDsettings")); //Print settings
    writeToLog(currentLogsFile, L"settingsPath('FBDdrivesSettings:')", settingsPathAD("FBDdrivesSettings")); //Print settings
    //Print all drives settings
    wprintf(L"\n");

    //Choice of method
    int scanfResult;

    wprintf(L"\n");


    //FlashBackDrive Settings

    int settingsReturn = settingsSetterDefaultFinal("FBDsettings");
    writeToLog(currentLogsFile, L"settingsReturn", intToWCHAR(settingsReturn));
    //All the free
    for (int i = 0; i < maxDrives; i++)
    {
        free(drivePaths[i]); drivePaths[i] = NULL;
    }
    free(userChoice); userChoice = NULL;
    CoTaskMemFree(rootPath); rootPath = NULL;
    free(star_rootPath); star_rootPath = NULL;
    free(currentPath);
    CoTaskMemFree(logsAdress); logsAdress = NULL;
    free(currentDate); currentDate = NULL;
    free(currentTime); currentTime = NULL;
    free(LogsName); LogsName = NULL;

    proceed();
    return 0;
}
