import QtQuick 2.15
import MediaPlayer 1.0

Item {
    width: 640; height: 360
    VideoItem {
        anchors.fill: parent
        videoOutput: player.videoOutput
    }
}
