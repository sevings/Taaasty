import QtQuick 2.7
import QtQuick.Controls 2.0 as Q
import QtQuick.Controls.Material 2.0
import org.binque.taaasty 1.0

Q.Drawer {
    id: drawer
    edge: Qt.RightEdge
    height: window.height
    width: 40 * mm
    property Item page: Item { }
    property bool openable: true
    onOpened: {
        if (!openable || column.height < 5 * mm)
            drawer.close();
    }
    Flickable {
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            right: parent.right
        }
        height: contentHeight > parent.height ? parent.height : contentHeight
        flickableDirection: Flickable.VerticalFlick
        topMargin: 1 * mm
        bottomMargin: 1 * mm
        contentWidth: parent.width
        contentHeight: column.height
        Column {
            id: column
            width: parent.width
            spacing: 1 * mm

            // BEST SECTION
            MenuItem {
                text: 'Лучшее'
                onTriggered: {
                    window.setFeedMode(FeedModel.BestMode);
                    drawer.close();
                }
                highlighted: page.mode === FeedModel.BestMode
                visible: page.bestMode === true
            }
            MenuItem {
                text: 'Отличное'
                onTriggered: {
                    window.setFeedMode(FeedModel.ExcellentMode);
                    drawer.close();
                }
                highlighted: page.mode === FeedModel.ExcellentMode
                visible: page.bestMode === true
            }
            MenuItem {
                text: 'Хорошее'
                onTriggered: {
                    window.setFeedMode(FeedModel.GoodMode);
                    drawer.close();
                }
                highlighted: page.mode === FeedModel.GoodMode
                visible: page.bestMode === true
            }
            MenuItem {
                text: 'Неплохое'
                onTriggered: {
                    window.setFeedMode(FeedModel.WellMode);
                    drawer.close();
                }
                highlighted: page.mode === FeedModel.WellMode
                visible: page.bestMode === true
            }
            MenuItem {
                text: 'По рейтингу'
                onTriggered: {
                    window.setFeedMode(FeedModel.BetterThanMode);
                    drawer.close();
                }
                highlighted: page.mode === FeedModel.BetterThanMode
                visible: page.bestMode === true
            }
            MenuSeparator {
                visible: page.bestMode === true
            }

            // FEED SECTION
            MenuItem {
                text: 'Поиск'
                onTriggered: {
                    window.showLineInput('query');
                    drawer.close();
                }
                highlighted: visible && page.query.length > 0
                visible: page.isFeedView === true
            }

            // CHATS SECTION
            MenuItem {
                text: 'Все беседы'
                onTriggered: {
                    ChatsModel.mode = 0//ChatsModel.AllChatsMode
                    window.setFooterFromStack();
                    drawer.close();
                }
                highlighted: ChatsModel.mode == 0 //= ChatsModel.AllChatsMode
                visible: page.isChatsView === true
            }
            MenuItem {
                text: 'Личные'
                onTriggered: {
                    ChatsModel.mode = 1//ChatsModel.PrivateChatsMode
                    window.setFooterFromStack();
                    drawer.close();
                }
                highlighted: ChatsModel.mode == 1 //= ChatsModel.PrivateChatsMode
                visible: page.isChatsView === true
            }
            MenuItem {
                text: 'Комментарии'
                onTriggered: {
                    ChatsModel.mode = 2//ChatsModel.EntryChatsMode
                    window.setFooterFromStack();
                    drawer.close();
                }
                highlighted: ChatsModel.mode == 2 //= ChatsModel.EntryChatsMode
                visible: page.isChatsView === true
            }

            // ENTRY SECTION
            MenuItem {
                text: page.entry && page.entry.isFavorited
                      ? 'Удалить из избранного' : 'Добавить в избранное'
                onTriggered: {
                    page.entry.favorite();
                    drawer.close();
                }
                visible: page.isFullEntryView === true
                         && page.entry.isFavoritable
            }
            MenuItem {
                text: 'Открыть в браузере'
                onTriggered: {
                    var url = page.entry.url;
                    Qt.openUrlExternally(url);
                    drawer.close();
                }
                visible: page.isFullEntryView === true
                         && page.entry.url.length > 0
            }
            MenuItem {
                text: ((page.isFullEntryView === true
                        && page.entry.chat.type === Chat.PrivateConversation)
                       || (page.isMessagesView === true
                           && page.chat.type === Chat.PrivateConversation))
                      ? 'Удалить переписку' : 'Покинуть беседу'
                onTriggered: {
                    if (page.isFullEntryView)
                        page.entry.chat.remove();
                    else
                        page.chat.remove();

                    drawer.close();
                }
                visible: (page.isFullEntryView === true
                          && page.entry.chat.isInvolved)
                         || (page.isMessagesView === true
                             && page.chat.isInvolved)
            }
            MenuSeparator {
                visible: page.isFullEntryView === true
            }
            MenuItem {
                text: 'Перезагрузить'
                onTriggered: {
                    page.entry.reload();
                    drawer.close();
                }
                visible: page.isFullEntryView === true
            }

            // FLOWS SECTION
            MenuItem {
                text: 'Все потоки'
                onTriggered: {
                    page.mode = 0; //= FlowsModel.AllFlowsMode
                    window.setFooterFromStack();
                    drawer.close();
                }
                highlighted: page.mode === FlowsModel.AllFlowsMode
                visible: page.isFlowsView === true
            }
            MenuItem {
                text: 'Мои потоки'
                onTriggered: {
                    page.mode = 1; //= FlowsModel.MyFlowsMode
                    window.setFooterFromStack();
                    drawer.close();
                }
                highlighted: page.mode === FlowsModel.MyFlowsMode
                visible: page.isFlowsView === true
            }

            // PROFILE SECTION
            MenuItem {
                text: 'Обзор постов'
                onTriggered: {
                    window.pushCalendar(page.author.id);
                    drawer.close();
                }
                highlighted: page.mode === FlowsModel.AllFlowsMode
                visible: page.isProfileView === true
                         && (!page.author.isPrivacy
                             || page.tlog.myRelationship === Tlog.Friend
                             || page.tlog.myRelationship === Tlog.Me)
            }
        }
    }
}
