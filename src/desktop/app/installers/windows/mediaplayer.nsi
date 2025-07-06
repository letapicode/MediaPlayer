; NSIS installer script for MediaPlayer
; Uses files prepared by package.ps1 which runs windeployqt.

!include "MUI2.nsh"

!ifndef APP_DIR
!error "APP_DIR not defined. Pass /DAPP_DIR=path to makensis."
!endif

Name "MediaPlayer"
OutFile "MediaPlayerSetup.exe"
InstallDir "$PROGRAMFILES\MediaPlayer"
InstallDirRegKey HKLM "Software\MediaPlayer" "Install_Dir"

ShowInstDetails show
ShowUninstDetails show

Page directory
Page instfiles
UninstPage uninstConfirm
UninstPage instfiles

Section "Install" SEC01
    SetOutPath "$INSTDIR"
    File /r "${APP_DIR}\*"
    WriteRegStr HKLM "Software\MediaPlayer" "Install_Dir" "$INSTDIR"
    CreateDirectory "$SMPROGRAMS\MediaPlayer"
    CreateShortCut "$SMPROGRAMS\MediaPlayer\MediaPlayer.lnk" "$INSTDIR\mediaplayer_desktop_app.exe"
    CreateShortCut "$DESKTOP\MediaPlayer.lnk" "$INSTDIR\mediaplayer_desktop_app.exe"
SectionEnd

Section "Uninstall" SEC02
    Delete "$DESKTOP\MediaPlayer.lnk"
    Delete "$SMPROGRAMS\MediaPlayer\MediaPlayer.lnk"
    RMDir "$SMPROGRAMS\MediaPlayer"
    DeleteRegKey HKLM "Software\MediaPlayer"
    RMDir /r "$INSTDIR"
SectionEnd
