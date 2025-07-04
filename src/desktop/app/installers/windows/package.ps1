# PowerShell script to deploy Qt dependencies and build the NSIS installer
$ErrorActionPreference = 'Stop'

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Definition
$projectRoot = Resolve-Path "$scriptDir\..\..\..\..\.."
$buildDir = $env:BUILD_DIR
if (-not $buildDir) { $buildDir = Join-Path $projectRoot 'build\Release' }
$distDir = Join-Path $projectRoot 'dist'
$appName = 'mediaplayer_desktop_app.exe'
$deployDir = Join-Path $distDir 'MediaPlayer'

New-Item -ItemType Directory -Path $deployDir -Force | Out-Null

Copy-Item "$buildDir\$appName" $deployDir -Force
Copy-Item "$buildDir\qml" $deployDir -Recurse -Force -ErrorAction SilentlyContinue
Copy-Item "$buildDir\translations" $deployDir -Recurse -Force -ErrorAction SilentlyContinue

& windeployqt "$deployDir\$appName" --qmldir "$projectRoot\src\desktop\app\qml"

& makensis /DAPP_DIR="$deployDir" "$scriptDir\mediaplayer.nsi"
Move-Item "$scriptDir\MediaPlayerSetup.exe" $distDir -Force
Write-Host "Installer created at $distDir\MediaPlayerSetup.exe"
