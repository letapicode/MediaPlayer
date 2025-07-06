import QtQuick 2.15
import QtQuick.Controls 2.15

Dialog {
    id: dlg
    title: qsTr("Smart Playlist Editor")
    standardButtons: Dialog.Ok | Dialog.Cancel
    property var rules: []
    property string playlistName: ""
    ColumnLayout {
        anchors.fill: parent
        spacing: 8
        TextField { id: nameField; placeholderText: qsTr("Name"); text: dlg.playlistName }
        Repeater {
            model: dlg.rules
            delegate: RowLayout {
                spacing: 4
                ComboBox {
                    id: fieldBox
                    model: ["title", "artist", "genre"]
                    currentText: modelData.field
                    onCurrentTextChanged: dlg.rules[index].field = currentText
                }
                ComboBox {
                    id: opBox
                    model: ["contains", "equals"]
                    currentText: modelData.op
                    onCurrentTextChanged: dlg.rules[index].op = currentText
                }
                TextField {
                    id: valueField
                    text: modelData.value
                    onTextChanged: dlg.rules[index].value = text
                }
                Button { text: qsTr("Remove"); onClicked: dlg.rules.splice(index,1) }
            }
        }
        Button { text: qsTr("Add Rule"); onClicked: dlg.rules.push({field:"title", op:"contains", value:""}) }
    }
    onAccepted: {
        var query = "";
        for (var i=0; i<rules.length; ++i) {
            var r = rules[i];
            if (i > 0) query += " AND ";
            query += r.field + " " + r.op + " " + r.value;
        }
        playlistModel.createSmartPlaylist(nameField.text, query)
    }
}
