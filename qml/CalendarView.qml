import QtQuick 2.7
import QtQuick.Controls.Material 2.0
import org.binque.taaasty 1.0

Pane {
    id: back
    property int tlogId: 0
    property Tlog tlog: Tlog {
        tlogId: back.tlogId
    }
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
        section.property: 'entry.month'
        section.criteria: ViewSection.FullString
        section.labelPositioning: ViewSection.InlineLabels | ViewSection.CurrentLabelAtStart
        section.delegate: Rectangle {
            color: sectionMouse.pressed ? Material.accent : Material.primary
            width: window.width
            height: 5 * mm + monthText.contentHeight
            Poppable {
                id: sectionMouse
                body: back
                onClicked: {
                    mouse.accepted = true;
                    var e = calendarModel.firstMonthEntry(section);
                    if (back.tlog.author.isDaylog)
                        window.pushTlog(back.tlogId, 0, e.date);
                    else
                        window.pushTlog(back.tlogId, e.id);
                }
            }
            ThemedText {
                id: monthText
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left
                    right: parent.right
                }
                horizontalAlignment: Text.AlignHCenter
                text: section
                font.pointSize: window.fontBigger
            }
        }
        delegate: Rectangle {
            id: entryView
            width: window.width
            height: 5 * mm + date.height + entryTitle.height + comments.height
            color: pop.pressed ? Material.primary : 'transparent'
            Poppable {
                id: pop
                body: back
                onClicked: {
                    if (back.x > 0) {
                        mouse.accepted = false;
                        return;
                    }

                    window.pushFullEntryById(entry.id);
                }
            }
            ThemedText {
                id: entryTitle
                anchors {
                    top: parent.top
                    left: parent.left
                    right: parent.right
                    topMargin: 2 * mm
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
