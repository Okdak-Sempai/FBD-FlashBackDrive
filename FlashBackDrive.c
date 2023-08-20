#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <Windows.h>
#include <shlobj.h>
#include <Shellapi.h>
#include <tchar.h>
#include <fileapi.h>
#include <time.h>
#include <wchar.h>

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
        wprintf(L"\nImpossible d'obtenir les informations du fichier.\n");
        return L"NOT ACCESSIBLE";
    }
}

WCHAR* createFilename(const WCHAR* filepath)
{
    const WCHAR* filename = wcsrchr(filepath, L'\\');
    if (filename != NULL) {
        filename++;  // Move past the '\\' character
    }
    else {
        return filepath;  // Use the whole path if '\\' not found
    }
}

void createBackupFile(const WCHAR* filePath, int params, const WCHAR* folderOutput)
{
    //params 0 ou default only create the file
    //params 1 add file extension and size of the file
    // Extract the filename from the filePath
    const WCHAR* filename = createFilename(filePath);

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

void createDirectory(const wchar_t* foldername, const wchar_t* path)
{
    // Construire le chemin complet du nouveau dossier
    wchar_t newFolderPath[MAX_PATH];
    swprintf(newFolderPath, MAX_PATH, L"%s\\%s", path, foldername);

    if (CreateDirectory(newFolderPath, NULL))
    {
        //wprintf(L"Folder created: %ls\n", newFolderPath);
    }
    else
    {
        wprintf(L"Failed to create folder: %ls\n", newFolderPath);
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

driveSelect(WCHAR* drivePathsP[], int maxDrivesP,WCHAR** userPathChoice) 
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

void driveNameGetter(WCHAR* drivePathsP[], int maxDrivesP, WCHAR** userPathChoice,WCHAR** driveName) // you have to free driveName
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

        // Si la lettre saisie n'est pas valide, demander à l'utilisateur de réessayer
        wprintf(L"Invalid drive letter. Please try again: ");
        userChoice = NULL;
    }

}

void openFolderInExplorer(const WCHAR* rootPath)
{
    ShellExecute(NULL, L"open", rootPath, NULL, NULL, SW_SHOWNORMAL);
}

int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
    if (uMsg == BFFM_INITIALIZED) {
        // Obtenir la liste des lecteurs disponibles
        int drives = GetLogicalDrives();
        wchar_t currentDrive = L'A'; // Début des lettres de lecteur

        while (drives) {
            if (drives & 1) {
                // Configurer le répertoire racine pour chaque lecteur
                WCHAR rootPath[4];
                swprintf(rootPath, sizeof(rootPath) / sizeof(rootPath[0]), L"%c:\\", currentDrive);
                SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)rootPath);
                break; // Afficher uniquement le premier lecteur
            }

            drives >>= 1;
            ++currentDrive;
        }
    }

    return 0;
}

