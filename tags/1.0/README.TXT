README.TXT
LevelZap project
================

This file is a quick guide to the files found in the SVN repository of the LevelZap project.


1. LevelZap.sln

Visual Studio 2010 solution file for the LevelZap project. The project is written in C++ using ATL. In order to build all targets, make sure to install the x64 Compiler and Tools option in Visual Studio when you install it.


2. Testing

Contains an alternative shell program, mtExplorer, which I used to debug LevelZap during development. It's useful to avoid killing explorer.exe every time you need to run again.


3. Installer

Contains the project's installation script. The script was made with Inno Setup (http://www.jrsoftware.org/isinfo.php), which is free (as in beer AND in speech). In order to build the installer, you will have to download the Inno Setup Compiler. Open the .iss script in the Compiler and choose "Compile". (Make sure you built LevelZap with Visual Studio first, both Release|Win32 and Release|x64 configurations.)

Note: the installation script relies on the Inno Setup Preprocessor. This third-party add-on can be installed along with Inno Setup using the QuickStart pack; see Inno Setup's download page for details.


5. screenshots

Backup of the project screenshots featured on CodePlex.


6. HISTORY.TXT

Version history.


7. LICENSE.TXT

Free-software license used by the LevelZap project. Corresponds to the MIT license.
