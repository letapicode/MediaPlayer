import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.3

Dialog {
    id: settingsDialog
    title: qsTr("Settings")
    standardButtons: Dialog.Close
    onOpened: {
        themeBox.checked = settings.theme == "dark"
        for (let i = 0; i < audioDevicesModel.rowCount(); ++i) {
            if (audioDevicesModel.get(i).name == settings.audioDevice) {
                deviceBox.currentIndex = i
                break
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 8

        CheckBox {
            id: themeBox
            text: qsTr("Dark theme")
            onToggled: {
                Qt.application.theme = checked ? "dark" : "light"
                settings.theme = checked ? "dark" : "light"
            }
        }

        RowLayout {
            spacing: 4
            Label { text: qsTr("Audio device") }
            ComboBox {
                id: deviceBox
                model: audioDevicesModel
                textRole: "name"
                onActivated: {
                    let dev = audioDevicesModel.deviceAt(currentIndex)
                    player.setAudioDevice(dev)
                    settings.audioDevice = audioDevicesModel.get(currentIndex).name
                }
            }
            Button {
                text: qsTr("Refresh")
                onClicked: audioDevicesModel.refresh()
            }
        }

        RowLayout {
            spacing: 4
            Label { text: qsTr("Language") }
            ComboBox {
                id: lang
                model: [
                    { text: "English", value: "en" },
                    { text: "Español", value: "es" }
                ]
                textRole: "text"
                onActivated: translation.switchLanguage(model[currentIndex].value)
            }
        }

        Button {
            text: qsTr("Discover Devices")
            onClicked: discoveredDevices = sync.discoverDevices()
        }

        ListView {
            id: deviceList
            Layout.fillWidth: true
            Layout.preferredHeight: 100
            model: discoveredDevices
            delegate: Row {
                spacing: 4
                Text { text: name }
                Button {
                    text: qsTr("Send")
                    onClicked: sync.sendSync(address, port, win.currentFile, player.position)
                }
            }
        }

        RowLayout {
            spacing: 4
            Button {
                text: qsTr("Scan Library")
                onClicked: scanDialog.open()
            }
            ProgressBar {
                id: scanProgress
                value: 0
                visible: libraryQt.scanRunning()
            }
        }

        CheckBox {
            id: visToggle
            text: qsTr("Enable Visualization")
            checked: true
            onToggled: player.visualizer.setEnabled(checked)
        }

        RowLayout {
            spacing: 4
            Button {
                text: qsTr("Prev Preset")
                onClicked: player.visualizer.previousPreset()
            }
            Button {
                text: qsTr("Next Preset")
                onClicked: player.visualizer.nextPreset()
            }
        }

        GroupBox {
            title: qsTr("Format Converter")
            ColumnLayout {
                spacing: 4

                RowLayout {
                    spacing: 4
                    Button {
                        text: qsTr("Input")
                        onClicked: inDialog.open()
                    }
                    Label { text: convInput }
                }

                RowLayout {
                    spacing: 4
                    Button {
                        text: qsTr("Output")
                        onClicked: outDialog.open()
                    }
                    Label { text: convOutput }
                }

                ComboBox {
                    id: convType
                    model: [ qsTr("Audio"), qsTr("Video") ]
                }

                Button {
                    text: qsTr("Start")
                    enabled: convInput != "" && convOutput != ""
                    onClicked: {
                        converting = true
                        if (convType.currentIndex == 0)
                            formatConverter.convertAudio(convInput, convOutput)
                        else
                            formatConverter.convertVideo(convInput, convOutput)
                    }
                }

                ProgressBar {
                    id: convProgress
                    value: 0
                    visible: converting
                }
            }
        }
    }

    property var discoveredDevices: []
    property string convInput: ""
    property string convOutput: ""
    property bool converting: false

    FileDialog {
        id: scanDialog
        selectFolder: true
        onAccepted: libraryQt.startScan(folder)
    }

    FileDialog {
        id: inDialog
        onAccepted: convInput = file
    }

    FileDialog {
        id: outDialog
        onAccepted: convOutput = file
    }

    Connections {
        target: libraryQt
        onScanProgress: scanProgress.value = total > 0 ? current / total : 0
        onScanFinished: scanProgress.value = 1
    }

    Connections {
        target: formatConverter
        onProgressChanged: convProgress.value = progress
        onConversionFinished: converting = false
    }
}
