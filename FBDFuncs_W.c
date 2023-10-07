#include <stdio.h>
#include <wchar.h>
#include <windows.h>

#include "FoldersFuncs_W.h"
#include "FilesFuncs_W.h"
#include "Tools_W.h"
#include "DrivesFuncs_W.h"

int writeLineRegister(FILE* file, const WCHAR* content)
/*
* Includes:
*   stdio.h [fseek(); fputwc(); fputws()]
*   wchar.h [wcslen()]
* Params:
*   FILE* file = The file, already opened as "a"(to write on next line)
*   const WCHAR* content = The content to write on the line
* Return:
*  int
*       -1 => Error when moving the cursor
*       0 => Sucess in writting on the line
* Description:
*   This function write the content to the next line of the file in argument, the file has to be already opened when passed to the function (As "a") and returns 0 for sucess or -1 for failure
*   This function is intended to be used for the Register file of the FBDFuncs library
* Notes:
*   To write on other than first line you'd need to open(Beforehand) the file as "r"
*/
{
    // Move the cursor at the end of the file
    if (fseek(file, 0, SEEK_END) != 0)
    {
        return -1; // Error
    }

    // add a new line if not already done
    int contentLength = wcslen(content);
    if (contentLength > 0 && content[contentLength - 1] != L'\n')
    {
        fputwc(L'\n', file);
    }

    // Write the content
    fputws(content, file);

    return 0; // Sucess
}

void writeToLog(WCHAR* logsAdress, WCHAR* variableName, WCHAR* variableValue)
/*
* Includes:
*   stdlib.h [FILE; malloc(); _wfopen_s(); fclose(); free()]
*   wchar.h [wcslen()]
*   windows.h [swprintf()]
* 
*   FBDFuncs_W.h/.c {writeLineRegister()}
* Params:
*   WCHAR* logsAdress = The path of the logs
*   WCHAR* variableName = The name of the variable
*   WCHAR* variableValue = The value of the variable
* Return:
*   None
* Description:
*   This function writes to the log using the variable name and it's value
* Notes:
*   Tha value HAS to be a WCHAR, so be careful and convert, recommended usage of intToWCHAR() from Tools_W.h/.c
*/
{
    WCHAR* result = NULL;
    size_t bufferSize = NULL;
    if(variableValue == NULL)
    {
        bufferSize = wcslen(variableName) + wcslen(L"NULL") + 3; // +3 for ": " and null terminator
    }
    else
    {
        bufferSize = wcslen(variableName) + wcslen(variableValue) + 3; // +3 for ": " and null terminator
    }

    // Allocate memory for the result
    result = (WCHAR*)malloc(bufferSize * sizeof(WCHAR));

    if (result != NULL)
    {
        // Create the concatenated string
        if (variableValue == NULL)
        { 
            swprintf(result, bufferSize, L"%s: %s", variableName, L"NULL");

        }
        else
        {
            swprintf(result, bufferSize, L"%s: %s", variableName, variableValue);
        }
    }

    FILE* fLogs = NULL;
    _wfopen_s(&fLogs, logsAdress, L"a");
    writeLineRegister(fLogs, result);
    fclose(fLogs);
    free(result);

}

int defaultLogSetter()
/*
* Includes:
*   FoldersFuncs_W.h/.c{currentDirectory(),folderExists()}
* Params:
*   None
* Return:
*   int
*       0 => The FBD Logs file didn't exist and has been created
*       -1 => The FBD Logs file already existed
* Description:
*   This function create the FBD Logs if it doesn't exist already
* Notes:
*   This only creates the file
*/
{
    WCHAR* currentDir = currentDirectory();
    if (!(folderExists(currentDir, L"FBD Logs")))
    {
        createFolder(L"FBD Logs", currentDir);
        free(currentDir);
        return 0; // The file didn't exist and has been created
    }
    free(currentDir);
    return -1; // The file already exists
}

