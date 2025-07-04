; NSIS installer script placeholder for MediaPlayer
OutFile "MediaPlayerSetup.exe"
InstallDir "$PROGRAMFILES\MediaPlayer"
Section "MainSection" SEC01
    SetOutPath "$INSTDIR"
    ; Files will be added here by packaging workflow
SectionEnd
