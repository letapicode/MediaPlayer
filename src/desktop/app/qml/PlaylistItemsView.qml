import QtQuick 2.15
import QtQuick.Controls 2.15

ListView {
    id: playlistItems
    anchors.fill: parent
    property string playlistName: ""
    model: playlistModel.playlistItems(playlistName)
    delegate: Row {
        spacing: 4
        Text { text: model.title }
        Button { text: qsTr("Remove"); onClicked: playlistModel.removeFromPlaylist(playlistName, model.path) }
    }
}