int defaultBackupSetter()
/*
* Includes:
*   FoldersFuncs_W.h/.c{currentDirectory(),folderExists()}
* Params:
*   None
* Return:
*   int
*       0 => The FBD Default Backups file didn't exist and has been created
*       -1 => The FBD Default Backups file already existed
* Description:
*   This function create the FBD Default Backups if it doesn't exist already
* Notes:
*   This only creates the file
*/
{
    WCHAR* currentDir = currentDirectory();
    if (!(folderExists(currentDir, L"FBD Default Backups")))
    {
        createFolder(L"FBD Default Backups", currentDir);
        free(currentDir);
        return 0; // The file didn't exist and has been created
    }
    free(currentDir);
    return -1; // The file already exists
}

int settingsSetter(char* settingsfilename) //Create a file setting or not if it already exists
/*
* Includes:
*   stdlib.h [free()]
*   stdio.h [FILE; fopen_s(); fclose(); fprintf()]
*   wchar.h [WCHAR; size_t; wcslen(); malloc(); wcscpy_s(); wcscat_s(); wprintf()]
* 
*   FoldersFuncs_W.h/.c {currentDirectory()}
*   FilesFuncs_W.h/.c {writeLineNumber(); readLineNumber()}
* Params:
*   char* settingsfilename = Name of the FBD Default Backups; waits for "FBDsettings" string
* Return:
*   int
*       1 => The file already exists and is not NULL inside
*       0 => The file was created sucessfully
*       -1 => Error creating this file
* Description:
*   This function initialize the FBDsettings file at the root of the Software/Code
*   This also put the "FBD Default Backups" folder at the root of the Software/Code as first line AKA default parameters
* Notes:
*   This doesn't create the FBD Default Backups folder
*   This checks if the path is NULL, and if so it will write the default path
*/
{
    FILE* file = NULL;

    // Existing check
    int err = fopen_s(&file, settingsfilename, "r");

    // File already exists
    if (err == 0 && file != NULL)
    {
        //NULL test
        WCHAR* firstLine = readLineNumber(file, 1);
        if(firstLine != NULL)
        {
            fclose(file);
            return 1;
        }
        fclose(file);
    }
    // Create file if non existant
    
    err = fopen_s(&file, settingsfilename, "w");
    if (err == 0 && file != NULL)
    {
        WCHAR* defaultBackupFolder = currentDirectory();
        size_t requiredSize = wcslen(defaultBackupFolder) + wcslen(L"\\FBD Default Backups") + 1; // +1 for null character
        // Allocate full memory for the full path
        WCHAR* fullPath = (WCHAR*)malloc(requiredSize * sizeof(WCHAR));
        if (fullPath != NULL)
        {
            // Copy existing path to new path
            wcscpy_s(fullPath, requiredSize, defaultBackupFolder);

            // Add "\\FBD Default Backups" at the end of the path
            wcscat_s(fullPath, requiredSize, L"\\FBD Default Backups");
        }
        else
        {
            wprintf(L"Erreur lors de l'allocation de mémoire.\n");
        }
        //moveToFolder(defaultBackupFolder,L"FBD Default Backups");
        writeLineNumber(file, 1, fullPath);
        free(defaultBackupFolder);
        free(fullPath);
        fclose(file);
        return 0; // Create the file sucessfully
    }
    else
    {
        fprintf(stderr, "Impossible to create FBDsettings file.\n");
        return -1; // Creation error
    }
    
}

