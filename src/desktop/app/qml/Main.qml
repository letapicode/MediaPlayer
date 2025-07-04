import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow {
    id: win
    visible: true
    width: 800
    height: 600
    title: qsTr("MediaPlayer")

    MediaPlayerController { id: player }
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
            ToolButton {
                icon.source: "qrc:/icons/prev.svg"
                onClicked: player.seek(player.position() - 10)
            }
            ToolButton {
                id: playPause
                icon.source: player.playing ? "qrc:/icons/pause.svg" : "qrc:/icons/play.svg"
                onClicked: player.playing ? player.pause() : player.play()
            }
            ToolButton {
                icon.source: "qrc:/icons/next.svg"
                onClicked: player.seek(player.position() + 10)
            }
            Slider { Layout.fillWidth: true; from: 0; to: 100; onMoved: player.seek(value) }
            Slider { from: 0; to: 1; value: 1; onValueChanged: player.setVolume(value) }
        }
    }
}
