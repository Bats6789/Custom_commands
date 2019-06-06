# Custom Commands
## Table of Contents

[1. Author's Info.](https://github.com/Bats6789/Custom_commands#1-authors-info)
[2. Description.](#2.-Description.)
[3. Setup](#3. Setup)
[4. Adding your own command](#4. Adding your own command)
[5. TODO list.](#5. TODO list.)

---

## 1. Author's Info
Name: Blake Wingard
Contact info: Bats23456789@gmail.com

## 2. Description
This is a set of custom commands for Windows I created for fun. 
For information on the commands use "helpC" in the command line after the setup.
To report any bugs or suggest new commands to be added. Please contact me through email with the subject "custom commands bugs/suggestions".

## 3. Setup
After cloning or downloading the repository two things need to be done for the commands to work.
1. Add the custom commands directory to the path.
    1. Press windows key.
    2. Type "environment variables" press enter.
    3. Click on the "environment variables" button.
    4. select "Path" and click edit.
    5. Click "New".
    6. Type the directory to the custom commands' "commands" directory
        (e.g. C:\path\to\custom_cmd\commands)
    7. Click "OK" twice.
2. Add an environment variable named "commands".
    1. Press windows key.
    2. Type "environment variables" press enter.
    3. Click on the "environment variables" button.
    4. Click "New".
    5. Type the following information
        * Variable name: commands
        * Variable value: C:\path\to\custom_cmd\help
    6. Click "OK" twice.
3. Make sure to reset any terminals before attempting to use any of the commands.

## 4. Adding your own command
To add your own command it is recommended that you:
1. Place your source code into the "source" folder.
    If your source uses your own custom library. It is recommended to place the library files into the "library" folder.
2. Compile your executable to the "commands" folder.
    (note: Can be compiled using the makefile after step. 4.)
3. Add help information into the "help" folder.
    1. Create a general info file titled "name of your executable".txt. (must be a TXT file)
    2. Add the name of your executable to "commands.list" without the extension.
    3. Add your executable name with some basic info to the "help.txt" file.
4. Add your command to the makefile.
    1. Add your executable name to the FILES section before the "#". Do not include the extension.
    2. At the bottom add the following two lines.
        ```makefile
        executable_name:
            ${CC} source/executable_name.c -o commands/executable_name ${INCLUDES} ${OPTIONS}
        ```
        (note for makefile)  
        A tab is required for the second line. Not spaces.

## 5. TODO list
- [x] Add a file organizer.
- [x] Add a file list creator.
- [x] Add a file merger.
- [x] Add a directory list creator.
- [x] Add a help command.
- [ ] Add a file/directory searcher.
- [ ] Add a timer for commands.
- [ ] Add a string in file replacer.
- [ ] Make the switches independent of order entered.
