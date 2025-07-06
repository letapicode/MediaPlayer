import QtQuick 2.15
import QtQuick.Controls 2.15

ColumnLayout {
    spacing: 4
    anchors.fill: parent

    SmartPlaylistEditor { id: smartEditor }
    PlaylistItemsDialog { id: itemsDialog }

    RowLayout {
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
        Layout.fillWidth: true
        Layout.fillHeight: true
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