wchar_t* selectPathFolder(WCHAR* strPath)
{
    BROWSEINFO browseInfo = { 0 };
    browseInfo.hwndOwner = NULL;
    browseInfo.pidlRoot = NULL;
    browseInfo.pszDisplayName = NULL;
    browseInfo.ulFlags = BIF_USENEWUI | BIF_RETURNONLYFSDIRS;
    browseInfo.lpfn = BrowseCallbackProc; // Utilisation de la fonction de rappel
    WCHAR title[MAX_PATH + 50];
    swprintf(title, sizeof(title) / sizeof(title[0]), L"Select the folder path for %s", strPath);
    browseInfo.lpszTitle = title;

    // Afficher la boîte de dialogue de sélection de dossier
    LPITEMIDLIST pidlSelectedFolder = SHBrowseForFolder(&browseInfo);

    if (pidlSelectedFolder != NULL) {
        wchar_t* selectedFolderPath = (wchar_t*)CoTaskMemAlloc(MAX_PATH * sizeof(wchar_t));
        if (SHGetPathFromIDList(pidlSelectedFolder, selectedFolderPath)) {
            wprintf(L"The selected folder is: %s\n", selectedFolderPath);
            CoTaskMemFree(pidlSelectedFolder);
            return selectedFolderPath;
        }
        CoTaskMemFree(selectedFolderPath);
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

void fileExplorer(WCHAR* Real_filepathInit, WCHAR* filepathInit, WCHAR* Real_filepathDEST, WCHAR* filepathDEST, DWORD rootsize,DWORD pathsize,WCHAR* usedPATH)
{
    WIN32_FIND_DATA FileData;
    HANDLE hFind = FindFirstFile(filepathInit, &FileData);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (!(FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) // For files
            {
                wchar_t fullPath[MAX_PATH];
                swprintf(fullPath, MAX_PATH, L"%s\\%s", Real_filepathInit, FileData.cFileName);

                // Afficher le chemin complet du fichier
                //wprintf(L"File: %ls\n", fullPath);
                pathsize = countFilesInFolder(usedPATH);
                wprintf(L"\rProgression: %d%%", (int)(((float)pathsize / rootsize) * 100));


                // Appeler createBackupFile seulement pour les fichiers, pas pour les dossiers
                createBackupFile(fullPath, 1, Real_filepathDEST);
            }
            else// For folders
            {
                if (wcscmp(FileData.cFileName, L".") != 0 && wcscmp(FileData.cFileName, L"..") != 0)
                {
                    wchar_t fullPath[MAX_PATH];
                    swprintf(fullPath, MAX_PATH, L"%s\\%s", Real_filepathInit, FileData.cFileName);

                    // Afficher le chemin complet du fichier
                    //wprintf(L"File: %ls\n", fullPath);
                    WCHAR* newfileNAME = createFilename(fullPath);
                    createDirectory(newfileNAME, Real_filepathDEST);

                    // Construire le chemin complet du nouveau dossier dans newDEST
                    size_t newDESTLength = wcslen(Real_filepathDEST) + wcslen(newfileNAME) + 2; // +2 pour le backslash et le caractère nul
                    wchar_t* newDEST = (wchar_t*)malloc(newDESTLength * sizeof(wchar_t));
                    swprintf(newDEST, newDESTLength, L"%s\\%s", Real_filepathDEST, newfileNAME);
                    // Construire le chemin complet du nouveau dossier dans initDEST
                    size_t initDESTLength = wcslen(Real_filepathInit) + wcslen(newfileNAME) + 2; // +2 pour le backslash et le caractère nul
                    wchar_t* initDEST = (wchar_t*)malloc(initDESTLength * sizeof(wchar_t));
                    swprintf(initDEST, initDESTLength, L"%s\\%s", Real_filepathInit, newfileNAME);

                    wchar_t* star_initDEST = finalPath(initDEST);
                    wchar_t* star_newDEST = finalPath(newDEST);

                    fileExplorer(initDEST, star_initDEST, newDEST, star_newDEST, rootsize, pathsize, usedPATH);
                    free(initDEST);
                    free(star_initDEST);
                    free(newDEST);
                    free(star_newDEST);

                }
            }

        } while (FindNextFile(hFind, &FileData));
        FindClose(hFind);
    }
    return;
}

int countFilesInFolder(const wchar_t* folderPath)
{
    wchar_t searchPath[MAX_PATH];
    wcscpy_s(searchPath, MAX_PATH, folderPath);
    wcscat_s(searchPath, MAX_PATH, L"\\*.*");

    int itemCount = 0;
    WIN32_FIND_DATA findData;
    HANDLE hFind = FindFirstFile(searchPath, &findData);

    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
                itemCount++;
            }
            else if (wcscmp(findData.cFileName, L".") != 0 && wcscmp(findData.cFileName, L"..") != 0) {
                // Exclude "." and ".." entries
                itemCount++;
            }
        } while (FindNextFile(hFind, &findData));

        FindClose(hFind);
    }

    return itemCount;
}