int settingsSetterDefaultFinal(char* settingsfilename)
/*
* Includes:
*   stdlib.h [free()]
*   stdio.h [FILE; fopen_s(); fclose()]
*   wchar.h [WCHAR; size_t; wprintf(); malloc(); wcscpy_s(); wcscat_s(); wcslen()]
* 
*   FilesFuncs_W.h/.c {readLineNumber(); writeLineNumber()}
*   FoldersFuncs_W.h" {selectPathFolder(); currentDirectory()}
*   Tools_W.h/.c {choiceBTW()}
* Params:
*   char* settingsfilename = The name of the settings file
* Return:
*   int:
*       -1 => failed to open the file
*       0 => The path got back to the defaultPath successfully
*       1 => The path got kept successfully
*       2 => The path got changed successfully
* Description:
*   This function is a complete menu to manage the setting file.
*   Choice 0: To set back the defaultPath
*   Choice 1: To keep the path
*   Choice 2: To change the path
* Notes:
*   If a change is made but nothing is choosen, it will put the default path back automatically
*/
{
    system("cls");
    wprintf(L"\033[1mFlashBackDrive Settings\033[0m\n");
    wprintf(L"==========================================");

    WCHAR* fullPath = NULL;


    // Print the 1st line (The default path stored)
    
    FILE* file = NULL;
    int err = fopen_s(&file, settingsfilename, "r");
    // Existing check
    if (err != 0 || file == NULL)
    {
        // File couldn't be opened
        wprintf(L"Failed to open the file: %s\n", settingsfilename);
        return -1;
    }
    WCHAR* firstLine = readLineNumber(file, 1);
    wprintf(L"\nThe current path is :\n[%ls]\n", firstLine);
    fclose(file);
    
    
    // Put back the defaultPath OR keep the current one OR Change to another file

    int choice = choiceBTW(L"To set back the defaultPath do", 0, L"To keep the path do: 1\nTo change the path do", 2);
    if (choice == 2)
    {
        err = fopen_s(&file, settingsfilename, "w");
        if (err != 0 || file == NULL)
        {
            wprintf(L"Failed to open the file for writing: %s\n", settingsfilename);
            return -1;
        }
        WCHAR* newPath = selectPathFolder(L"Backup folder");
        if (newPath != NULL)
        {
            writeLineNumber(file, 1, newPath);
            firstLine = newPath;

        }
        else
        {
            //RESTORE THE DEFAULT PATH
            WCHAR* defaultBackupFolder = currentDirectory();
            size_t requiredSize = wcslen(defaultBackupFolder) + wcslen(L"\\FBD Default Backups") + 1; // +1 pour le caractère nul
            // Allouez de la mémoire pour le chemin complet
            fullPath = (WCHAR*)malloc(requiredSize * sizeof(WCHAR));
            if (fullPath != NULL)
            {
                // Copy current path to the new path
                wcscpy_s(fullPath, requiredSize, defaultBackupFolder);

                // Add "\\FBD Default Backups" at the end of the path
                wcscat_s(fullPath, requiredSize, L"\\FBD Default Backups");
            }
            else
            {
                wprintf(L"Erreur lors de l'allocation de mémoire.\n");
            }
            //moveToFolder(defaultBackupFolder,L"FBD Default Backups");
            writeLineNumber(file, 1, fullPath);
            firstLine = fullPath;
            free(defaultBackupFolder);
            wprintf(L"Restored to default path\n");
        }
        fclose(file);


        wprintf(L"\nThe current path is now:\n[%ls]\n", firstLine);
        free(fullPath);
        //free(firstLine);
        return 2;
    }
    if (choice == 0)
    {
        err = fopen_s(&file, settingsfilename, "w");
        if (err != 0 || file == NULL)
        {
            wprintf(L"Failed to open the file for writing: %s\n", settingsfilename);
            return -1;
        }
        //RESTORE THE DEFAULT PATH
        WCHAR* defaultBackupFolder = currentDirectory();
        size_t requiredSize = wcslen(defaultBackupFolder) + wcslen(L"\\FBD Default Backups") + 1; // +1 for null character
        // Allocate memory full path
        fullPath = (WCHAR*)malloc(requiredSize * sizeof(WCHAR));
        if (fullPath != NULL)
        {
            // Copy current path to the new path
            wcscpy_s(fullPath, requiredSize, defaultBackupFolder);

            // Add "\\FBD Default Backups" at the end of the path
            wcscat_s(fullPath, requiredSize, L"\\FBD Default Backups");
        }
        else
        {
            wprintf(L"Memory allocation error.\n");
        }
        //moveToFolder(defaultBackupFolder,L"FBD Default Backups");
        writeLineNumber(file, 1, fullPath);
        firstLine = fullPath;
        free(defaultBackupFolder);
        wprintf(L"Restored to default path\n");

        fclose(file);


        wprintf(L"\nThe current path is now:\n[%ls]\n", firstLine);
        free(fullPath);
        //free(firstLine);
        return 0;
    }

    //Choice 1 gets here directly obviously
    wprintf(L"\nThe current path is now:\n[%ls]\n", firstLine);

    free(firstLine);
    return 1;
}

