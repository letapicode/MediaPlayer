import QtQuick 2.15
import QtQuick.Controls 2.15

ListView {
    id: nowPlaying
    anchors.fill: parent
    model: nowPlayingModel
    delegate: Row {
        id: row
        spacing: 4
        property int itemIndex: index
        Text { text: model.path }
        Button {
            text: qsTr("Remove")
            onClicked: nowPlayingModel.removeAt(index)
        }
        Drag.active: drag.active
        Drag.mimeData: { "index": itemIndex }
    }
    DropArea {
        anchors.fill: parent
        onDropped: {
            if (drop.mimeData.hasOwnProperty("index"))
                nowPlayingModel.moveItem(drop.mimeData.index, nowPlaying.indexAt(drop.position.x, drop.position.y))
            else if (drop.mimeData.hasOwnProperty("path"))
                player.addToQueue(drop.mimeData.path)
        }
    }
}
