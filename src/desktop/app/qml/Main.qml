import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    id: window
    visible: true
    width: 800
    height: 600
    title: qsTr("Media Player")

    MediaPlayerController {
        id: player
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 8

        VideoPlayer {
            id: video
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            ToolButton {
                id: playPause
                icon.source: player.playing ? "qrc:/icons/pause.svg" : "qrc:/icons/play.svg"
                onClicked: player.playing ? player.pause() : player.play()
            }

            Slider {
                id: seek
                Layout.fillWidth: true
                from: 0
                to: player.duration
                value: player.position
                onMoved: player.seek(value)
            }

            Slider {
                id: volume
                width: 100
                from: 0
                to: 1
                value: player.volume
                onValueChanged: player.setVolume(value)
            }
        }
    }
}
