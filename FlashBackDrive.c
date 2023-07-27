#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

void driveSelect() {}
void fileparser() {}
void createBackupFile() {}


void getDriveNames(WCHAR* drivePaths[], int maxDrives) {
    DWORD cchBuffer;
    WCHAR* driveStrings;
    WCHAR volumeName[MAX_PATH];

    // Find out how big a buffer we need
    cchBuffer = GetLogicalDriveStrings(0, NULL);

    driveStrings = (WCHAR*)malloc((cchBuffer + 1) * sizeof(WCHAR));
    if (driveStrings == NULL) {
        return;
    }

    // Fetch all drive strings    
    GetLogicalDriveStrings(cchBuffer, driveStrings);

    int count = 0; // To keep track of the number of drives found

    // Loop until we find the final '\0'
    // driveStrings is a double null terminated list of null terminated strings)
    WCHAR* currentDrive = driveStrings;
    while (*currentDrive && count < maxDrives) {
        // Get the volume name associated with the drive
        WCHAR volumeName[MAX_PATH] = { 0 };
        drivePaths[count] = _wcsdup(currentDrive);
        if (drivePaths[count] == NULL) {
            // Memory allocation failed
            break;
        }
        if (GetVolumeInformation(currentDrive, volumeName, MAX_PATH, NULL, NULL, NULL, NULL, 0)) {
            printf("Drive: %S - Volume Name: %S\n", currentDrive, volumeName);
        }
        else {
            printf("Drive: %S - Volume Name: Not available\n", currentDrive);
        }

        // Move to next drive string
        currentDrive += lstrlen(currentDrive) + 1;
        count++;
    }

    free(driveStrings);
}

void printDrives(WCHAR* drivePathsP[], int maxDrivesP)
{
    for (int i = 0; i < maxDrivesP; i++) {
        if (drivePathsP[i] != NULL) {
            wprintf(L"Drive %d: %s\n", i + 1, drivePathsP[i]);
            free(drivePathsP[i]); // Don't forget to free the memory
        }
    }
}

int main() {
    const int maxDrives = 20;
    WCHAR* drivePaths[20] = { NULL }; // maxDrives which is 20
    getDriveNames(drivePaths, maxDrives);
    printDrives(drivePaths, maxDrives);
    return 0;
}
