import QtQuick 2.15
import MediaPlayer 1.0

Item {
    width: 640; height: 360
    VideoItem {
        id: item
        anchors.fill: parent
        videoOutput: player.videoOutput
        MouseArea {
            anchors.fill: parent
            onDoubleClicked: fsWindow.visible = true
        }
    }
    Window {
        id: fsWindow
        visible: false
        flags: Qt.Window | Qt.FramelessWindowHint
        visibility: Window.FullScreen
        color: "black"
        VideoItem {
            anchors.fill: parent
            videoOutput: player.videoOutput
            MouseArea { anchors.fill: parent; onDoubleClicked: fsWindow.visible = false }
        }
    }
}