WCHAR* settingsPath(char* settingsfilename)
/*
* Includes:
*   stdio.h [FILE; fopen_s();fclose()]
*   wchar.h [WCHAR; wprintf()]
* 
*   FilesFuncs_W.h/.c {readLineNumber()}
* Params:
*   char* settingsfilename = The name of the settings file
* Return:
*   WCHAR* => The path stored in the first line of the Settings file
* Description:
*   This function prints the current default path and returns it
*   Ex:
*       The current default Path is :
*       [C:\Users\Steve\source\repos\Safe file\FBD Default Backups]
* Notes:
*   None
*/
{
    FILE* file = NULL;
    int err = fopen_s(&file, settingsfilename, "r");
    // Existing check
    if (err != 0 || file == NULL)
    {
        // File coulden't be opened
        wprintf(L"Failed to open the file: %s\n", settingsfilename);
        return NULL;
    }
    WCHAR* firstLine = readLineNumber(file, 1);
    wprintf(L"\nThe current default Path is :\n[%ls]\n", firstLine);
    fclose(file);
    return firstLine;
}

WCHAR* getSettingsPath(char* settingsfilename)
/*
* Includes:
*   stdio.h [FILE; fopen_s();fclose()]
*   wchar.h [WCHAR; wprintf()]
*
*   FilesFuncs_W.h/.c {readLineNumber()}
* Params:
*   char* settingsfilename = The name of the settings file
* Return:
*   WCHAR* => The path stored in the first line of the Settings file
* Description:
*   This function returns the current default path
* Notes:
*   This function prints nothing if no errors
*/
{
    FILE* file = NULL;
    int err = fopen_s(&file, settingsfilename, "r");
    // Existing check
    if (err != 0 || file == NULL)
    {
        // File coulden't be opened
        wprintf(L"Failed to open the file: %s\n", settingsfilename);
        return NULL;
    }
    WCHAR* firstLine = readLineNumber(file, 1);
    fclose(file);
    return firstLine;
}

