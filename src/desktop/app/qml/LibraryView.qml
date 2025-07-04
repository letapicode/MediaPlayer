import QtQuick 2.15
import QtQuick.Controls 2.15

ListView {
    id: view
    anchors.fill: parent
    model: libraryModel
    delegate: Item {
        width: parent.width
        height: 24
        Text { text: model.title; anchors.verticalCenter: parent.verticalCenter }
        MouseArea {
            anchors.fill: parent
            onDoubleClicked: player.openFile(model.path)
        }
    }
}
