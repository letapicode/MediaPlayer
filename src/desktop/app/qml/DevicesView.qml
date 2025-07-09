import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    property var model: []
    ListView {
        anchors.fill: parent
        model: model
        delegate: Row {
            spacing: 8
            Text { text: name }
            Button {
                text: qsTr("Send")
                onClicked: sync.sendSync(address, port, player.currentFile, player.position)
            }
        }
    }
}
