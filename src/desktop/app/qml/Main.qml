import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    id: win
    visible: true
    width: 800
    height: 600
    title: qsTr("MediaPlayer")
    property string errorMessage: ""

    MediaPlayerController {
        id: player
        onErrorOccurred: {
            win.errorMessage = message
            errorDialog.open()
        }
    }
    LibraryModel { id: libraryModel }
    PlaylistModel { id: playlistModel }

    header: ToolBar {
        RowLayout {
            anchors.fill: parent
            TextField {
                id: search
                placeholderText: qsTr("Search")
                onTextChanged: libraryModel.search(text)
            }
            Button { text: qsTr("Settings"); onClicked: settings.open() }
        }
    }

    SettingsDialog { id: settings }

    Dialog {
        id: errorDialog
        title: qsTr("Playback Error")
        standardButtons: Dialog.Ok
        Column {
            spacing: 8
            Label { text: win.errorMessage }
        }
    }

    focus: true
    Keys.onPressed: {
        if (event.key === Qt.Key_Space) {
            player.playing ? player.pause() : player.play();
            event.accepted = true;
        }
    }

    ColumnLayout {
        anchors.fill: parent
        VideoPlayer { Layout.fillWidth: true; height: 300 }
        LibraryView { Layout.fillWidth: true; Layout.fillHeight: true }
        PlaylistView { Layout.fillWidth: true; height: 100 }
        VisualizationView { Layout.fillWidth: true; height: 150 }
        RowLayout {
            Layout.fillWidth: true
            // Icon resources are omitted in the repository. Text labels are used
            // as placeholders. Replace with icons when available.
            Button { text: qsTr("Prev"); onClicked: player.seek(player.position() - 10) }
            Button { text: player.playing ? qsTr("Pause") : qsTr("Play"); onClicked: player.playing ? player.pause() : player.play() }
            Button { text: qsTr("Next"); onClicked: player.seek(player.position() + 10) }
            Slider { Layout.fillWidth: true; from: 0; to: 100; onMoved: player.seek(value) }
            Slider { from: 0; to: 1; value: 1; onValueChanged: player.setVolume(value) }
        }
    }
}
