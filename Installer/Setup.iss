; Inno Setup script file for LevelZap
; Copyright (c) 2011 Charles Lechasseur
;
; Permission is hereby granted, free of charge, to any person obtaining a copy
; of this software and associated documentation files (the "Software"), to deal
; in the Software without restriction, including without limitation the rights
; to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
; copies of the Software, and to permit persons to whom the Software is
; furnished to do so, subject to the following conditions:
;
; The above copyright notice and this permission notice shall be included in
; all copies or substantial portions of the Software.
;
; THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
; IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
; FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
; AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
; LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
; OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
; THE SOFTWARE.

#define MyAppName "LevelZap"
#define MyAppVersion "1.0.2"
#define MyAppFullVersion "1.0.2"
#define MyAppVerName "LevelZap 1.0.2"
#define MyAppPublisher "Charles Lechasseur"
#define MyAppURL "http://levelzap.codeplex.com/"
#define MyAppDescription "LevelZap Shell Contextual Menu Extension"
#define MyAppCopyright "(c) 2011, Charles Lechasseur. See LICENSE.TXT for details."
#define MyLicenseFile "..\LICENSE.TXT"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{14F7123C-682C-4BE9-8A5E-414BE047C1A0}
AppName={#MyAppName}
AppVerName={#MyAppVerName}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\{#MyAppName}
DefaultGroupName={#MyAppName}
DisableProgramGroupPage=true
LicenseFile={#MyLicenseFile}
OutputBaseFilename=LevelZap{#MyAppVersion}
Compression=lzma
SolidCompression=true
VersionInfoVersion={#MyAppFullVersion}
VersionInfoDescription={#MyAppDescription}
VersionInfoTextVersion={#MyAppFullVersion}
ArchitecturesInstallIn64BitMode=x64
VersionInfoCopyright={#MyAppCopyright}
DisableReadyMemo=true
AlwaysShowComponentsList=false
DisableReadyPage=true
OutputDir=Output

[Languages]
Name: english; MessagesFile: compiler:Default.isl

[Files]
Source: ..\bin\Win32\Release\LevelZap.dll; DestDir: {app}; Flags: ignoreversion regserver restartreplace overwritereadonly uninsrestartdelete uninsremovereadonly 32bit; DestName: LZap32.dll
Source: ..\bin\x64\Release\LevelZap.dll; DestDir: {app}; Flags: ignoreversion regserver restartreplace overwritereadonly uninsrestartdelete uninsremovereadonly 64bit; DestName: LZap64.dll; Check: Is64BitInstallMode
Source: ..\LICENSE.TXT; DestDir: {app}; Flags: overwritereadonly uninsremovereadonly
Source: ..\HISTORY.TXT; DestDir: {app}; Flags: overwritereadonly uninsremovereadonly
Source: .\Input\LevelZap on CodePlex.url; DestDir: {app}; Flags: overwritereadonly uninsremovereadonly

[Registry]
; Delete the old association with "Folder" that was used in LevelZap 1.0.2 or less.
; The DLL now associates with "Directory" which prevents us from trying to zap ZIP files.
Root: HKCR; SubKey: Folder\shellex\ContextMenuHandlers\LevelZap; Flags: DeleteKey DontCreateKey;
