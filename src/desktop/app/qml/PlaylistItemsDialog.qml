import QtQuick 2.15
import QtQuick.Controls 2.15

Dialog {
    id: dlg
    property string playlistName: ""
    title: playlistName
    width: 400
    height: 300
    standardButtons: Dialog.Close

    PlaylistItemsView {
        anchors.fill: parent
        playlistName: dlg.playlistName
    }
}
