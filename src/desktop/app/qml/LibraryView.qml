import QtQuick 2.15
import QtQuick.Controls 2.15

Column {
    spacing: 4

    TextField {
        id: searchField
        placeholderText: qsTr("Search")
        onTextChanged: libraryModel.search(text)
    }

    ListView {
        id: view
        anchors.fill: parent
        Layout.fillWidth: true
        Layout.fillHeight: true
        model: libraryModel
        delegate: Item {
            width: parent.width
            implicitHeight: 24
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
}
