import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    LibraryQt {
        id: lib
    }

    ListView {
        anchors.fill: parent
        model: lib.allMedia()
        delegate: Text { text: modelData["title"] }
    }
}
