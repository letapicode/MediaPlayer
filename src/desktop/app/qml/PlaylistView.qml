import QtQuick 2.15
import QtQuick.Controls 2.15

Column {
    spacing: 4
    TextField {
        id: nameField
        placeholderText: qsTr("New playlist")
        onAccepted: { playlistModel.createPlaylist(text); text = "" }
    }
    ListView {
        id: list
        anchors.fill: parent
        model: playlistModel
        delegate: Row {
            spacing: 4
            Text { text: model.name }
            Button { text: qsTr("Delete"); onClicked: playlistModel.removePlaylist(model.name) }
        }
    }
}
