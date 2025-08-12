# FBD-FlashBackDrive

The FBD, short for FlashBackDrive is a systeme that saves the content of a drive ( Or even a folder) in a Register and can even duplicate the drive by duplicating The folders tree and duplicate files as a txt counterpart with most essential informations.<br />

This is a backup system that aims the informations rather than the actual files. Meant to be fast and prevent unkown lost if a drive of device is lost/Stolen/Broken. Preventing Large expenses for maybe a drive that had not much interesting worth such a price.<br />

# PURPOSE

This is meant to prevent unknown loss, at first it was intended to launch when a device is plugged in but this is way more complicated than expected so It will just have to be done manually.<br />

In short this software has the capacity of saving the content informations of a device or even multiple ones.<br />
This is a pre-backup software that is lighter, faster, bettet, stronger method since it either write everything in a register file or a duplicate of the drive(Even if the files are created as .txt this could be a long process depending the tree of the directories.<br />
This is still a good way to at least have an history of files that were there at a time and date.<br />

# FEATURES

## There is 2 way of Operating

- ### Register Only
This option will save all of the path on the selected drive or folder. <br />
It will retains essentials informations:<br />
-Path<br />
-Size<br />
-extension<br />
-last modification date<br />

- ### File Hierarchy duplication(as txt) and Register
This option will make a perfect copy of the selected drive or folder in either the default path or specific path. This will make a tree similar to original path.<br />
This will include a Register with it, because we love windows and its 255 path limitations, so errors may occur and we can't do much about it so the Register will be here to at least keep track of path that are too long.<br />
The Register retains essentials informations:<br />
- Path<br />
- Size<br />
- extension<br />
- last modification date<br />

Those informations are stored on the respective txt duplicate AND in the Register. So a file copy will be the same name and it's content the inforamtion of the original file.<br />

## Multiples Options avalaible

### 1 All the drives[Following DrivesSettings]
This will launch the FBD for all drives on the machine, following the default setting for all drives(Excluding the C drive by default).
You may want to change which Drives are included or not, this can be changed in the Settings.
### 2 Drive select
This will launch the FBD for a specific drive on the machine.
### 3 Folder selection
This will launch the FBD for a specific folder on the machine.
### 4 The Settings
This will open settings for 2 main things
The path default path where to save the backups
### 5 End the Software
This cleanly end the Software.

## The Startup executable
This will be included if the FBD is installed using the Setup files.<br />
This will run, on each Startup of the machine, the FBD for all drives, following the FBD FlashBackDrive Startup Settings. If some drives or paths needs to be modified only this file will be able to do so. <br />

## Installation notice
Please be sure to install the FBD in the Documents folder or in a Drive that has permissions to write(Unlike the C drive), else the software will never work.<br /><br />
Also After installation please be sure to check the FBD FlashBackDrive Startup Settings so you desactivate the drive you'd want to exclude for the Startup Backup. If you want to avoid large Drives or just specific drive don't forget to do it right after the installation. <br /> <br />

The Software files will be in the subfolder D_O-S/FBD FlashBackDrive
# Notes
This only works for windows x64, but feel free to try and compile it for x86 or make a linux version.(I'm done.)
You will have a log folder to keep track of the execution of a FBD.

# License

A quick note on this, I just don't want my code or part of it to be used and commercialized without my consent.<br/>
Forks and usages of it are allowed as part of open source code. Sound fair to me.
