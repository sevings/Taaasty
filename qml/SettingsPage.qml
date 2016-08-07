import QtQuick 2.7
import QtQuick.Controls 2.0 as Q
import QtQuick.Controls.Material 2.0
import org.binque.taaasty 1.0

Pane {
    id: back
    Poppable {
        body: back
    }
    Flickable {
        anchors {
            top: parent.top
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
            Component.onCompleted: {
                imagesBox.checked = Settings.autoloadImages;
                shortBox.checked  = Settings.hideShortPosts;
                nbcBox.checked    = Settings.hideNegativeRated;
                darkBox.checked   = Settings.darkTheme;
            }
            ThemedText {
                text: 'Настройки'
                width: parent.width
                font.pointSize: window.fontBigger
                horizontalAlignment: Text.AlignHCenter
            }
            ThemedCheckBox {
                id: imagesBox
                text: 'Загружать изображения'
                onCheckedChanged: { Settings.autoloadImages = checked; }
            }
            ThemedCheckBox {
                id: shortBox
                text: 'Скрывать короткие посты'
                onCheckedChanged: { Settings.hideShortPosts = checked; }
            }
            ThemedCheckBox {
                id: nbcBox
                text: 'Включить НБК'
                onCheckedChanged: { Settings.hideNegativeRated = checked; }
            }
            ThemedCheckBox {
                id: darkBox
                text: 'Темная тема'
                onCheckedChanged: { Settings.darkTheme = checked; }
            }
            MenuSeparator { }
            ThemedText {
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
                text: Tasty.isAuthorized ? 'Вы вошли как <i>' + Settings.login + '</i>' : 'Вы не вошли'
            }
            ThemedButton {
                anchors.horizontalCenter: parent.horizontalCenter
                width: 40 * mm
                text: Tasty.isAuthorized ? 'Сменить тлог' : 'Войти'
                onClicked: window.pushLoginDialog()
            }
            MenuSeparator { }
            ThemedButton {
                anchors.horizontalCenter: parent.horizontalCenter
                width: 40 * mm
                text: 'О клиенте'
                onClicked: { window.pushAbout(); }
            }
        }
    }
}
