; -------------------------------
; Basic Configuration
; -------------------------------
Unicode true
SetCompressor /SOLID lzma
Name "DNS2HostsSyncer"
!system 'mkdir "../../build"'
OutFile "../../build/DNS2HostsSyncer_Installer.exe"
Caption "DNS2HostsSyncer Installation"
VIProductVersion "4.1.0.0"
VIAddVersionKey "ProductName" "DNS2HostsSyncer"
VIAddVersionKey "FileVersion" "1.0.0"
VIAddVersionKey "LegalCopyright" "© 2025 Until Software"
VIAddVersionKey "FileDescription" "DNS2HostsSyncer Installer"

!define PROGRAM_CONFIG_DIR "C:\ProgramData\neboer\DNS2HostsSyncer"
!define PROGRAM_LOG_DIR "C:\ProgramData\neboer\DNS2HostsSyncer"
; -------------------------------
; Modern UI Configuration
; -------------------------------
!include MUI2.nsh
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"
; !define MUI_WELCOMEFINISHPAGE_BITMAP "welcome.bmp"  ; Recommended size: 164x314
; !define MUI_HEADERIMAGE
; !define MUI_HEADERIMAGE_BITMAP "header.bmp"         ; Recommended size: 150x57

; -------------------------------
; Page Sequence
; -------------------------------
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "../../LICENSE"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

; -------------------------------
; Multi-language Support
; -------------------------------
!insertmacro MUI_LANGUAGE "English"

; -------------------------------
; Installation Directory Settings
; -------------------------------
InstallDir "$PROGRAMFILES64\DNS2HostsSyncer"  ; Default to 64-bit Program Files
InstallDirRegKey HKLM "Software\DNS2HostsSyncer" "Install_Dir"

; -------------------------------
; Installation Logic
; -------------------------------
Section "Main Application" SecMain
  SetOutPath $INSTDIR
  
  ; Copy all files from CMake's package_install directory
  File /r "..\..\build\Release\*.*"
  File "reshedule_task.ps1"  ; add reshedule_task.ps1 to the installer
  File "..\..\config\example_d2hs.json" 

  DetailPrint "Detected user AppData: $0"
  ; Create AppData directory
  CreateDirectory "${PROGRAM_CONFIG_DIR}"
  DetailPrint "Creating directory: ${PROGRAM_CONFIG_DIR} (skip if exists)"

  SetOutPath "${PROGRAM_CONFIG_DIR}"

  DetailPrint "Checking if file exists: [$OUTDIR\d2hs.json]"
  IfFileExists "d2hs.json" skip_config_copy 0
      DetailPrint "File does not exist, starting to copy the configuration file..."
      File /oname=d2hs.json "..\..\config\example_d2hs.json"
      DetailPrint "Created new configuration file: $OUTDIR\d2hs.json"
      Goto after_copy
  skip_config_copy:
      DetailPrint "File already exists, skipping copy"
  after_copy:

  
  ; Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"
  
  ; Registry Entries
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DNS2HostsSyncer" \
                   "DisplayName" "DNS2HostsSyncer"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DNS2HostsSyncer" \
                   "UninstallString" '"$INSTDIR\Uninstall.exe"'
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DNS2HostsSyncer" \
                   "DisplayIcon" "$INSTDIR\DNS2HostsSyncer.exe"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DNS2HostsSyncer" \
                   "Publisher" "Until Software"
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DNS2HostsSyncer" \
                   "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DNS2HostsSyncer" \
                   "NoRepair" 1

  ; create shedule task
  DetailPrint "now creating shedule task..."
  nsExec::ExecToLog 'powershell.exe -ExecutionPolicy Bypass -Command "& { \
    $env:PSModulePath = [System.Environment]::GetEnvironmentVariable(\"PSModulePath\", \"Machine\"); \
    & \"$INSTDIR\reshedule_task.ps1\" -BinaryPath \"$INSTDIR\d2hs.exe\" -LogFilePath \"${PROGRAM_LOG_DIR}\D2HSAutoSync.log\" \
  }"'
  Pop $0  ; get the return value
  ${If} $0 != 0
    MessageBox MB_ICONSTOP|MB_OK "task create failed with error code $0"
    Abort
  ${EndIf}
SectionEnd

; -------------------------------
; Uninstallation Logic
; -------------------------------
Section "Uninstall"
  ; Remove installation directory
  RMDir /r "$INSTDIR"

  ; Remove Start Menu entries
  RMDir /r "$SMPROGRAMS\DNS2HostsSyncer"

  ; Remove registry entries
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\DNS2HostsSyncer"
  DeleteRegKey HKLM "Software\DNS2HostsSyncer"
SectionEnd

; -------------------------------
; Version Info Resource (Optional)
; -------------------------------
!ifdef INCLUDE_VERSION_INFO
VIAddVersionKey "ProductVersion" "4.1.0"
VIAddVersionKey "OriginalFilename" "DNS2HostsSyncer_Installer.exe"
!endif


; -------------------------------
; Resheducle Task
; -------------------------------
