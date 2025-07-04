import QtQuick 2.15
import QtQuick.Controls 2.15

ListView {
    id: nowPlaying
    anchors.fill: parent
    model: nowPlayingModel
    delegate: Row {
        id: row
        spacing: 4
        Text { text: model.path }
        Button {
            text: qsTr("Remove")
            onClicked: nowPlayingModel.removeAt(index)
        }
        Drag.active: drag.active
    }
}
