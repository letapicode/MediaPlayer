import QtQuick 2.15
import QtQuick.Controls 2.15
import MediaPlayer 1.0

Item {
    id: root
    anchors.fill: parent

    VisualizerQt { id: vis }
    VisualizerItem {
        id: canvas
        anchors.fill: parent
        visualizer: vis
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
            text: "Prev"
            onClicked: vis.previousPreset()
        }
        Button {
            text: "Next"
            onClicked: vis.nextPreset()
        }
    }

    Component.onCompleted: vis.setEnabled(true)
}
