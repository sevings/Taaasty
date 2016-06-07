import QtQuick 2.5
import org.binque.taaasty 1.0

MouseArea {
    id: area
    anchors.top: parent.top
    anchors.bottom: parent.bottom
    anchors.left: parent.left
    width: parent.width * 0.7
    propagateComposedEvents: true
    enabled: false
    signal modeChanged(int mode)
    signal settingsRequested
    signal waterTlogsRequested
    signal fireTlogsRequested
    signal loginRequested
    onModeChanged: backAnimation.start();
    Component.onCompleted: {
        enabled = true;
    }
    onReleased: {
        if (area.x > -area.width / 3 * 2)
            forwardAnimation.start();
        else
            backAnimation.start();
    }
    onClicked: {
        if (mouse.x > width)
            backAnimation.start();
        mouse.accepted = x > - width;
    }
    Rectangle {
        id: menu
        anchors.fill: parent
        color: window.backgroundColor
        Flickable {
            anchors.fill: parent
            flickableDirection: Flickable.VerticalFlick
            topMargin: 1 * mm
            bottomMargin: 1 * mm
            contentWidth: parent.width
            property int buttonsCount: Tasty.isAuthorized ? 13 : 11
            contentHeight: 9 * mm * buttonsCount + space.height
            Column {
                id: column
                anchors.fill: parent
                spacing: 1 * mm
                ThemedButton {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    text: 'Мой тлог'
                    onClicked: { modeChanged(FeedModel.MyTlogMode) }
                    visible: Tasty.isAuthorized
                }
                ThemedButton {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    text: 'Подписки'
                    onClicked: { modeChanged(FeedModel.FriendsMode) }
                    visible: Tasty.isAuthorized
                }
                ThemedButton {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    text: 'Прямой эфир'
                    onClicked: { modeChanged(FeedModel.LiveMode) }
                }
                ThemedButton {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    text: 'Анонимки'
                    onClicked: { modeChanged(FeedModel.AnonymousMode) }
                }
                ThemedButton {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    text: 'Лучшее'
                    onClicked: { modeChanged(FeedModel.BestMode) }
                }
                ThemedButton {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    text: 'Отличное'
                    onClicked: { modeChanged(FeedModel.ExcellentMode) }
                }
                ThemedButton {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    text: 'Хорошее'
                    onClicked: { modeChanged(FeedModel.WellMode) }
                }
                ThemedButton {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    text: 'Неплохое'
                    onClicked: { modeChanged(FeedModel.GoodMode) }
                }
                ThemedButton {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    text: 'Избранное'
                    onClicked: { modeChanged(FeedModel.FavoritesMode) }
                    visible: Tasty.isAuthorized
                }
                ThemedButton {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    text: 'Тлог'
                    onClicked: { modeChanged(FeedModel.TlogMode) }
                }
                Item {
                    id: space
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.margins: 1 * mm
                    height: 2 * mm
                }
//                ThemedButton {
//                    anchors.left: parent.left
//                    anchors.right: parent.right
//                    text: 'Water tlogs'
//                    onClicked: { area.waterTlogsRequested() }
//                }
//                ThemedButton {
//                    anchors.left: parent.left
//                    anchors.right: parent.right
//                    text: 'Fire tlogs'
//                    onClicked: { area.fireTlogsRequested() }
//                }
//                ThemedButton {
//                    anchors.left: parent.left
//                    anchors.right: parent.right
//                    text: 'Settings'
//                    onClicked: { area.settingsRequested() }
//                }
                ThemedButton {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    text: 'Загружать изображения'
                    checked: Settings.autoloadImages
                    onClicked: { Settings.autoloadImages = !checked }
                }
                ThemedButton {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    text: 'Скрывать короткие посты'
                    checked: Settings.hideShortPosts
                    onClicked: { Settings.hideShortPosts = !checked }
                }
                ThemedButton {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    text: 'Скрывать посты с отрицательным рейтингом'
                    checked: Settings.hideNegativeRated
                    onClicked: { Settings.hideNegativeRated = !checked }
                }
                ThemedButton {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    text: 'Войти'
                    visible: !Tasty.isAuthorized
                    onClicked: { loginRequested() }
                }
            }
        }
    }
}
