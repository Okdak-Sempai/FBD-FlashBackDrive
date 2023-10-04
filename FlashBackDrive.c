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

int main()
{
    /*int exitStatus = executeDisableLongPathCode();*/
    unsigned int methodChoice = 2;
    while (methodChoice > 0 && methodChoice < 4)
    {
        methodChoice = 4;
        WCHAR* currentPath = currentDirectory();

        //Logs Folder
        int defaultLogStatus = defaultLogSetter();

        WCHAR* logsAdress = NULL;
        logsAdress = (WCHAR*)CoTaskMemAlloc((wcslen(currentPath) + 1) * sizeof(WCHAR));
        wcscpy_s(logsAdress, wcslen(currentPath) + 1, currentPath);
        int LogsAdressStatus = moveToFolder(&logsAdress, L"FBD Logs");
        if(LogsAdressStatus == -1)
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


        //Setting DefaultBackups file
        int defaultbackupStatus = defaultBackupSetter(); writeToLog(currentLogsFile, L"defaultbackupStatus", intToWCHAR(defaultbackupStatus));

        //Settings Setup
        int settingStatus = settingsSetter("FBDsettings"); writeToLog(currentLogsFile, L"settingStatus", intToWCHAR(settingStatus));


        //Initialisations
        const int maxDrives = 26; writeToLog(currentLogsFile, L"maxDrives", intToWCHAR(maxDrives));
        WCHAR* userChoice = NULL; writeToLog(currentLogsFile, L"userChoice", userChoice);
        WCHAR* drivePaths[26] = { NULL }; // maxDrives which is 20
        //Logs

        wprintf(L"==========================================\n");
        getDriveNames(drivePaths, maxDrives);
        //printDrives(drivePaths, maxDrives);
        for (int n = 0; n < 26; n++)
        {
            writeToLog(currentLogsFile, L"drivePaths", drivePaths[n]);

        }

        wprintf(L"\n");
        writeToLog(currentLogsFile, L"settingsPath('FBDsettings:')", settingsPath("FBDsettings")); //Print settings
        wprintf(L"\n");

        //Choice of method
        int scanfResult;
        methodChoice = 0; writeToLog(currentLogsFile, L"methodChoice", intToWCHAR(methodChoice));
        wprintf(L"\nPress 1 for Drive select\nPress 2 for folder selection\nPress 3 for the Settings\nPress 4 to end the Software\nChoice: ");
        do
        {
            scanfResult = scanf_s("%d%*c", &methodChoice);
            if (scanfResult != 1 || (methodChoice < 1 || methodChoice > 4))
            {
                wprintf(L"Only 1 or 2 or 3 or 4.\tChoice: ");
                while (getchar() != '\n');
            }
        } while (methodChoice < 1 || methodChoice > 4);
        writeToLog(currentLogsFile, L"methodChoice", intToWCHAR(methodChoice));
        wprintf(L"\n");

        WCHAR* rootPath = NULL;  writeToLog(currentLogsFile, L"rootPath", rootPath);
        WCHAR* star_rootPath = NULL; 
        
        switch (methodChoice)
        {

        case 1://Drives select
        {
            driveSelect(drivePaths, maxDrives, &userChoice); 
            rootPath = userChoice; writeToLog(currentLogsFile, L"userChoice", userChoice); writeToLog(currentLogsFile, L"rootPath", rootPath);
            star_rootPath = finalPathFileExplorer(rootPath); writeToLog(currentLogsFile, L"star_rootPath", star_rootPath);
            break;
        }

        case 2://Custom path select
        {
            rootPath = selectPathFolder(L"Backup path"); writeToLog(currentLogsFile, L"rootPath", rootPath);
            star_rootPath = finalPathFileExplorer(rootPath); writeToLog(currentLogsFile, L"star_rootPath", star_rootPath);
            break;
        }

        case 3://FlashBackDrive Settings
        {
            
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
            break;
        }

        case 4://End the Software
        {

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

            break;
        }
        }

        if (methodChoice == 1 || methodChoice == 2)
        {

            WCHAR* targetPath = NULL; writeToLog(currentLogsFile, L"targetPath", targetPath);
            writeToLog(currentLogsFile, L"settingsPath('FBDsettings:')", settingsPath("FBDsettings"));// Print the default
            if ((choice(L"Default Path", 0, L"Custom Path", 1)) == 1)
            {
                targetPath = selectPathFolder(L"Target"); writeToLog(currentLogsFile, L"targetPath", targetPath);
            }
            else
            {
                WCHAR* settingsResult = settingsPath("FBDsettings"); writeToLog(currentLogsFile, L"settingsResult",settingsResult);
                if (settingsResult != NULL)
                {
                    targetPath = (WCHAR*)CoTaskMemAlloc((wcslen(settingsResult) + 1) * sizeof(WCHAR));
                    if (targetPath != NULL)
                    {
                        wcscpy_s(targetPath, wcslen(settingsResult) + 1, settingsResult);
                    }
                }
                writeToLog(currentLogsFile, L"targetPath", targetPath);
            }
            WCHAR* star_targetPath = finalPathFileExplorer(targetPath); writeToLog(currentLogsFile, L"star_targetPath", star_targetPath);

            // Execution time starts
            LARGE_INTEGER startTime, endTime, frequency;
            QueryPerformanceFrequency(&frequency);
            QueryPerformanceCounter(&startTime);

            //Move to custom folder
            if (moveToFolder(&targetPath, newBackupFolderName(drivePaths, maxDrives, rootPath)) == -1)
            {
                wprintf(L"Critical Allocation error");
                for (int i = 0; i < maxDrives; i++)
                {
                    free(drivePaths[i]);
                }
                free(userChoice);
                CoTaskMemFree(targetPath);
                writeToLog(currentLogsFile, L"Return Error", L"-421");
                return -421; //Code 421 is just a code -1 but more easier to separate from others
            }

            // Register creation
            FILE* fp;
            WCHAR* registerAdress = createFileInDirectory(targetPath, L"FBDRegister"); 
            if (_wfopen_s(&fp, registerAdress, L"w") == 0)
            {
                fclose(fp);
            }
            else
            {
                wprintf(L"Failed to create the register: %ls\n", registerAdress);
            }

            int rootsize = countFilesInFolder(rootPath);
            int pathsize = countFilesInFolder(targetPath); 

            //Code usage
            fileExplorer(rootPath, star_rootPath, targetPath, star_targetPath, rootsize, pathsize, targetPath, registerAdress);
            writeToLog(currentLogsFile, L"rootPath", rootPath);
            writeToLog(currentLogsFile, L"star_rootPath", star_rootPath);
            writeToLog(currentLogsFile, L"targetPath", targetPath);
            writeToLog(currentLogsFile, L"star_targetPath", star_targetPath);
            writeToLog(currentLogsFile, L"registerAdress", registerAdress);
            writeToLog(currentLogsFile, L"rootsize", intToWCHAR(rootsize));
            writeToLog(currentLogsFile, L"pathsize", intToWCHAR(pathsize));

            pathsize = countFilesInFolder(targetPath); writeToLog(currentLogsFile, L"pathsize", intToWCHAR(pathsize));
            wprintf(L"\rProgression: %d%%", (int)(((float)pathsize / rootsize) * 100));
            wprintf(L"\n");

            //Result
            wprintf(L"Work done.");
            // Execution time ends
            QueryPerformanceCounter(&endTime);
            double elapsedTime = (endTime.QuadPart - startTime.QuadPart) * 1000.0 / frequency.QuadPart; writeToLog(currentLogsFile, L"elapsedTime", intToWCHAR((int)elapsedTime));
            int totalSeconds = (int)(elapsedTime / 1000); writeToLog(currentLogsFile, L"totalSeconds", intToWCHAR(totalSeconds));
            int hours = totalSeconds / 3600; writeToLog(currentLogsFile, L"hours", intToWCHAR(hours));
            int minutes = (totalSeconds % 3600) / 60; writeToLog(currentLogsFile, L"minutes", intToWCHAR(minutes));
            int seconds = totalSeconds % 60; writeToLog(currentLogsFile, L"seconds", intToWCHAR(seconds));
            wprintf(L"\nExecution time: %.2f millisecondes\t%02dH %02dM %02dS\n\a", elapsedTime, hours, minutes, seconds);

            wprintf(L"\nPress any button to open the folder.");
            getchar();
            openFolderInExplorer(targetPath);
            proceed();

            //All the free
            for (int i = 0; i < maxDrives; i++)
            {
                free(drivePaths[i]); drivePaths[i] = NULL;
            }
            free(userChoice); userChoice = NULL;
            free(registerAdress); registerAdress = NULL;
            CoTaskMemFree(targetPath); targetPath = NULL;
            free(star_rootPath); star_rootPath = NULL;
            free(star_targetPath); star_targetPath = NULL;
            //CoTaskMemFree(rootPath); rootPath = NULL;
            CoTaskMemFree(logsAdress); logsAdress = NULL;
            free(currentPath); currentPath = NULL;
            free(currentDate); currentDate = NULL;
            free(currentTime); currentTime = NULL;  
            free(LogsName); LogsName = NULL;
        }
    }
    return 0;
}
