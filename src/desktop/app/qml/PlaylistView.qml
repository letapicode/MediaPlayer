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

    Rectangle {
        id: removeArea
        width: parent.width
        height: 40
        color: "#e0cccc"
        radius: 4
        border.width: 1
        border.color: "#a08080"
        Text {
            anchors.centerIn: parent
            text: qsTr("Drop here to remove")
        }
        DropArea {
            anchors.fill: parent
            onDropped: if (drop.mimeData.hasOwnProperty("path") &&
                           drop.mimeData.hasOwnProperty("sourcePlaylist"))
                             playlistModel.removeFromPlaylist(
                                 drop.mimeData.sourcePlaylist,
                                 drop.mimeData.path)
        }
    }

    ListView {
        id: list
        anchors.fill: parent
        anchors.topMargin: removeArea.height + 4
        model: playlistModel
        delegate: Rectangle {
            width: parent.width
            height: 24
            color: "transparent"
            Row {
                anchors.verticalCenter: parent.verticalCenter
                spacing: 4
                property string plName: model.name
                Text { text: plName }
                Button {
                    text: qsTr("Open")
                    onClicked: {
                        itemsDialog.playlistName = plName
                        itemsDialog.open()
                    }
                }
                Button {
                    text: qsTr("Delete")
                    onClicked: playlistModel.removePlaylist(plName)
                }
            }
            DropArea {
                anchors.fill: parent
                onDropped: if (drop.mimeData.hasOwnProperty("path")) {
                              playlistModel.addItem(plName, drop.mimeData.path)
                              if (drop.mimeData.hasOwnProperty("sourcePlaylist") &&
                                  drop.mimeData.sourcePlaylist !== plName)
                                  playlistModel.removeFromPlaylist(
                                      drop.mimeData.sourcePlaylist,
                                      drop.mimeData.path)
                          }
            }
        }
    }
}
