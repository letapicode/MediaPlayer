import QtQuick 2.15
import QtQuick.Controls 2.15

Dialog {
    id: dlg
    title: qsTr("Smart Playlist Editor")
    standardButtons: Dialog.Ok | Dialog.Cancel
    Column {
        spacing: 8
        TextField { id: field; placeholderText: qsTr("Filter query") }
    }
}
