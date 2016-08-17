import QtQuick 2.7
import org.binque.taaasty 1.0

Pane {
    id: back
    property int tlogId: 0
    property Tlog tlog: Tlog {
        tlogId: back.tlogId
    }
    signal pushed
    Poppable {
        body: back
    }
    Splash {
        id: splash
        visible: !listView.visible
    }
    MyListView {
        id: listView
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        height: contentHeight > parent.height ? parent.height : contentHeight
        visible: count > 0
        interactive: back.x == 0
        Connections {
            target: back
            onPushed: {
                if (listView.currentIndex >= 0)
                    listView.positionViewAtIndex(listView.currentIndex, ListView.Center)
            }
        }
        model: CalendarModel {
            id: calendarModel
            Component.onCompleted: {
                setTlog(back.tlogId);
            }
            onLoaded: {
                if (!listView.visible)
                    splash.text = 'Нет записей'
            }
        }
        delegate: Item {
            id: entryView
            width: window.width
            height: 5 * mm + date.height + entryTitle.height + comments.height
            function saveCurrentIndex() {
                listView.currentIndex = listView.indexAt(entryView.x + 1, entryView.y + 1);
            }
            Poppable {
                body: back
                onClicked: {
                    if (back.x > 0) {
                        mouse.accepted = false;
                        return;
                    }

                    saveCurrentIndex();
                    window.pushFullEntryById(entry.id);
                }
            }
            ThemedText {
                id: entryTitle
                anchors {
                    top: parent.top
                    left: parent.left
                    right: parent.right
                }
                text: entry.truncatedTitle || '(без заголовка)'
                textFormat: Text.RichText
                horizontalAlignment: Text.AlignHCenter
            }
            ThemedText {
                id: date
                anchors {
                    top: entryTitle.bottom
                    left: parent.left
                    right: comments.left
                }
                text: entry.createdAt
                color: window.secondaryTextColor
                font.pointSize: window.fontSmallest
                elide: Text.AlignRight
                wrapMode: Text.NoWrap
            }
            ThemedText {
                id: comments
                anchors {
                    top: entryTitle.bottom
                    right: parent.right
                }
                text: entry.commentsCount + ' коммент.'
                font.pointSize: window.fontSmallest
                color: window.secondaryTextColor
            }
        }
    }
}
