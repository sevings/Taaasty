import QtQuick 2.7

Pane {
    id: back
    property bool fullLoad: true
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
        anchors.bottom: allBar.visible ? parent.verticalCenter : undefined
        anchors.verticalCenter: allBar.visible ? undefined : parent.verticalCenter
        anchors.left: parent.left
        anchors.right: parent.right
        text: Trainer.currentName || 'Тлог'
        value: Trainer.trainedEntriesCount
        to: Trainer.entriesCount
    }
    ThemedProgressBar {
        id: allBar
        visible: back.fullLoad
        anchors.top: parent.verticalCenter
        anchors.left: parent.left
        anchors.right: parent.right
        text: 'Всего'
        value: Trainer.currentTlog
        to: Trainer.tlogsCount
        percents: true
    }
}
