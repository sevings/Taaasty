import QtQuick 2.7

Pane {
    id: back
    readonly property bool isTrainingProgress: true
    Poppable {
        body: back
        shiftable: false
    }
    Splash {
        visible: tlogBar.to <= 0
    }
    ThemedProgressBar {
        id: tlogBar
        visible: to > 0
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            right: parent.right
        }
        text: Trainer.currentName || 'Тлог'
        value: Trainer.trainedEntriesCount
        to: Trainer.entriesCount
    }
}
