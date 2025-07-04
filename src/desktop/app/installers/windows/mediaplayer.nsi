; NSIS installer script for MediaPlayer
!include "MUI2.nsh"

!ifndef APP_DIR
!error "APP_DIR not defined. Pass /DAPP_DIR=path to makensis."
!endif

OutFile "MediaPlayerSetup.exe"
InstallDir "$PROGRAMFILES\MediaPlayer"

Section "MainSection" SEC01
    SetOutPath "$INSTDIR"
    File /r "${APP_DIR}\*"
SectionEnd
