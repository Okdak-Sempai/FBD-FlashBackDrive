#include <windows.h>
#include <wchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

void getDriveNames(WCHAR* drivePaths[], int maxDrives)
/*
* Includes:
*   windows.h [GetLogicalDriveStrings(); GetVolumeInformationW()]
*   stdio.h
*   stdlib.h [malloc(); free()]
*   wchar.h
* Params:
*   WCHAR* drivePaths[] = A WCHAR* array, (with the same size of maxDrives) It have to be empty because the function will fill it
*   int maxDrives = The maximum number of drive, it could be set by default to 26
* Return:
*   None
* Description:
*   This function prints all the drive on the machine with their name also. And stores their path in drivePaths[]
*   Ex:
*       Drive [C:\] - Volume Name: Boot
*       Drive [D:\] - Volume Name: Recover
* Notes:
*   It only stores the path and note the names
*/
{
    DWORD cchBuffer;
    WCHAR* driveStrings;
    WCHAR volumeName[MAX_PATH];

    // Find out how big a buffer we need
    cchBuffer = GetLogicalDriveStrings(0, NULL);

    driveStrings = (WCHAR*)malloc((cchBuffer + 1) * sizeof(WCHAR));
    if (driveStrings == NULL)
    {
        return;
    }

    // Fetch all drive strings    
    GetLogicalDriveStrings(cchBuffer, driveStrings);

    int count = 0; // To keep track of the number of drives found

    // Loop until we find the final '\0'
    // driveStrings is a double null terminated list of null terminated strings)
    WCHAR* currentDrive = driveStrings;
    while (*currentDrive && count < maxDrives)
    {
        // Get the volume name associated with the drive
        WCHAR volumeName[MAX_PATH] = { 0 };
        drivePaths[count] = _wcsdup(currentDrive);
        if (drivePaths[count] == NULL)
        {
            // Memory allocation failed
            break;
        }
        if (GetVolumeInformation(currentDrive, volumeName, MAX_PATH, NULL, NULL, NULL, NULL, 0))
        {
            printf("Drive [%S] - Volume Name: %S\n", currentDrive, volumeName);
        }
        else
        {
            printf("Drive [%S] - Volume Name: Not available\n", currentDrive);
        }

        // Move to next drive string
        currentDrive += lstrlen(currentDrive) + 1;
        count++;
    }

    free(driveStrings);
}

void printDrives(WCHAR* drivePathsP[], int maxDrivesP)
//Doc
/*
* Includes:
*   stdio.h [wprtinf()]
*   wchar.h [WCHAR]
* Params:
*   WCHAR* drivePathsP[] = A WCHAR* array that contains the drivepaths that have to be printed
*   int maxDrivesP = The maximum number of drivePathsP[]
* Return:
*   None
* Description:
*   This function prints the paths of each drive in drivePathsP[]
* Notes:
*   None
*/
{
    for (int i = 0; i < maxDrivesP; i++)
    {
        if (drivePathsP[i] != NULL)
        {
            wprintf(L"Drive %d: [%s]\n", i + 1, drivePathsP[i]);
        }
    }
}

void driveSelect(WCHAR* drivePathsP[], int maxDrivesP, WCHAR** userPathChoice)
/*
* Includes:
*   windows.h [GetVolumeInformationW()]
*   wchar.h [wprintf(); wscanf_s(); towupper()]
*   stdio.h [wprintf(); wprintf(); malloc()]
*   ctype.h [toupper()]
* Params:
*   WCHAR* drivePathsP[] = The array that stores the drives paths
*   int maxDrivesP = The size of the drivePathsP[] array
*   WCHAR** userPathChoice = The adress of a pointor that will store the path choosen
* Return:
*   None
* Description:
*   This function Prints all the drives on the machine and let the user choose for one of them by inputing its letter
*   This will put the path in userPathChoice, the path will allocated using malloc so it has to be free later
* Notes:
*   /!\ The userPathChoice has to be free() /!\
*/
{
    WCHAR userChoice;
    wprintf(L"Which drive do you want to choose, just Type its letter: ");
    while (1)
    {
        wscanf_s(L" %c", &userChoice, 1);
        userChoice = toupper(userChoice);
        for (int i = 0; i < maxDrivesP; i++)
        {
            if (drivePathsP[i] != NULL && (char)(drivePathsP[i][0]) == userChoice)
            {
                WCHAR volumeName[MAX_PATH] = { 0 };
                if (GetVolumeInformation(drivePathsP[i], volumeName, MAX_PATH, NULL, NULL, NULL, NULL, 0))
                {
                    wprintf(L"Drive letter '%c' corresponds to path: %s - %s\n", userChoice, drivePathsP[i], volumeName);
                    *userPathChoice = (WCHAR*)malloc((wcslen(drivePathsP[i]) + 1) * sizeof(WCHAR));
                    if (*userPathChoice != NULL)
                    {
                        wcscpy_s(*userPathChoice, wcslen(drivePathsP[i]) + 1, drivePathsP[i]);
                    }
                    else
                    {
                        wprintf(L"Memory allocation failed.\n");
                    }
                }
                return;
            }
        }

        // If the letter is invalid, the user have to retry
        wprintf(L"Invalid drive letter. Please try again: ");
        userChoice = NULL;
    }

}

void driveNameGetter(WCHAR* drivePathsP[], int maxDrivesP, WCHAR** userPathChoice, WCHAR** driveName)
/*
* Includes:
*   windows.h [GetVolumeInformationW()]
*   wchar.h [_wcsdup()]
*   stdio.h [wprintf()]
*   ctype.h [toupper()]
* Params:
*   WCHAR* drivePathsP[] = The array that contains all the drive paths
*   int maxDrivesP = The size of drivePathsP[]
*   WCHAR** userPathChoice = The pointor of the the path to get the drive name
*   WCHAR** driveName = A WCHAR* pointor that will receive the Name of the Drive of the userPathChoice, Data will be written int it
* Return:
*   None
* Description:
*   This function put the name of the drive where userPathChoice is located in driveName using the array of all the drive on the machine
* Notes:
*   /!\ The driveName has to be free() /!\
*/
{
    WCHAR userChoice;
    while (1)
    {
        userChoice = toupper((*userPathChoice)[0]);
        for (int i = 0; i < maxDrivesP; i++)
        {
            if (drivePathsP[i] != NULL && (char)(drivePathsP[i][0]) == userChoice)
            {
                WCHAR volumeName[MAX_PATH] = { 0 };
                if (GetVolumeInformation(drivePathsP[i], volumeName, MAX_PATH, NULL, NULL, NULL, NULL, 0))
                {
                    wprintf(L"Drive letter '%c' corresponds to path: %s - %s\n", userChoice, drivePathsP[i], volumeName);
                    *driveName = _wcsdup(volumeName);
                    if (*driveName == NULL)
                    {
                        wprintf(L"Memory allocation failed.\n");
                    }
                }
                return;
            }
        }

        // If the letter is invalid, the user have to retry
        wprintf(L"Invalid drive letter. Please try again: ");
        userChoice = NULL;
    }

}