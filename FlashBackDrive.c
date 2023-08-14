#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <shlobj.h>
#include <Shellapi.h>
#include <tchar.h>
#include <fileapi.h>

DWORD getFileSize(const wchar_t* filePath) {
    HANDLE hFile = CreateFile(filePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        return -1;
    }

    DWORD fileSize = GetFileSize(hFile, NULL);
    CloseHandle(hFile);
    return fileSize;
}

const WCHAR* GetFileExtension(const WCHAR* filePath) {
    const WCHAR* lastDot = wcsrchr(filePath, L'.');
    if (lastDot != NULL) {
        return lastDot + 1;
    }
    else {
        return NULL; // Aucune extension trouvée
    }
}

WCHAR* GetLastModifiedDate(const WCHAR* filePath) {
    WIN32_FILE_ATTRIBUTE_DATA fileInfo;

    if (GetFileAttributesEx(filePath, GetFileExInfoStandard, &fileInfo)) {
        FILETIME lastModifiedTime = fileInfo.ftLastWriteTime;

        SYSTEMTIME sysTime;
        FileTimeToSystemTime(&lastModifiedTime, &sysTime);

        // Créer une chaîne au format souhaité
        WCHAR* result = (WCHAR*)malloc(20 * sizeof(WCHAR)); // As per "YYYY-MM-DD HH:MM:SS" format
        if (result != NULL) {
            swprintf(result, 20, L"%04d-%02d-%02d %02d:%02d:%02d",
                sysTime.wYear, sysTime.wMonth, sysTime.wDay,
                sysTime.wHour, sysTime.wMinute, sysTime.wSecond);
        }

        return result;
    }
    else {
        wprintf(L"Impossible d'obtenir les informations du fichier.\n");
        return NULL;
    }
}

void createBackupFile(const WCHAR* filePath, int params, const WCHAR* folderOutput)
{
    //params 0 ou default only create the file
    //params 1 add file extension and size of the file
    // Extract the filename from the filePath
    const WCHAR* filename = wcsrchr(filePath, L'\\');
    if (filename != NULL) {
        filename++;  // Move past the '\\' character
    }
    else {
        filename = filePath;  // Use the whole path if '\\' not found
    }

    size_t len1=wcslen(folderOutput);
    size_t len2=wcslen(filename);

    WCHAR* result = (WCHAR*)malloc((len1 + len2 + 7) * sizeof(WCHAR));
    if (result == NULL) {
        wprintf(L"Memory allocation failed for 'result'\n");
        return;  // Memory allocation failed
    }

    wcscpy_s(result, len1 + len2 + 7, folderOutput);

    if (len1 > 0 && result[len1 - 1] != L'\\')
    {
        wcscat_s(result, len1 + len2 + 7, L"\\");
    }

    wcscat_s(result, len1 + len2 + 7, filename);

    wcscat_s(result, len1 + len2 + 7, L".txt");

    FILE* fp;
    switch (params)
    {
    case 0:
        if (_wfopen_s(&fp, result, L"w") == 0) {
            fclose(fp);
        }
        else {
            wprintf(L"Failed to open output file: %ls\n", result);
        }
        free(result);
        break;
    case 1:
    {
        WCHAR* filextension = GetFileExtension(filePath); //extension
        DWORD filesize = getFileSize(filePath); //size in lu
        WCHAR* filelastmodif = GetLastModifiedDate(filePath);//last modification in ls

        if (filextension == NULL) 
        {
            filextension = (WCHAR*)malloc((wcslen(L"No Extension") + 1) * sizeof(WCHAR));
            if (filextension != NULL) {
                wcscpy_s(filextension, wcslen(L"No Extension") + 1, L"No Extension");
            }
            else {
                wprintf(L"Memory allocation failed for 'filextension'\n");
                free(filextension);
                return;
            }
        }
        size_t bufferSize = wcslen(L"Extension: ") + wcslen(filextension) + wcslen(L"\nSize: ") + 20 /* Maximum characters for DWORD value */ + wcslen(L" bytes\nLast modification: ") + wcslen(filelastmodif) + wcslen(L"\n") + 1; // +1 for null-terminator





        WCHAR* fileINFOS = (WCHAR*)malloc(bufferSize * sizeof(WCHAR));
        if (fileINFOS == NULL) 
        {
            wprintf(L"Memory allocation failed for 'fileINFOS'\n");
            free(result); // Don't forget to free previously allocated memory
            return;  // Memory allocation failed
        }
        if (fileINFOS != NULL)
        {
            swprintf(fileINFOS, bufferSize, L"Extension: .%ls\nSize: %lu bytes\nLast modification: %ls\n", filextension, filesize, filelastmodif);
        }

        if (_wfopen_s(&fp, result, L"w") == 0) {
            if (fileINFOS != NULL) {
                fputws(fileINFOS, fp);
                free(fileINFOS);
            }
            fclose(fp);
        }
        else {
            wprintf(L"Failed to open output file: %ls\n", result);

            free(filextension);
            free(filelastmodif);
            free(fileINFOS);
            free(result);
            break;
        }
    default:
        free(result);
        break;
    }
    }
}




void getDriveNames(WCHAR* drivePaths[], int maxDrives) 
{
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
            printf("Drive [%S] - Volume Name: %S\n", currentDrive, volumeName);
        }
        else {
            printf("Drive [%S] - Volume Name: Not available\n", currentDrive);
        }

        // Move to next drive string
        currentDrive += lstrlen(currentDrive) + 1;
        count++;
    }

    free(driveStrings);
}

