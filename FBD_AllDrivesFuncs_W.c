#include <windows.h>
#include <wchar.h>
#include <stdio.h>
#include <stdlib.h>

int allDriveSelect(WCHAR* drivePathsP[], int maxDrivesP, WCHAR* drivesLetters, int* drivesLettersCounter, WCHAR** userPathChoice)
/*
* Includes:
*   windows.h [GetVolumeInformation)]
*   wchar.h [WCHAR; wprintf(); wcscpy_s(); towupper()]
*   stdio.h [wprintf(); wprintf(); malloc()]
* Params:
*   WCHAR* drivePathsP[] = The array that stores the drives paths
*   int maxDrivesP = The size of the drivePathsP[] array
*   WCHAR* drivesLetters = The list of drives to parse, an array of the 26 letters
*   int* drivesLettersCounter = The adress of the counter of the driveLetters array
*   WCHAR** userPathChoice = The adress of a pointor that will store the path choosen
* Return:
*   int => The value of drivesLettersCounter
* Description:
*   This function will seeking the matching letters in drivesLetters and returns a path of a drive corresponding if it exists
*   This will put the path in userPathChoice, the path will allocated using malloc so it has to be free later
* Notes:
*   /!\ The userPathChoice has to be free() /!\
*/
{
    while (*drivesLettersCounter<26)
    {
        //Get the letter 
        WCHAR letter = drivesLetters[*drivesLettersCounter];
        letter = toupper(letter);

        for (int i = 0; i < maxDrivesP; i++)
        {
            //if (drivePathsP[i] != NULL && (char)(drivePathsP[i][0]) == letter)
            if (drivePathsP[i] != NULL && (char)(drivePathsP[i][0]) == letter && wcscmp(drivePathsP[i], L".") != 0)
            {
                WCHAR volumeName[MAX_PATH] = { 0 };
                if (GetVolumeInformation(drivePathsP[i], volumeName, MAX_PATH, NULL, NULL, NULL, NULL, 0))
                {
                    wprintf(L"Drive letter '%c' corresponds to path: %s - %s\n", letter, drivePathsP[i], volumeName);
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

                (*drivesLettersCounter)++;
                return *drivesLettersCounter;
            }
        }

        // Letter non existent
        (*drivesLettersCounter)++;
    }
    return *drivesLettersCounter;

}

void allGetDriveNames(WCHAR* drivePaths[], int maxDrives)
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
*   This function stores the path of the drives on the machine in drivePaths[]
*   Ex:
*       Drive [C:\] - Volume Name: Boot
*       Drive [D:\] - Volume Name: Recover
* Notes:
*   This functions does not makes any print
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
        GetVolumeInformation(currentDrive, volumeName, MAX_PATH, NULL, NULL, NULL, NULL, 0);

        // Move to next drive string
        currentDrive += lstrlen(currentDrive) + 1;
        count++;
    }

    free(driveStrings);
}