WCHAR* newBackupFolderName(WCHAR* drivePathsP[], int maxDrivesPconst, WCHAR* fullPath)
/*
* Includes:
*   stdlib.h [free()]
*   wchar.h [WCHAR; size_t; wcslen(); malloc(); swprintf(); wprintf()]
* 
*   Tools_W.h/.c {getCurrentDateWCHAR(); getCurrentTimeWCHAR()}
* Params:
*   WCHAR* drivePathsP[] = The array of all the drivePaths on the machine
*   int maxDrivesPconst = Size of the drivePathsP array
*   WCHAR* fullPath = The path of the folder to create the Backup name
* Return:
*   WCHAR* => Path of the backup folder created(only the name)
* Description:
*   This function create a file name fore the backup folder
*   Follow this format  {Drive_letter+Drive_Name+YYYYMMDD+HHMN}
*   Ex:
*       [Fresh Drive] 2023-09-28_14H04
* Notes:
*   This does not create any folder, it only creates the name
*/
{

    WCHAR driveLetter = fullPath[0];
    WCHAR* driveBackup = NULL;
    driveNameGetter(drivePathsP, maxDrivesPconst, &fullPath, &driveBackup);
    WCHAR* dateBackup = getCurrentDateWCHAR();
    WCHAR* timeBackup = getCurrentTimeWCHAR();


    // Allocate memory for BackupFolderName
    size_t bufferSize = wcslen(driveBackup) + wcslen(dateBackup) + wcslen(timeBackup) + 20; // Estimate size
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

void createBackupFile(int Params, const WCHAR* filePath, const WCHAR* folderOutput, const WCHAR* registerAdress, int* registerLine)
/*
* Includes:
*   stdio.h [FILE; malloc(); free(); _wfopen_s(); fclose(); fputws()]
*   windows.h [WCHAR; swprintf()]
*   wchar.h [size_t; wcslen(); wprintf(); wcscpy_s(); wcscat_s()]
*
*   FilesFuncs_W.h/.c {getFilename(); getFileSize(); GetLastModifiedDate(); GetFileExtension(); }
*   FBDFuncs_W.h/.c {writeLineRegister()}
* Params:
*   int Params =
*       0 => Default only create the file (So it's empty)[Still make the Register]
*       1 => file name And extension And size of the file And date[And register also]
*   const WCHAR* filePath = The path of the file
*   const WCHAR* folderOutput = The path of the folder where to write
*   const WCHAR* registerAdress = The adress of the register(The path)
*   int* registerLine = The line where the information will be stored in the Register
* Return:
*   None
* Description:
*   This function make a copy of filePath file to the folderOutput as a .txt while writing it's path in the Register
*   This will either only create the copy as an empty .txt if the 0 option is selected or Keep the name, extension, size and last modification date in the copy(the .txt file) And in the Register on the same line as the file.
*
*   Ex:
*   Register: D:\Documents\Bonnie\Clyde\[Da SSD] 2023-09-28_13H36\Yurina.jpg.txt	[Extension: .jpg]	[Size: 0 bytes]	[Last modification: 2023-08-15 13:18:44]
*
*   In the backup file created if the folderOutput:
*       Filename: Yurina.jpg
*       Extension: .jpg
*       Size: 0 bytes
*       Last modification: 2023-08-15 13:18:44
* Notes:
*   None
*/
{

    // Extract the filename from the filePath
    (*registerLine)++;
    const WCHAR* filename = NULL;
    filename = getFilename(filePath);

    size_t len1 = wcslen(folderOutput);
    size_t len2 = wcslen(filename);

    //result creation

    size_t sizeOfResult = (len1 + len2 + 7);
    WCHAR* result = (WCHAR*)malloc(sizeOfResult * sizeof(WCHAR));
    if (result == NULL)
    {
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

    //Register 
    FILE* fp;
    FILE* fpRegister = NULL;

    switch (Params)
    {
    case 0: // Empty files
    {
        //Register
        _wfopen_s(&fpRegister, registerAdress, L"a");
        writeLineRegister(fpRegister, result);
        fclose(fpRegister);

        if (_wfopen_s(&fp, result, L"w") == 0)
        {
            fclose(fp);
        }
        else
        {
            wprintf(L"Failed to open output file: %ls\n", result);
        }
        free(result);
        break;
    }
    case 1: // Files with Informations
    {

        DWORD filesize = getFileSize(filePath); // Size in lu
        WCHAR* filelastmodif = GetLastModifiedDate(filePath);// Last modification in ls

        WCHAR* filextension = NULL; //extension
        // Get the value of GetFileExtension(filePath)
        const WCHAR* extensionFromFunction = GetFileExtension(filePath);

        if (extensionFromFunction != NULL)
        {
            // Allocate memory for filextension and copy it
            filextension = (WCHAR*)malloc((wcslen(extensionFromFunction) + 1) * sizeof(WCHAR));

            if (filextension != NULL)
            {
                wcscpy_s(filextension, wcslen(extensionFromFunction) + 1, extensionFromFunction);
            }
            else
            {
                wprintf(L"Memory allocation failed for 'filextension'\n");
            }
        }

        if (filextension == NULL)
        {
            filextension = (WCHAR*)malloc((wcslen(L"No Extension") + 1) * sizeof(WCHAR));
            if (filextension != NULL)
            {
                wcscpy_s(filextension, wcslen(L"No Extension") + 1, L"No Extension");
            }
            else
            {
                wprintf(L"Memory allocation failed for 'filextension'\n");
                free(filextension);
                return;
            }
        }
        size_t bufferSize = wcslen(L"Filename: ") + wcslen(filename) + wcslen(L"\nExtension: ") + wcslen(filextension) + wcslen(L"\nSize: ") + 20 /* Maximum characters for DWORD value */ + wcslen(L" bytes\nLast modification: ") + wcslen(filelastmodif) + wcslen(L"\n") + 1; // +1 for null-terminator


        WCHAR* fileINFOS = (WCHAR*)malloc(bufferSize * sizeof(WCHAR)); //String for the file informations
        if (fileINFOS == NULL)
        {
            wprintf(L"Memory allocation failed for 'fileINFOS'\n");
            free(result); // Don't forget to free previously allocated memory
            return;  // Memory allocation failed
        }
        if (fileINFOS != NULL)
        {
            swprintf(fileINFOS, bufferSize, L"Filename: %ls\nExtension: .%ls\nSize: %lu bytes\nLast modification: %ls\n", filename, filextension, filesize, filelastmodif);
        }

        //Register
        WCHAR* fileINFOSR = (WCHAR*)malloc(bufferSize * sizeof(WCHAR)); //String for the file informations
        if (fileINFOSR == NULL)
        {
            wprintf(L"Memory allocation failed for 'fileINFOS'\n");
            free(result); // Don't forget to free previously allocated memory
            return;  // Memory allocation failed
        }
        if (fileINFOSR != NULL)
        {
            swprintf(fileINFOSR, bufferSize, L"\t[Extension: .%ls]\t[Size: %lu bytes]\t[Last modification: %ls]", filextension, filesize, filelastmodif);
        }

        size_t totalLength = wcslen(result) + wcslen(fileINFOSR) + 1; // +1 for the null-terminator
        WCHAR* resultWithINFOS = (WCHAR*)malloc(totalLength * sizeof(WCHAR));
        if (resultWithINFOS == NULL)
        {
            wprintf(L"Memory allocation failed for 'resultWithINFOS'\n");
            return;  // Memory allocation failed
        }
        wcscpy_s(resultWithINFOS, totalLength, result);
        wcscat_s(resultWithINFOS, totalLength, fileINFOSR);

        _wfopen_s(&fpRegister, registerAdress, L"a");
        writeLineRegister(fpRegister, resultWithINFOS);
        fclose(fpRegister);


        if (_wfopen_s(&fp, result, L"w") == 0)
        {
            if (fileINFOS != NULL)
            {
                fputws(fileINFOS, fp);

                free(fileINFOS);
            }
            fclose(fp);
            free(result);
            break;
        }
        else
        {
            wprintf(L"Failed to open output file: %ls\n", result);
            free(filextension);
            free(fileINFOS);
            free(fileINFOSR);
            free(result);
            break;
        }
    default:
        free(result);
        break;
    }
    }
}

void fileExplorer(WCHAR* Real_filepathInit, WCHAR* filepathInit, WCHAR* Real_filepathDEST, WCHAR* filepathDEST, DWORD rootsize, DWORD pathsize, WCHAR* usedPATH, const WCHAR* registerAdress, WCHAR* extraExcludePath)
/*
* Includes:
*   stdlib.h [free()]
*   windows.h [WIN32_FIND_DATA; HANDLE; FILE_ATTRIBUTE_DIRECTORY; swprintf(); FindNextFile]
*   wchar.h [WCHAR; size_t; wcscmp(); wcslen(); malloc();wcsstr()]
*
*   FoldersFuncs_W.h/.c {countFilesInFolder(); createFolder(); finalPathFileExplorer()}
*   FilesFuncs_W.h/.c {getFilename()}
*   FBDFuncs_W.h/.c {createBackupFile(); fileExplorer()}
* Params:
*   WCHAR* Real_filepathInit = The path to explore
*   WCHAR* filepathInit = The path to explore with the L"\\*"
*   WCHAR* Real_filepathDEST = The destination path
*   WCHAR* filepathDEST = The destination path with the L"\\*"
*   DWORD rootsize = Number of folders in filepathInit
*   DWORD pathsize = Number of folders in filepathDEST
*   WCHAR* usedPATH = The path of the root of the folder destination(Used for the Progression bar
*   const WCHAR* registerAdress = The adress of the register(The path)
*   WCHAR* extraExcludePath = The Second path to exclude
* Return:
*   None
* Description:
*   This function parse the folder as filepathInit and make a copy of it to filepathDEST as .txt
*   There is a recursion so to function can act by going as deep as possible in the folders while creating the files and proceed to create the copy of the files
* Notes:
*   Be carefull of the arguments so the recursion goes well
*   This will ignore all path that has "FBD FlashBackDrive"
*/
{
    WCHAR* pathToIgnore = NULL;
    pathToIgnore = L"FBD FlashBackDrive";
    WIN32_FIND_DATA FileData;
    HANDLE hFind = FindFirstFile(filepathInit, &FileData);
    int line = 0;
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (!(FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) // For files
            {
                WCHAR fullPath[MAX_PATH];
                swprintf(fullPath, MAX_PATH, L"%s\\%s", Real_filepathInit, FileData.cFileName);

                pathsize = countFilesInFolder(usedPATH)-1; //Do not count the register
                wprintf(L"\rProgression: %d%%", (int)(((float)pathsize / rootsize) * 100));


                // Call createBackupFile() only for files, not folders
                createBackupFile(1, fullPath, Real_filepathDEST, registerAdress, &line);
            }
            else // For folders
            {
                if (wcscmp(FileData.cFileName, L".") != 0 && wcscmp(FileData.cFileName, L"..") != 0)
                {
                    WCHAR fullPath[MAX_PATH];
                    swprintf(fullPath, MAX_PATH, L"%s\\%s", Real_filepathInit, FileData.cFileName);
                    if ((wcsstr(fullPath, pathToIgnore) == NULL) && (wcscmp(extraExcludePath, fullPath)!=0)) //Ignore paths
                    {
                        WCHAR* newfileNAME = NULL;
                        newfileNAME = getFilename(fullPath);
                        createFolder(newfileNAME, Real_filepathDEST);

                        // Create the complete path of the new folder in newDEST
                        size_t newDESTLength = wcslen(Real_filepathDEST) + wcslen(newfileNAME) + 2; // +2 for the backslash and the null character
                        WCHAR* newDEST = (WCHAR*)malloc(newDESTLength * sizeof(WCHAR));
                        swprintf(newDEST, newDESTLength, L"%s\\%s", Real_filepathDEST, newfileNAME);
                        // Create the complete path of the new folder in initDEST
                        size_t initDESTLength = wcslen(Real_filepathInit) + wcslen(newfileNAME) + 2; // +2 for the backslash and the null character
                        WCHAR* initDEST = (WCHAR*)malloc(initDESTLength * sizeof(WCHAR));
                        swprintf(initDEST, initDESTLength, L"%s\\%s", Real_filepathInit, newfileNAME);

                        WCHAR* star_initDEST = finalPathFileExplorer(initDEST);
                        WCHAR* star_newDEST = finalPathFileExplorer(newDEST);

                        fileExplorer(initDEST, star_initDEST, newDEST, star_newDEST, rootsize, pathsize, usedPATH, registerAdress, extraExcludePath);
                        free(initDEST);
                        free(star_initDEST);
                        free(newDEST);
                        free(star_newDEST);

                    }
                }
            }

        } while (FindNextFile(hFind, &FileData));
        FindClose(hFind);
    }
    return;
}


