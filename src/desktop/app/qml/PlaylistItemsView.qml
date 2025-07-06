import QtQuick 2.15
import QtQuick.Controls 2.15

ListView {
    id: playlistItems
    anchors.fill: parent
    Layout.fillWidth: true
    Layout.fillHeight: true
    property string playlistName: ""
    model: playlistModel.playlistItems(playlistName)
    delegate: Row {
        spacing: 4
        property int itemIndex: index
        Text { text: model.title }
        Button {
            text: qsTr("Remove")
            onClicked: playlistModel.removeFromPlaylist(playlistName, model.path)
        }
        Drag.active: drag.active
        Drag.mimeData: {
            "index": itemIndex,
            "path": model.path,
            "sourcePlaylist": playlistName
        }
    }
    DropArea {
        anchors.fill: parent
        onDropped: {
            if (drop.mimeData.hasOwnProperty("index"))
                playlistModel.moveItem(
                    playlistName,
                    drop.mimeData.index,
                    playlistItems.indexAt(drop.position.x, drop.position.y))
            else if (drop.mimeData.hasOwnProperty("path")) {
                playlistModel.addItem(playlistName, drop.mimeData.path)
                if (drop.mimeData.hasOwnProperty("sourcePlaylist") &&
                        drop.mimeData.sourcePlaylist !== playlistName)
                    playlistModel.removeFromPlaylist(
                        drop.mimeData.sourcePlaylist,
                        drop.mimeData.path)
            }
        }
    }

    Connections {
        target: playlistModel
        onPlaylistItemsChanged: if (name === playlistName)
            playlistItems.model = playlistModel.playlistItems(name)
    }
}
