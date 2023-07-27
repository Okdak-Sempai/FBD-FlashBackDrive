#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

void driveSelect() {}
void fileparser() {}
void createBackupFile() {}


void getDriveNames() {
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

    // Loop until we find the final '\0'
    // driveStrings is a double null terminated list of null terminated strings)
    WCHAR* currentDrive = driveStrings;
    while (*currentDrive) {
        // Get the volume name associated with the drive
        WCHAR volumeName[MAX_PATH] = { 0 };
        if (GetVolumeInformation(currentDrive, volumeName, MAX_PATH, NULL, NULL, NULL, NULL, 0)) {
            printf("Drive: %S - Volume Name: %S\n", currentDrive, volumeName);
        }
        else {
            printf("Drive: %S - Volume Name: Not available\n", currentDrive);
        }

        // Move to next drive string
        currentDrive += lstrlen(currentDrive) + 1;
    }

    free(driveStrings);
}

int main() {
    getDriveNames();
    return 0;
}