int folderExists(const WCHAR* folderPath, const WCHAR* folderName)
{
    WCHAR fullPath[MAX_PATH];
    swprintf(fullPath, MAX_PATH, L"%s\\%s", folderPath, folderName);

    DWORD attributes = GetFileAttributes(fullPath);

    if (attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY))
    {
        return 1; // Le dossier existe
    }
    else
    {
        return 0; // Le dossier n'existe pas
    }
}

WCHAR* currentDirectory() // To free
{
    CHAR buffer[MAX_PATH];
    DWORD size = GetCurrentDirectoryA(MAX_PATH, buffer); // Utilisation de GetCurrentDirectoryA pour obtenir une chaîne de caractères multi-octets
    if (size == 0)
    {
        wprintf(L"Erreur lors de l'obtention du répertoire de travail.\n");
        return NULL;
    }

    // Conversion de la chaîne de caractères multi-octets en wchar_t
    int wideCharLength = MultiByteToWideChar(CP_UTF8, 0, buffer, -1, NULL, 0);
    if (wideCharLength == 0)
    {
        wprintf(L"Erreur lors de la conversion de la chaîne de caractères.\n");
        return NULL;
    }

    WCHAR* wideBuffer = (WCHAR*)malloc((wideCharLength + 1) * sizeof(WCHAR));
    if (wideBuffer == NULL)
    {
        wprintf(L"Erreur lors de l'allocation de mémoire.\n");
        return NULL;
    }

    if (MultiByteToWideChar(CP_UTF8, 0, buffer, -1, wideBuffer, wideCharLength + 1) == 0)
    {
        wprintf(L"Erreur lors de la conversion de la chaîne de caractères.\n");
        free(wideBuffer);
        return NULL;
    }

    wprintf(L"%s", wideBuffer);
    return wideBuffer;
}

int defaultBackupSetter()
{

    if (!(folderExists(currentDirectory(), L"FBD Default Backups")));
    {
        createDirectory(L"FBD Default Backups", currentDirectory());
        return 0;
    }

    return -1;
}


int settingsSetter(char* settingsfilename) //Create a file setting or not if it already exists
{

    FILE* file = NULL;

    // Existing check
    int err = fopen_s(&file, settingsfilename, "r");

    // File already exists
    if (err == 0 && file != NULL)
    {
        fclose(file);
        return 1; 
    }
    // Create file if non existant
    else
    {
        err = fopen_s(&file, settingsfilename, "w");
        if (err == 0 && file != NULL)
        {
            fclose(file);
            return 0; // Create the file sucessfully
        }
        else
        {
            fprintf(stderr, "Impossible de créer le fichier FBDsettings.\n");
            return -1; // Creation error
        }
    }
}


int choice(WCHAR* sentence1, int x, WCHAR* sentence2, int y)
{
    int scanfResult;
    int methodChoice = 0;
    wprintf(L"\n%s: %d\n%s: %d\nChoice: ",sentence1,x,sentence2,y);
    do
    {
        scanfResult = scanf_s("%d%*c", &methodChoice);
        if (methodChoice != x && methodChoice != y)
        {
            wprintf(L"Only %d or %d.\tChoice: ",x,y);
            while (getchar() != '\n');
        }
    } while (methodChoice != x && methodChoice != y);

    wprintf(L"\n");
    return methodChoice;
}

wchar_t* readLineNumber(FILE* file, int lineNumber) //First line is 1 and not 0
{
    wchar_t buffer[4096]; // Supposons que la ligne ne dépasse pas 255 caractères
    int currentLine = 0;

    while (currentLine < lineNumber && fgetws(buffer, sizeof(buffer) / sizeof(buffer[0]), file) != NULL) {
        currentLine++;
    }

    if (currentLine == lineNumber) {
        // Retourner une copie allouée dynamiquement de la ligne lue
        return _wcsdup(buffer);
    }
    else {
        return NULL; // La ligne demandée n'a pas pu être lue
    }
}

