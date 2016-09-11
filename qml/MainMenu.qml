import QtQuick 2.7
import org.binque.taaasty 1.0

Item {
    id: menu
    anchors {
        top: parent.top
        bottom: parent.bottom
        left: parent.left
    }
    width: 40 * mm
    function changeMode(mode) {
        backAnimation.start();
        window.setFeedMode(mode);
    }
    onVisibleChanged: {
        window.hideFooter();
    }
    Flickable {
        anchors.fill: parent
        flickableDirection: Flickable.VerticalFlick
        topMargin: 1 * mm
        bottomMargin: 1 * mm
        contentWidth: parent.width
        contentHeight: column.height
        Column {
            id: column
            width: parent.width
            spacing: 1 * mm
            MenuItem {
                text: 'Мой тлог'
                onTriggered: { changeMode(FeedModel.MyTlogMode) }
                visible: Tasty.isAuthorized
                highlighted: stack.currentItem.mode === FeedModel.MyTlogMode
            }
            MenuItem {
                text: 'Подписки'
                onTriggered: { changeMode(FeedModel.FriendsMode) }
                visible: Tasty.isAuthorized
                highlighted: stack.currentItem.mode === FeedModel.FriendsMode
            }
            MenuSeparator {
                visible: Tasty.isAuthorized
            }
            MenuItem {
                text: 'Прямой эфир'
                onTriggered: { changeMode(FeedModel.LiveMode) }
                highlighted: stack.currentItem.mode === FeedModel.LiveMode
            }
            MenuItem {
                text: 'Лучшее'
                onTriggered: { changeMode(FeedModel.BestMode) }
                highlighted: stack.currentItem.bestMode === true
            }
            MenuItem {
                text: 'Анонимки'
                onTriggered: { changeMode(FeedModel.AnonymousMode) }
                highlighted: stack.currentItem.mode === FeedModel.AnonymousMode
            }
            MenuItem {
                text: 'Потоки'
                onTriggered: { 
                    backAnimation.start();
                    window.pushFlows();
                }
            }
            MenuItem {
                text: 'Тлог'
                onTriggered: { 
                    backAnimation.start();
                    window.showLineInput('tlog');
                }
                highlighted: stack.currentItem.mode === FeedModel.TlogMode
            }
            MenuSeparator {
                visible: Tasty.isAuthorized
            }
            MenuItem {
                text: 'Избранное'
                onTriggered: { changeMode(FeedModel.MyFavoritesMode) }
                visible: Tasty.isAuthorized
                highlighted: stack.currentItem.mode === FeedModel.MyFavoritesMode
            }
            MenuSeparator { }
            MenuItem {
                text: 'Настройки'
                onTriggered: { 
                    backAnimation.start();
                    window.pushSettings();
                }
            }
            MenuItem {
                text: 'Войти'
                visible: !Tasty.isAuthorized
                onTriggered: { 
                    backAnimation.start();
                    window.pushLoginDialog();
                }
            }
        }
    }
}
