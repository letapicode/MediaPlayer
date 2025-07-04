import QtQuick 2.15
import QtQuick.Controls 2.15

Dialog {
    id: settings
    title: qsTr("Settings")
    standardButtons: Dialog.Close
    Column {
        spacing: 8
        CheckBox { text: qsTr("Dark theme"); onToggled: Qt.application.theme = checked ? "dark" : "light" }
    }
}