int writeLineNumber(FILE* file, int lineNumber, const wchar_t* content)
{
    // Replacer le curseur de fichier au début
    if (fseek(file, 0, SEEK_SET) != 0)
    {
        return -1; // Erreur lors du déplacement du curseur
    }

    // Écrire le contenu sur les lignes précédentes
    wchar_t buffer[4096]; // Supposons que la ligne ne dépasse pas 255 caractères
    int currentLine = 0;
    while (currentLine < lineNumber - 1 && fgetws(buffer, sizeof(buffer) / sizeof(buffer[0]), file) != NULL)
    {
        currentLine++;
    }

    if (currentLine != lineNumber - 1)
    {
        return -2; // La ligne demandée n'a pas pu être atteinte
    }

    // Réécrire le contenu sur la ligne spécifique
    fputws(content, file);

    // Copier le reste du contenu du fichier
    while (fgetws(buffer, sizeof(buffer) / sizeof(buffer[0]), file) != NULL)
    {
        fputws(buffer, file);
    }

    return 0; // Écriture réussie
}

int settingsSetterDefaultFinal(char* settingsfilename)
{
    // Afficher la premiere ligne
    //
    FILE* file = NULL;
    int err = fopen_s(&file, settingsfilename, "r");
    // Existing check
    if (err != 0 || file == NULL) 
    {
        // Gestion d'erreur si le fichier ne peut pas être ouvert
        wprintf(L"Failed to open the file: %s\n", settingsfilename);
        return -1;
    }
    WCHAR* firstLine = readLineNumber(file, 1);
    wprintf(L"\nThe current path is :\n[%ls]\n",firstLine);
    fclose(file);
    // Remplacer la ligne si l'user le veut
    //
    if(choice(L"To keep the path do",1, L"To change the path do",2) == 2)
    {
        err = fopen_s(&file, settingsfilename, "w");
        if (err != 0 || file == NULL)
        {
            wprintf(L"Failed to open the file for writing: %s\n", settingsfilename);
            return -1;
        }
        wchar_t* newPath = selectPathFolder(L"Backup folder");
        writeLineNumber(file, 1, newPath);
        fclose(file);

        firstLine = newPath;
        wprintf(L"\nThe current path is now:\n[%ls]\n", firstLine);
        return 1;
    }

    wprintf(L"\nThe current path is now:\n[%ls]\n", firstLine);

    free(firstLine);
    return 0;
}

WCHAR* settingsPath(char* settingsfilename)
{
    FILE* file = NULL;
    int err = fopen_s(&file, settingsfilename, "r");
    // Existing check
    if (err != 0 || file == NULL)
    {
        // Gestion d'erreur si le fichier ne peut pas être ouvert
        wprintf(L"Failed to open the file: %s\n", settingsfilename);
        return NULL;
    }
    WCHAR* firstLine = readLineNumber(file, 1);
    wprintf(L"\nThe current path is :\n[%ls]\n", firstLine);
    fclose(file);
    return firstLine;
}

wchar_t* getCurrentDateWCHAR() // Ex: 2023-08-19
{
    time_t currentTime;
    struct tm timeInfo;
    wchar_t formattedDate[11];

    // Obtenir le temps actuel
    time(&currentTime);

    // Convertir le temps en une structure tm de manière sécurisée
    if (localtime_s(&timeInfo, &currentTime) != 0) {
        return NULL; // Erreur lors de la conversion de la date
    }

    // Format de date : "YYYY-MM-DD"
    if (wcsftime(formattedDate, sizeof(formattedDate), L"%Y-%m-%d", &timeInfo) == 0) {
        return NULL; // Erreur lors du formatage de la date
    }

    // Allouer de la mémoire pour la chaîne formatée
    wchar_t* result = (wchar_t*)malloc(sizeof(formattedDate));
    if (result == NULL) {
        return NULL; // Erreur lors de l'allocation mémoire
    }

    // Utiliser wcscpy_s pour copier la date formatée dans le résultat
    if (wcscpy_s(result, sizeof(formattedDate) / sizeof(formattedDate[0]), formattedDate) != 0) {
        free(result); // Libérer la mémoire en cas d'erreur
        return NULL;
    }

    return result;
}

