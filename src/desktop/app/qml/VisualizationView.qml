import QtQuick 2.15
import QtQuick.Controls 2.15
import MediaPlayer 1.0

Item {
    id: root
    Layout.fillWidth: true
    Layout.fillHeight: true
    VisualizerItem {
        id: canvas
        anchors.fill: parent
        visualizer: player.visualizer
    }

    Timer {
        interval: 16
        running: true
        repeat: true
        onTriggered: canvas.update()
    }

    Row {
        spacing: 10
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        Button {
            text: qsTr("Prev")
            onClicked: player.visualizer.previousPreset()
        }
        Button {
            text: qsTr("Next")
            onClicked: player.visualizer.nextPreset()
        }
    }

    Component.onCompleted: player.visualizer.setEnabled(true)
}
