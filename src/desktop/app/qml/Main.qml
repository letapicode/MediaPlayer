import QtQuick 2.15 import QtQuick.Controls 2.15 import QtQuick.Controls.Material 2.15 import QtQuick.Layouts 1.15 import QtQuick.Dialogs 1.3 import "NowPlayingView.qml" as NowPlayingView

    ApplicationWindow{
        id: window
        visible: true
        width: 800
        height: 600
        title: qsTr("MediaPlayer")
        property string errorMessage: ""
        property string notificationText: ""
        property bool showNotification: false
        Material.theme: settings.theme == "dark" ? Material.Dark : Material.Light
        property string currentFile: ""

                                                                                Connections{target:sync function onSyncReceived(path, position){player.openFile(path) player.seek(position) window.currentFile = path } }

                                                                                                                                                MediaPlayerController{id:player onErrorOccurred: {window.errorMessage = message errorDialog.open() } onPositionChanged:sync.updateStatus(window.currentFile, position) } LibraryModel{id:libraryModel } PlaylistModel{id:playlistModel }

                                                                                                                                                                                                  header:ToolBar{RowLayout{anchors.fill:parent TextField{id:search placeholderText:qsTr("Search") onTextChanged:libraryModel.search(text) } Button{text:qsTr("Open") onClicked:fileDialog.open() } Button{text:qsTr("Settings");
onClicked : settings.open()
}
}
}

FileDialog {
id:
  fileDialog onAccepted : {
    player.openFile(fileDialog.file) window.currentFile =
        fileDialog.file sync.updateStatus(window.currentFile, 0)
  }
}

SettingsDialog{id : settings}

MessageDialog{
  id : errorDialog title : qsTr("Playback Error")
  text : window.errorMessage standardButtons : Dialog.Ok
}
Timer {
    id: notificationTimer
    interval: 2000
    running: false
    repeat: false
    onTriggered: window.showNotification = false
}
Connections {
    target: voiceCommandProcessor
    onCommandUnknown: {
        window.notificationText = qsTr("Command not understood.")
        window.showNotification = true
        notificationTimer.restart()
    }
}
Connections {
    target: microphoneInput
    onErrorOccurred: {
        window.notificationText = message
        window.showNotification = true
        notificationTimer.restart()
    }
}
Connections {
    target: voiceRecognizer
    onError: {
        window.notificationText = message
        window.showNotification = true
        notificationTimer.restart()
    }
}

focus : true Keys.onPressed : {
  if (event.key == = Qt.Key_Space) {
    player.playing ? player.pause() : player.play() event.accepted = true
  } else if (event.key == = Qt.Key_Left) {
    player.seek(player.position - 5) event.accepted = true
  } else if (event.key == = Qt.Key_Right) {
    player.seek(player.position + 5) event.accepted = true
  } else if (event.key == = Qt.Key_Up) {
    player.setVolume(Math.min(1, player.volume + 0.05)) event.accepted = true
  } else if (event.key == = Qt.Key_Down) {
    player.setVolume(Math.max(0, player.volume - 0.05)) event.accepted = true
  } else if (event.key == = Qt.Key_MediaNext) {
    player.nextTrack() event.accepted = true
  } else if (event.key == = Qt.Key_MediaPrevious) {
    player.previousTrack() event.accepted = true
  }
}

ColumnLayout {
  anchors.fill : parent
                 VideoPlayer{Layout.fillWidth : true Layout.preferredHeight : 300} RowLayout {
  spacing:
    8 Label{text : player.title} Label{text : player.artist} Label {
    text:
      player.album
    }
  }
  LibraryView{Layout.fillWidth : true Layout.fillHeight : true}
  PlaylistView{Layout.fillWidth : true Layout.preferredHeight : 100}
  NowPlayingView{Layout.fillWidth : true Layout.preferredHeight : 120}
  VisualizationView{Layout.fillWidth : true Layout.preferredHeight : 150} RowLayout {
    Layout.fillWidth : true ToolButton{
      icon.source : "qrc:/icons/prev.svg" onClicked : player.seek(player.position - 10)
    } ToolButton{
      id : playPause icon.source : player.playing        ? "qrc:/icons/pause.svg"
      : "qrc:/icons/play.svg" onClicked : player.playing ? player.pause()
                                                         : player.play()
    } ToolButton{
      icon.source : "qrc:/icons/next.svg" onClicked : player.seek(player.position + 10)
    } ToolButton {
    text:
      "\uD83C\uDFA4" onClicked : {
        if (voiceRecognizer.running) {
          microphoneInput.stop() voiceRecognizer.stop()
        } else {
          voiceRecognizer.start() microphoneInput.start()
        }
      }
    }
    Label{text : Math.floor(player.position)} Slider{
      id : seekSlider Layout.fillWidth : true from : 0 to : player.duration
      value : player.position onMoved : player.seek(value)
    } Label{text : Math.floor(player.duration)} Slider {
    from:
      0;
    to:
      1;
    value:
      1;
    onValueChanged:
      player.setVolume(value)
    }
  }
}
  Rectangle {
    anchors.fill : parent color : "#80000000" visible
        : voiceRecognizer.running z : 10
          Text{anchors.centerIn : parent text : qsTr("Listening...") color : "white"}
  }
  Rectangle {
    id: notificationBar
    visible: window.showNotification
    width: parent.width * 0.6
    height: implicitHeight
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.bottom: parent.bottom
    anchors.bottomMargin: 20
    color: "#323232"
    radius: 4
    z: 20
    Text { anchors.centerIn: parent; color: "white"; text: window.notificationText }
  }
}
