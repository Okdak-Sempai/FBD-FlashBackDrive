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
#include "FBD_AllDrivesFuncs_W.h"

// V 1.1

int main()
{
    WCHAR* currentLogsFile = NULL;
    /*int exitStatus = executeDisableLongPathCode();*/
    unsigned int methodChoice = 2;
    unsigned int alldrivesFLAG = 0;
    unsigned int alldrivesCount = 0;
    int prog = 1;
    int onlyRegisterAlldrives;
    WCHAR alldrives[] = { L'A', L'B', L'C', L'D', L'E', L'F', L'G', L'H', L'I', L'J', L'K', L'L', L'M', L'N', L'O', L'P', L'Q', L'R', L'S', L'T', L'U', L'V', L'W', L'X', L'Y', L'Z', L'\0' }; // To keep for comparison
    //WCHAR alldrives[] = { L'a', L'b', L'.', L'd', L'.', L'f', L'g', L'h', L'i', L'j', L'k', L'l', L'.', L'.', L'o', L'p', L'q', L'r', L'.', L't', L'u', L'v', L'w', L'x', L'y', L'z', L'\0' };
    //WCHAR alldrives[] = { L'.', L'.', L'.', L'.', L'.', L'.', L'.', L'.', L'.', L'.', L'.', L'.', L'.', L'.', L'.', L'.', L'.', L'.', L'.', L'.', L'.', L'.', L'.', L'.', L'.', L'.', L'\0' };

    WCHAR* pathToExclude = NULL;

    while (methodChoice > 0 && methodChoice < 5)
    {
        methodChoice = 4;
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


        currentLogsFile = createFileInDirectory(logsAdress, LogsName);

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

        if (alldrivesFLAG == 0)
        {
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
            methodChoice = 0; writeToLog(currentLogsFile, L"methodChoice", intToWCHAR(methodChoice));
            wprintf(L"\nPress 1 for All the drives[Following DrivesSettings]\nPress 2 for Drive select\nPress 3 for folder selection\nPress 4 for the Settings\nPress 5 to end the Software\nChoice: ");
            do
            {
                scanfResult = scanf_s("%d%*c", &methodChoice);
                if (scanfResult != 1 || (methodChoice < 1 || methodChoice > 5))
                {
                    wprintf(L"Only 1 or 2 or 3 or 4.\tChoice: ");
                    while (getchar() != '\n');
                }
            } while (methodChoice < 1 || methodChoice > 5);
            writeToLog(currentLogsFile, L"methodChoice", intToWCHAR(methodChoice));
            wprintf(L"\n");

        }
        else //drivesFLOG == 1
        {
            allGetDriveNames(drivePaths, maxDrives);
            for (int n = 0; n < 26; n++)
            {
                writeToLog(currentLogsFile, L"drivePaths", drivePaths[n]);
            }
            methodChoice = 1; writeToLog(currentLogsFile, L"methodChoice", intToWCHAR(methodChoice));

        }

        switch (methodChoice)
        {

        case 1:
        {
            if (alldrivesFLAG == 0)
            {
                onlyRegisterAlldrives = 0;
                onlyRegisterAlldrives = choice(L"To only save the paths in the register(Faster)[No progress bar]", 0, L"To duplicate the file hierarchy(as txt) AND the register(Slower but better visually)[Includ progress bar]", 1);
                wprintf(L"Press any button to confirm.\n");
                getchar();
            }
            alldrivesFLAG = 1; writeToLog(currentLogsFile, L"alldrivesFLAG", intToWCHAR(alldrivesFLAG));
            allDriveSelect(drivePaths, maxDrives, settingsAllDrivesPath, &alldrivesCount, &userChoice); writeToLog(currentLogsFile, L"alldrivesCount", intToWCHAR(alldrivesCount));
            //Loop break
            if (alldrivesCount > 25)
            {
                alldrivesFLAG = 0; writeToLog(currentLogsFile, L"alldrivesFLAG", intToWCHAR(alldrivesFLAG));
                methodChoice = 4; writeToLog(currentLogsFile, L"methodChoice", intToWCHAR(methodChoice));
                alldrivesCount = 0; writeToLog(currentLogsFile, L"alldrivesCount", intToWCHAR(alldrivesCount));
                wprintf(L"Work done.\n");
                proceed();
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

            //UserChoice get the adress of the drive
            rootPath = userChoice; writeToLog(currentLogsFile, L"userChoice", userChoice); writeToLog(currentLogsFile, L"rootPath", rootPath);
            star_rootPath = finalPathFileExplorer(rootPath); writeToLog(currentLogsFile, L"star_rootPath", star_rootPath);

            break;
        }

        case 2://Drives select
        {
            driveSelect(drivePaths, maxDrives, &userChoice);
            rootPath = userChoice; writeToLog(currentLogsFile, L"userChoice", userChoice); writeToLog(currentLogsFile, L"rootPath", rootPath);
            star_rootPath = finalPathFileExplorer(rootPath); writeToLog(currentLogsFile, L"star_rootPath", star_rootPath);
            break;
        }

        case 3://Custom path select
        {
            rootPath = selectPathFolder(L"Backup path"); writeToLog(currentLogsFile, L"rootPath", rootPath);
            if (rootPath == NULL)
            {
                wprintf(L"Empty path!\a\n");
                proceed();
                system("cls");
                methodChoice = 4;
                break;
            }
            star_rootPath = finalPathFileExplorer(rootPath); writeToLog(currentLogsFile, L"star_rootPath", star_rootPath);
            break;
        }

        case 4://FlashBackDrive Settings
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

        case 5://End the Software
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

        if (methodChoice == 2 || methodChoice == 3)
        {
            int onlyRegister = 0;
            onlyRegister = choice(L"To only save the paths in the register(Faster)[No progress bar]", 0, L"To duplicate the file hierarchy(as txt) AND the register(Slower but better visually)[Includ progress bar]", 1);

            WCHAR* targetPath = NULL; writeToLog(currentLogsFile, L"targetPath", targetPath);
            writeToLog(currentLogsFile, L"settingsPath('FBDsettings:')", settingsPath("FBDsettings"));// Print the default
            if ((choice(L"Default Path", 0, L"Custom Path", 1)) == 1)
            {
                targetPath = selectPathFolder(L"Target"); writeToLog(currentLogsFile, L"targetPath", targetPath);
            }
            else
            {
                WCHAR* settingsResult = settingsPath("FBDsettings"); writeToLog(currentLogsFile, L"settingsResult", settingsResult);
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
            if (targetPath != NULL)
            {
                WCHAR* star_targetPath = finalPathFileExplorer(targetPath); writeToLog(currentLogsFile, L"star_targetPath", star_targetPath);
                WCHAR* pathToExclude = (WCHAR*)malloc((wcslen(targetPath) + 1) * sizeof(WCHAR)); // +1 for the null-terminator
                if (pathToExclude != NULL)
                {
                    wcscpy_s(pathToExclude, wcslen(targetPath) + 1, targetPath); writeToLog(currentLogsFile, L"pathToExclude", pathToExclude);
                }
                else
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
                int pathsize = countFilesInFolder(targetPath);//idk why

                //Code usage
                if (onlyRegister == 0)
                {
                    fileExplorerRegisterOnly(rootPath, star_rootPath, targetPath, star_targetPath, rootsize, pathsize, targetPath, registerAdress, pathToExclude);
                }
                else
                {
                    fileExplorer(rootPath, star_rootPath, targetPath, star_targetPath, rootsize, pathsize, targetPath, registerAdress, pathToExclude);
                }
                writeToLog(currentLogsFile, L"rootPath", rootPath);
                writeToLog(currentLogsFile, L"star_rootPath", star_rootPath);
                writeToLog(currentLogsFile, L"targetPath", targetPath);
                writeToLog(currentLogsFile, L"star_targetPath", star_targetPath);
                writeToLog(currentLogsFile, L"registerAdress", registerAdress);
                writeToLog(currentLogsFile, L"rootsize", intToWCHAR(rootsize));
                writeToLog(currentLogsFile, L"pathsize", intToWCHAR(pathsize));

                pathsize = countFilesInFolder(targetPath) - 1; writeToLog(currentLogsFile, L"pathsize", intToWCHAR(pathsize)); //Do not count the register
                if (onlyRegister == 0)
                {
                    wprintf(L"\rProgression: %d%%", (int)(((float)pathsize / rootsize) * 100));
                }
                else
                {
                    wprintf(L"\rProgression: 100%%");
                }
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
                free(pathToExclude); pathToExclude = NULL;
            }
            else
            {
                CoTaskMemFree(targetPath); targetPath = NULL;
                wprintf(L"Empty path!\a\n");
                proceed();
                system("cls");
                methodChoice = 4;
            }
        }

        if (methodChoice == 1) // All drives methods

        {

            WCHAR* targetPath = NULL; writeToLog(currentLogsFile, L"targetPath", targetPath);
            writeToLog(currentLogsFile, L"settingsPath('FBDsettings:')", settingsPath("FBDsettings"));// Print the default
            WCHAR* settingsResult = settingsPath("FBDsettings"); writeToLog(currentLogsFile, L"settingsResult", settingsResult);
            if (settingsResult != NULL)
            {
                targetPath = (WCHAR*)CoTaskMemAlloc((wcslen(settingsResult) + 1) * sizeof(WCHAR));
                if (targetPath != NULL)
                {
                    wcscpy_s(targetPath, wcslen(settingsResult) + 1, settingsResult);
                }
            }
            writeToLog(currentLogsFile, L"targetPath", targetPath);

            WCHAR* star_targetPath = finalPathFileExplorer(targetPath); writeToLog(currentLogsFile, L"star_targetPath", star_targetPath);
            WCHAR* pathToExclude = (WCHAR*)malloc((wcslen(targetPath) + 1) * sizeof(WCHAR)); // +1 for the null-terminator
            if (pathToExclude != NULL)
            {
                wcscpy_s(pathToExclude, wcslen(targetPath) + 1, targetPath); writeToLog(currentLogsFile, L"pathToExclude", pathToExclude);
            }
            else
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
            //fileExplorer(rootPath, star_rootPath, targetPath, star_targetPath, rootsize, pathsize, targetPath, registerAdress, pathToExclude);
            if (onlyRegisterAlldrives == 0)
            {
                fileExplorerRegisterOnly(rootPath, star_rootPath, targetPath, star_targetPath, rootsize, pathsize, targetPath, registerAdress, pathToExclude);

            }
            else
            {
                fileExplorer(rootPath, star_rootPath, targetPath, star_targetPath, rootsize, pathsize, targetPath, registerAdress, pathToExclude);
            }
            writeToLog(currentLogsFile, L"rootPath", rootPath);
            writeToLog(currentLogsFile, L"star_rootPath", star_rootPath);
            writeToLog(currentLogsFile, L"targetPath", targetPath);
            writeToLog(currentLogsFile, L"star_targetPath", star_targetPath);
            writeToLog(currentLogsFile, L"registerAdress", registerAdress);
            writeToLog(currentLogsFile, L"rootsize", intToWCHAR(rootsize));
            writeToLog(currentLogsFile, L"pathsize", intToWCHAR(pathsize));

            pathsize = countFilesInFolder(targetPath)-1; writeToLog(currentLogsFile, L"pathsize", intToWCHAR(pathsize)); //Do not count the register
            if (onlyRegisterAlldrives == 0)
            {
                wprintf(L"\rProgression: %d%%", (int)(((float)pathsize / rootsize) * 100));
            }
            else
            {
                wprintf(L"\rProgression: 100%%");
            }
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
            wprintf(L"\nExecution time: %.2f millisecondes\t%02dH %02dM %02dS\n\a\n", elapsedTime, hours, minutes, seconds);

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
            free(pathToExclude); pathToExclude = NULL;
        }
    }

    writeToLog(currentLogsFile, L"FBD FlashBackDrive Return", L"0");
    return 0;
}
