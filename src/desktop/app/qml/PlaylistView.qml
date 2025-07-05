import QtQuick 2.15
import QtQuick.Controls 2.15

Column {
    spacing: 4

    SmartPlaylistEditor { id: smartEditor }
    PlaylistItemsDialog { id: itemsDialog }

    Row {
        spacing: 4
        TextField {
            id: nameField
            placeholderText: qsTr("New playlist")
            onAccepted: { playlistModel.createPlaylist(text); text = "" }
        }
        Button {
            text: qsTr("New smart playlist")
            onClicked: smartEditor.open()
        }
    }

    ListView {
        id: list
        anchors.fill: parent
        model: playlistModel
        delegate: Row {
            spacing: 4
            property string plName: model.name
            Text { text: plName }
            Button { text: qsTr("Open"); onClicked: { itemsDialog.playlistName = plName; itemsDialog.open() } }
            Button { text: qsTr("Delete"); onClicked: playlistModel.removePlaylist(plName) }
        }
    }
}
