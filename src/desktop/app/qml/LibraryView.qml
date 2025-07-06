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
        model: libraryModel
        delegate: Item {
            width: parent.width
            height: 24
            property string path: model.path
            Text {
                text: model.title
                anchors.verticalCenter: parent.verticalCenter
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