wchar_t* getCurrentTimeWCHAR()
{
    time_t currentTime;
    struct tm timeInfo;
    wchar_t formattedTime[6]; // HHHMM\0

    // Obtenir le temps actuel
    time(&currentTime);

    // Convertir le temps en une structure tm de manière sécurisée
    if (localtime_s(&timeInfo, &currentTime) != 0) {
        return NULL; // Erreur lors de la conversion de l'heure
    }

    // Format de l'heure : "HH-MM"
    if (wcsftime(formattedTime, sizeof(formattedTime), L"%HH%M", &timeInfo) == 0) {
        return NULL; // Erreur lors du formatage de l'heure
    }

    // Allouer de la mémoire pour la chaîne formatée
    size_t resultSize = wcslen(formattedTime) + 1; // Taille + 1 pour le caractère nul
    wchar_t* result = (wchar_t*)malloc(resultSize * sizeof(wchar_t));
    if (result == NULL) {
        return NULL; // Erreur lors de l'allocation mémoire
    }

    // Copier la date formatée dans le résultat
    if (wcscpy_s(result, resultSize, formattedTime) != 0) {
        free(result); // Libérer la mémoire en cas d'erreur
        return NULL;
    }

    return result;
}

WCHAR* newBackupFolderName(WCHAR* drivePathsP[], int maxDrivesPconst, WCHAR* fullPath)
{
    // Drive_letter+Drive_Name+YYYYMMDD+HHMN
    WCHAR driveLetter = fullPath[0];
    WCHAR* driveBackup = NULL;
    driveNameGetter(drivePathsP, maxDrivesPconst, &fullPath, &driveBackup);
    WCHAR* dateBackup = getCurrentDateWCHAR();
    WCHAR* timeBackup = getCurrentTimeWCHAR();    
    

    // Allouer de la mémoire pour BackupFolderName
    size_t bufferSize = wcslen(driveBackup) + wcslen(dateBackup) + wcslen(timeBackup) + 20; // Taille estimée
    WCHAR* BackupFolderName = (WCHAR*)malloc(bufferSize * sizeof(WCHAR));

    if (BackupFolderName != NULL)
    {
        swprintf(BackupFolderName, bufferSize, L"[%c_%s] %s_%s", driveLetter, driveBackup, dateBackup, timeBackup);
        free(driveBackup);
        free(dateBackup);
        free(timeBackup);
        return BackupFolderName;
    }
    else
    {
        wprintf(L"Memory allocation failed.\n");
        free(driveBackup);
        free(dateBackup);
        free(timeBackup);
        return NULL;
    }

}

void moveToFolder(WCHAR* folderAddress, WCHAR* movetoFolder)
{
    createDirectory(movetoFolder, folderAddress);
    // Ajouter "\\" à la fin de folderAddress
    wcscat_s(folderAddress, MAX_PATH, L"\\");

    // Ajouter movetoFolder à la suite
    wcscat_s(folderAddress, MAX_PATH, movetoFolder);

    //wprintf(L"Le chemin final est : %s\n", folderAddress);
}

void proceed()
{
    wprintf(L"\nPress any button to continue.");
    getchar();
    system("cls");
}

