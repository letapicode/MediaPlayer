import QtQuick 2.15
import QtQuick.Controls 2.15

Dialog {
    id: settings
    title: qsTr("Settings")
    standardButtons: Dialog.Close
    Column {
        spacing: 8
        CheckBox { text: qsTr("Dark theme"); onToggled: Qt.application.theme = checked ? "dark" : "light" }
        Row {
            spacing: 4
            Label { text: qsTr("Audio device") }
            ComboBox { id: deviceBox; model: audioDevicesModel; textRole: "name" }
            Button { text: qsTr("Refresh"); onClicked: audioDevicesModel.refresh() }
        }
        Row {
            spacing: 4
            Label { text: qsTr("Language") }
            ComboBox {
                id: lang
                model: [ { text: "English", value: "en" }, { text: "Español", value: "es" } ]
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
            height: 100
            width: parent.width
            model: discoveredDevices
            delegate: Row {
                spacing: 4
                Text { text: name }
                Button {
                    text: qsTr("Send")
                    onClicked: sync.sendSync(address, port, win.currentFile, player.position())
                }
            }
        }
        CheckBox {
            id: visToggle
            text: qsTr("Enable Visualization")
            checked: true
            onToggled: player.visualizer.setEnabled(checked)
        }
        Row {
            spacing: 4
            Button { text: qsTr("Prev Preset"); onClicked: player.visualizer.previousPreset() }
            Button { text: qsTr("Next Preset"); onClicked: player.visualizer.nextPreset() }
        }
        GroupBox {
            title: qsTr("Format Converter")
            Column {
                spacing: 4
                Row {
                    spacing: 4
                    Button { text: qsTr("Input"); onClicked: inDialog.open() }
                    Label { text: convInput }
                }
                Row {
                    spacing: 4
                    Button { text: qsTr("Output"); onClicked: outDialog.open() }
                    Label { text: convOutput }
                }
                ComboBox { id: convType; model: [qsTr("Audio"), qsTr("Video")] }
                Button {
                    text: qsTr("Start")
                    enabled: convInput !== "" && convOutput !== ""
                    onClicked: {
                        converting = true
                        if (convType.currentIndex === 0)
                            formatConverter.convertAudio(convInput, convOutput)
                        else
                            formatConverter.convertVideo(convInput, convOutput)
                    }
                }
                ProgressBar { id: convProgress; value: 0; visible: converting }
            }
        }
    }

    property var discoveredDevices: []
    property string convInput: ""
    property string convOutput: ""
    property bool converting: false

    FileDialog { id: inDialog; onAccepted: convInput = file }
    FileDialog { id: outDialog; onAccepted: convOutput = file }

    Connections {
        target: formatConverter
        onProgressChanged: convProgress.value = progress
        onConversionFinished: converting = false
    }
}
