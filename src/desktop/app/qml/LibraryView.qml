import QtQuick 2.15
import QtQuick.Controls 2.15

ListView {
    id: view
    anchors.fill: parent
    model: libraryModel
    delegate: Text {
        text: model.title
    }
}