int main()
{

    unsigned int methodChoice=2;
    while (methodChoice > 0 && methodChoice < 4)
    {
        methodChoice = 4;
        //Setting DefaultBackups file
        int defaultbackupStatus = defaultBackupSetter();
        //Settings Setup
        int settingStatus = settingsSetter("FBDsettings");


        //Initialisations
        const int maxDrives = 20;
        WCHAR* userChoice = NULL;
        WCHAR* drivePaths[20] = { NULL }; // maxDrives which is 20

        wprintf(L"==========================================\n");
        getDriveNames(drivePaths, maxDrives);
        //printDrives(drivePaths, maxDrives);

        //Choice of method
        int scanfResult;
        methodChoice = 0;
        wprintf(L"\nPress 1 for Drive select\nPress 2 for folder selection\nPress 3 for the Settings\nPress 4 to end the Software\nChoice: ");
        do
        {
            scanfResult = scanf_s("%d%*c", &methodChoice);
            if (scanfResult != 1 || (methodChoice < 1 || methodChoice > 4))
            {
                wprintf(L"Only 1 or 2 or 3.\tChoice: ");
                while (getchar() != '\n');
            }
        } while (methodChoice < 1 || methodChoice > 4);

        wprintf(L"\n");

        WCHAR* rootPath = NULL;
        wchar_t* star_rootPath = NULL;
        //Via les Drives
        // 
        switch (methodChoice)
        {
            case 1:
            {
                driveSelect(drivePaths, maxDrives, &userChoice); // Cette fonction fait le malloc de userChoice donc attention TOUNLOCK
                rootPath = userChoice;
                star_rootPath = finalPath(rootPath);
                break;
            }

            //Via la selection
            // 
            case 2:
            {
                rootPath = selectPathFolder(L"Backup path");
                star_rootPath = finalPath(rootPath);
                break;
            }
            //Settings
            //
            case 3:
            {
                system("cls");
                wprintf(L"\033[1mFlashBackDrive Settings\033[0m\n");
                wprintf(L"==========================================");
                settingsSetterDefaultFinal(L"FBDsettings");
                //All the free
                for (int i = 0; i < maxDrives; i++)
                {
                    free(drivePaths[i]);
                }
                free(userChoice);
                free(rootPath);
                free(star_rootPath);

                proceed();
                break;
            }
            //Stops the code
            //
            case 4:
            {

                //All the free
                for (int i = 0; i < maxDrives; i++)
                {
                    free(drivePaths[i]);
                }
                free(userChoice);
                free(rootPath);
                free(star_rootPath);
                break;
            }
        }

        if (methodChoice == 1 || methodChoice == 2)
        {
            // Execution time starts
            LARGE_INTEGER startTime, endTime, frequency;
            QueryPerformanceFrequency(&frequency);
            QueryPerformanceCounter(&startTime); 
            //

            WCHAR* targetPath = selectPathFolder(L"Target");
            wchar_t* star_targetPath = finalPath(targetPath);
            //Move to custom folder
            moveToFolder(targetPath, newBackupFolderName(drivePaths, maxDrives, targetPath));

            int rootsize = countFilesInFolder(rootPath);
            int pathsize = countFilesInFolder(targetPath);

            //Code usage
            fileExplorer(rootPath, star_rootPath, targetPath, star_targetPath, rootsize, pathsize, targetPath);

            pathsize = countFilesInFolder(targetPath);
            wprintf(L"\rProgression: %d%%", (int)(((float)pathsize / rootsize) * 100));
            wprintf(L"\n");

            //Result
            wprintf(L"Work done.");
            // Execution time ends
            QueryPerformanceCounter(&endTime);
            double elapsedTime = (endTime.QuadPart - startTime.QuadPart) * 1000.0 / frequency.QuadPart;
            int totalSeconds = (int)(elapsedTime / 1000);
            int hours = totalSeconds / 3600;
            int minutes = (totalSeconds % 3600) / 60; 
            int seconds = totalSeconds % 60; 

            wprintf(L"\nExecution time: %.2f millisecondes\t%02dH %02dM %02dS\n",elapsedTime, hours, minutes, seconds);
            //

            openFolderInExplorer(targetPath);
            proceed();
            //All the free
            for (int i = 0; i < maxDrives; i++)
            {
                free(drivePaths[i]);
            }
            free(userChoice);
            //free(rootPath);
            //free(targetPath);
            //free(star_rootPath);
            //free(star_targetPath);
        }
    }
    return 0;
}