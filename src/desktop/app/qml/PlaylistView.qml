import QtQuick 2.15
import QtQuick.Controls 2.15

ListView {
    id: list
    anchors.fill: parent
    model: playlistModel
    delegate: Text { text: model.name }
}
