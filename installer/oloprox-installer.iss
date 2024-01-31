#define MyAppName "oloproxx"
#define MyAppVersion "1.0"
#define MyAppPublisher "@olokreaz"
#define MyAppURL "https://github.com/olokreaz/oloproxx-game"
#define MyAppExeName "oloprox.exe"
#define MyBinaryPath "E:\c++\oloprox\build.x64\Debug"
#define MyContentPath "E:\c++\oloprox\data"
#define MyProtocolName "oloproxx"

[Setup]
AppId={{DFC44B4D-56D1-4AB1-9548-03B2647B9409}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppVerName={#MyAppName}-{#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf64}\{#MyAppName}
DefaultGroupName=MyAppPublisher
AllowNoIcons=yes
LicenseFile=E:\c++\oloprox\LICENSE.md
PrivilegesRequiredOverridesAllowed=dialog                      
OutputDir=E:\c++\oloprox\installer\output
OutputBaseFilename=oloprox-setup
SetupIconFile={#MyBinaryPath}\{#MyAppName}.ico
Password=oloproxx
Encryption=yes
Compression=lzma
SolidCompression=yes
WizardStyle=modern
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64
CreateUninstallRegKey=yes
CreateAppDir=yes
LanguageDetectionMethod=locale

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "russian"; MessagesFile: "compiler:Languages\Russian.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Registry]
Root: HKCR; Subkey: "{#MyProtocolName}"; ValueType: string; ValueName: ""; ValueData: "URL:{#MyAppName} Protocol"
Root: HKCR; Subkey: "{#MyProtocolName}"; ValueType: string; ValueName: "URL Protocol"; ValueData: ""
Root: HKCR; Subkey: "{#MyProtocolName}\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},1"
Root: HKCR; Subkey: "{#MyProtocolName}\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" --url=""%1"""
                                                                                                                                 

[Files]
Source: "{#MyBinaryPath}\{#MyAppExeName}"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MyBinaryPath}\*.dll"; DestDir: "{app}"

[Dirs]
Name: "{app}\assets"; Flags: uninsalwaysuninstall
Name: "{app}\plugins"; Flags: uninsalwaysuninstall

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]                                                                                                                                             
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

