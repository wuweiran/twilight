#define VSPlatform "{param:PLATFORM}"
#define SourceDir "{param:TARGETDIR}"

#if VSPlatform == "x64"
  #define Architecture "x64"
#elif VSPlatform == "ARM64EC"
  #define Architecture "arm64"
#else
  #define Architecture "x86"
#endif

[Setup]
; NOTE: The value of AppId uniquely identifies this application. Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{B434F175-E9E1-44D2-BBEE-6E43B3002F4E}
AppName=Twilight
AppVersion=1.0
;AppVerName=Twilight 1.0
AppPublisher=M1Knight Technology
AppPublisherURL=https://www.example.com/
AppSupportURL=https://www.example.com/
AppUpdatesURL=https://www.example.com/
DefaultDirName={autopf}\Twilight
UninstallDisplayIcon={app}\twilight.exe
ArchitecturesAllowed={#Architecture}
ArchitecturesInstallIn64BitMode=x64compatible
DefaultGroupName=Twilight
DisableProgramGroupPage=yes
; Uncomment the following line to run in non administrative install mode (install for current user only).
;PrivilegesRequired=lowest
OutputBaseFilename=twilight_setup_{#VSPlatform}.exe
SolidCompression=yes
WizardStyle=modern

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Dirs]
Name: "{app}"; Permissions: users-modify

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "{#SourceDir}\twilight.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SourceDir}\twilight.pdb"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SourceDir}\jsoncpp.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SourceDir}\WebView2Loader.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SourceDir}\index.html"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SourceDir}\assets\*"; DestDir: "{app}\assets"; Flags: ignoreversion recursesubdirs createallsubdirs
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\Twilight"; Filename: "{app}\twilight.exe"
Name: "{autodesktop}\Twilight"; Filename: "{app}\twilight.exe"; Tasks: desktopicon

[Run]
Filename: "{app}\twilight.exe"; Description: "{cm:LaunchProgram,Twilight}"; Flags: nowait postinstall skipifsilent