void printDrives(WCHAR* drivePathsP[], int maxDrivesP)
{
    for (int i = 0; i < maxDrivesP; i++) 
    {
        if (drivePathsP[i] != NULL) 
        {
            wprintf(L"Drive %d: [%s]\n", i + 1, drivePathsP[i]);        
        }
    }
}

void driveSelect(WCHAR* drivePathsP[], int maxDrivesP,WCHAR** userPathChoice) 
{
    WCHAR userChoice;
    wprintf(L"Which drive do you want to choose, just Type its letter: ");
    while(1)
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

        // Si la lettre saisie n'est pas valide, demander à l'utilisateur de réessayer
        wprintf(L"Invalid drive letter. Please try again: ");
        userChoice = NULL;
    }

}

void openFolderInExplorer(const WCHAR* rootPath)
{
    ShellExecute(NULL, L"open", rootPath, NULL, NULL, SW_SHOWNORMAL);
}

wchar_t* selectPathFolder(WCHAR* strPath)
{
    BROWSEINFO browseInfo = { 0 };
    browseInfo.hwndOwner = NULL;
    browseInfo.pidlRoot = NULL;
    browseInfo.pszDisplayName = NULL;
    browseInfo.ulFlags = BIF_USENEWUI;
    browseInfo.lpfn = NULL;
    WCHAR title[MAX_PATH + 50];
    swprintf(title, sizeof(title) / sizeof(title[0]), L"Select the folder path for %s", strPath);
    browseInfo.lpszTitle = title;
    LPITEMIDLIST pidlSelectedFolder = SHBrowseForFolder(&browseInfo);

    if (pidlSelectedFolder != NULL) {
        wchar_t* selectedFolderPath = (wchar_t*)CoTaskMemAlloc(MAX_PATH * sizeof(wchar_t));
        if (SHGetPathFromIDList(pidlSelectedFolder, selectedFolderPath)) {
            wprintf(L"The selected folder is: %s\n", selectedFolderPath);
            CoTaskMemFree(pidlSelectedFolder);
            return selectedFolderPath;
        }
        CoTaskMemFree(selectedFolderPath); // En cas d'échec de SHGetPathFromIDList
        CoTaskMemFree(pidlSelectedFolder);
    }

    return NULL;
}

wchar_t* finalPath(const wchar_t* strPath) 
{
    const wchar_t* addition = L"\\*";
    size_t originalLength = wcslen(strPath);
    size_t additionLength = wcslen(addition);

    // Calculer la longueur totale du nouveau chemin
    size_t newPathLength = originalLength + additionLength + 1; // +1 pour le caractère nul

    // Allouer de la mémoire pour le nouveau chemin
    wchar_t* newPath = (wchar_t*)malloc(newPathLength * sizeof(wchar_t));

    // Copier le chemin original dans le nouveau chemin
    wcscpy_s(newPath, newPathLength, strPath);

    // Ajouter l'addition à la fin du nouveau chemin
    wcscat_s(newPath, newPathLength, addition);

    return newPath; // Retourner le nouveau chemin
}

void fileExplorer_list(WCHAR* filepathInit)
{
    WIN32_FIND_DATA FileData;
    HANDLE hFind = FindFirstFile(filepathInit, &FileData);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            wchar_t fullPath[MAX_PATH];
            swprintf(fullPath, MAX_PATH, L"%s\\%s", filepathInit, FileData.cFileName);
            // Afficher le chemin complet du fichier
            wprintf(L"File: %ls\n", fullPath);
        } while (FindNextFile(hFind, &FileData));
        FindClose(hFind);
    }
    return;
}

void fileExplorer(WCHAR* Real_filepathInit, WCHAR* filepathInit, WCHAR* Real_filepathDEST, WCHAR* filepathDEST)
{
    WIN32_FIND_DATA FileData;
    HANDLE hFind = FindFirstFile(filepathInit, &FileData);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (!(FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                wchar_t fullPath[MAX_PATH];
                swprintf(fullPath, MAX_PATH, L"%s\\%s", Real_filepathInit, FileData.cFileName);

                // Afficher le chemin complet du fichier
                wprintf(L"File: %ls\n", fullPath);

                // Appeler createBackupFile seulement pour les fichiers, pas pour les dossiers
                createBackupFile(fullPath, 1, Real_filepathDEST);
            }

        } while (FindNextFile(hFind, &FileData));
        FindClose(hFind);
    }
    return;
}



int main()
{
    const int maxDrives = 20;
    WCHAR* userChoice = NULL;
    WCHAR* drivePaths[20] = { NULL }; // maxDrives which is 20

    getDriveNames(drivePaths, maxDrives);
    //printDrives(drivePaths, maxDrives);

    //driveSelect(drivePaths, maxDrives, &userChoice); // Cette fonction fait le malloc de userChoice donc attention TOUNLOCK

    WCHAR* rootPath = selectPathFolder(L"Backup path");
    wchar_t* star_rootPath = finalPath(rootPath);


    WCHAR* targetPath = selectPathFolder(L"Target");
    wchar_t* star_targetPath = finalPath(targetPath);
    fileExplorer(rootPath, star_rootPath, targetPath, star_targetPath);
    //The work
    //createBackupFile(L"S:\\DocumentsX\\42\\Dummy1\\BN3 Lotto.jpg", 1, targetPath); TOUNLOCK
    //Work done and show results

    //    
    
    
    
    //

    wprintf(L"Work done.");
    openFolderInExplorer(targetPath);

    //All the free
    for (int i = 0; i < maxDrives; i++)
    {
        free(drivePaths[i]);
    }
    return 0;
    //free(userChoice); TOUNLOCK
    free(rootPath);
    free(targetPath);
    free(star_targetPath);
}
