import QtQuick 2.15
import QtQuick.Controls 2.15

ListView {
    id: view
    anchors.fill: parent
    model: libraryModel
    delegate: Item {
        width: parent.width
        height: 40
        property string path: model.path
        Column {
            anchors.verticalCenter: parent.verticalCenter
            Text { text: model.title }
            Text {
                text: model.artist
                font.pixelSize: 10
                color: "#666"
            }
        }
        MouseArea {
            id: dragArea
            anchors.fill: parent
            drag.target: dragArea
            onDoubleClicked: player.openFile(model.path)
        }
        Drag.active: dragArea.drag.active
        Drag.mimeData: { "path": path }
    }
}
